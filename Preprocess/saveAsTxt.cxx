#include <string>
#include <vector>
#include <iostream>
#include <Sequence.h>
#include <BamSequence.h>

using namespace std;
void usage(char* argv[]){
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

  cout << ((BamSequence *)seq)->Position() << endl;
  
  //cout << "BEGIN" << endl;

  int p = 1;
  do {
    // This may not be very efficient
    cout << read;
    if(p % 100000 == 0)
      cout << endl;
    p++;
  } while( read = seq->next() );

  //cout << endl << "END" << endl;
  //cout << "Ending position " << ((BamSequence *)seq)->Position() << endl;
  return 0;
}
