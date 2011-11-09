// ***************************************************************************                         
// StringSequence.h (c) 2011 Sky Faber                                               
// SPROUT Lab, Department of Computer Science, UC: Irvine                                              
// ---------------------------------------------------------------------------         
// Last modified:  October 2011 (SF)                                       
// ---------------------------------------------------------------------------
// Provides a concrete impelmentation of the Sequence api via bamfiles
// *************************************************************************** 
#ifndef _BAM_SEQUENCE_H
#define _BAM_SEQUENCE_H
#include <ostream>
#include <string>
#include <exception>
#include <deque>
// #include <boost/circular_buffer.hpp>

#include "Sequence.h"
#include <api/BamMultiReader.h>
#include <api/BamAlignment.h>

class BamSequence : public Sequence {
 // Exceptions
 public:
  // TODO: Add in 'which method' info
  class NotImplementedException : public std::exception {
    virtual const char* what() const throw()
    {
      return "Method not implemnted.";
    }
  }notImplemented;
  
 public:
  // Constructor / Destructor
  BamSequence(const std::string & file);
  BamSequence(const std::vector<std::string> & files);
  virtual ~BamSequence();

  // TODO: Copy Constructor
  
  // Implement the virtual functions
  virtual char current();
  virtual char next();
  virtual char rewind(int n);

  virtual Sequence& concat(const Sequence& other){ throw notImplemented; };
  virtual Sequence& concat(const char& c) {throw notImplemented; };
  
  virtual void PrintSelf(std::ostream& s){
    //s << m_alignment.Position << " : "<< m_idx << std::endl;
    //s << m_alignment.AlignedBases << std::endl;
    //s << m_alignment.AlignedBases.length() << std::endl;
    //s << m_alignment.QueryBases << std::endl;
    //s << m_alignment.QueryBases.length() << std::endl;
    //s << m_alignment.Length << std::endl << std::endl;
  }
  
  int Position(void){ return cur_pos; }

 // Data members
 private:
  BamTools::BamMultiReader m_reader;
  
  bool read_all;
  char m_cur;
  int cur_pos;
  std::deque<char> m_ring;
  //boost::circular_buffer<char> m_ring;
  std::deque<BamTools::BamAlignment> m_alignments;

 // Util functions
 private:
  void retrieve(int n);
  void init();
  char calc_position(int i);
};
#endif //_BAM_SEQUENCE_H
