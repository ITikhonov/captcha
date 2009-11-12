#include <stdio.h>

#include "font.h"

void copy(int s, int e, int idx) {
	printf("char lt%u[] = {",idx);
	int r;
	for(r=5;r<28;r++) {
		char w='.';
		int i=s,last=s;
		for(;;) {
			while(im[r][i]==w) { i++; if(i==e) goto re; }
			printf("%u,",i-last); last=i;
			w=w=='.'?' ':'.';
		}
		re:
			if(w=='.') printf("%u,",i-last);
			printf("-1,");
	}
	printf(" -2};\n");
}

int main() {
	int i,st=0,idx=-1;
	for(i=0;i<width;i++) {
		if(im[0][i]=='.') {
			if(idx>=0) copy(st,i,idx);
			idx++;
			st=i;
		}
	}

	printf("char *lt[]={\n");
	for(i=0;i<idx;i++) {
		printf("    lt%d,\n",i);
	}
	printf("};\n\n");
}


