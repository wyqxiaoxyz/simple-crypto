#ifndef _ECC_K1_PR_BASICS_DEFINED
#define _ECC_K1_PR_BASICS_DEFINED

#include <NTL/config.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <math.h>
#include <gmp.h>
#include <NTL/ZZ.h>
#include "uint256.h"
#include "include/secp256k1.h"
#include <openssl/bn.h>//needed for BN_hex2bn
#include <openssl/ec.h>


void Espace(void);
int Bitcoin_ECC_GMP_Tests(void);
int UCLECCLab1_BIGNUM_Tests(void);
int test_pointer(void);
int test_NTL(void);
int test_int(void);


#endif
