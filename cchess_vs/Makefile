# 参考《C语言核心技术》第19章
#vpath %.h src/head src/json
#vpath %.cpp src
#vpath %.o obj

CC = g++
CFLAGS = -Wall -std=c++11  -fexec-charset=gbk  -iquote # -g 
#LDFLAGS = -L/C/msys32/mingw32/lib -lpcre16 lib/pdcurses.a
P = ./
PO = obj/
OBJS = $(PO)jsoncpp.obj $(PO)Tools.obj $(PO)Piece.obj $(PO)Seat.obj $(PO)Board.obj $(PO)ChessManual.obj $(PO)main.obj

a.exe: $(OBJS)
	$(CC) -Wall -o $@ $^ $(LDFLAGS) 
	
$(OBJS): $(PO)%.obj : $(P)%.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

$(OBJS): $(P)*.h
#dependencies: $(OBJS: .o=.c)
#	$(CC) -M $^ > $@\
#
#include dependencies

.PHONY: clean
clean:
	rm a.exe obj/*.o