#include "ecc_k1_projects_basics.h"

void Espace(void)	/* waits until SPACE key is pressed.*/
/* in borland does not work in win32s programs ????*/
{
	printf("\nPress Space : ");
	#ifdef _MSC_VER
//		while(getch()!=32);//This POSIX function is deprecated. Use the ISO C++ conformant _getch instead.
		while(_getch()!=32);
	#else
		while(getchar()!=32);
	#endif
	printf("continuing.\n");
}


bool IsZero(mpz_t x)
{
	return mpz_sgn(x)==0;
};
bool IsOdd(mpz_t x)
{
	return mpz_odd_p(x);
};
bool IsEven(mpz_t x)
{
	return mpz_even_p(x);
};
bool IsEqual(mpz_t x,mpz_t y)
{
	return !mpz_cmp(x,y);
};
bool IsEqualMod(mpz_t x,mpz_t y,mpz_t p)
{
	mpz_t yy;mpz_init(yy);
	mpz_sub (yy, x, y);
	mpz_mod (yy, yy, p);
	int retval=0;
	if(IsZero(yy))
		retval=1;
	else
		retval=0;
	mpz_clear(yy);
	return retval;
};
bool IsEqualMod_ui(mpz_t x,unsigned int y,mpz_t p)
{
	mpz_t yy;mpz_init(yy);
	mpz_sub_ui (yy, x, y);
	mpz_mod (yy, yy, p);
	int retval=0;
	if(IsZero(yy))
		retval=1;
	else
		retval=0;
	mpz_clear(yy);
	return retval;
};

void MulMod(mpz_t m,mpz_t x,mpz_t y,mpz_t p)
{
	mpz_t yy;mpz_init(yy);
	mpz_mul(m ,x, y);
	mpz_mod (m, m, p);
	mpz_clear(yy);
};
void AddMod(mpz_t s,mpz_t x,mpz_t y,mpz_t p)
{
	mpz_add(s ,x, y);
	mpz_mod (s, s, p);
};
//x-y
void SubMod(mpz_t s,mpz_t x,mpz_t y,mpz_t p)
{
	mpz_sub(s ,x, y);
	mpz_mod (s, s, p);
};



//0,0 is also accepted
//does nothing unless it is incorrect
//y^2=x^3+Ax+B mod P 
int CheckECCEQuation(mpz_t &x,mpz_t &y,  mpz_t &A,mpz_t &B,mpz_t &P)
{
	if(IsEqualMod_ui(x,0,P)&&IsEqualMod_ui(y,0,P))
		return 1;
	//y^2=x^3+Ax+B mod P 
	mpz_t Left;mpz_init(Left);		MulMod(Left,y,y,P);
	mpz_t xx;mpz_init(xx);			MulMod(xx,x,x,P);
	mpz_t Ax;mpz_init(Ax);			MulMod(Ax,A,x,P);
	mpz_t Right1;mpz_init(Right1);	MulMod(Right1,xx,x,P);
	mpz_t Right2;mpz_init(Right2);	AddMod(Right2,Ax,B,P);
	mpz_t Right;mpz_init(Right);	AddMod(Right,Right1,Right2,P);
	mpz_clear(xx);mpz_clear(Ax);
	mpz_clear(Right1);mpz_clear(Right2);
	if(IsEqualMod(Left,Right,P))
	{
		mpz_clear(Left);mpz_clear(Right);
		return 1;
	}
	else
	{
		mpz_clear(Left);mpz_clear(Right);
		//printf("Not on the curve!\n");
		return 0;
	};
}

void to_mpz_t_FromHex(mpz_t &x,char *string)
{
	int flag = mpz_set_str(x, string, 16);
};

void DecodeHexPrependZeros(char * Buffer,mpz_t X,int EntropyBytes)
{
	int Len=mpz_sizeinbase(X,256);	
	char Format[10]={0};
	sprintf(Format,"%%0%dZx\0",2*EntropyBytes);
	gmp_sprintf (Buffer,Format, X);	
};


