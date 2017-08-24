//(c) Nicolas Courtois 12 March 2015, 
//free software to be used without any restrictions as far as I am concerned
//contains slightly modified libraries written by other people 
//this Visual Studio project has been prepared by NCourtois for educational purposes 
//in order to demonstrate the usage of four different C++ packages useful to implement the bitcoin cryptography 
//and in order to experiment with bitcoin elliptic curve and ECDSA 
//most of the current code inside this uses 
//1) GMP/MPIR, the most advanced large number library, 
//but it is also possible to use: 
//2) uint256, just integers of 256 bits
//3) NTL::ZZ, (currently in this project NTL library is configured to fall back to GMP library, change it with LESS_MPIR) 
/////this project also contains some quick fixes to make NTL::ZZ work correctly at all (unhappily latest version of NTL is NOT compatible with latest version of GMP/MPIR due to access to internal implementation) 
//4) Crypto++ library, has things like CryptoPP::Integer and some ECC code 
//5) sec256k1 library (which has lots of interesting options, modify and recompile), 
//so this project offers ENDLESS possibilities to implement elliptic curves differently 
//and to compare and benchmark different implementations 
//
//Remarks and notes:

#include "ecc_k1_projects_basics.h" 


void main(int argc, char * argv[])/*argv[0] is the name, last argv[argc] is empty.*/
{
	//Bitcoin_ECC_GMP_Tests();
	//UCLECCLab1_BIGNUM_Tests();
	test_pointer();
	//test_int();
	//test_NTL();
};

