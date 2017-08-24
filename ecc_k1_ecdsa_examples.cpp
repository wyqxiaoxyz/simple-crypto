#include "ecc_k1_projects_basics.h"
#include "uint256.h"
#include <openssl/ecdsa.h>//needed for ECDSA_verify
#include <openssl/obj_mac.h>//needed for NID_secp256k1
using namespace std;//to avoid writing std::string or std::cout
#include <vector>
using namespace std;//to avoid writing std::string or std::cout


 
//my own debug replacement for ECDSA_verify, written using BIGNUM OpenSSL internals, essentially the same as the original one
int ECDSA_verify2(int type,const unsigned char *dgst, int dgst_len,const unsigned char *sigbuf,int sig_len,EC_KEY *pkey)
{
	int result=0;
	ECDSA_SIG *sig;
	sig = ECDSA_SIG_new();
	d2i_ECDSA_SIG(&sig, &sigbuf, sig_len);
		BIGNUM   *order, *u1, *u2, *m, *X;
		BN_CTX *ctx = BN_CTX_new();
		BN_CTX_start(ctx);
			order = BN_CTX_get(ctx);	
			u1    = BN_CTX_get(ctx);
			u2    = BN_CTX_get(ctx);
			m     = BN_CTX_get(ctx);
			X     = BN_CTX_get(ctx);
		const EC_GROUP *group = EC_KEY_get0_group(pkey);
		EC_GROUP_get_order(group, order, ctx);
		// calculate tmp1 = inv(S) mod order */
		BN_mod_inverse(u2, sig->s, order, ctx);
		BN_bin2bn(dgst, dgst_len, m);
		// u1 = m * tmp1 mod order , so overall u1=h/s
		BN_mod_mul(u1, m, u2, order, ctx);
		// u2 = r * tmp1 mod order, overall u2=r/s
		BN_mod_mul(u2, sig->r, u2, order, ctx);
		EC_POINT *point = EC_POINT_new(group);
		const EC_POINT *pub_key = EC_KEY_get0_public_key(pkey);
		EC_POINT_get_affine_coordinates_GFp(group, point, X, NULL, ctx);
		EC_POINT_mul(group, point, u1, pub_key, u2, ctx);
		EC_POINT_get_affine_coordinates_GFp(group,	point, X, NULL, ctx);
		BN_nnmod(u1, X, order, ctx);//probably NOT necessary reduction, but present in OpenSSL source code!!! 
		//  if the signature is correct u1 is equal to sig->r */
		result = (BN_ucmp(u1, sig->r) == 0);
		BN_CTX_end(ctx);
		BN_CTX_free(ctx);
	//};
	ECDSA_SIG_free(sig);
	return result;
};

int VerifyDERSignatureUsingOpenSSL(const std::vector<unsigned char> &vchSig,const char *pkeyX,const char *pkeyY, uint256 sighash)
{
	int VerifOK=0;
	
	//implementation which uses OpenSSL written by NCourtois
		EC_KEY *pkey=EC_KEY_new_by_curve_name(NID_secp256k1);
		BIGNUM *x = BN_new();
		BIGNUM *y = BN_new();
		BN_hex2bn(&x,pkeyX);
		BN_hex2bn(&y,pkeyY);
		EC_KEY_set_public_key_affine_coordinates(pkey,x,y);

		//OpenSSL code vs. my own code re-written 
		//int result=
		//	ECDSA_verify(0,(unsigned char *)&sighash, sizeof(uint256),
		//	&vchSig[0],vchSig.size(),pkey);
		int result=
			ECDSA_verify2(0,(unsigned char *)&sighash, sizeof(uint256),
			&vchSig[0],vchSig.size(),pkey);

		if(result==1)
			VerifOK=1;
		if(result==-1)
			printf("Incorrect signature, result=0\n");
		if(result==-1)
			printf("Some error within ECDSA_verify, result=-1\n");
        BN_free(x);
        BN_free(y);
		EC_KEY_free(pkey);

	return VerifOK;
}

