#define gifsize 17646

#ifdef __cplusplus
extern "C" {
#endif

//letter indices to letters
void translate_letter_ids(unsigned char l[6]);
void captcha_for_letters(unsigned char im[70*200], unsigned char* letter_ids);

#ifdef __cplusplus
}
#endif
