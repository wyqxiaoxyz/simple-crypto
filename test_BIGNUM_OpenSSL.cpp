//developed by Jeremiah Onaolapo and Nicolas Courtois in March 2015
#include "ecc_k1_projects_basics.h" 

#include <time.h>
#include <windows.h> 
//#include <WinSock2.h>//for struct timeval
//Windows port of gettimeofday copied from: http://www.openasthra.com/wp-content/uploads/gettimeofday.c
//The gettimeofday() function obtains the current time, expressed as seconds and microseconds since the Epoch, and store it in the timeval structure pointed to by tv. As posix says gettimeoday should return zero and should not reserve any value for error, this function returns zero. Here is the program, I?ve given definition struct timezeone and for others I didn?t give as all other data types definitions are available in windows include files itself.
#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
  #define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
  #define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif

inline void swap(char& a, char& b)  {  char tt;  tt = a; a = b; b = tt;}
typedef int s32;
int LookBufferBytes=4*1024*1024;//default, can be resized  later !
char *LookBuffer=(char *)malloc(LookBufferBytes);
char *Look(NTL::ZZ X)
{
	s32 Len=0;int negative=0;
	if(X<0)
	{
		X=-X;
		negative=1;
	};
	while(X!=0)
	{
		sprintf(LookBuffer+Len,"%d",X%10);
		X/=10;
		Len++;
		if((int)Len>LookBufferBytes-2)
		{
			LookBufferBytes=LookBufferBytes+LookBufferBytes/2;
			LookBuffer=(char *)realloc(LookBuffer,LookBufferBytes);
			if(LookBuffer==NULL)
				printf("realloc failed in Look(NTL::ZZ X)\n");
		};
	};
	if(negative)
	{
		sprintf(LookBuffer+Len,"-");
		Len++;
	};
	if(Len==0) {Len=1;LookBuffer[0]='0';LookBuffer[1]=0;};
	/*mirror*/
	for(s32 i=0;i<=(Len-1)/2;i++)
		swap(LookBuffer[i],LookBuffer[Len-i-1]);
	LookBuffer[Len]=0;
	return LookBuffer;
}

NTL::ZZ ntl_ZZ_from_mpz(const mpz_t &a)
{
	size_t countp=0;
	countp=gmp_sprintf(LookBuffer,"%Zd\0", a);	
	LookBuffer[countp]=0;
	return NTL::to_ZZ(LookBuffer);
};

//temporary slow version works through conversion basis 2^16
void ntl_ZZ_to_mpz(mpz_t &x, const NTL::ZZ &X)
{
	Look(X);
	int flag = mpz_set_str(x,LookBuffer, 10);	
};

NTL::ZZ to_ZZ(const mpz_t &a)
{
	return ntl_ZZ_from_mpz(a);
};

void DivideBy2(NTL::ZZ& e)
{
	NTL::ZZ f;
	//Display(e));
	divide(f,e,2);
	NTL::ZZ test=e-f-f;
	if(!IsZero(test) && !IsOne(test))
	{
		//printf("DivideBy2 failed, due to bug in NTL, wrong assumption that the number of bits per limb_t is at least...");
		//here is the current super slow work-around....
		mpz_t E;	mpz_init(E);
		ntl_ZZ_to_mpz(E,e);
		mpz_tdiv_q_2exp(E,E,1);
		e=to_ZZ(E);
		mpz_clear(E);
	}
	else
	{
		e=f;
	};
	//Display(e));
};


//rewritten by NCourtois, the original one is incorrect with NTL+MPIR+Visual Studio 10.0!
//plain square-and-multiply
NTL::ZZ PowerMod2(const NTL::ZZ& a,const NTL::ZZ& ee, const NTL::ZZ& n)
{
  NTL::ZZ x, y;

  NTL::ZZ e;
  if (ee < 0)
     e = - ee;
  else
     e = ee;

  x = 1;
  y = a;
  while (!IsZero(e))
  {
     if (IsOdd(e))
		  x = MulMod(x, y, n);
     y = MulMod(y, y, n);
	//e = e >> 1;
	  DivideBy2(e);
  }

  if (ee < 0) x = InvMod(x, n);

  return x;
}

struct timezone 
{
  int  tz_minuteswest; /* minutes W of Greenwich */
  int  tz_dsttime;     /* type of dst correction */
};
 
