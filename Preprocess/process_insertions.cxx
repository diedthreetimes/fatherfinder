//Process only the insertions, and the reference position they are next to
#include <string>
#include <vector>
#include <iostream>
#include <Sequence.h>
#include <InsertionSequence.h>

#define DEBUG true

using namespace std;

void usage(char* argv[]){
  cout << argv[0] << "" << endl;
  cout << "EG:" << endl;
  exit(1);
}

int main(int argc, const char* argv[]) {
  string bamFilename = argv[1];

  InsertionSequence * seq;

  // Load the files
  try{
    // TODO: Incorporate file reader based on filetype
    seq = new InsertionSequence(bamFilename);    
  }
  // TODO: More restrictive error handling
  catch( ... ){
    cerr << "Error reading " << bamFilename << endl;
    exit(1);
  }

  
  // Run through the insertion's and save to file with position
  char read = seq->begin();
  
  std::cout << "Starting at position " << seq->Position() << std::endl;
  

  int p = 0;
  do {
    p += 1;
    
    if((p % 100000 == 0) && DEBUG)
      std::cout << "Looked at " << p << "base pairs"<< read << std::endl;

    
    std::cout << read << seq->Position << std::endl;
  }while( read = seq->next() );
}