void ShowHex(mpz_t X)
{
	int Len=mpz_sizeinbase(X,256);	
	gmp_printf ("%Zx\0", X);	
};
void ShowHex(char *dest,mpz_t X)
{
	int Len=mpz_sizeinbase(X,256);	
	gmp_sprintf (dest,"%Zx\0", X);	
};





#undef ASSERT



//by Robert Ostling robert at robos.org 
/* mpz_sqrtm -- modular square roots using Shanks-Tonelli
taken from GNU MP Library.*/
#include "gmp.h"
#include "gmp-impl.h"

/* Solve the modular equatioon x^2 = n (mod p) using the Shanks-Tonelli
 * algorihm. x will be placed in q and 1 returned if the algorithm is
 * successful. Otherwise 0 is returned (currently in case n is not a quadratic
 * residue mod p). A check is done if p = 3 (mod 4), in which case the root is
 * calculated as n ^ ((p+1) / 4) (mod p).
 *
 * Note that currently mpz_legendre is called to make sure that n really is a
 * quadratic residue. The check can be skipped, at the price of going into an
 * eternal loop if called with a non-residue.
 */
int mpz_sqrtm(mpz_t q, const mpz_t n, const mpz_t p) {
    mpz_t w, n_inv, y;
    unsigned int i, s;
    TMP_DECL;
    TMP_MARK;

    if(mpz_divisible_p(n, p)) {         /* Is n a multiple of p?            */
        mpz_set_ui(q, 0);               /* Yes, then the square root is 0.  */
        return 1;                       /* (special case, not caught        */
    }                                   /* otherwise)                       */
    if(mpz_legendre(n, p) != 1)         /* Not a quadratic residue?         */
        return 0;                       /* No, so return error              */
    if(mpz_tstbit(p, 1) == 1) {         /* p = 3 (mod 4) ?                  */
        mpz_set(q, p);
        mpz_add_ui(q, q, 1);
        mpz_fdiv_q_2exp(q, q, 2);
        mpz_powm(q, n, q, p);           /* q = n ^ ((p+1) / 4) (mod p)      */
        return 1;
    }
    MPZ_TMP_INIT(y, 2*SIZ(p));
    MPZ_TMP_INIT(w, 2*SIZ(p));
    MPZ_TMP_INIT(n_inv, 2*SIZ(p));
    mpz_set(q, p);
    mpz_sub_ui(q, q, 1);                /* q = p-1                          */
    s = 0;                              /* Factor out 2^s from q            */
    while(mpz_tstbit(q, s) == 0) s++;
    mpz_fdiv_q_2exp(q, q, s);           /* q = q / 2^s                      */
    mpz_set_ui(w, 2);                   /* Search for a non-residue mod p   */
    while(mpz_legendre(w, p) != -1)     /* by picking the first w such that */
        mpz_add_ui(w, w, 1);            /* (w/p) is -1                      */
    mpz_powm(w, w, q, p);               /* w = w^q (mod p)                  */
    mpz_add_ui(q, q, 1);
    mpz_fdiv_q_2exp(q, q, 1);           /* q = (q+1) / 2                    */
    mpz_powm(q, n, q, p);               /* q = n^q (mod p)                  */
    mpz_invert(n_inv, n, p);
    for(;;) {
        mpz_powm_ui(y, q, 2, p);        /* y = q^2 (mod p)                  */
        mpz_mul(y, y, n_inv);
        mpz_mod(y, y, p);               /* y = y * n^-1 (mod p)             */
        i = 0;
        while(mpz_cmp_ui(y, 1) != 0) {
            i++;
            mpz_powm_ui(y, y, 2, p);    /*  y = y ^ 2 (mod p)               */
        }
        if(i == 0) {                    /* q^2 * n^-1 = 1 (mod p), return   */
            TMP_FREE;
            return 1;
        }
        if(s-i == 1) {                  /* In case the exponent to w is 1,  */
            mpz_mul(q, q, w);           /* Don't bother exponentiating      */
        } else {
            mpz_powm_ui(y, w, 1 << (s-i-1), p);
            mpz_mul(q, q, y);
        }
        mpz_mod(q, q, p);               /* r = r * w^(2^(s-i-1)) (mod p)    */
    }
}




