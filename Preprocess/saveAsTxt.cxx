#include <string>
#include <vector>
#include <iostream>
#include <Sequence.h>
#include <BamSequence.h>

using namespace std;
void ussage(char* argv[]){
  cout << argv[0] << " filename" << endl;
  cout << "EG:" << argv[0] << " mytest.bam" << endl;
  exit(1);
}

int main(int argc, const char* argv[] )
{
  
  string bamFilename = argv[1];
  
  Sequence * seq;

  try{
    seq = new BamSequence(bamFilename);
  }
  catch( ... ){
    cerr << "Error reading " << bamFilename << endl;
    exit(1);
  }
  
  // Loop through the genome
  char read = seq->begin();

  cout << "Starting at position " << ((BamSequence *)seq)->Position() << endl;
  
  cout << "BEGIN" << endl;

  int p = 0;
  do {
    // This may not be very efficient
    cout << read;
    if(p % 100000 == 0)
      cout << endl;
    p++;
  } while( read = seq->next() );

  cout << "END" << endl;
  cout << p << endl;
  cout << "Ending position " << ((BamSequence *)seq)->Position() << endl;
}
