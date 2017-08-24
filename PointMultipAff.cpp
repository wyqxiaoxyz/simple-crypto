#include "PointMultipAff.h"
using namespace std;
#include <iostream>
#include <openssl/ec.h>
#include <openssl/bn.h>

#define BN_negate(x) ((x)->neg = (!((x)->neg)) & 1)

static BIGNUM *zero, *two;

PointMultipAff::PointMultipAff() {
	zero = BN_new();
	two = BN_new();
	BN_zero(zero);
	BN_set_word(two, 2);

}

/* This method performs addition on points p1 and p2, and stores result in pr */
bool PointMultipAff::addPoints(EC_POINT *pr, EC_POINT *p1, EC_POINT *p2, EC_GROUP *curve) {
	BN_CTX *ctx = BN_CTX_new();	
	EC_POINT_set_affine_coordinates_GFp(curve, pr, zero, zero, ctx);
	BIGNUM *p1x, *p1y, *p2x, *p2y, *negp1y;
	p1x = BN_new();
	p1y = BN_new();
	p2x = BN_new(); 
	p2y = BN_new();
	negp1y = BN_new();
	// Store x and y affine coordinates of p1 in p1x and p1y respectively
	EC_POINT_get_affine_coordinates_GFp(curve, p1, p1x, p1y, ctx);

	// Store x and y affine coordinates of p2 in p2x and p2y respectively
	EC_POINT_get_affine_coordinates_GFp(curve, p2, p2x, p2y, ctx);

	// negp1y = -p1y
	BN_copy(negp1y, p1y);
	BN_negate(negp1y);
	
	// p1 + p2 is at infinity
	if (BN_cmp(p2y, zero) == 0 || BN_cmp(p1y, zero) == 0 || (BN_cmp(p2x, p1x) == 0 && BN_cmp(p2y, negp1y) == 0)) {
		EC_POINT_set_to_infinity(curve, pr);
	}
	// if p1 == p2, pr = 2 x p1
	else if (BN_cmp(p1x, p2x) == 0 && BN_cmp(p1y, p2y) == 0){
		EC_POINT_dbl(curve, pr, p1, ctx); // doubles p1 and stores result in pr
	}
	// if one of the points is at infinity
	else if (EC_POINT_is_at_infinity(curve, p1) == 1) {
		// pr = O + p2 = p2. Result is stored in pr.
		EC_POINT_copy(pr, p2);
	}
	else if (EC_POINT_is_at_infinity(curve, p2) == 1) {
		// pr = p1 + O = p1. Result is stored in pr.
		EC_POINT_copy(pr, p1);
	}
	else {
		// pr = p1 + p2. Result is stored in pr.
		EC_POINT_add(curve, pr, p1, p2, ctx);
	}
	
	return true;
}

bool PointMultipAff::multipScalar(EC_POINT *pr, BIGNUM *k, EC_POINT *p1, EC_GROUP *curve) {
	BN_CTX *ctx = BN_CTX_new();	
	// pr = k x p1
	EC_POINT_mul(curve, pr, NULL, p1, k, ctx);
	return true;
}