void GMPSqrRootMod(mpz_t &r,mpz_t &s,const mpz_t &p)
{
	mpz_sqrtm(r,s,p);
	if(0)
	{
		mpz_t rr;mpz_init(rr);
		mpz_t yy;mpz_init(yy);
		mpz_mul(yy, r, r);
		mpz_mod (yy, yy, p);
		///INCORRECT to use != in GMP!!!!! if(yy!=s)
		if(mpz_cmp(yy,s))
		    if(mpz_legendre(s, p) == 1)
				printf("GMPSqrRootMod fails, Jacobi=+1\n");
		mpz_clear(rr);
		mpz_clear(yy);
	};
};

int ECCRecoverY(mpz_t &x,mpz_t &y,	mpz_t &A,mpz_t &B,mpz_t &P,int reporterrorsonscreen)
{
	//y^2=x^3+Ax+B mod P 
	//mpz_t Left;mpz_init(Left);		MulMod(Left,y,y,P);
	mpz_t xx;mpz_init(xx);			MulMod(xx,x,x,P);
	mpz_t Ax;mpz_init(Ax);			MulMod(Ax,A,x,P);
	mpz_t Right1;mpz_init(Right1);	MulMod(Right1,xx,x,P);
	mpz_t Right2;mpz_init(Right2);	AddMod(Right2,Ax,B,P);
	mpz_t Right;mpz_init(Right);	AddMod(Right,Right1,Right2,P);
	mpz_clear(xx);mpz_clear(Ax);
	mpz_clear(Right1);mpz_clear(Right2);

    if(mpz_legendre(Right, P) != 1)
	{
		if(reporterrorsonscreen) 
		printf("Not a QR, cannot extract a square root\n");
		mpz_clear(Right);
		return 0;
	};

	GMPSqrRootMod(y,Right,P);
	
	//if wrong result, what to do?
	mpz_t yy;mpz_init(yy);
	MulMod(yy,y,y,P);
	if(!IsEqual(yy,Right))
	{
		if(mpz_legendre(Right, P)!=1)
			if(reporterrorsonscreen) 
			printf("Not a QR\n");
		printf("a=\n");
		//Display(Right));
		printf("p=\n");
		//Display(P));
		printf("result=???\n");
		//Display(y));
		printf("result^2=\n");
		//Display(yy));
		printf("SqrRootMod(a) mod p failed, maybe not a square?\n");
	};

	//in case x=0, it is 0 
	//this would mean that I cannot reliably represent the point at infinity by 0,0
	if(IsEqualMod_ui(x,0,P))
	{
		if(!IsZero(y))
			printf("The curve has a point sth,0, so I cannot reliably use 0,0 because 3 solutions for x=0\n");
		mpz_set_ui(y,0);
	};
	if(0==CheckECCEQuation(x,y,A,B,P))
	{
		printf("not on the curve after recovery\n");
	};
	mpz_clear(Right);
	mpz_clear(yy);
	return 1;
};


