CFLAGS=-g

all: a.gif

a.gif: captcha	
	./captcha > a.gif


captcha: f.h

f.h: unfont
	./unfont > .f.h
	mv .f.h f.h

