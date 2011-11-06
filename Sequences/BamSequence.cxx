//Implement an abstract sequence via bamtools
// This reader will handle all statistical processing
// For now this is a very naive pileup

// Some things to consider
//   - Map Quality?


// There is a lot of "interesting" fields in both the bam and bas files,
//   but the literature on it seems to expect at least some prior knowledge
//   for now we just do the naieve thing and ignore it all (hoping our algorithim provides enough resilience)


#include "BamSequence.h"
#include "iostream"

// TODO: propogate errors up to the main program
BamSequence::BamSequence(const std::string & file)
{
  if( !m_reader.OpenFile(file) ){
    std::cerr << "Could not open input BAM files: " << file << std::endl;
    EXIT_FAILURE;
  }

  m_idx = 1;

  // TODO: remove this init when switching to a 'correct' enumerating model
  next();
}

BamSequence::BamSequence(const std::vector<std::string> & files)
{
  if( !m_reader.Open(files) ){
    std::cerr << "Could not open input BAM files: " << &files[0] << std::endl;
    EXIT_FAILURE;
  }
  m_idx = 1;

  // TODO: remove this init (see above)
  next();
}

BamSequence::~BamSequence()
{
  m_reader.Close();
}

char BamSequence::current()
{
  return m_alignment.AlignedBases[m_idx];
}

char BamSequence::next()
{  
  if(m_alignment.Length < m_idx) {
    m_reader.GetNextAlignment(m_alignment);
    m_idx = -1;
  }
  
  m_idx++;

  return m_alignment.AlignedBases[m_idx];
  // int i, j = 0;

  // while ( reader.GetNextAlignment(al) ) {

  //   if( j < 10 )
  //     cout << al.Position << ":" << al.AlignedBases << ":" << al.AlignedBases.length() << endl;

  //   j += 1;
  //   i += al.AlignedBases.length();
  // }

  // cout << "Made: " << i << " reads." << endl;
  // reader.Close();
}

char BamSequence::rewind(int n)
{
  return '\0';
}
