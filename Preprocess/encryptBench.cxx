#include <string>
#include <iostream>
#include <fstream>
#include "elgamal.h"
#include "ecelgamal.h"
#include "cstdlib"
#include <sys/time.h>

#define USE_ECC

#ifdef USE_ECC
#define ENC_SIZE 52
#else
#define ENC_SIZE 132 
#endif

#define SK_ONLY

using namespace std;
void usage(const char* argv[]){
  cout << "USAGE: " << argv[0] << " filename" << endl;
  cout << "\tEG:" << argv[0] << " mytest.txt" << "mytest.out" << endl;
  exit(1);
}

int main(int argc, const char* argv[] )
{
  #ifdef USE_ECC
  typedef ECElgamal scheme;
  typedef ECElgamal_PublicKey PK;
  typedef ECElgamal_SecretKey SK;
  typedef ECElgamal_Encryption Enc;
  int security = 160;
  #else
  typedef Elgamal scheme;
  typedef Elgamal_PublicKey PK;
  typedef Elgamal_SecretKey SK;
  typedef Elgamal_Encryption Enc;
  int security = 1024;
  #endif

  EncryptionScheme * enc = new scheme(security);
  PublicKey* pk = new PK();
  SecretKey* sk = new SK();
  Encryption* e = new Enc();  

  mpz_class r;
  gmp_randclass rr( gmp_randinit_default );
  rr.seed(time(NULL));

  enc->GenerateKeys(pk,sk);
  
  cout << "Key Gen done" << endl;

  int key_length;

  string alphabet = "AGCTN";
  
  struct timeval start, end;
  long mtime, seconds, useconds;
  gettimeofday(&start,NULL);
  for(int i=0; i < 100000; i++){
    e->encrypt(alphabet[i%5],pk);
  }
  gettimeofday(&end,NULL);
  seconds = end.tv_sec - start.tv_sec;
  useconds = end.tv_usec - start.tv_usec;
  double elapsed_secs  = ((seconds) * 1000 + (double)useconds/1000.0);
  cout << "Encryption: " << elapsed_secs << " milli seconds" << endl;

  string str;
  gettimeofday(&start,NULL);
  for(int i=0; i < 100000; i++){

#ifdef USE_ECC
//TODO: Add a get order function
  r = rr.get_z_bits(160);// THIS IS NOT SECURE it should be order but too complicated for now!!
#else
  r = rr.get_z_range(((PK *)pk)->q);
#endif

  str = r.get_str(16);
  
    e->encrypt(str,pk);
  }
  gettimeofday(&end,NULL);
  seconds = end.tv_sec - start.tv_sec;
  useconds = end.tv_usec - start.tv_usec;
  elapsed_secs  = ((seconds) * 1000 + (double)useconds/1000.0);
  cout << "Encryption Rand: " << elapsed_secs << " milli seconds" << endl;



  Encryption* e1 = new Enc();  
  e1->encrypt(alphabet[0],pk);

  gettimeofday(&start,NULL);
  for(int i=0; i < 100000; i++){
    e->plus(e1);
  }
  gettimeofday(&end,NULL);
  seconds = end.tv_sec - start.tv_sec;
  useconds = end.tv_usec - start.tv_usec;
  elapsed_secs  = ((seconds) * 1000 + (double)useconds/1000.0);
  cout << "Mults: " << elapsed_secs << " milli seconds" << endl;

#ifdef USE_ECC
//TODO: Add a get order function
  r = rr.get_z_bits(160);// THIS IS NOT SECURE it should be order but too complicated for now!!
#else
  r = rr.get_z_range(((PK *)pk)->q);
#endif

  gettimeofday(&start,NULL);
  for(int i=0; i < 100000; i++){
    e->mult(r);
  }
  gettimeofday(&end,NULL);
  seconds = end.tv_sec - start.tv_sec;
  useconds = end.tv_usec - start.tv_usec;
  elapsed_secs  = ((seconds) * 1000 + (double)useconds/1000.0);
  cout << "EXPS: " << elapsed_secs << " milli seconds" << endl;

    
  delete enc,pk,sk,e;
  return 0;
}
