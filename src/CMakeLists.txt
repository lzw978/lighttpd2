INCLUDE(CheckIncludeFiles)
INCLUDE(CheckFunctionExists)
INCLUDE(CheckVariableExists)
INCLUDE(CheckTypeSize)
INCLUDE(CheckLibraryExists)
INCLUDE(CMakeDetermineCCompiler)
INCLUDE(FindThreads)
INCLUDE(CPack)
#INCLUDE(UsePkgConfig)
INCLUDE(FindPkgConfig)

INCLUDE(LighttpdMacros)
FIND_PACKAGE(Ragel REQUIRED)
cmake_policy(SET CMP0005 OLD)

ADD_DEFINITIONS(-D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGE_FILES)

OPTION(WITH_XATTR "with xattr-support for the stat-cache [default: on]" ON)
OPTION(WITH_OPENSSL "with openssl-support [default: on]" ON)
OPTION(WITH_PCRE "with regex support [default: on]" ON)
OPTION(WITH_LUA "with lua 5.1 for lua-configfile [default: on]" ON)
OPTION(BUILD_STATIC "build a static lighttpd with all modules added")
IF(BUILD_STATIC)
  SET(LIGHTTPD_STATIC 1)
ELSE(BUILD_STATIC)
  SET(CMAKE_SHARED_LIBRARY_PREFIX "")
ENDIF(BUILD_STATIC)

CHECK_INCLUDE_FILES(sys/devpoll.h HAVE_SYS_DEVPOLL_H)
CHECK_INCLUDE_FILES(sys/epoll.h HAVE_SYS_EPOLL_H)
CHECK_INCLUDE_FILES(sys/event.h HAVE_SYS_EVENT_H)
CHECK_INCLUDE_FILES(sys/mman.h HAVE_SYS_MMAN_H)
CHECK_INCLUDE_FILES(sys/poll.h HAVE_SYS_POLL_H)
CHECK_INCLUDE_FILES(sys/port.h HAVE_SYS_PORT_H)
CHECK_INCLUDE_FILES(sys/prctl.h HAVE_SYS_PRCTL_H)
CHECK_INCLUDE_FILES(sys/resource.h HAVE_SYS_RESOURCE_H)
CHECK_INCLUDE_FILES(sys/sendfile.h HAVE_SYS_SENDFILE_H)
CHECK_INCLUDE_FILES(sys/select.h HAVE_SYS_SELECT_H)
CHECK_INCLUDE_FILES(sys/syslimits.h HAVE_SYS_SYSLIMITS_H)
CHECK_INCLUDE_FILES(sys/types.h HAVE_SYS_TYPES_H)
CHECK_INCLUDE_FILES(sys/uio.h HAVE_SYS_UIO_H)
CHECK_INCLUDE_FILES(sys/un.h HAVE_SYS_UN_H)
CHECK_INCLUDE_FILES(sys/wait.h HAVE_SYS_WAIT_H)
CHECK_INCLUDE_FILES(sys/time.h HAVE_SYS_TIME_H)
CHECK_INCLUDE_FILES(time.h HAVE_TIME_H)
CHECK_INCLUDE_FILES(unistd.h HAVE_UNISTD_H)
CHECK_INCLUDE_FILES(pthread.h HAVE_PTHREAD_H)
CHECK_INCLUDE_FILES(getopt.h HAVE_GETOPT_H)
CHECK_INCLUDE_FILES(inttypes.h HAVE_INTTYPES_H)
CHECK_INCLUDE_FILES(poll.h HAVE_POLL_H)
CHECK_INCLUDE_FILES(pwd.h HAVE_PWD_H)
CHECK_INCLUDE_FILES(stddef.h HAVE_STDDEF_H)
CHECK_INCLUDE_FILES(stdint.h HAVE_STDINT_H)
CHECK_INCLUDE_FILES(syslog.h HAVE_SYSLOG_H)

CHECK_INCLUDE_FILES(crypt.h HAVE_CRYPT_H)
IF(HAVE_CRYPT_H)
  ## check if we need libcrypt for crypt()
  CHECK_LIBRARY_EXISTS(crypt crypt "" HAVE_LIBCRYPT)
ENDIF(HAVE_CRYPT_H)

