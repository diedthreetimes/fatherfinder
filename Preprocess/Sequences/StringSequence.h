// ***************************************************************************                         
// StringSequence.h (c) 2011 Sky Faber                                               
// SPROUT Lab, Department of Computer Science, UC: Irvine                                              
// ---------------------------------------------------------------------------         
// Last modified:  October 2011 (SF)                                       
// ---------------------------------------------------------------------------
// Provides a concrete impelmentation of the Sequence api via a string
// *************************************************************************** 
#ifndef _STRING_SEQUENCE_H
#define _STRING_SEQUENCE_H
#include <string>
#include <ostream>
#include "Sequence.h"
class StringSequence : public Sequence {
  
 public:
  // Constructor / Destructor
  StringSequence(void): m_seq(""), m_idx(0) {}
  StringSequence(std::string s): m_seq(s), m_idx(0) {}
  virtual ~StringSequence() {}

  //pTODO: Copy Constructor
  
  // Implement the virtual functions
  virtual char current();
  virtual char next();
  virtual char rewind(int n);
  virtual int  length();

  virtual Sequence& concat(const Sequence& other);
  virtual Sequence& concat(const char& c);
  
  virtual void PrintSelf(std::ostream& s){
    s << "String: " << m_seq << std::endl;;
    s << "Current: " << m_idx << std::endl;
  }

 // Data members
 private:
  std::string m_seq;
  // Invariant - m_idx is always within bounds of the string
  int m_idx;
  
};
#endif //_STRING_SEQUENCE_H
