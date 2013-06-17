#include <string>
#include <iostream>
#include <fstream>
#include "cstdlib"
#include <ctime>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include "elgamal.h"
#include "ecelgamal.h"


// #define USE_ECC

#ifdef USE_ECC
#define ENC_SIZE 44
#else
#define ENC_SIZE 132
#endif

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

#define ONLINE_MULTS
// #define NO_OPT

// Run the genomic-privacy protocol
// Here we don't actually do any data transfer! Primarily, this is because entire data file can easily be transfered using some other means
//   e.g. via usb stick
// As a consequence of this, we simulate all communication. Including sending the final accumulator.
using namespace std;
void usage(const char* argv[]){
  cout << "USAGE: " << argv[0] << " filename [length]" << endl;
  cout << "\tEG:" << argv[0] << " mytest.txt" << "mytest.out" << endl;
  exit(1);
}

gmp_randclass rr( gmp_randinit_default );

// DNA holder
void bob(SecretKey *sk, Encryption * acc) {
  
  if(acc->isZero(sk)){
    std::cout << "Match Found!" << std::endl;
  }else {
    std::cout << "Match Not Found!" << std::endl;
  }
}

// Pattern holder
void alice(const string privKeyFile, const string inputFile, int pattern_length = 10) {
  
  #ifdef NO_OPT
  cout << "No optimization" << endl;
  #elif defined ONLINE_MULTS 
  cout << "Mult only for online phase" << endl;
  #else
  cout << "No key knowledge optimization" << endl;
  #endif

  // Alices pattern
  // This is a matching pattern
  // int l = 9993+3581;
  // string pattern = "TACAAAGGTGAAACCCAGGAGAGT";
  
  // This is a test pattern
  int l = 9993;
  string pattern = "";
  for(int i=0; i < pattern_length; i++){
    pattern += "A";
  }
      

  ifstream ifs(inputFile.c_str(), ios::in | ios::binary);

  // Read and send the starting position 
  
  // TODO: Need to fix this to be just a binary number
  char line [5];
  ifs.read(line, 5);
  int position = atoi(line);
  
  cout << "Starting at postiion " << position << endl;
  SecretKey* sk = new SK();
  

  char * buffer = new char[512];
  int length;

    
  ifstream skf(privKeyFile.c_str(), ios::in | ios::binary);
  skf.read(buffer, 512);
  if(sk->deserialize(buffer,512) == -1){
    cout << "Secret key read incorectly" << std::endl;
    exit(-1);
  }
  PublicKey* pk = new PK(*(SK*)sk);
  //  PublicKey* pk = new PK();  
  //  ((ECElgamal_PublicKey*)pk)->init(*(SK*)sk);
  
  skf.close();

  char alphabet [6] = "ACGTN";
  
  struct timeval start, end;
  long mtime, seconds, useconds;
  gettimeofday(&start,NULL);
#ifdef NO_OPT
  Encryption * tmp = new Enc();
#elif defined ONLINE_MULTS
  Encryption **ePattern = new Encryption * [pattern.size()];
  for(int i =0; i < pattern.size(); i++){
    std::string tmp = pattern[i] + "|" + (char)i;
    ePattern[i] = new Enc();
    ePattern[i]-> encrypt(pattern[i],pk); //TODO: This should encrypt the result of a hash.
    ePattern[i]->mult(-1);
  }
#else
  // When doing full protocol we only need to encrypt once
  Encryption * eAlphabet [5];
  
  for(int i = 0; i < 5; i++){
    eAlphabet[i] = new Enc();
    eAlphabet[i]->encrypt(alphabet[i], pk);
    eAlphabet[i]->mult(-1);
  }
#endif
  gettimeofday(&end,NULL);
  
  seconds = end.tv_sec - start.tv_sec;
  useconds = end.tv_usec - start.tv_usec;
  
  double elapsed_secs  = ((seconds) * 1000 + (double)useconds/1000.0);
  cout << "Pattern Preprocessing time: " << elapsed_secs << " milli seconds" << endl;

  int pattern_offset = 0; // Our current position in the pattern
  
  
  Encryption* acc  = new Enc();  
  acc->encrypt(0, pk);
  
  Encryption* e  = new Enc();  
  int offset = 0; // Used for buffer managment

  mpz_class r;


  gettimeofday(&start,NULL);

  // TODO: Go directly to the pattern location
  // TODO: Make sure to use fixed with deserialization
  // TODO: This will also make the read code much simpler
  ifs.seekg((l-position)*ENC_SIZE, ifs.cur);
  position = l;
  // cout << position << endl;
  // cout << ifs.tellg() << endl;
  do {
    ifs.read(buffer, ENC_SIZE);
    length = e->deserialize(buffer, ENC_SIZE, pk);
        
    if( position == (l + pattern_offset)) {
      bool found = false;
      for(uint i=0; i < 5; i++){
	if( pattern[pattern_offset] == alphabet[i] ){
#ifdef NO_OPT
	  tmp->encrypt(alphabet[i], pk);
	  tmp->mult(-1);
	  e->plus(tmp);
#elif defined ONLINE_MULTS
	  *e = *(ePattern[pattern_offset]);
#else
	  e->plus(eAlphabet[i]);
#endif
	  found = true;
	}
     }

      if(!found){
	cout << "Pattern did not mach any position" << endl;
	exit(-1);
      }

      // If we include position then we don't have to use seperate r. Otherwise we do, since we get cross cancelation.
      // TODO: Add to encryption a "random element" api

#ifndef ONLINE_MULTS 

      #ifdef USE_ECC
//TODO: Add a get order function
      r = ::rr.get_z_bits(160);// THIS IS NOT SECURE it should be order but too complicated for now!!
      #else
      r = ::rr.get_z_range(((PK *)pk)->q);
      #endif

      e->mult(r);
#endif
      acc->plus(e);
      
      pattern_offset++;
    }

    // Are we are done?
    if( pattern_offset >= pattern.length() )
      break;
    
    position += 1;
  }while(!ifs.eof());

#ifdef ONLINE_MULTS
  #ifdef USE_ECC
  r = ::rr.get_z_bits(160);
  #else
  r = ::rr.get_z_range(((PK *)pk)->q);
  #endif

  acc->mult(r);	  
#endif

  ifs.close();

  // This should be a tcp connection, but for this prototype it need not be
  bob(sk, acc);

  gettimeofday(&end,NULL);

  seconds = end.tv_sec - start.tv_sec;
  useconds = end.tv_usec - start.tv_usec;
  
  elapsed_secs  = ((seconds) * 1000 + (double)useconds/1000.0);
  cout << "Online time: " << elapsed_secs << " milli seconds" << endl;

#ifdef NO_OPT
  delete tmp;
#elif defined ONLINE_MULTS
  for(int i=0; i < pattern.size(); i++)
    delete ePattern[i];

  delete [] ePattern;
#else
  for(int i=0; i < 5; i++)
    delete eAlphabet[i];
#endif
  
  delete pk;
  delete sk;
  delete e;
  delete acc;
  delete [] buffer;
}

int main(int argc, const char* argv[] )
{
  if(argc < 1 )
    usage(argv);

  ::rr.seed(time(NULL));

  std::string inFilename(argv[1]);
  int pattern_length = atoi(argv[2]);
  
  string pubKeyFile = "genome.pub";
  string privKeyFile = "genome.priv";

  // At this stage alice has recieved the dna and needs to process it
  alice(privKeyFile, inFilename, pattern_length);
  // Seg fault when alic exits??

  return 0;
  
}