int gettimeofday(struct timeval *tv, struct timezone *tz)
{
  FILETIME ft;
  unsigned __int64 tmpres = 0;
  static int tzflag;
 
  if (NULL != tv)
  {
    GetSystemTimeAsFileTime(&ft);
 
    tmpres |= ft.dwHighDateTime;
    tmpres <<= 32;
    tmpres |= ft.dwLowDateTime;
 
    /*converting file time to unix epoch*/
    tmpres -= DELTA_EPOCH_IN_MICROSECS; 
    tmpres /= 10;  /*convert into microseconds*/
    tv->tv_sec = (long)(tmpres / 1000000UL);
    tv->tv_usec = (long)(tmpres % 1000000UL);
  }
 
  if (NULL != tz)
  {
    if (!tzflag)
    {
      _tzset();
      tzflag++;
    }
    tz->tz_minuteswest = _timezone / 60;
    tz->tz_dsttime = _daylight;
  }
 
  return 0;
}
//Usage
// Code Block
//gettimeofday(&now, NULL); 
//gettimeofday(&now, &tzone); 

#define NUMBER_OF_TIMES 5000000 // For benchmarking
static struct timeval tStart, tEnd;
static int tDur;
// CLOCK CLOCK, WHO GOES THERE?
void startClock() 
{
	gettimeofday(&tStart, NULL);
}
void stopClock(std::string description, int numoftimes) 
{
	gettimeofday(&tEnd, NULL);
	tDur = (tEnd.tv_sec - tStart.tv_sec)*1000000 + (tEnd.tv_usec - tStart.tv_usec);
	double dur = (float)tDur/numoftimes; 
	printf("%s ran in %.4f us avg.\n", description.c_str(), dur);
}//*/




