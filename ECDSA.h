using namespace std;
#include "KeyGen.h"
#include "PointMultipAff.h"
#include <iostream>
#include <openssl/sha.h>
#include <vector>
#include <openssl/ec.h>
#include <openssl/bn.h>

class ECDSA {
public:
	ECDSA();
	bool selectk(BIGNUM* k);
	vector<BIGNUM *> sign(string message, BIGNUM *privatekey);
	bool SHA1(string message, BIGNUM *sha1digest);
	bool verify(string message, vector<BIGNUM *> signature, EC_POINT *publickey);
	bool computeSHA1(unsigned char *input, unsigned long length, unsigned char *md);
	string hexify(unsigned char s);

};

int UCLECCLab2_ECDSA_Exercise(void);
