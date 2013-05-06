#include <string>
#include <iostream>
#include <fstream>
#include "elgamal.h"
#include "cstdlib"

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

  EncryptionScheme * enc = new Elgamal(1024);
  PublicKey* pk = new Elgamal_PublicKey();
  SecretKey* sk = new Elgamal_SecretKey();
  Encryption* e = new Elgamal_Encryption();  
  char *buffer = new char[512];
  int length;

  enc->GenerateKeys(pk,sk);
  //TODO: Write out the keys to a file
  ofstream skf(privKeyFile.c_str(), ios::out | ios::binary);
  ofstream pkf(pubKeyFile.c_str(), ios::out | ios::binary);
  
  int key_length = pk->serialize(buffer, 512);
  if(key_length == -1){
    exit(-1);
  }
  pkf.write(buffer, key_length);
  
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
    // Also do lower case letters
    if(bp=='A' || bp== 'G' || bp == 'C' || bp == 'T' || bp == 'N' ){
      e->encrypt(bp,pk);
      
      length = e->serialize(buffer, 512);
      
      // TODO: Make sure every buffer is the same size
      // 82 works for 1024bit security
      ofs.write(buffer, length);
    }
    bp = ifs.get();  
  }

  ifs.close();
  ofs.close();
  
  delete enc,pk,sk,e;
  return 0;
}