//UCL GA12 lab examples using OpenSSL and BIGNUM
//developed by Jeremiah Onaolapo and Nicolas Courtois in March 2015
#include <openssl/bn.h>//needed for BN_hex2bn
#include <openssl/ec.h>
#include <openssl/ecdsa.h>//needed for ECDSA_verify
#include <iostream>//for cout 
using namespace std;//to avoid writing std::string or std::cout
int UCLECCLab1_BIGNUM_Tests(void)
{
	// PARAMETERS INIT
	cout << "PARAMETERS INIT" << endl;
	// Secp256k1 curve parameters obtained from:
	// https://en.bitcoin.it/wiki/Secp256k1
	BIGNUM *p, *a, *b, *n, *Gx, *Gy, *three, *two, *one, *zero; // Declaration
	p = BN_new(); // initialization of large prime p
	a = BN_new(); // initialization of coefficient a
	b = BN_new(); // initialization of coefficient b
	n = BN_new(); // initialization of order n
	Gx = BN_new(); // initialization of Gx 
	Gy = BN_new(); // initialization of Gy
	three = BN_new();
	two = BN_new();
	one = BN_new();
	zero = BN_new();

 	// ASSIGNMENT
	cout << "ASSIGNMENT" << endl;
	// Assignment of values
	BN_dec2bn(&p, "115792089237316195423570985008687907853269984665640564039457584007908834671663");
	//in hex BN_hex2bn(&p,"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F");
	BN_dec2bn(&a, "0");
	BN_dec2bn(&b, "07");
	BN_dec2bn(&n, "115792089237316195423570985008687907852837564279074904382605163141518161494337");

	// Gx in hex = "79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798"
	// Gy in hex = "483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8";
	BN_dec2bn(&Gx, "55066263022277343669578718895168534326250603453777594175500187360389116729240");
	BN_dec2bn(&Gy, "32670510020758816978083085130507043184471273380659243275938904335757337482424");
	BN_dec2bn(&three, "3");
	BN_dec2bn(&two, "2");
	BN_dec2bn(&one, "1");
	BN_dec2bn(&zero, "0");

	// PRINTING
	char *pdec = BN_bn2dec(p);
	string strdec(pdec);
	cout << "p = " << strdec.data() << endl;

	char *phex = BN_bn2hex(p);
	string strhex(phex);
	cout << "p = " << strhex.data() << endl;
	 
	char *ndec = BN_bn2dec(n);
	string strndec(ndec);
	cout << "n = " << strndec.data() << endl;

	char *nhex = BN_bn2hex(n);
	string strnhex(nhex);
	cout << "n = " << strnhex.data() << endl;

	char *Gxhex = BN_bn2hex(Gx);
	string strgxhex(Gxhex);
	cout << "Gx = " << strgxhex.data() << endl;

	char *Gyhex = BN_bn2hex(Gy);
	string strgyhex(Gyhex);
	cout << "Gy = " << strgyhex.data() << endl;

	// POINT VERIFICATION
	cout << "POINT VERIFICATION" << endl;
	// Curve: y^2 = x^3 + b >>> So y^2 - x^3 - b = 0 
	// Check to see if a point is on curve
	BIGNUM *Tx, *Ty, *Txcubed, *Tysqrd, *bmodp, *lhs;
	Tx = BN_new();
	Ty = BN_new();
	Txcubed = BN_new(); 
	Tysqrd = BN_new();
	lhs = BN_new();
	bmodp = BN_new();
	
	// EXERCISE
	cout << "EXERCISE" << endl;
	BN_copy(Tx, Gx); // Tx = Gx 
	BN_copy(Ty, Gy); // Ty = Gy

	BN_CTX *ctx = BN_CTX_new();
	BN_mod_sqr(Tysqrd, Ty, p, ctx); // y^2 mod p
	BN_mod_exp(Txcubed, Tx, three, p, ctx); // x^3 mod p
	BN_mod(bmodp, b, p, ctx); // b mod p
	
	BN_sub(lhs, Tysqrd, Txcubed); // lhs = Tysqrd - Txcubed
	BN_sub(lhs, lhs, bmodp); // lhs = lhs - bmodp
	BN_mod(lhs, lhs, p, ctx); // b mod p

	// If point {Tx, Ty} is on curve, lhs will be equal to zero
	char *result = BN_bn2dec(lhs);
	string strlhs(result);
	cout << "strlhs = " << strlhs.data() << endl;

	// IS POINT ON CURVE?
	cout << "IS POINT ON CURVE?" << endl;
	if (BN_is_zero(lhs) == 1) {
		cout << "Point {Tx, Ty} is ON curve" << endl;
	}
	else {
		cout << "Point {Tx, Ty} is NOT on curve!" << endl;
	}

	// Another comparison
	if (BN_cmp(lhs, zero) == 0) {
		cout << "Point {Tx, Ty} is ON curve" << endl;
	}
	else {
		cout << "Point {Tx, Ty} is NOT on curve!" << endl;
	}

	// BENCHMARKING
	cout << "BENCHMARKING" << endl;
	startClock();
	for (int i=0; i < NUMBER_OF_TIMES; i++) {
		BN_mul(lhs, Gx, Gy, ctx);
	}
	stopClock("mul",NUMBER_OF_TIMES);	

	startClock();
	for (int i=0; i < NUMBER_OF_TIMES; i++) {
		BN_mod_mul(lhs, Gx, Gy, p, ctx);
	}
	stopClock("mod_mul", NUMBER_OF_TIMES);	

	// COMPUTE AND VERIFY 2G
	cout << "COMPUTE AND VERIFY 2G" << endl;
	// Point doubling on G to get 2G
	BIGNUM *twoGx = BN_new(); // x-coordinate of 2G
	BIGNUM *twoGy = BN_new(); // y-coordinate of 2G
	BIGNUM *slopenum = BN_new();
	BIGNUM *slopeden = BN_new();
	BIGNUM *s1 = BN_new();
	BIGNUM *s2 = BN_new();
	BIGNUM *s2inv = BN_new();
	BIGNUM *slope = BN_new();

	// To calculate slope
	BN_sqr(slopenum, Gx, ctx); // slopenum = Gx^2
	BN_mul(slopenum, slopenum, three, ctx); // slopenum = 3*slopenum
	BN_add(slopenum, slopenum, a); // slopenum = slopenum + a
	BN_mod(s1, slopenum, p, ctx); // s1 = slopenum mod p	
	
	BN_mul(slopeden, Gy, two, ctx); // slopeden = 2*Gy
	BN_mod(s2, slopeden, p, ctx); // s2 = slopeden mod p
	BN_mod_inverse(s2inv, s2, p, ctx); // s2inv = s2^-1 mod p

	BN_mod_mul(slope, s1, s2inv, p, ctx); // slope = s1*s2inv, that is, slope = s1/s2

	BIGNUM *slopesq = BN_new();
	BIGNUM *tmp = BN_new();
	BN_sqr(slopesq, slope, ctx); // slopesq = slope^2
	BN_mul(tmp, Gx, two, ctx); // tmp = 2 * Gx
	BN_sub(tmp, slopesq, tmp); // tmp = slopesq - tmp
	BN_mod(Tx, tmp, p, ctx); // Tx = tmp mod p

	BN_sub(tmp, Gx, Tx); // tmp = Gx - Tx
	BN_mul(Ty, slope, tmp, ctx); // Ty = slope*tmp
	BN_sub(Ty, Ty, Gy); // Ty = Ty - Gy
	BN_copy(tmp, Ty); // tmp = Ty
	BN_mod(Ty, tmp, p, ctx); // Ty = tmp mod p

	// {Tx, Ty} is now 2G. Verify that it's on the curve
	BN_mod_sqr(Tysqrd, Ty, p, ctx); // y^2 mod p
	BN_mod_exp(Txcubed, Tx, three, p, ctx); // x^3 mod p
	BN_mod(bmodp, b, p, ctx); // b mod p
	
	BN_copy(lhs, Tysqrd);
	BN_sub(lhs, Tysqrd, Txcubed);
	BN_sub(lhs, lhs, bmodp);

	// If point {Tx, Ty} is on curve, lhs will be equal to zero
	if (BN_is_zero(lhs) == 1) {
		cout << "Point 2G = {Tx, Ty} is ON curve" << endl;
	}
	else {
		cout << "Point 2G = {Tx, Ty} is NOT on curve!" << endl;
	}

	// FREE MEMORY
	// Clear BIGNUMs after use, to free memory
	BN_free(slopeden); // to free up memory held by slopeden
	BN_clear(tmp); // to destroy sensitive data (overwrite with 0)
	BN_clear_free(slopenum); // does both
	BN_CTX_end(ctx);
	BN_CTX_free(ctx);

	Espace();
	return 1;	
}

