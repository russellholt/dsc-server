# server & stuff makefile
#
# $Id: makefile,v 1.3 1996/03/20 17:15:41 holtrf Exp holtrf $
#

BASE = /mongo/dest
OBJ = $(BASE)/obj
INCL = $(BASE)/include

CC = gcc
CCC = g++

LFLAGS = -lg++ -ll -ly
CFLAGS = -I$(INCL) -DDEBUG

SRC =	Server.cc 5main.cc Mime.cc http.cc httpserver.cc httpResHandler.cc \
		metaurl.cc surl.cc resHandler.cc httpresponse.cc httprequest.cc cgi.cc \
		cgirtest.cc ohtml.cc htmltable.cc \
		data2html.cc

HDR =	Mime.h SLListSock.h SLList_util.h \
		Server.h Socket.h cgi.h cmd.h htmltable.h http.h \
		httpResHandler.h httprequest.h \
		httpresponse.h httpserver.h \
		list.aug.h metaurl.h \
		ohtml.h resHandler.h \
		status_codes.h stringpair.h \
		surl.h unixsystem.h data2html.h

SERVER_OBJS =	$(OBJ)/Socket.o http.o 5main.o \
				Server.o Mime.o httpserver.o \
				httpResHandler.o surl.o R_Surlist.o \
				resHandler.o httpresponse.o httprequest.o
				
# This stuff is no longer needed here.
# Use the rsl library instead (-lrsl)
RSL_OBJS =	../crb/R_File.o ../crb/R_Output.o \
			../crb/R_String.o ../crb/R_Boolean.o ../crb/R_html.o \
			../crb/R_Integer.o ../crb/SLList_res_util.o \
			../crb/resource.o ../crb/rescall.o ../crb/restable.o \
			../crb/rsl_control.o ../crb/ifrescall.o \
			../crb/y.tab.o ../crb/lex.yy.o


RSRC_OBJS =		cgi.o ../log/slog.o ohtml.o htmltable.o ../crb/R_html.o

RSL_LIB = -L/mongo/dest/crb -lrsl

.cc.o:
	$(CCC) $(CFLAGS) -c $<

.c.o:
	$(CCC) -c $<

all: server5

server5: $(SERVER_OBJS)
	$(CCC) $(SERVER_OBJS) $(RSRC_OBJS) $(RSL_LIB) $(LFLAGS) -o bserver5

install: server5
	strip bserver5
	cp bserver5 server5

cgirtest: cgirtest.o $(RSRC_OBJS)
	$(CCC) $(LFLAGS) cgirtest.o $(RSRC_OBJS) -o cgirtest
	strip cgirtest

dmap: dmap.o checkline.o
	$(CCC) $(LFLAGS) dmap.o checkline.o -o dimagemap
	strip dmap

ht: ohtml.o htmltable.o testtable.o data2html.o
	$(CCC) ohtml.o htmltable.o testtable.o data2html.o $(LFLAGS) -o testtable
	strip testtable

cleancgi:
	rm -f cgirtest.o $(RSRC_OBJS)

clean:
	rm -f $(SERVER_OBJS)

veryclean:
	rm -f *.o

depend:
	make depend -I/usr/local/lib/g++-include ./*.cc

checkout:
	../bin/coall $(SRC)

hcheckout:
	../bin/coall $(HDR)

checkin:
	/usr/local/bin/ci -q $(SRC)
