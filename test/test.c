#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>

const int gifsize;
void makegif(unsigned char im[70*200], unsigned char gif[gifsize]);

void captcha(unsigned char im[70*200], unsigned char l[6]) {
	unsigned char swr[200];
	uint8_t s1,s2;

	int f=open("/dev/urandom",O_RDONLY);
	read(f,l,5); read(f,swr,200); read(f,dr,sizeof(dr)); read(f,&s1,1); read(f,&s2,1);
	close(f);

	l[0]%=25;
	l[1]%=25;
	l[2]%=25;
	l[3]%=25;
	l[4]%=25;
	l[5]=0;

	memset(im,0xff,200*70);
	s1=s1&0x7f;
	s2=s2&0x3f;
	
	int p=30;
	p=letter(l[0],p,im,swr,s1,s2);
	p=letter(l[1],p,im,swr,s1,s2);
	p=letter(l[2],p,im,swr,s1,s2);
	p=letter(l[3],p,im,swr,s1,s2);
	letter(l[4],p,im,swr,s1,s2);

	dots(im); blur(im); filter(im); line(im,swr,s1); 
	translate_letter_ids(l);
}

int main() {
    char l[6];
    unsigned char im[70*200];
    unsigned char gif[gifsize];

    captcha(im, l);
    makegif(im, gif);

    write(1, gif, gifsize);
    write(2, l, 5);

    return 0;
}

