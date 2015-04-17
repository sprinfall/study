INCLUDES=-I../../include
EXTRA_DIST=*.hpp
lib_LTLIBRARIES=libpluginconsole.la
libpluginconsole_la_SOURCES= \
	plugin_console.cpp \
	server.cpp \
	session.cpp \
	../../src/plugin_source.cpp

libpluginconsole_la_LDFLAGS=-lboost_thread -lboost_program_options

