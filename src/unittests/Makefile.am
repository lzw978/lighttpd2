

AM_CFLAGS = -I$(top_srcdir)/include -I$(top_builddir)/include
AM_CFLAGS += $(GTHREAD_CFLAGS) $(GMODULE_CFLAGS) $(LIBEV_CFLAGS) $(LUA_CFLAGS)
AM_LDFLAGS = -export-dynamic -avoid-version -no-undefined $(GTHREAD_LIBS) $(GMODULE_LIBS) $(LIBEV_LIBS) $(LUA_LIBS)
LDADD = ../common/liblighttpd2-common.la ../main/liblighttpd2-shared.la

test_binaries=test-chunk test-ip-parser test-range-parser test-utils test-radix

check_PROGRAMS=$(test_binaries)

TESTS=$(test_binaries)
TESTS_ENVIRONMENT=gtester
