//Emulate RFLP to generate fragment lengths=
#include <api/BamReader.h>
#include <string>
#include <vector>
#include <iostream>

using namespace std;
using namespace BamTools;

void ussage(char* argv[]){
  cout << argv[0] << "" << endl;
  cout << "EG:" << endl;
  exit(1);
}

int main(int argc, const char* argv[] )
{

  // General TODO
  // Read in the bam file as a sequence
  // Verify it is working by counting the basepairs on chromosome 20

  // Restructure Bam reading as a Sequence object

  // Read in Enzymes
  // Conduct RFLP store as another useful file format (presumably text)
  // Probe l fragments based on Markers
  
  // Add an RFLP reader
  // Restructure redoing of enzymes

  // Renamespace

  // Start crypto phase
  // Build a new set with fragments concated with enzyme identifier

  string inputFilename = argv[1];

  // attempt to open our BamMultiReader
  BamReader reader;
  if ( !reader.Open(inputFilename) ) {
    cerr << "Could not open input BAM files." << endl;
    return -1;
  }

  BamAlignment al;
  int i = 0;

  while ( reader.GetNextAlignmentCore(al) ) {
    i += 1;
    
    if( i < 10 )
      cout << al.Position;
  }

  cout << "Made: " << i << " reads." << endl;
  
  reader.Close();
}

