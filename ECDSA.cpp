#include "ECDSA.h"
using namespace std;
#include <iostream>
#include <sstream>
#include <openssl/sha.h>
#include <vector>
#include <openssl/obj_mac.h>
#include <openssl/ec.h>
#include <openssl/bn.h>
#include <cstring>
#include <time.h>

#define NUMBER_OF_TIMES 1000 // For benchmarking

void startClock();
void stopClock(string description, int numoftimes); 

static KeyGen keygen;
static curveparams_t curveparams;
static keypair_t keypair;
static PointMultipAff pmultip;
static int tDur;

ECDSA::ECDSA() {
	// Get curve parameters and fresh keys
	keygen = KeyGen();
	curveparams = keygen.getCurveParams();
	keypair = keygen.getKeypair();
	pmultip = PointMultipAff();
}

bool ECDSA::selectk(BIGNUM *k) {
	// Select strong random k
	BIGNUM *nminusone = BN_new();
	BN_sub(nminusone, (BIGNUM *)curveparams.n, BN_value_one()); // n -1

	BN_CTX *ctx = BN_CTX_new();
	BIGNUM *ktemp = BN_new();
	BIGNUM *gcd = BN_new();

	int cntr = 1;
	do {
		BN_rand(ktemp, 256, -1, 0); // random 256-bit number
		BN_mod(k, ktemp, nminusone, ctx); // k = k mod (n-1)
		BN_gcd(gcd, k, curveparams.p, ctx); // gcd(k,p) to ensure coprime
		cntr++;
	} while (BN_is_zero(k) == 1 || BN_cmp(gcd, BN_value_one()) != 0);
	//cout << cntr << " tries before I found a suitable k" << endl;

	return true;
}

vector<BIGNUM *> ECDSA::sign(string message, BIGNUM *privatekey) {

	vector<BIGNUM *> signature;

	BN_CTX *ctx = BN_CTX_new();
	BIGNUM *k = BN_new();
	BIGNUM *kin = BN_new();
	BIGNUM *sha1 = BN_new();
	EC_POINT *q = EC_POINT_new((EC_GROUP *)curveparams.curve); 
	BIGNUM *qx = BN_new();
	BIGNUM *qy = BN_new();
	BIGNUM *r = BN_new();
	BIGNUM *s = BN_new();
	this->SHA1(message, sha1);

	// PUBKEY
	do {
		this->selectk(k);
		// Calculate public key curve point q = k x G
		pmultip.multipScalar(q, k, (EC_POINT *)curveparams.G, (EC_GROUP *)curveparams.curve);

		// Compute r = qx mod n (that is affine x-coord of q)

		// Compute s = k^-1 (sha1 + privkey * r) mod n


	} while (BN_is_zero(r) == 1 || BN_is_zero(s) == 1);

	signature.clear(); // clear signature vector
	signature.push_back(r);
	signature.push_back(s); // signature = {r,s}
	
	return signature;
}

bool ECDSA::SHA1(string msg, BIGNUM *sha1digest) {
	string message = msg;
	unsigned char md[SHA_DIGEST_LENGTH]; // 32 bytes
	unsigned char *dataBuffer = new unsigned char[message.size()+1]; //+1
	dataBuffer[message.size()]=0; // nuffn
	memmove(dataBuffer, (unsigned char *)message.c_str(), message.size());

	//unsigned char dataBuffer[message.length()] = message.c_str(); 
	// -1 necessary to exlude the null char terminating the string
	if (!this->computeSHA1(dataBuffer, message.size(), md)) {
  	// handle error
		cout << "An error occurred during SHA1 computation" << endl;
	}
	else {
		string hash = "";    
    for(int i = 0; i < SHA_DIGEST_LENGTH; i++) {
        hash += this->hexify(md[i]);
    }
		//cout << "sha1 of '" << msg << "': " << hash << endl;
	}
	BN_bin2bn(md, SHA_DIGEST_LENGTH, sha1digest); // Store hash in sha1digest
	//cout << "sha1digest (BIGNUM version): " << BN_bn2hex(sha1digest) << endl;

	return true;
}

