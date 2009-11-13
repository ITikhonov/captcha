#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>

#include "f.h"

unsigned char im[50*200];

int writegif() {
	write(1,"GIF89a",6);
	write(1,"\xc8\0\x32\0",4); // widthxheight
	write(1,"\x87",1); // GCT:0:0:7
	write(1,"\0\0",2); // bgcolor + aspect

	int ci;
	for(ci=0;ci<256;ci++) { // GCT
		write(1,&ci,1);
		write(1,&ci,1);
		write(1,&ci,1);
	}

	
	write(1,",",1); // Image Separator
	write(1,"\0\0\0\0",4); // left x top
	write(1,"\xc8\0\x32\0",4); // widthxheight
	write(1,"\0",1); // Flags

	write(1,"\x08",1); // LZW code size


	int x,y;
	unsigned char *i=im;
	for(y=0;y<50;y++) {
		int n;
		for(n=0;n<2;n++) {
			write(1,"\xe1",1); // Data length 9*25=225
			for(x=0;x<25;x++)
			{
				unsigned char o[9];

				o[0]=0;			// 00000000
				o[1]=1|(i[0]<<1);	// bbbbbbb1
				o[2]=i[0]>>7;		// 000000xb
				o[3]=4|(i[1]<<3);	// bbbbb100
				o[4]=i[1]>>5;		// 0000xbbb
				o[5]=16|(i[2]<<5);	// bbb10000
				o[6]=i[2]>>3;		// 00xbbbbb
				o[7]=64|(i[3]<<7);	// b1000000
				o[8]=i[3]>>1;		//xbbbbbbb
				write(1,o,9);
				i+=4;
			}
		}
	}
	write(1,"\x02",1); // Data length
	write(1,"\x01\x01",2); // End of LZW
	write(1,"\x00",1); // Terminator
	write(1,";",1); // GIF End
}

char sw[]={0, 4, 9, 14, 19, 23, 28, 33, 37, 42, 46, 51, 55, 59, 64, 68, 72, 76, 79, 83, 87, 90, 94, 97, 100, 103, 105, 108, 111, 113, 115, 117, 119, 120, 122, 123, 124, 125, 126, 127, 127, 127, 127, 127, 127, 127, 126, 125, 124, 123, 122, 120, 118, 117, 115, 112, 110, 107, 105, 102, 99, 96, 93, 89, 86, 82, 79, 75, 71, 67, 63, 59, 54, 50, 45, 41, 36, 32, 27, 22, 18, 13, 8, 3, -1, -5, -10, -15, -20, -24, -29, -34, -38, -43, -47, -52, -56, -60, -65, -69, -73, -77, -80, -84, -88, -91, -94, -97, -100, -103, -106, -109, -111, -113, -115, -117, -119, -121, -122, -124, -125, -126, -126, -127, -127, -127, -127, -127, -127, -126, -126, -125, -124, -123, -121, -120, -118, -116, -114, -112, -109, -107, -104, -101, -98, -95, -92, -89, -85, -82, -78, -74, -70, -66, -62, -58, -53, -49, -44, -40};

int letter(int n, int pos, int dx, int dy) {
	char w=0, *p=lt[n];
	unsigned char *r=im+200*15+pos*25+30;
	unsigned char *i=r;
	for(;*p!=-2;p++) {
		if(*p==-1) { r+=200; i=r; w=0; continue; }
		int j,s=0; for(j=0;j<*p;j++) {
			s+=dx;
			i[j+(s/1024)*200]&=w?255:0;
		}
		i+=*p;
		w=!w;
	}
}

#define L(x) ((x&0xff)%25)
#define X(x) ((x>>8)&0xff)

int main() {
	uint32_t r[5];

	int f=open("/dev/urandom",O_RDONLY);
	read(f,r,5*4);
	read(f,im,200*50);
	close(f);

	memset(im,255,200*50);

	letter(L(r[0]),0,X(r[0]),0);
	letter(L(r[1]),1,X(r[1]),0);
	letter(L(r[2]),2,X(r[2]),0);
	letter(L(r[3]),3,X(r[3]),0);
	letter(L(r[4]),4,X(r[4]),0);
	
	writegif();
}

