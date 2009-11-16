#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>

#include <stdio.h>

#include "f.h"

int writegif(unsigned char im[70*200]) {
	write(1,"GIF89a" "\xc8\0\x46\0" "\x87" "\0\0", 13); // tag ; widthxheight ; GCT:0:0:7 ; bgcolor + aspect
	int ci; for(ci=0;ci<256;ci++) { write(1,&ci,1); write(1,&ci,1); write(1,&ci,1); } // GCT

	
	write(1,"," "\0\0\0\0" "\xc8\0\x46\0" "\0",10);
// Image Separator // left x top // widthxheight // Flags

	write(1,"\x08",1); // LZW code size
	int x,y;
	unsigned char *i=im;
	for(y=0;y<70;y++) {
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

static const char sw[200]={0, 4, 8, 12, 16, 20, 23, 27, 31, 35, 39, 43, 47, 50, 54, 58, 61, 65, 68, 71, 75, 78, 81, 84, 87, 90, 93, 96, 98, 101, 103, 105, 108, 110, 112, 114, 115, 117, 119, 120, 121, 122, 123, 124, 125, 126, 126, 127, 127, 127, 127, 127, 127, 127, 126, 126, 125, 124, 123, 122, 121, 120, 119, 117, 115, 114, 112, 110, 108, 105, 103, 101, 98, 96, 93, 90, 87, 84, 81, 78, 75, 71, 68, 65, 61, 58, 54, 50, 47, 43, 39, 35, 31, 27, 23, 20, 16, 12, 8, 4, 0, -4, -8, -12, -16, -20, -23, -27, -31, -35, -39, -43, -47, -50, -54, -58, -61, -65, -68, -71, -75, -78, -81, -84, -87, -90, -93, -96, -98, -101, -103, -105, -108, -110, -112, -114, -115, -117, -119, -120, -121, -122, -123, -124, -125, -126, -126, -127, -127, -127, -127, -127, -127, -127, -126, -126, -125, -124, -123, -122, -121, -120, -119, -117, -115, -114, -112, -110, -108, -105, -103, -101, -98, -96, -93, -90, -87, -84, -81, -78, -75, -71, -68, -65, -61, -58, -54, -50, -47, -43, -39, -35, -31, -27, -23, -20, -16, -12, -8, -4};


#define MAX(x,y) ((x>y)?(x):(y))

static int letter(int n, int pos, unsigned char im[70*200], unsigned char swr[200], uint8_t s1, uint8_t s2) {
	char *p=lt[n];
	unsigned char *r=im+200*16+pos;
	unsigned char *i=r;
	int sk1=s1+pos;
	int sk2=s2+pos;
	int mpos=pos;
	int row=0;
	for(;*p!=-101;p++) {
		if(*p<0) {
			if(*p==-100) { r+=200; i=r; sk1=s1+pos; row++; continue; }
			i+=-*p;
			continue;
		}

		if(sk1>=200) sk1=sk1%200;
		int skew=sw[sk1]/16;
		sk1+=(swr[pos+i-r]&0x1)+1;

		if(sk2>=200) sk2=sk2%200;
		int skewh=sw[sk2]/70;
		sk2+=(swr[row]&0x1);

		unsigned char *x=i+skew*200+skewh;
		mpos=MAX(mpos,pos+i-r);
		
		if((x-im)<70*200) *x=(*p)<<4;
		i++;
	}
	return mpos-1;
}

#define NDOTS 100

uint32_t dr[NDOTS];

static void line(unsigned char im[70*200], unsigned char swr[200], uint8_t s1) {
	int x;
	int sk1=s1;
	for(x=0;x<199;x++) {
		if(sk1>=200) sk1=sk1%200;
		int skew=sw[sk1]/16;
		sk1+=swr[x]&0x3+1;
		unsigned char *i= im+(200*(45+skew)+x);
		i[0]=0; i[1]=0; i[200]=0; i[201]=0;
	}
}

static void dots(unsigned char im[70*200]) {
	int n;
	for(n=0;n<NDOTS;n++) {
		uint32_t v=dr[n];
		unsigned char *i=im+v%(200*67);
		
		i[0]=0xf0;
		i[1]=0xf0;
		i[2]=0xf0;
		i[200]=0xf0;
		i[201]=0xf0;
		i[202]=0xf0;
	}
}

static void blur(unsigned char im[70*200]) {
	unsigned char *i=im;
	int x,y;
	for(y=0;y<68;y++) {
               for(x=0;x<198;x++) {
                       int c11=*i,c12=i[1],c13=i[2],
                           c21=i[200],c22=i[201],c23=i[202],
                           c31=i[400],c32=i[402],c33=i[403];
                       *i++=((c11+c12+c21+c22)/4)&0xf0;
               }
	}
}

static const char *letters="abcdefahijklmnopqrstuvwxyz";

void captcha(unsigned char im[70*200], unsigned char l[6]) {
	unsigned char swr[200];
	uint8_t s1,s2;

	int f=open("/dev/urandom",O_RDONLY);
	read(f,l,5); read(f,swr,200); read(f,dr,sizeof(dr)); read(f,&s1,1); read(f,&s2,1);
	close(f);

	memset(im,0xf0,200*70); s1=s1&0x7f; s2=s2&0x3f; l[0]%=25; l[1]%=25; l[2]%=25; l[3]%=25; l[4]%=25; l[5]=0;
	int p=30; p=letter(l[0],p,im,swr,s1,s2); p=letter(l[1],p,im,swr,s1,s2); p=letter(l[2],p,im,swr,s1,s2); p=letter(l[3],p,im,swr,s1,s2); letter(l[4],p,im,swr,s1,s2);
	line(im,swr,s1); dots(im); blur(im);
	l[0]=letters[l[0]]; l[1]=letters[l[1]]; l[2]=letters[l[2]]; l[3]=letters[l[3]]; l[4]=letters[l[4]];
}

int main() {
	char l[6];
	unsigned char im[70*200];

	captcha(im,l);

	writegif(im);
	write(2,l,5);
	return 0;
}

