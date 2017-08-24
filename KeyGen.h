#include <openssl/bn.h>
#include <openssl/ec.h>
using namespace std;
#include <vector>

struct curveparams_t {
	const BIGNUM* p;
	const BIGNUM* a;
	const BIGNUM* b;
	const BIGNUM* n; 
	const BIGNUM* h;
	const EC_POINT* G;
	const EC_GROUP* curve;
};

struct keypair_t {
	const EC_POINT* pubkey;
	const BIGNUM* privkey;
};

class KeyGen {
	public:
		KeyGen(void);
		bool setupParams();
		curveparams_t getCurveParams();
		keypair_t getKeypair();
}; 