CHECK_INCLUDE_FILES(sys/inotify.h HAVE_SYS_INOTIFY_H)
IF(HAVE_SYS_INOTIFY_H)
  CHECK_FUNCTION_EXISTS(inotify_init HAVE_INOTIFY_INIT)
ENDIF(HAVE_SYS_INOTIFY_H)

SET(CMAKE_EXTRA_INCLUDE_FILES sys/socket.h)
CHECK_TYPE_SIZE(socklen_t HAVE_SOCKLEN_T)
SET(CMAKE_EXTRA_INCLUDE_FILES)

CHECK_TYPE_SIZE(long SIZEOF_LONG)
CHECK_TYPE_SIZE(off_t SIZEOF_OFF_T)

CHECK_FUNCTION_EXISTS(chroot HAVE_CHROOT)
CHECK_FUNCTION_EXISTS(crypt HAVE_CRYPT)
CHECK_FUNCTION_EXISTS(epoll_ctl HAVE_EPOLL_CTL)
CHECK_FUNCTION_EXISTS(fork HAVE_FORK)
CHECK_FUNCTION_EXISTS(getrlimit HAVE_GETRLIMIT)
CHECK_FUNCTION_EXISTS(getuid HAVE_GETUID)
CHECK_FUNCTION_EXISTS(gmtime_r HAVE_GMTIME_R)
CHECK_FUNCTION_EXISTS(inet_ntop HAVE_INET_NTOP)
CHECK_FUNCTION_EXISTS(kqueue HAVE_KQUEUE)
CHECK_FUNCTION_EXISTS(localtime_r HAVE_LOCALTIME_R)
CHECK_FUNCTION_EXISTS(lstat HAVE_LSTAT)
CHECK_FUNCTION_EXISTS(madvise HAVE_MADVISE)
CHECK_FUNCTION_EXISTS(memcpy HAVE_MEMCPY)
CHECK_FUNCTION_EXISTS(memset HAVE_MEMSET)
CHECK_FUNCTION_EXISTS(mmap HAVE_MMAP)
CHECK_FUNCTION_EXISTS(pathconf HAVE_PATHCONF)
CHECK_FUNCTION_EXISTS(poll HAVE_POLL)
CHECK_FUNCTION_EXISTS(port_create HAVE_PORT_CREATE)
CHECK_FUNCTION_EXISTS(prctl HAVE_PRCTL)
CHECK_FUNCTION_EXISTS(pread HAVE_PREAD)
CHECK_FUNCTION_EXISTS(posix_fadvise HAVE_POSIX_FADVISE)
CHECK_FUNCTION_EXISTS(select HAVE_SELECT)
CHECK_FUNCTION_EXISTS(sendfile HAVE_SENDFILE)
CHECK_FUNCTION_EXISTS(sendfile64 HAVE_SENDFILE64)
CHECK_FUNCTION_EXISTS(sendfilev HAVE_SENDFILEV)
CHECK_FUNCTION_EXISTS(sigaction HAVE_SIGACTION)
CHECK_FUNCTION_EXISTS(signal HAVE_SIGNAL)
CHECK_FUNCTION_EXISTS(sigtimedwait HAVE_SIGTIMEDWAIT)
CHECK_FUNCTION_EXISTS(strptime HAVE_STRPTIME)
CHECK_FUNCTION_EXISTS(syslog HAVE_SYSLOG)
CHECK_FUNCTION_EXISTS(writev HAVE_WRITEV)
CHECK_FUNCTION_EXISTS(inet_aton HAVE_INET_ATON)
CHECK_FUNCTION_EXISTS(inet_atop HAVE_IPV6)

