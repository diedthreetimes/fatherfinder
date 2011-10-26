//Emulate RFLP to generate fragment lengths=
#include <api/BamReader.h>
#include <string>
#include <vector>
#include <iostream>
#include <Sequence.h>
#include <Marker.h>
#include <Enzyme.h>
#include <BamSequence>
#include <StringSequence>

using namespace std;

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

  // Conduct RFLP store as another useful file format (presumably text)
  // Test Enzyme and Marker computing using c code (also may be useful to print fragments)
  // Restructure reading enzyme and markers from files
  // Allow reading in of multiple bam files

  // Add an RFLP reader

  // Renamespace
  // [optional] think about restructuring to allow markers to contain info about there matched fragments

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
  vector<Sequence *> markers;
  
  // Load the files
  try{
    // TODO: Incorporate file reader based on filetype
    Sequence *seq = new BamSequence(inputFilename);
    
    // Here the cut, position is the start of the second half
    enzymes.push( new Enzyme("CTGCAG", "pstI", 5) );
    markers.push( new Marker("GCTGCCCACTTCTTCCAGAGGGCCTGGCCATGGGTGAGGGCCCTGGGTAGAAGACCCC");
  }
  // TODO: More restrictive error handling
  catch( ... ){
    cerr << "Error reading " << inputFilename << endl;
    exit(1);
  }
  
  
  // Have each enzyme digest the genome while checking for markers
  char read;
  int num_enzymes = enzymes.length(); // we assume the enzyme and marker list don't change
  int num_markers = markers.length();
  Sequence* [] fragments    = new Sequence*[num_enzymes];      // 2d arrays of pointers
  Sequence* [] marked_frags = new Sequence*[num_enzymes];      // 2d arrays of pointers
  int       [] skips        = new      int[num_enzymes];       // Handles skiping matches
  bool    * [] marks        = new     bool[num_enzymes];       // Keeps track of when a marker has been identified.

  // Init arrays
  for( int i=0; i < num_enzymes; i++){
    
    fragments[i]    = new Sequence*[num_markers];
    marked_frags[i] = new Sequence*[num_markers];
    marks[i]        = new     bool[num_markers];
    
    for( int j=0; j < num_markers; j++){
      fragments   [i][j]    = new StringSequence();
      marked_frags[i][j]    = NULL;
      marks     [i][j]    = false;
    }
    skips[i] = 0;
  }

  // Loop through the genome
  while( read = seq.next() ){
    
    // Let the enzymes digest
    for( int i = 0; i < num_enzymes; i++ ){
      // This enzyme has found a match skip some reads
      // NOTE: This means that markers will not match over enzyme boundries
      //    We may want to verify that enzymes won't cut marker to see if this is an issue
      if(skips[i] != 0){
	skips[i]--;
	next;
      }
      
      // Check to see if the enzyme can cut at this pos in the sequence
      if ( seq->isMatch(enzymes[i]) ){
	*fragments[i] += enzymes[i]->first();

	marked = false;
	for(int m=0;m<num_markers;m++)
	  marked = marked || marks[i][m];

	// No marker is using this fragment
	if(!marked){
	  delete fragments[i];
	}

	// Init a new fragment beginning with the matched piece
	fragments[i] = new StringSequence(enzymes[i]->last());
	
	// Don't check the matching slots again (this would be biologically inacurate)
	skips[i] = enzymes[i]->length()-1;
      }
      else {
	// Collect this read onto the current fragment
	*fragments[i] += read;
      }
    }//end for enzyme

    // Check if we have found our marker
    for(int i=0; i < num_markers; i++){
      // NOTE: Skips are not as usefull here since a second match is unlikely by design
      //       and in 99% of cases would return the same fragment (but may give perf boost)
      
      if( seq->isMatch(markers[i]) ){
	for(int e=0; e < num_enzymes;e++){
	  marks[e][i] = true;
	  
	  // Save this fragment since it is a match
	  marked_frags[e][i] = fragments[i];
	}
      }
      
    }// end for marker
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

