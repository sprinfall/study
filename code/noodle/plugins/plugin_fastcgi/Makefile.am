INCLUDES=-I../../include
lib_LTLIBRARIES=libfastcgi.la
libfastcgi_la_SOURCES= \
	../../src/plugin_source.cpp \
	fcgiapp.c \
	fcgio.cpp \
	fcgi_stdio.c \
	os_unix.c \
	plugin_fastcgi.cpp
	
libfastcgi_la_LDFLAGS=-lboost_thread