//DER encoding 
//Publickeys (in scripts) are given as 04<x><y>
//where x and y are 32byte big-endian integers representing the
//coordinates of a point on the curve or in compressed form given
//as <sign><x> where <sign> is
//0x02 if y is even and
//0x03 if y is odd.
//source: https://en.bitcoin.it/wiki/Protocol_documentation
int ConvertPK66To130GMP(char *PK130,char *PK66,int reporterrors)
{
	if(strlen(PK66)==130 && PK66[1]=='4')
	{
		strncpy(PK130,PK66,2+64+64);
		PK130[2+64+64]=0;
		return 1;
	};

	//bitcoin ECC
	mpz_t P;mpz_init(P);
	int flag = mpz_set_str(P,"115792089237316195423570985008687907853269984665640564039457584007908834671663", 10);
	mpz_t A;mpz_init(A);mpz_set_str(A,"0",10);
	mpz_t B;mpz_init(B);mpz_set_str(B,"7",10);
	
	//from r1 mod p compute the point R by solving the equation of the curve
	char PK64Copy[65]={0};
	strncpy(PK64Copy,PK66+2,64);
	mpz_t Rx;mpz_init(Rx);
	mpz_t Ry;mpz_init(Ry);
	to_mpz_t_FromHex(Rx,PK64Copy);
	int qrok=
		ECCRecoverY(Rx,Ry,	A,B,P,0);
	if(PK66[1]=='2')//y even
	{
		if(IsOdd(Ry))
			mpz_sub(Ry,P,Ry);
	}
	else//y odd
	{
		if(!IsOdd(Ry))
			mpz_sub(Ry,P,Ry);
	};
	if(0)
	{
		ShowHex(Rx);printf("\n");
		ShowHex(Ry);printf("\n");
	};
	int result=0;
	if(qrok)
		result=CheckECCEQuation(Rx,Ry,A,B,P);
	if(qrok==0 || result==0)
	{
		//Not a QR, cannot extract a square root542e54b8020e53f9458e680e07 288171 467
		//Invalid public key, y does not exist
		//Not a QR, cannot extract a square root780fd823d1a4567d9902f12af2 218353 34
		//Invalid public key, y does not exist
		if(reporterrors) 
			printf("Invalid public key, y does not exist\n%s\n",PK66);
		mpz_clear(P);mpz_clear(A);mpz_clear(B);mpz_clear(Rx);mpz_clear(Ry);
		return 0;
	}
	else
	{
		PK130[0]='0';
		PK130[1]='4';
		char LookHere[200]={0};
		DecodeHexPrependZeros(LookHere,Rx,32);//also x to have consistent capital letter version!
		strncpy(PK130+2,LookHere,64);
		DecodeHexPrependZeros(LookHere,Ry,32);
		strncpy(PK130+2+64,LookHere,64);
		PK130[2+64+64]=0;
		mpz_clear(P);mpz_clear(A);mpz_clear(B);mpz_clear(Rx);mpz_clear(Ry);
		return 1;
	};
	mpz_clear(P);mpz_clear(A);mpz_clear(B);mpz_clear(Rx);mpz_clear(Ry);
	return 130;
};
int ConvertPK66To130GMP(char *PK130,char *PK66)
{
	return ConvertPK66To130GMP(PK130,PK66,1);
};





//normal Weierstrass form point doubling with slope
//point at infinity = 0,0
void PointDoubling(mpz_t &Rx,mpz_t &Ry,	mpz_t &Px,mpz_t &Py,	mpz_t &ECa,mpz_t &ECb,mpz_t &ECp)
{
	mpz_t slope,temp;
	mpz_init(temp);
	mpz_init(slope);
	if(mpz_cmp_ui(Py,0)!=0)
	{
		mpz_t Tx,Ty;mpz_init(Tx);mpz_init(Ty);
		mpz_mul_ui(temp,Py,2);
		mpz_invert(temp,temp,ECp);
		mpz_mul(slope,Px,Px);
		mpz_mul_ui(slope,slope,3);
		mpz_add(slope,slope,ECa);
		mpz_mul(slope,slope,temp);
		mpz_mod(slope,slope,ECp);
		mpz_mul(Tx,slope,slope);
		mpz_sub(Tx,Tx,Px);
		mpz_sub(Tx,Tx,Px);
		mpz_mod(Tx,Tx,ECp);
		mpz_sub(temp,Px,Tx);
		mpz_mul(Ty,slope,temp);
		mpz_sub(Ty,Ty,Py);
		mpz_mod(Ry,Ty,ECp);
		mpz_set(Rx,Tx);
		mpz_clear(Tx);mpz_clear(Ty);
	}
	else
	{
		mpz_set_ui(Rx,0);
		mpz_set_ui(Ry,0);
	}
	mpz_clear(temp);mpz_clear(slope);
}

