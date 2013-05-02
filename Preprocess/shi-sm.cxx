#include <string>
#include <iostream>
#include <fstream>
#include "elgamal.h"
#include "cstdlib"


// Run the genomic-privacy protocol
// Here we don't actually do any data transfer! Primarily, this is because entire data file can easily be transfered using some other means
//   e.g. via usb stick
// As a consequence of this, we simulate all communication. Including sending the final accumulator.
using namespace std;
void usage(const char* argv[]){
  cout << "USAGE: " << argv[0] << " filename" << endl;
  cout << "\tEG:" << argv[0] << " mytest.txt" << "mytest.out" << endl;
  exit(1);
}

// DNA holder
void bob(SecretKey *sk, Encryption * acc) {

}

// Pattern holder
void alice(const string privKeyFile, const string inputFile) {
  ifstream ifs(inputFile.c_str(), ios::in | ios::binary);

  // Read and send the starting position 
  
  // TODO: Need to fix this to be just a binary number
  char line [5];
  ifs.read(line, 5);
  int position = atoi(line);
  
  cout << "Starting at postiion " << position << endl;
  SecretKey* sk = new Elgamal_SecretKey();
  

  Encryption* e  = new Elgamal_Encryption();  
  Encryption* ep = new Elgamal_Encryption();  
  char * buffer = new char[512];
  int length;

    
  ifstream skf(privKeyFile.c_str(), ios::in | ios::binary);
  skf.read(buffer, 512);
  if(sk->deserialize(buffer,512) == -1){
    cout << "Secret key read incorectly" << std::endl;
    exit(-1);
  }
  PublicKey* pk = new Elgamal_PublicKey(*(Elgamal_SecretKey*)sk);
  
  skf.close();


  // Read and send 
  
  int offset = 0;
  int expected_length = 200;
  do {
    ifs.read(buffer+offset, expected_length-offset);
    length = e->deserialize(buffer, expected_length, pk);

    // move the bit that we already read to the begginning of the buffer
    offset = expected_length-length;
    memcpy(buffer, buffer+length, expected_length - length);

    ep->encrypt('T', pk);
    ep->mult(-1);

    e->plus(ep);

    //TODO: Here we need to add to acc
    
    if(e->isZero(sk)){
      std::cout << "Correct" << std::endl;
    }else{

    }

    // END TODO:

  }while(!ifs.eof());


  ifs.close();

  // This should be a tcp connection, but for this prototype it need not be
  //bob
  
  delete pk,sk,e;
  delete [] buffer;
}

int main(int argc, const char* argv[] )
{
  if(argc < 1 )
    usage(argv);

  std::string inFilename = argv[1];
  
  string pubKeyFile = "genome.pub";
  string privKeyFile = "genome.priv";

  // At this stage alice has recieved the dna and needs to process it
  alice(privKeyFile, inFilename);
  
  return 0;
  
}
