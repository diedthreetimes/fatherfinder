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
  
  if(acc->isZero(sk)){
    std::cout << "Match Found!" << std::endl;
  }else {
    std::cout << "Match Not Found!" << std::endl;
  }
}

// Pattern holder
void alice(const string privKeyFile, const string inputFile) {

  // Alices pattern
  int l = 9993+3581;
  string pattern = "TACAAAGGTGAAACCCAGGAGAGT";
  //    

  ifstream ifs(inputFile.c_str(), ios::in | ios::binary);

  // Read and send the starting position 
  
  // TODO: Need to fix this to be just a binary number
  char line [5];
  ifs.read(line, 5);
  int position = atoi(line);
  
  cout << "Starting at postiion " << position << endl;
  SecretKey* sk = new Elgamal_SecretKey();
  

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

  
  char alphabet [6] = "ACGTN";
  Encryption * eAlphabet [5];

  for(int i = 0; i < 5; i++){
    eAlphabet[i] = new Elgamal_Encryption();
    eAlphabet[i]->encrypt(alphabet[i], pk);
    eAlphabet[i]->mult(-1);
  }

  int pattern_offset = 0; // Our current position in the pattern
  
  
  Encryption* acc  = new Elgamal_Encryption();  
  acc->encrypt(0, pk);
  
  Encryption* e  = new Elgamal_Encryption();  
  int offset = 0; // Used for buffer managment
  int expected_length = 200; // The maximal length of an encryption

  mpz_class r;
  do {
    ifs.read(buffer+offset, expected_length-offset);
    length = e->deserialize(buffer, expected_length, pk);

    // move the bit that we already read to the begginning of the buffer
    offset = expected_length-length;
    memcpy(buffer, buffer+length, expected_length - length);

    
    if( position == (l + pattern_offset)) {
      bool found = false;
      for(uint i=0; i < 5; i++){
	if( pattern[pattern_offset] == alphabet[i] ){
	  e->plus(eAlphabet[i]);
	  found = true;
	}
      }

      if(!found){
	cout << "Pattern did not mach any position" << endl;
	exit(-1);
      }

      // If we include position then we don't have to use seperate r. Otherwise we do, since we get cross cancelation.
      // TODO: Add to encryption a "random element" api
      r = Elgamal::rr.get_z_range(((Elgamal_PublicKey *)pk)->p);
      e->mult(r);

      acc->plus(e);
      
      // Test code
      // if(acc->isZero(sk)){
      // 	std::cout << "Correct "<< position << std::endl;
      // }
      // else{
      // 	std::cout << "Wrong " << position << std::endl;
      // }

      pattern_offset++;

      // Are we are done?
      if( pattern_offset >= pattern.length() )
	pattern_offset = 0;
    }
    
    position += 1;
  }while(!ifs.eof());


  ifs.close();

  // This should be a tcp connection, but for this prototype it need not be
  bob(sk, acc);

  for(int i=0; i < 5; i++)
    delete eAlphabet[i];

  delete pk,sk,e, acc;
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