//normal Weierstrass form point addition/doubling with slope
//point at infinity = 0,0
void AddECC(mpz_t &Rx,mpz_t &Ry,	mpz_t &Px,mpz_t &Py,mpz_t &Qx,mpz_t &Qy,	mpz_t &ECa,mpz_t &ECb,mpz_t &ECp)
{
	//mpz_mod(Px,Px,ECp);
	//mpz_mod(Py,Py,ECp);
	//mpz_mod(Qx,Qx,ECp);
	//mpz_mod(Qy,Qy,ECp);
	if(mpz_cmp_ui(Px,0)==0 &&
		mpz_cmp_ui(Py,0)==0)
	{ 
		mpz_set(Rx,Qx); mpz_set(Ry,Qy); 
		return;
	};
	if(mpz_cmp_ui(Qx,0)==0 &&
		mpz_cmp_ui(Qy,0)==0)
	{ 
		mpz_set(Rx,Px); mpz_set(Ry,Py);
		return;
	}
	mpz_t temp,slope;
	mpz_init(temp);
	mpz_init_set_ui(slope,0);
	if(mpz_cmp_ui(Qy,0)!=0)
	{
		mpz_sub(temp,ECp,Qy);
		mpz_mod(temp,temp,ECp);
	}
	else
	{
		mpz_set_ui(temp,0);
	};
	// gmp_printf("\n temp=%Zd\n",temp);
	if(mpz_cmp(Py,temp)==0 &&
		mpz_cmp(Px,Qx)==0)
	{ 
		mpz_set_ui(Rx,0); 
		mpz_set_ui(Ry,0); 
		mpz_clear(temp);mpz_clear(slope);
		return;
	}
	if(mpz_cmp(Px,Qx)==0 &&
		mpz_cmp(Py,Qy)==0)
	{
		PointDoubling(Rx,Ry,Px,Py,ECa,ECb,ECp);
		mpz_clear(temp);mpz_clear(slope);
		return;
	}
	else
	{
		mpz_t Tx,Ty;mpz_init(Tx);mpz_init(Ty);
		mpz_sub(temp,Px,Qx);
		mpz_mod(temp,temp,ECp);
		mpz_invert(temp,temp,ECp);
		mpz_sub(slope,Py,Qy);
		mpz_mul(slope,slope,temp);
		mpz_mod(slope,slope,ECp);
		mpz_mul(Tx,slope,slope);
		mpz_sub(Tx,Tx,Px);
		mpz_sub(Tx,Tx,Qx);
		mpz_mod(Tx,Tx,ECp);
		mpz_sub(temp,Px,Tx);
		mpz_mul(Ty,slope,temp);
		mpz_sub(Ty,Ty,Py);
		mpz_mod(Ry,Ty,ECp);
		mpz_set(Rx,Tx);
		mpz_clear(Tx);mpz_clear(Ty);
		mpz_clear(temp);mpz_clear(slope);
		return;
	}
}


//multiply a point on an elliptic curve by any large integer K
//y^2=x^3+Ax+B mod P 
void ScalarMultECC(mpz_t &x8,mpz_t &y8,	mpz_t &x1,mpz_t &y1,
				   mpz_t &A,mpz_t &B,mpz_t &P,
				   mpz_t &K)
{
	//square and multiply
	mpz_t x2,y2;mpz_init(x2);mpz_init(y2);
	mpz_init_set_ui(x2,0);	
	mpz_init_set_ui(y2,0);
	//accumulator initialized at the point at infinity
	
	mpz_t Px,Py;mpz_init(Px);mpz_init(Py);
	mpz_set(Px,x1);	
	mpz_set(Py,y1);
	//Display(Px));Display(Py));
	
	mpz_t M;mpz_init(M);
	mpz_set(M,K);
	while(!IsZero(M))
	{
		if(IsOdd(M))
		{
			//Y*=P
			AddECC(x2,y2,	x2,y2,Px,Py,	A,B,P);
			//Display(x2));Display(y2));
		};
		//P:=P^2
		AddECC(Px,Py,	Px,Py,Px,Py,	A,B,P);
		//Display(Px));Display(Py));
		mpz_tdiv_q_2exp(M,M,1);//divides by two, like a right shift >>=1
		//Display(M));
	};
	mpz_set(x8,x2);	mpz_set(y8,y2);
	mpz_clear(x2);mpz_clear(y2);
	mpz_clear(Px);mpz_clear(Py);
	mpz_clear(M);
}






