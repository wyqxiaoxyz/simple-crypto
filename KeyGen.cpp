#include "KeyGen.h"
using namespace std;
#include <iostream>
#include <vector>
#include <openssl/ec.h>
#include <openssl/obj_mac.h>

// 'Public' parameters
static BIGNUM *p, *a, *b, *n, *h;
const EC_GROUP *curve;
const EC_POINT *G;
const EC_POINT *pubkey;
const BIGNUM *privkey;

KeyGen::KeyGen(void) {
	setupParams();
}

bool KeyGen::setupParams() {
	// Sextuple T = (p, a, b, G, n, h) of curve Secp256k1 is defined here
	// Parameters obtained from:
	// https://en.bitcoin.it/wiki/Secp256k1
	p = BN_new();
	a = BN_new(); 
	b = BN_new();
	n = BN_new();
	h = BN_new();
 
	BN_hex2bn(&p, "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F");
	BN_hex2bn(&a, "0000000000000000000000000000000000000000000000000000000000000000");
	BN_hex2bn(&b, "0000000000000000000000000000000000000000000000000000000000000007");
	BN_hex2bn(&n, "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141");
	BN_hex2bn(&h, "01");

	char *nstr = BN_bn2hex(n);
	string ns(nstr);
	//cout << "n = " << ns << endl;
 	//EC_GROUP *curve;
	curve = EC_GROUP_new_by_curve_name(NID_secp256k1);
	if (curve == NULL) {
		cout << "An error occured while creating secp256k1 curve!" << endl;
		return 1;	
	}
	
	G = EC_GROUP_get0_generator(curve); // Set G as generator point

	BN_CTX *ctx = BN_CTX_new();
	char *generatorHex = EC_POINT_point2hex(curve, G,
       POINT_CONVERSION_UNCOMPRESSED, ctx);
	string GHex(generatorHex);
	//cout << "Generator (uncompressed) = " << GHex << endl;
	
	EC_KEY *keypair = EC_KEY_new();
	EC_KEY_set_group(keypair,curve); // Associate key to curve
	EC_KEY_generate_key(keypair);	// Generate random private and public key
	
	privkey = EC_KEY_get0_private_key(keypair);
	pubkey = EC_KEY_get0_public_key(keypair);

	// Sanity check on key
	while (EC_KEY_check_key(keypair) == 0) {
		// Generated keypair failed sanity checks
		// Generate key again
		EC_KEY_generate_key(keypair);
	}

	return true;
}

curveparams_t KeyGen::getCurveParams() {
	curveparams_t curveParams;
	curveParams.p = p;
	curveParams.a = a;
	curveParams.b = b;
	curveParams.G = G;
	curveParams.n = n;
	curveParams.h = h;
	curveParams.curve = curve;

	return curveParams;
}

keypair_t KeyGen::getKeypair() {
	keypair_t keyPair;
	keyPair.pubkey = pubkey;
	keyPair.privkey = privkey;

	return keyPair;
}
