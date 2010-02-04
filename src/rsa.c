#include "rsa.h"

//tested
unsigned char *rsaEncrypt(unsigned int seed)
{
	unsigned char p = randomlyChoosePrime(seed);
	//display2CharsOn7SegBCD(p);
	//_delay_ms (1000.0);
	seed++;
	unsigned char q = randomlyChoosePrime(seed);
	while(p == q)
	{
		seed++;
		q = randomlyChoosePrime(seed);
	}
	//display2CharsOn7SegBCD(q);
	//_delay_ms (1000.0);
	unsigned int n = p*q;
	//display2CharsOn7SegBCD(n);
	//_delay_ms (1000.0);
	unsigned int totient = (int)(p-1)*(int)(q-1);
	//display2CharsOn7SegBCD(totient);
	//_delay_ms (1000.0);
	seed++;
	unsigned int e = getCoPrimeNumber(totient, seed);
	//display2CharsOn7SegBCD(e);
	//_delay_ms (1000.0);
	unsigned int d = getD(totient, e);
	//display2CharsOn7SegBCD(d);
	//_delay_ms (1000.0);

	unsigned char *keys = 	(unsigned char*)malloc(sizeof(unsigned char)*6);
	*(keys + 0) = (unsigned char)e;
	*(keys + 1) = (unsigned char)(e>>8);
	*(keys + 2) = (unsigned char)d;
	*(keys + 3) = (unsigned char)(d>>8);
	*(keys + 4) = (unsigned char)n;
	*(keys + 5) = (unsigned char)(n>>8);
	return keys;
}

//tested
unsigned char encryptDecryptByte(unsigned char x, unsigned char p, unsigned int n)
{
	unsigned char encryptedByte = 0;
	unsigned char temp[1];
	temp[0] = x;
	unsigned char *a = (unsigned char*)calloc(p,sizeof(unsigned char));
	a = (unsigned char *)power(x, p);
	//encryptedByte = avrMod(result, p, n);
	for(int i = p-1; i >= 0; i--)
	{
		// This process allows for the carry byte of previous mod operation to be included
		unsigned char currByte = *(a + i);
		unsigned int currInt = ((int)encryptedByte) << 8;
		currInt += currByte;
		unsigned int remainder = currInt % n;
		encryptedByte = (unsigned char)remainder;
	}
	return encryptedByte;
}

//tested
unsigned int getD(unsigned int totient, unsigned int e)
{
	unsigned int d;
	for(d = 2; d < 0xffff; d++)
	{
		unsigned int ed = e * d;
		unsigned int EDmodTotient = (ed) % totient;
		if(EDmodTotient == 1)
		{
			return d;
		}
	}
	return d;
}

//tested
unsigned int getCoPrimeNumber(unsigned int a, unsigned int seed)
{
	unsigned int b;
	int areCoprime = 0;
	for(b = 2; b < 0xffff; b++)
	{
		areCoprime = testCoprimality(a,b);
		if(areCoprime == 1)
		{
			return b;
		}
	}
	return 1;
}

//test
int testCoprimality(unsigned int a, unsigned int b)
{
	int coprime = 0;
	unsigned int gcdVal = avrGcd(a,b);
	if(gcdVal == 1)
	{
		coprime = 1;
	}
	return coprime;
}

//Tested
unsigned int avrGcd(unsigned int a, unsigned int b)
{
	while(a!=b)
	{
		if(a>b)
		{
			a=a-b;
		}
		else
		{
			b=b-a;
		}
	}
	return a;
}

//tested
unsigned char randomlyChoosePrime(unsigned int seed)
{
	//I have not included 2,or 3 because a combination of these primes may
	//	limit expressiveness of characters used(ie less than a-z)
	unsigned char PRIMES[] = {5, 7, 11, 13, 17};//, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181, 191, 193, 197, 199, 211, 223, 227, 229, 233, 239, 241, 251};
	unsigned char numPrimes = sizeof(PRIMES);
	unsigned char prime = 1;
	unsigned char primeIndex = randWithLimit(numPrimes, seed);
	prime = PRIMES[primeIndex];
	return prime;
}

//tested
unsigned char randWithLimit(unsigned char limit, unsigned int seed)
{
	unsigned char randomValue = 0;
	srand(seed);
	int randomInt = rand();
	randomValue = randomInt % limit;
	return randomValue;
}

//tested
unsigned char *power(unsigned char a, unsigned char n)
{
	unsigned char *b = (unsigned char*)malloc(sizeof(unsigned char)*n);
	unsigned char *c = (unsigned char*)malloc(sizeof(unsigned char)*n);
	for(int j = 0; j < n; j++)
	{
		*(b+j) = 0x00;
	}
	if(n > 1)
	{
		*b = a;
		for(int i = 2; i <= n; i++)
		{
			for(int j = 0; j < n; j++)
			{
				*(c+j) = 0x00;
			}
			//do byte-array byte multiplication
			for(int j = 0; j < i; j++)
			{
				unsigned int res = *(b+j)*a;
				if(res > 0)
				{
					unsigned char loRes = (unsigned char)res;
					unsigned char hiRes = (unsigned char)(res >> 8);
					if(loRes > 0)
					{
						unsigned int loResloC = loRes + *(c+j);
						for(int k = j+1; k <= i; k++)
						{
							unsigned char tempHi = (unsigned char)(loResloC >> 8);
							unsigned char tempLo = (unsigned char)(loResloC);	
							*(c+k-1) = tempLo;
							if(tempHi > 0)
							{
								loResloC = tempHi + *(c+k);
							}
							else
							{
								break;
							}
						}
					}
					if(hiRes > 0)
					{
						unsigned int hiReshiC = hiRes + *(c+j+1);
						for(int k = j+2; k <=i; k++)
						{
							unsigned char tempHi = (unsigned char)(hiReshiC >> 8);
							unsigned char tempLo = (unsigned char)(hiReshiC);
							*(c+k-1) = tempLo;
							if(tempHi > 0)
							{
								hiReshiC = tempHi + *(c+k);
							}
							else
							{
								break;
							}
						}
					}
				}
			}

			for(int j = 0; j < n; j++)
			{
				*(b+j) = *(c+j);
			}
		}
	}
	else if(n == 1)
	{
		*b = a;
	}
	else
	{
		*b = 1;
	}
	return b;
}
