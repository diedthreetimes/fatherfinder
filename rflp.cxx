//Emulate RFLP to generate fragment lengths=
#include <string>
#include <vector>
#include <iostream>
#include <Sequence.h>
// #include <Marker.h>
#include <Enzyme.h>
#include <BamSequence.h>
#include <StringSequence.h>

using namespace std;

void ussage(char* argv[]){
  cout << argv[0] << "" << endl;
  cout << "EG:" << endl;
  exit(1);
}

int main(int argc, const char* argv[] )
{

  // General TODO
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
  vector<Enzyme *> enzymes;
  vector<Sequence *> markers;
  
  // Load the files
  try{
    // TODO: Incorporate file reader based on filetype
    Sequence *seq = new BamSequence(bamFilename);
    
    // Here the cut, position is the start of the second half
    enzymes.push_back( new Enzyme("CTGCAG", "pstI", 5) );
    enzymes.back()->PrintSelf(std::cout);
    markers.push_back( new StringSequence("GCTGCCCACTTCTTCCAGAGGGCCTGGCCATGGGTGAGGGCCCTGGGTAGAAGACCCC") );
  }
  // TODO: More restrictive error handling
  catch( ... ){
    cerr << "Error reading " << bamFilename << endl;
    exit(1);
  }
  
  
  // Have each enzyme digest the genome while checking for markers
  char read;
  int num_enzymes = enzymes.size(); // we assume the enzyme and marker list don't change
  int num_markers = markers.size();
  Sequence** fragments    = new Sequence* [num_enzymes];      // 1d arrays of pointers
  Sequence*** marked_frags= new Sequence** [num_enzymes];      // 2d arrays of pointers
  int     *  skips        = new      int    [num_enzymes];       // Handles skiping matches
  bool    ** marks        = new     bool*   [num_enzymes];       // Keeps track of when a marker has been identified.

  // Init arrays
  for( int i=0; i < num_enzymes; i++){
    
    fragments[i]    = new StringSequence("");
    marked_frags[i] = new Sequence*[num_markers];
    marks[i]        = new     bool[num_markers];
    
    for( int j=0; j < num_markers; j++){
      marked_frags[i][j]    = NULL;
      marks       [i][j]    = false;
    }
    skips[i] = 0;
  }

  // Loop through the genome
  read = seq->begin();
  do {   
    // Let the enzymes digest
    for( int i = 0; i < num_enzymes; i++ ){
      // This enzyme has found a match skip some reads
      // NOTE: This means that markers will not match over enzyme boundries
      //    We may want to verify that enzymes won't cut marker to see if this is an issue
      if(skips[i] != 0){
	skips[i]--;
	continue;
      }
      
      // Check to see if the enzyme can cut at this pos in the sequence
      if ( seq->isMatch(*enzymes[i]) ){
	*(fragments[i]) += enzymes[i]->first();

	bool marked = false;
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
      
      if( seq->isMatch(*markers[i]) ){
	for(int e=0; e < num_enzymes;e++){
	  marks[e][i] = true;
	  
	  // Save this fragment since it is a match
	  marked_frags[e][i] = fragments[i];
	}
      }
      
    }// end for marker
  }while( read = seq->next() );

  // Process Fragments
}

