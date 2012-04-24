// Implement the insertion only bam sequence
//  as of now (4/16/12) it may not be the most efficient TODO

#include "InsertionSequence.h"
#include <map>

#define SKIP_CHAR 'S'

//TODO: Finish implementing this

// Just like bamsequence, but filters only insertions
char InsertionSequence::calc_position(int i)
{
  // Possible optimization is to reuse alignments
  // While we don't have every alignment containing i add alignments
  while( !read_all && (m_alignments.size() == 0 ||  m_alignments.back().Position <= i) ){
    m_alignments.push_back( BamTools::BamAlignment() );
    
    if( m_reader.GetNextAlignment(m_alignments.back()) == false ){
      read_all = true;
    }
  }
  
  if( m_alignments.size() == 0  && read_all)
    return '\0';

  // Find all alignments containing position i
  //std::string sum = "D";
  std::map<char, int> hash;
  char vote;
  for(int j=0; j < m_alignments.size(); j++){
    if( m_alignments[j].Position + m_alignments[j].AlignedBases.length() -1 < i || m_alignments[j].Position > i)
      break;

    int bp_idx = i - m_alignments[j].Position;
    
    vote = m_alignments[j].AlignedBases[ bp_idx ];
    
    if(filter(m_alignments[j])){
      vote = SKIP_CHAR;
    }

    hash[vote]++; 
  }

  
  // Find the maximum occuring character
  // NOTE: This will always pick the 'smaller' character for ties since indicies are stored in order
  // A more robust solution is to add randomnes or other data metrics like read quality
  int max = 0;
  char max_c = 'N';
  std::map<char,int>::iterator it = hash.begin();
  for(; it != hash.end(); it++){
    if( (*it).second > max ){
      max_c = (*it).first;
      max = (*it).second;
    }
  }
  
  return max_c;
}

char InsertionSequence::next()
{
  char n;
  do{
    n = BamSequence::next();
  }while(n == SKIP_CHAR);

  return n;
}


bool InsertionSequence::filter(BamTools::BamAlignment align){
  //TODO:
  return false;
}
