CFLAGS=-g -Wall
LDFLAGS=

CXX=g++

INSTALL?=install
MAKE?=make

prefix=/usr/local

soversion=0
libname=libtek_vxi11.so
full_libname=$(libname).$(soversion)
