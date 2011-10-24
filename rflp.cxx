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
  // This means doing some form of pileup

  // Restructure Bam reading as a Sequence object

  // Read in Enzymes (more than 1) and markers
  // Conduct RFLP store as another useful file format (presumably text)
  // First break into fragments based on enzyme
  // Probe l fragments based on Markers (ask about this)
  
  // Add an RFLP reader

  // Renamespace

  // Start crypto phase
  // Build a new set with fragments concated with enzyme identifier

  // TODO: Allow this to be a directory or list
  string bamFilename = argv[1];

  // CSV of enzymes one per line (name,string,cut position)
  string enzymeFilename = argv[2];

  // CSV of markers one per line
  string markersFilename = argv[3];


  Sequence * seq;
  vector<Sequence *> enzymes;
  //vector<Sequence *> markers;
  
  // Load the files
  try{
    // TODO: Incorporate file reader based on filetype
    Sequence *seq = new BamSequence(inputFilename);
    
    // Here the cut, position is the start of the second half
    enzymes.push( new Enzyme("CTGCAG", "pstI", 5) );
  }
  // TODO: More restrictive error handling
  catch( ... ){
    cerr << "Error reading " << inputFilename << endl;
    exit(1);
  }
  
  
  // Have each enzyme digest the genome

  char read;
  int num_enzymes = enzymes.length();
  vector<Sequence *> [] fragments = new vector<Sequence *> [num_enzymes];
  int [] idxs  = new int[num_enzymes];
  int [] skips = new int[num_enzymes];
  for( int i = 0; i < num_enzymes; i++ ){
    fragments[i].push(new StringSequence());
    idxs[i] = 0;
    skips[i] = 0;
  }

  while( seq.hasNext() ){
    read = seq.next();
    
    for( int i = 0; i < num_enzymes; i++ ){
      // This enzyme has found a match skip some reads
      if(skips[i] != 0){
	skips[i]--;
	next;
      }

      // The current fragment for this enzyme
      idx = idxs[i];
      
      // Check to see if the enzyme can cut at this pos in the sequence
      if ( seq.match(enzymes[i]) ){
	fragments[i][idx] += enzymes[i].first();
	  
	// Init a new fragment beginning with the matched piece
	idxs[i]++;
	fragments[i].push(	
			  new StringSequence(enzymes[i].last()) 
				);
	
	skips[i] = enzymes[i].length()-1;
      }
      else {
	// Collect this read onto the current fragment
	fragments[i][idx] += read;
      }
    }//end for
  }//end while
  
  // Process Fragments

  if ( !reader.Open(inputFilename) ) {
    cerr << "Could not open input BAM files." << endl;
    return -1;
  }

  BamAlignment al;
  int i, j = 0;

  while ( reader.GetNextAlignment(al) ) {

    if( j < 10 )
      cout << al.Position << ":" << al.AlignedBases << ":" << al.AlignedBases.length() << endl;

    j += 1;
    i += al.AlignedBases.length();
  }

  cout << "Made: " << i << " reads." << endl;
  reader.Close();
}