bool ECDSA::verify(string message, vector<BIGNUM *> signature, EC_POINT *publickey) {
	// Expects {r,s} in signature
	
	BN_CTX *ctx = BN_CTX_new();

	BIGNUM *r = BN_new();
	BIGNUM *s = BN_new();
	BIGNUM *zero = BN_new();
	BN_zero(zero);
	BIGNUM *one = BN_new();
	BN_one(one);

	if (signature.size() == 2) {
		BN_copy(s, signature.back()); signature.pop_back();
		BN_copy(r, signature.back()); signature.pop_back();
	}
	else {
		cout << "Malformed signature received!" << endl;
	}

	BIGNUM *nminusone = BN_new();
	BN_sub(nminusone, (BIGNUM *)curveparams.n, BN_value_one()); // n -1

	// if r and s are not in [1,n-1], signature is invalid
	if (BN_cmp(r, one) == -1 || BN_cmp(r, nminusone) == 1 || BN_cmp(s, one) == -1 || BN_cmp(s, nminusone) == 1) {
		cout << "ERROR: Invalid Signature (signature not in range)" << endl;
	}

	BIGNUM *hash = BN_new();
	this->SHA1(message, hash);

	// VER
	// Compute w = s^-1 mod n


	// Compute u1 = hash*w mod n and u2 = r*w mod n 

	
	// Compute the curve point pt = u1*G + u2*publickey


	// Calculate v = ptx mod n from pt


	// If r == v, signature is valid
	/*
	if (BN_cmp(r,v) == 0) {
		cout << "+"; // valid signature
	}
	else {
		cout << "-" << endl; // invalid signature
	}*/
	
	return true;
}

bool ECDSA::computeSHA1(unsigned char* input, unsigned long length, unsigned char* md) {
// Source:
// http://stackoverflow.com/questions/918676/generate-sha-hash-in-openssl
	SHA_CTX context;
	if(!SHA1_Init(&context))
		return false;

	if(!SHA1_Update(&context, (unsigned char*)input, length))
		return false;

	if(!SHA1_Final(md, &context))
		return false;

	return true;
}

string ECDSA::hexify(unsigned char c) {
	stringstream ss;
	ss << hex << (int) c;
	return ss.str();
} 

int UCLECCLab2_ECDSA_Exercise() {

	ECDSA ecdsa = ECDSA();
	string message = "Signthismessage";
	BIGNUM *privkey = BN_new(); // To store private key privkey
	BN_copy(privkey, (BIGNUM *)keypair.privkey);

	BN_CTX *ctx = BN_CTX_new();
	char *pubkeyHex = EC_POINT_point2hex(curveparams.curve, (EC_POINT *)keypair.pubkey,
       POINT_CONVERSION_UNCOMPRESSED, ctx);
	string pubkHex(pubkeyHex);
	cout << "Public key (uncompressed) = " << pubkHex << endl;

	// DER encoding
	pubkeyHex = EC_POINT_point2hex(curveparams.curve, (EC_POINT *)keypair.pubkey,
       POINT_CONVERSION_COMPRESSED, ctx);
	string pubkHexc(pubkeyHex);
	cout << "Public key (DER format, that is, compressed) = " << pubkHexc << endl;

	cout << "Private key: " << BN_bn2hex(privkey) << endl;

	// BENCHMARK
	// Sign and verify
	vector<BIGNUM *> signat;
	signat = ecdsa.sign(message, privkey);	

	// BENCHMARK SIGN
	ecdsa.verify(message, signat, (EC_POINT *)keypair.pubkey);
	
	// BENCHMARK VERIFY
	ecdsa.verify("signthismessage", signat, (EC_POINT *)keypair.pubkey);

	return 0;
}
