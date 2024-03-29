//Implement an abstract sequence via bamtools
// This reader will handle all statistical processing
// For now this is a very naive pileup

// Some things to consider
//   - Map Quality?
//   - Cigar Aligned bases vs Query bases. What is the difference? Which should we use? For now we use align

// There is a lot of "interesting" fields in both the bam and bas files,
//   but the literature on it seems to expect at least some prior knowledge
//   for now we just do the naieve thing and ignore it all (hoping our algorithim provides enough resilience)


#include "BamSequence.h"
#include "iostream"
#include <map>

// TODO: propogate errors up to the main program
// TOOD: add error checking for alignment retrieval
BamSequence::BamSequence(const std::string & file)
{
  if( !m_reader.OpenFile(file) ){
    std::cerr << "Could not open input BAM files: " << file << std::endl;
    EXIT_FAILURE;
  }
  
  init();
}

BamSequence::BamSequence(const std::vector<std::string> & files)
{
  if( !m_reader.Open(files) ){
    std::cerr << "Could not open input BAM files: " << &files[0] << std::endl;
    EXIT_FAILURE;
  }
  init();
}

BamSequence::~BamSequence()
{
  m_reader.Close();
}

void BamSequence::init()
{

  // For now we skip to the first write, this helps when looking at only a certain chromosome
  //  This may cause certain errors if two different people's reads don't start in the same place
  m_alignments.push_back( BamTools::BamAlignment() );
  m_reader.GetNextAlignment(m_alignments.back());
  
  cur_pos = m_alignments.back().Position;
  m_cur = calc_position(cur_pos);
 
  read_all = false; // This is required
  //cur_pos = -1;
  //next();

  
}

char BamSequence::current()
{
  return m_cur;
}

char BamSequence::next()
{  
  //  if(true){
  //    std::cout << "Alignment length: " << m_alignments.size() << std::endl;
  //  }
  // This keeps no history
  if(m_ring.size() == 0){
      retrieve(1);// TODO: tune this
  }

  m_cur = m_ring.front();
  m_ring.pop_front();


  // Move to the next position removing uneeded elements
  cur_pos++;

  // While the alignment doesn't contain any new data remove
  // TODO: Think about how we are getting position look at #GetEndPosition usePadded
  while(!m_alignments.empty() &&  m_alignments.front().Position + m_alignments.front().AlignedBases.length() - 1 < cur_pos)
    m_alignments.pop_front();

  return m_cur;
}

char BamSequence::rewind(int n)
{
  // Rewind isn't as important for bam sequences we leave it unimplemented currently
  throw notImplemented;
}

char BamSequence::begin()
{
  m_reader.Rewind();
  
  init();

  return current();
}

// Fill the circular buffer the next n bps (including current)
void BamSequence::retrieve(int n)
{
  if( m_ring.size() > n )
    return;
  // If we need more room than we have
  //if( m_ring.capacity() < n)
  //  m_ring.set_capacity(n);

  // Calculate needed base pairs
  for(int i=m_ring.size(); i < n; i++)
    m_ring.push_back(calc_position(cur_pos + i));    
}

// Calculate the basepair at position i asuming that all alignments have already been loaded 
char BamSequence::calc_position(int i)
{
  // Possible optimization is to reuse alignments
  // While we don't have every alignment containing i add alignments
  while( !read_all && (m_alignments.size() == 0 ||  m_alignments.back().Position <= i) ){
    m_alignments.push_back( BamTools::BamAlignment() );

    if( m_reader.GetNextAlignment(m_alignments.back()) == false ){
      read_all = true;
      m_alignments.pop_back(); // The last alignment is invalid
    }
  }
  
  if( m_alignments.size() == 0  && read_all)
    return '\0';

  // Find all alignments containing position i
  //std::string sum = "D";
  std::map<char, int> hash;
  for(int j=0; j < m_alignments.size(); j++){
    if( m_alignments[j].Position + m_alignments[j].AlignedBases.length() -1 < i || m_alignments[j].Position > i)
      break;

    int bp_idx = i - m_alignments[j].Position;
    hash[m_alignments[j].AlignedBases[ bp_idx ]]++; 
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

// Do we match the other sequence? It is important to note that a 'N' is considered a non match
bool BamSequence::isMatch(Sequence & other)
{
  char o_cur = other.begin();
  char cur = this->current();

  if( !o_cur || !cur){
    return false;
  }
  
  retrieve(other.length());
  std::deque<char>::iterator it = m_ring.begin();
  
  int moved = 0;
  bool match = true;
  // Walk along both strings, breaking when we have no match
  do {
    if( o_cur != cur ){
      match = false;
      break;
    }

    moved++;
    cur = *it++;
  } while( o_cur = other.next() );
  // If we've made it without any of them not matching match will be true

  // Rewind other
  other.begin();
  
  return match;
}
