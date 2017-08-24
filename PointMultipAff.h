using namespace std;
#include <iostream>
#include <openssl/ec.h>
#include <openssl/bn.h>

class PointMultipAff {
	public:
		PointMultipAff();
		bool addPoints(EC_POINT *pr, EC_POINT *p1, EC_POINT *p2, EC_GROUP *curve); // *pr stores result
		bool multipScalar(EC_POINT *pr, BIGNUM *k, EC_POINT *p1, EC_GROUP *curve);

};

