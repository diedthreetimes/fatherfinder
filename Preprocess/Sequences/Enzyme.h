// ***************************************************************************                         
// Enzyme.h (c) 2011 Sky Faber                                               
// SPROUT Lab, Department of Computer Science, UC: Irvine                                              
// ---------------------------------------------------------------------------         
// Last modified:  October 2011 (SF)                                       
// ---------------------------------------------------------------------------
// This is a sequence with special attributes to represent a digestion enczme
//    namely name and "cut"
// *************************************************************************** 
//TODO: Make this a templated class on a type of AbstractSequence (thus we can store our enzymes in something other than string)
#ifndef _ENZYME_H
#define _ENZYME_H

#include "StringSequence.h"
#include <string>

class Enzyme : public StringSequence {
  
 public:
  typedef StringSequence Super;
  // Constructor / Destructor
  Enzyme(std::string s, const char name [], int cut);

  // New api functionality
  std::string first(void){ return m_first; }
  std::string last(void){ return m_last; }
  std::string name(void){ return m_name; }

  virtual void PrintSelf(std::ostream& s){
    Super::PrintSelf(s);
    s << "First: " << m_first << std::endl;
    s << "Last: " << m_last << std::endl;
  }

 // Data members
 private:
  std::string m_name;
  std::string m_last;
  std::string m_first;
  
};
#endif //_ENZYME_H
