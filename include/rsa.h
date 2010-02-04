#include <avr/io.h>
#include <stdlib.h>
#include <math.h>
#include <util/delay.h>
#include "io.h"

unsigned char *rsaEncrypt(unsigned int seed);
unsigned char encryptDecryptByte(unsigned char x, unsigned char p, unsigned int n);
unsigned int getD(unsigned int totient, unsigned int e);
unsigned int getCoPrimeNumber(unsigned int a, unsigned int seed);
int testCoprimality(unsigned int a, unsigned int b);
unsigned int avrGcd(unsigned int a, unsigned int b);
unsigned char randomlyChoosePrime(unsigned int seed);
unsigned char *power(unsigned char a, unsigned char n);
unsigned char randWithLimit(unsigned char limit, unsigned int seed);