## refactor me
MACRO(XCONFIG _package _include_DIR _link_DIR _link_FLAGS _cflags)
# reset the variables at the beginning
  SET(${_include_DIR})
  SET(${_link_DIR})
  SET(${_link_FLAGS})
  SET(${_cflags})

  FIND_PROGRAM(${_package}CONFIG_EXECUTABLE NAMES ${_package} PATHS /usr/local/bin )

  # if pkg-config has been found
  IF(${_package}CONFIG_EXECUTABLE)
    SET(XCONFIG_EXECUTABLE "${${_package}CONFIG_EXECUTABLE}")
    MESSAGE(STATUS "found ${_package}: ${XCONFIG_EXECUTABLE}")

    EXEC_PROGRAM(${XCONFIG_EXECUTABLE} ARGS --libs OUTPUT_VARIABLE __link_FLAGS)
    STRING(REPLACE "\n" "" ${_link_FLAGS} ${__link_FLAGS})
    EXEC_PROGRAM(${XCONFIG_EXECUTABLE} ARGS --cflags OUTPUT_VARIABLE __cflags)
    STRING(REPLACE "\n" "" ${_cflags} ${__cflags})

  ELSE(${_package}CONFIG_EXECUTABLE)
    MESSAGE(STATUS "found ${_package}: no")
  ENDIF(${_package}CONFIG_EXECUTABLE)
ENDMACRO(XCONFIG _package _include_DIR _link_DIR _link_FLAGS _cflags)

##INCLUDE_DIRECTORIES(${GTHREAD_INCDIR})
ADD_DEFINITIONS(${GTHREAD_CFLAGS})

IF(WITH_XATTR)
  CHECK_INCLUDE_FILES(attr/attributes.h HAVE_ATTR_ATTRIBUTES_H)
ENDIF(WITH_XATTR)

CHECK_INCLUDE_FILES(ev.h HAVE_EV_H)
IF(HAVE_EV_H)
	CHECK_LIBRARY_EXISTS(ev ev_loop "" HAVE_LIBEV)
	IF(HAVE_LIBEV)
		SET(EV_LDFLAGS -lev)
	ELSE(HAVE_LIBEV)
		MESSAGE(FATAL_ERROR "Couldn't find lib ev")
	ENDIF(HAVE_LIBEV)
ELSE(HAVE_EV_H)
	MESSAGE(FATAL_ERROR "Couldn't find <ev.h>")
ENDIF(HAVE_EV_H)

IF(WITH_OPENSSL)
  CHECK_INCLUDE_FILES(openssl/ssl.h HAVE_OPENSSL_SSL_H)
  IF(HAVE_OPENSSL_SSL_H)
    CHECK_LIBRARY_EXISTS(crypto BIO_f_base64 "" HAVE_LIBCRYPTO)
    IF(HAVE_LIBCRYPTO)
      SET(OPENSSL_NO_KRB5 1)
      CHECK_LIBRARY_EXISTS(ssl SSL_new "" HAVE_LIBSSL)
    ENDIF(HAVE_LIBCRYPTO)
  ENDIF(HAVE_OPENSSL_SSL_H)
ENDIF(WITH_OPENSSL)

IF(WITH_PCRE)
  ## if we have pcre-config, use it
  XCONFIG(pcre-config PCRE_INCDIR PCRE_LIBDIR PCRE_LDFLAGS PCRE_CFLAGS)
  IF(PCRE_LDFLAGS OR PCRE_CFLAGS) 
    MESSAGE(STATUS "found pcre at: LDFLAGS: ${PCRE_LDFLAGS} CFLAGS: ${PCRE_CFLAGS}")

    IF(NOT PCRE_CFLAGS STREQUAL "\n")
      ## if it is empty we'll get newline returned
      SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${PCRE_CFLAGS}")
    ENDIF(NOT PCRE_CFLAGS STREQUAL "\n")

    CHECK_INCLUDE_FILES(pcre.h HAVE_PCRE_H)

    SET(CMAKE_REQUIRED_FLAGS ${PCRE_LDFLAGS})
    CHECK_LIBRARY_EXISTS(pcre pcre_exec "" HAVE_LIBPCRE)
    SET(CMAKE_REQUIRED_FLAGS)

  ELSE(PCRE_LDFLAGS OR PCRE_CFLAGS) 
    IF(NOT WIN32)
      CHECK_INCLUDE_FILES(pcre.h HAVE_PCRE_H)
      CHECK_LIBRARY_EXISTS(pcre pcre_exec "" HAVE_LIBPCRE)
  		SET(PCRE_LIBRARY pcre)
    ELSE(NOT WIN32)
      FIND_PATH(PCRE_INCLUDE_DIR pcre.h
      /usr/local/include
      /usr/include
      )
  
      SET(PCRE_NAMES pcre)
      FIND_LIBRARY(PCRE_LIBRARY
      NAMES ${PCRE_NAMES}
      PATHS /usr/lib /usr/local/lib
      )
  
      IF(PCRE_INCLUDE_DIR AND PCRE_LIBRARY)
        SET(CMAKE_REQUIRED_INCLUDES ${PCRE_INCLUDE_DIR})
        SET(CMAKE_REQUIRED_LIBRARIES ${PCRE_LIBRARY})
        CHECK_INCLUDE_FILES(pcre.h HAVE_PCRE_H)
        CHECK_LIBRARY_EXISTS(pcre pcre_exec "" HAVE_LIBPCRE)
        SET(CMAKE_REQUIRED_INCLUDES)
        SET(CMAKE_REQUIRED_LIBRARIES)
        INCLUDE_DIRECTORIES(${PCRE_INCLUDE_DIR})
      ENDIF(PCRE_INCLUDE_DIR AND PCRE_LIBRARY)
    ENDIF(NOT WIN32)
  ENDIF(PCRE_LDFLAGS OR PCRE_CFLAGS) 

  IF(NOT HAVE_PCRE_H)
    MESSAGE(FATAL_ERROR "pcre.h couldn't be found")
  ENDIF(NOT HAVE_PCRE_H)
  IF(NOT HAVE_LIBPCRE)
    MESSAGE(FATAL_ERROR "libpcre couldn't be found")
  ENDIF(NOT HAVE_LIBPCRE)

