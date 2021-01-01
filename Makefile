CFLAGS=-g -Iinclude

$(shell mkdir -p obj)

all: res/a.gif testlib

testlib: bin/libcaptcha.a
	gcc -o bin/test src/test.c bin/libcaptcha.a
	bin/test | identify -

res/a.gif: bin/captcha	
	bin/captcha | identify -

bin/libcaptcha.a: obj/libcaptcha.o
	ar r bin/libcaptcha.a obj/libcaptcha.o

bin/captcha: src/libcaptcha.c
	gcc $(CFLAGS) -DCAPTCHA -o bin/captcha src/libcaptcha.c

include/f.h: bin/unfont
	bin/unfont > include/.f.h
	mv include/.f.h include/f.h

bin/unfont: src/unfont.c
	gcc $(CFLAGS) -DCAPTCHA -o bin/unfont src/unfont.c

src/libcaptcha.c: src/captcha.c include/f.h
	(echo "// Version $(VERSION)" && echo '// zlib/libpng license is at the end of this file' && grep -v '^#include "f.h"' src/captcha.c && cat include/f.h && echo '/*' && cat LICENSE && echo '*/') > src/.libcaptcha.c
	mv src/.libcaptcha.c src/libcaptcha.c
	
clean:
	rm -f bin/captcha bin/libcaptcha.a obj/libcaptcha.o src/libcaptcha.c bin/test include/f.h src/.libcaptcha.c include/.f.h bin/unfont core


publish: clean
	$(MAKE) VERSION="$(shell date -I) (http://github.com/ITikhonov/captcha/tree/$(shell git show-ref --verify --hash refs/heads/master))" all
