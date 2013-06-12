#include <string>
#include <iostream>
#include <fstream>
#include "elgamal.h"
#include "ecelgamal.h"
#include "cstdlib"

// #define USE_ECC

#ifdef USE_ECC
#define ENC_SIZE 44
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
  if(argc < 3 )
    usage(argv);

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

  std::string inFilename = argv[1];
  std::string outFilename = argv[2];

  string pubKeyFile = "genome.pub";
  string privKeyFile = "genome.priv";
  
  ifstream ifs(inFilename.c_str());
  ofstream ofs(outFilename.c_str(), ios::out | ios::binary);

  // Read and write the starting position
  string line;
  getline(ifs,line);
  ofs.write(line.c_str(), line.length());
  ofs.write("\n", 1);
  
  int position = atoi(line.c_str());
  cout << "Starting at: " << position << endl;

  EncryptionScheme * enc = new scheme(security);
  PublicKey* pk = new PK();
  SecretKey* sk = new SK();
  Encryption* e = new Enc();  
  char *buffer = new char[512];
  int length;

  enc->GenerateKeys(pk,sk);
  //TODO: Write out the keys to a file
  ofstream skf(privKeyFile.c_str(), ios::out | ios::binary);
  ofstream pkf(pubKeyFile.c_str(), ios::out | ios::binary);
  
  int key_length;
#ifndef SK_ONLY
  key_length = pk->serialize(buffer, 512);
  if(key_length == -1){
    exit(-1);
  }
  pkf.write(buffer, key_length);
#endif
  
  key_length = sk->serialize(buffer, 512);
  if(key_length == -1) {
     cout << "Error serializing sk" << endl;
     exit(-1);
  }
  skf.write(buffer, key_length);

  skf.close();
  pkf.close();

  char bp = ifs.get();
  while(ifs.good()) {
    memset(buffer,0,ENC_SIZE);

    // Also do lower case letters
    if(bp=='A' || bp== 'G' || bp == 'C' || bp == 'T' || bp == 'N' ){
      e->encrypt(bp,pk);
      
      length = e->serialize(buffer, ENC_SIZE);
      
      // We need to align the encryptions
      ofs.write(buffer, ENC_SIZE);
    }
    bp = ifs.get();  
  }

  ifs.close();
  ofs.close();
  
  delete [] buffer;
  delete enc;
  delete pk;
  delete sk;
  delete e;
  return 0;
}