int test_pointer(void) {
	int a, b; // Declare variables
	float c;
	// Initialize a, b and c	
	a = 13;  
	b = 17;
	c = 3.142; 
	int *pa;
	pa = &a; // Declare and initialize pointer pa
	// Now pointer pa contains the address of variable a
	int *pb; 
	pb = &b;

	float *pc;
	pc = &c; 

	int **ppa;
	ppa = &pa; // Pointer pointing to the pointer pointing to pa

	int ***pppa;
	pppa = &ppa;

	// Calculate d = a times b using pointers only
	int ab = *pa * *pb;
	int ab2 = **ppa * *pb;
	int ab3 = ***pppa * *pb;
	
	// e = a * c using pointers only
	float e = *pa * *pc;

	cout << "==Results==" << endl;
	cout << "a = " << a << " and b = " << b << " and c = " << c << endl;
	cout << "pa = " << pa << " and pb = " << pb << endl;
	cout << "c = " << c << " at location " << pc << endl;
	cout << "a * b" << " = " << a * b << endl;
	cout << "ab = *pa * *pb" << " = " << ab << endl;
	cout << "ab2 = **ppa * *pb" << " = " << ab2 << endl;
	cout << "ab3 = ***pppa * *pb" << " = " << ab3 << endl;
	cout << "a * c = " << a * c << endl;
	cout << "e = *pa * *pc" << " = " << e << endl;
	
	Espace();
	return 1;
}

#include <climits>

int test_int(void) {
	cout << "Max value of unsigned int: " << dec << UINT_MAX << " = 0x" << hex << UINT_MAX << endl;
	cout << "Max value of signed long long int: " << dec << LLONG_MAX << " = 0x" << hex << LLONG_MAX << endl;
	cout << "Max value of unsigned long long int: " << dec << ULLONG_MAX << " = 0x" << hex << ULLONG_MAX << endl;
	// More limits macros can be found at http://www.cplusplus.com/reference/climits/
	Espace();
	return 1;
}
using namespace NTL;
int test_NTL(void)
{
	//initial
   	ZZ a, b, c, p, n, Gx, Gy;
	//assign
   	p = conv<ZZ>("115792089237316195423570985008687907853269984665640564039457584007908834671663");
   	a = ZZ(0);
	b = ZZ(7);
	n = conv<ZZ>("115792089237316195423570985008687907852837564279074904382605163141518161494337");

	Gx = conv<ZZ>("55066263022277343669578718895168534326250603453777594175500187360389116729240");
	Gy = conv<ZZ>("32670510020758816978083085130507043184471273380659243275938904335757337482424");
	// check value
   	cout << p << endl;
	cout << a << endl;
	cout << b << endl;
	cout << Gx << endl;
	cout << Gy << endl;
	cout << n << endl;
	// check point G on the curve
	ZZ ty2, bmodp, xcube, res;
	ty2=(Gy*Gy)%p;	   	
	cout << ty2 << endl;
	
	ty2 = SqrMod(Gy,p);
   	cout << ty2 << endl;

	xcube = PowerMod2(Gx,to_ZZ(3),p);
	res = SubMod(ty2,xcube,p);
	res = SubMod(res,b,p);
	
	if (IsZero(res)) {
		cout << "Point G is on the curve" << endl;
	}else{
		cout << "Point is not on the curve" << endl;
	}
	Espace();
	return 1;
}