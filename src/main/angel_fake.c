
#include <lighttpd/base.h>
#include <lighttpd/angel.h>
#include <lighttpd/ip_parsers.h>

#include <fcntl.h>

/* listen to a socket */
int li_angel_fake_listen(liServer *srv, GString *str) {
	guint32 ipv4;
#ifdef HAVE_IPV6
	guint8 ipv6[16];
#endif
	guint16 port;

#ifdef HAVE_SYS_UN_H
	if (0 == strncmp(str->str, "unix:/", 6)) {
		int s;
		struct sockaddr_un *un;
		socklen_t slen = str->len + 1 - 5 + sizeof(un->sun_family);

		un = g_malloc0(slen);
		un->sun_family = AF_UNIX;
		strcpy(un->sun_path, str->str + 5);

		if (-1 == unlink(un->sun_path)) {
			switch (errno) {
			case ENOENT:
				break;
			default:
				ERROR(srv, "removing old socket '%s' failed: %s\n", str->str, g_strerror(errno));
				g_free(un);
				return -1;
			}
		}
		if (-1 == (s = socket(AF_UNIX, SOCK_STREAM, 0))) {
			ERROR(srv, "Couldn't open socket: %s", g_strerror(errno));
			g_free(un);
			return -1;
		}
		if (-1 == bind(s, (struct sockaddr*) un, slen)) {
			close(s);
			ERROR(srv, "Couldn't bind socket to '%s': %s", str->str, g_strerror(errno));
			g_free(un);
			return -1;
		}
		if (-1 == listen(s, 1000)) {
			close(s);
			ERROR(srv, "Couldn't listen on '%s': %s", str->str, g_strerror(errno));
			g_free(un);
			return -1;
		}
		g_free(un);
		DEBUG(srv, "listen to unix socket: '%s'", str->str);
		return s;
	} else
#endif
	if (li_parse_ipv4(str->str, &ipv4, NULL, &port)) {
		int s, v;
		struct sockaddr_in addr;

		if (!port) port = 80;
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = ipv4;
		addr.sin_port = htons(port);
		if (-1 == (s = socket(AF_INET, SOCK_STREAM, 0))) {
			ERROR(srv, "Couldn't open socket: %s", g_strerror(errno));
			return -1;
		}
		v = 1;
		if (-1 == setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &v, sizeof(v))) {
			close(s);
			ERROR(srv, "Couldn't setsockopt(SO_REUSEADDR): %s", g_strerror(errno));
			return -1;
		}
		if (-1 == bind(s, (struct sockaddr*)&addr, sizeof(addr))) {
			close(s);
			ERROR(srv, "Couldn't bind socket to '%s': %s", inet_ntoa(addr.sin_addr), g_strerror(errno));
			return -1;
		}
		if (-1 == listen(s, 1000)) {
			close(s);
			ERROR(srv, "Couldn't listen on '%s': %s", inet_ntoa(addr.sin_addr), g_strerror(errno));
			return -1;
		}
		DEBUG(srv, "listen to ipv4: '%s' port: %d", inet_ntoa(addr.sin_addr), port);
		return s;
#ifdef HAVE_IPV6
	} else if (li_parse_ipv6(str->str, ipv6, NULL, &port)) {
		GString *ipv6_str = g_string_sized_new(0);
		int s, v;
		struct sockaddr_in6 addr;
		li_ipv6_tostring(ipv6_str, ipv6);
		
		if (!port) port = 80;
		memset(&addr, 0, sizeof(addr));
		addr.sin6_family = AF_INET6;
		memcpy(&addr.sin6_addr, ipv6, 16);
		addr.sin6_port = htons(port);
		if (-1 == (s = socket(AF_INET6, SOCK_STREAM, 0))) {
			ERROR(srv, "Couldn't open socket: %s", g_strerror(errno));
			g_string_free(ipv6_str, TRUE);
			return -1;
		}
		v = 1;
		if (-1 == setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &v, sizeof(v))) {
			close(s);
			ERROR(srv, "Couldn't setsockopt(SO_REUSEADDR): %s", g_strerror(errno));
			g_string_free(ipv6_str, TRUE);
			return -1;
		}
		if (-1 == setsockopt(s, IPPROTO_IPV6, IPV6_V6ONLY, &v, sizeof(v))) {
			close(s);
			ERROR(srv, "Couldn't setsockopt(IPV6_V6ONLY): %s", g_strerror(errno));
			g_string_free(ipv6_str, TRUE);
			return -1;
		}
		if (-1 == bind(s, (struct sockaddr*)&addr, sizeof(addr))) {
			close(s);
			ERROR(srv, "Couldn't bind socket to '%s': %s", ipv6_str->str, g_strerror(errno));
			g_string_free(ipv6_str, TRUE);
			return -1;
		}
		if (-1 == listen(s, 1000)) {
			close(s);
			ERROR(srv, "Couldn't listen on '%s': %s", ipv6_str->str, g_strerror(errno));
			g_string_free(ipv6_str, TRUE);
			return -1;
		}
		DEBUG(srv, "listen to ipv6: '%s' port: %d", ipv6_str->str, port);
		g_string_free(ipv6_str, TRUE);
		return s;
#endif
	} else {
		ERROR(srv, "Invalid ip: '%s'", str->str);
		return -1;
	}
}

/* print log messages during startup to stderr */
gboolean li_angel_fake_log(liServer *srv, GString *str) {
	const char *buf;
	guint len;
	ssize_t written;
	UNUSED(srv);

	/* g_string_prepend(str, "fake: "); */
	buf = str->str;
	len = str->len;

	while (len > 0) {
		written = write(2, buf, len);
		if (written < 0) {
			switch (errno) {
			case EAGAIN:
			case EINTR:
				continue;
			}
			g_string_free(str, TRUE);
			return FALSE;
		}
		len -= written;
		buf += written;
	}
	g_string_free(str, TRUE);
	return TRUE;
}

int li_angel_fake_log_open_file(liServer *srv, GString *filename) {
	int fd;

	fd = open(filename->str, O_RDWR | O_CREAT | O_APPEND, 0660);
	if (-1 == fd) {
		ERROR(srv, "failed to open log file '%s': %s", filename->str, g_strerror(errno));
	}

	return fd;
}