ENDIF(WITH_PCRE)

# glib/gthread
pkg_check_modules(GTHREAD REQUIRED gthread-2.0)
#INCLUDE_DIRECTORIES(${GTHREAD_INCLUDE_DIRS})

IF(WITH_LUA)
  pkg_search_module(LUA REQUIRED lua lua5.1)
  MESSAGE(STATUS "found lua at: INCDIR: ${LUA_INCLUDE_DIRS} LIBDIR: ${LUA_LIBRARY_DIRS} LDFLAGS: ${LUA_LDFLAGS} CFLAGS: ${LUA_CFLAGS}")
  SET(HAVE_LIBLUA 1 "Have liblua")
  SET(HAVE_LUA_H  1 "Have liblua header")
ENDIF(WITH_LUA)

IF(NOT BUILD_STATIC)
  CHECK_INCLUDE_FILES(dlfcn.h HAVE_DLFCN_H)
ENDIF(NOT BUILD_STATIC)

ADD_DEFINITIONS(
  -DLIGHTTPD_VERSION_ID=20000
  -DPACKAGE_NAME="\\"${CMAKE_PROJECT_NAME}\\""
  -DPACKAGE_VERSION="\\"${CPACK_PACKAGE_VERSION}\\""
  -DPACKAGE_BUILD_DATE="\\"-\\""
  )

IF(NOT SBINDIR)
    SET(SBINDIR "sbin")
ENDIF(NOT SBINDIR)

IF(NOT LIGHTTPD_MODULES_DIR)
    SET(LIGHTTPD_MODULES_DIR "lib${LIB_SUFFIX}/lighttpd")
ENDIF(NOT LIGHTTPD_MODULES_DIR)

IF(NOT WIN32)
ADD_DEFINITIONS(
  -DLIBRARY_DIR="\\"${CMAKE_INSTALL_PREFIX}/${LIGHTTPD_MODULES_DIR}\\""
)
ELSE(NOT WIN32)
## We use relative path in windows
ADD_DEFINITIONS(
  -DLIBRARY_DIR="\\"lib\\""
)
ENDIF(NOT WIN32)

## Write out config.h
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

ADD_DEFINITIONS(-DHAVE_CONFIG_H)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

SET(COMMON_SRC
	actions.c
	base.c
	chunk.c
	chunk_parser.c
	condition.c
	condition_parsers.c
	config_parser.c
	connection.c
	filter_chunked.c
	http_headers.c
	http_request_parser.c
	log.c
	network.c
	network_write.c network_writev.c
	network_linux_sendfile.c
	options.c
	plugin.c
	request.c
	response.c
	server.c
	sys-files.c
	sys-socket.c
	url_parser.c
	utils.c
	worker.c

	plugin_core.c
)

