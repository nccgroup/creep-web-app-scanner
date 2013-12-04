## Process this file with automake to produce Makefile.in

AM_LDFLAGS = -levent

lib_LTLIBRARIES = libcreep.la
libcreep_la_SOURCES = creep.c creep.h

bin_PROGRAMS = creep
creep_SOURCES = creep_main.c
creep_LDADD = libcreep.la
