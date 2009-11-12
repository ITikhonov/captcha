#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>

#include "f.h"

unsigned char im[50*160];

int writegif() {
	write(1,"GIF89a",6);
	write(1,"\xa0\0\x32\0",4); // widthxheight
	write(1,"\x80",1); // GCT:0:0:0
	write(1,"\0\0",2); // bgcolor + aspect
	write(1,"\0\0\0\xff\xff\xff",6); // GCT

	
	write(1,",",1); // Image Separator
	write(1,"\0\0\0\0",4); // left x top
	write(1,"\xa0\0\x32\0",4); // widthxheight
	write(1,"\0",1); // Flags

	write(1,"\x02",1); // LZW code size


	int x,y;
	unsigned char *i=im;
	for(y=0;y<50;y++) {
		write(1,"\x78",1); // Data length 160/4*3=120
		for(x=0;x<160;x+=4)
		{
			unsigned char o[3];
			o[0]=4|((i[0]&1)<<3); // 0000b100
			o[1]=0x41|((i[1]&1)<<1)|((i[2]&1)<<7); // b10000b1
			o[2]=0x10|((i[3]&1)<<5); // 00b10000
			write(1,o,3);
			i+=4;
		}
	}
	write(1,"\x01",1); // Data length
	write(1,"\x05",1); // End of LZW
	write(1,"\x00",1); // Terminator
	write(1,";",1); // GIF End
}

char sw[]={0, 4, 9, 14, 19, 23, 28, 33, 37, 42, 46, 51, 55, 59, 64, 68, 72, 76, 79, 83, 87, 90, 94, 97, 100, 103, 105, 108, 111, 113, 115, 117, 119, 120, 122, 123, 124, 125, 126, 127, 127, 127, 127, 127, 127, 127, 126, 125, 124, 123, 122, 120, 118, 117, 115, 112, 110, 107, 105, 102, 99, 96, 93, 89, 86, 82, 79, 75, 71, 67, 63, 59, 54, 50, 45, 41, 36, 32, 27, 22, 18, 13, 8, 3, -1, -5, -10, -15, -20, -24, -29, -34, -38, -43, -47, -52, -56, -60, -65, -69, -73, -77, -80, -84, -88, -91, -94, -97, -100, -103, -106, -109, -111, -113, -115, -117, -119, -121, -122, -124, -125, -126, -126, -127, -127, -127, -127, -127, -127, -126, -126, -125, -124, -123, -121, -120, -118, -116, -114, -112, -109, -107, -104, -101, -98, -95, -92, -89, -85, -82, -78, -74, -70, -66, -62, -58, -53, -49, -44, -40};

int letter(int n, int pos, int dx, int dy) {
	char w=0, *p=lt[n];
	unsigned char *r=im+160*15+pos*25+30;
	unsigned char *i=r;
	for(;*p!=-2;p++) {
		if(*p==-1) { r+=160; i=r; w=0; continue; }
		int j,s=0; for(j=0;j<*p;j++) {
			s+=dx;
			i[j+(s/1024)*160]&=w;
		}
		i+=*p;
		w=!w;
	}
}

#define L(x) ((x&0xff)%25)
#define X(x) ((x>>8)&0xff)

int main() {
	uint32_t r[4];
	int f=open("/dev/urandom",O_RDONLY);
	read(f,r,4*4);
	close(f);

	memset(im,1,160*50);
	letter(L(r[0]),0,X(r[0]),0);
	letter(L(r[1]),1,X(r[1]),0);
	letter(L(r[2]),2,X(r[2]),0);
	letter(L(r[3]),3,X(r[3]),0);
	
	writegif();
}