IF(WITH_LUA)
SET(COMMON_SRC ${COMMON_SRC}
	actions_lua.c
	condition_lua.c
	config_lua.c
	options_lua.c
)
ENDIF(WITH_LUA)

## Build parsers by using ragel...
RAGEL_PARSER(condition_parsers.rl)
RAGEL_PARSER(config_parser.rl)
RAGEL_PARSER(http_request_parser.rl)
RAGEL_PARSER(url_parser.rl)

SET(L_INSTALL_TARGETS)

ADD_EXECUTABLE(lighttpd
	lighttpd.c
	${COMMON_SRC}
)
SET(L_INSTALL_TARGETS ${L_INSTALL_TARGETS} lighttpd)

IF(HAVE_PCRE_H)
  TARGET_LINK_LIBRARIES(lighttpd ${PCRE_LIBRARY})
ENDIF(HAVE_PCRE_H)

ADD_TARGET_PROPERTIES(lighttpd LINK_FLAGS ${LUA_LDFLAGS})
ADD_TARGET_PROPERTIES(lighttpd COMPILE_FLAGS ${LUA_CFLAGS})

ADD_TARGET_PROPERTIES(lighttpd LINK_FLAGS ${EV_LDFLAGS})
ADD_TARGET_PROPERTIES(lighttpd COMPILE_FLAGS ${EV_CFLAGS})

IF(CMAKE_COMPILER_IS_GNUCC)
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -Wall -g -Wshadow -W -pedantic")
  SET(CMAKE_C_FLAGS_RELEASE        "${CMAKE_C_FLAGS_RELEASE}     -O2")
  SET(CMAKE_C_FLAGS_DEBUG          "${CMAKE_C_FLAGS_DEBUG}       -O0")
  SET(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_WITHDEBINFO} -O2")
  ADD_DEFINITIONS(-D_GNU_SOURCE)
ENDIF(CMAKE_COMPILER_IS_GNUCC)

ADD_TARGET_PROPERTIES(lighttpd LINK_FLAGS ${GTHREAD_LDFLAGS})
ADD_TARGET_PROPERTIES(lighttpd COMPILE_FLAGS ${GTHREAD_CFLAGS})

SET_TARGET_PROPERTIES(lighttpd PROPERTIES CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})

IF(WIN32)
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DNVALGRIND")
  ADD_TARGET_PROPERTIES(lighttpd COMPILE_FLAGS "-DLI_DECLARE_EXPORTS")
  TARGET_LINK_LIBRARIES(lighttpd ws2_32)

  IF(MINGW)
    TARGET_LINK_LIBRARIES(lighttpd msvcr70)
    ADD_TARGET_PROPERTIES(lighttpd LINK_FLAGS "-Wl,-subsystem,console")
  ENDIF(MINGW)
ENDIF(WIN32)

IF(HAVE_PCRE_H)
  TARGET_LINK_LIBRARIES(lighttpd pcre)
ENDIF(HAVE_PCRE_H)

IF(HAVE_LIBSSL AND HAVE_LIBCRYPTO)
  TARGET_LINK_LIBRARIES(lighttpd ssl)
  TARGET_LINK_LIBRARIES(lighttpd crypto)
ENDIF(HAVE_LIBSSL AND HAVE_LIBCRYPTO)

IF(NOT WIN32)
INSTALL(TARGETS ${L_INSTALL_TARGETS}
	RUNTIME DESTINATION ${SBINDIR}
	LIBRARY DESTINATION ${LIGHTTPD_MODULES_DIR}
	ARCHIVE DESTINATION ${LIGHTTPD_MODULES_DIR}/static)
ELSE(NOT WIN32)
## HACK to make win32 to install our libraries in desired directory..
INSTALL(TARGETS lighttpd
	RUNTIME DESTINATION ${SBINDIR}
	ARCHIVE DESTINATION lib/static)
LIST(REMOVE_ITEM L_INSTALL_TARGETS lighttpd)
INSTALL(TARGETS ${L_INSTALL_TARGETS}
	RUNTIME DESTINATION ${SBINDIR}/lib
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib/static)
ENDIF(NOT WIN32)