//some test computations with the bitcoin elliptic curve written by NCourois 
int Bitcoin_ECC_GMP_Tests(void)
{
	mpz_t P;mpz_init(P);
	int flag = mpz_set_str(P,"115792089237316195423570985008687907853269984665640564039457584007908834671663", 10);
	mpz_t A;mpz_init(A);mpz_set_str(A,"0",10);
	mpz_t B;mpz_init(B);mpz_set_str(B,"7",10);

	//base point 
	//The base point G (generator) is: (could be any element)
	//in compressed form is: 
	//G = 02 79BE667E F9DCBBAC 55A06295 CE870B07 029BFCDB 2DCE28D9 59F2815B 16F81798
	//In uncompressed form it is: G = 04 
	//	79BE667E F9DCBBAC 55A06295 CE870B07 029BFCDB 2DCE28D9 59F2815B 16F81798
	//	483ADA77 26A3C465 5DA4FBFC 0E1108A8 FD17B448 A6855419 9C47D08F FB10D4B8
	//or simply x,y are
	//x=55066263022277343669578718895168534326250603453777594175500187360389116729240
	//y=32670510020758816978083085130507043184471273380659243275938904335757337482424
	mpz_t Gx;mpz_init(Gx);
	flag = mpz_set_str(Gx,"55066263022277343669578718895168534326250603453777594175500187360389116729240", 10);
	mpz_t Gy;mpz_init(Gy);
	flag = mpz_set_str(Gy,"32670510020758816978083085130507043184471273380659243275938904335757337482424", 10);
	CheckECCEQuation(Gx,Gy,A,B,P);

	//recover y for x (two solutions!)
	mpz_t Gy2;mpz_init(Gy2);
	ECCRecoverY(Gx,Gy2	,A,B,P,0);
	mpz_t MGy2;mpz_init(MGy2);SubMod(MGy2,P,Gy2,P);//MGy2=-Gy2, the second solution
	if(!IsEqualMod(Gy,Gy2,P) && !IsEqualMod(Gy,MGy2,P))
		printf("Failed REcoverY");

	//add twice the base point
	mpz_t Hx;mpz_init(Hx);	mpz_t Hy;mpz_init(Hy);
	AddECC(Hx,Hy,	Gx,Gy,Gx,Gy,	A,B,P);
	//Display(Hx));Display(Hy));
//should be
//89565891926547004231252920425935692360644145829622209833684329913297188986597
//12158399299693830322967808612713398636155367887041628176798871954788371653930
	mpz_t Hx2;mpz_init(Hx2);
	mpz_set_str(Hx2,"89565891926547004231252920425935692360644145829622209833684329913297188986597", 10);
	if(!IsEqualMod(Hx,Hx2,P))
		printf("Failed AddECC");

	CheckECCEQuation(Hx,Hy,A,B,P);

	//2x the base point
	mpz_t Zero;mpz_init(Zero);mpz_set_ui(Zero,0);
	mpz_t Two;mpz_init(Two);mpz_set_ui(Two,2);
	ScalarMultECC(Hx,Hy,Gx,Gy, A,B,P, Two);
	//Display(Hx));Display(Hy));
	CheckECCEQuation(Hx,Hy,A,B,P);

	//group order 
	mpz_t Q;mpz_init(Q);
	mpz_set_str(Q,"115792089237316195423570985008687907852837564279074904382605163141518161494337",10);

	//uses AddECC
	mpz_t Rx,Ry;mpz_init(Rx);mpz_init(Ry);
	ScalarMultECC(Rx,Ry,Gx,Gy, A,B,P, Q);
	//Display(Rx));Display(Ry));
	CheckECCEQuation(Rx,Ry,A,B,P);
	//check if R is the point at infinity
	if(!IsEqualMod(Rx,Zero,P)||!IsEqualMod(Ry,Zero,P))
	{
		//Display(Rx);Display(Ry);
		printf("Wrong Result");
	}
	else
		printf("Bitcoin ECC scalar mult. test passed\n");

	mpz_clear(A);mpz_clear(B);
	mpz_clear(Q);
	mpz_clear(Gx);mpz_clear(Gy);
	mpz_clear(Hx);mpz_clear(Hy);mpz_clear(Hx2);
	mpz_clear(Rx);mpz_clear(Ry);
	mpz_clear(P);mpz_clear(Two);mpz_clear(Zero);

	Espace();
	return 1;
};







