// ***************************************************************************                         
// InsertionSequence.h (c) 2011 Sky Faber                                               
// SPROUT Lab, Department of Computer Science, UC: Irvine                                              
// ---------------------------------------------------------------------------         
// Last modified:  October 2011 (SF)                                       
// ---------------------------------------------------------------------------
// Provides a concrete impelmentation of the Sequence api via bamfiles
// *************************************************************************** 
#ifndef _INSERTION_SEQUENCE_H
#define _INSERTION_SEQUENCE_H
#include <ostream>
#include <string>
#include <exception>


#include "BamSequence.h"

class InsertionSequence : public BamSequence {
 public:
  // Constructor / Destructor
  InsertionSequence(const std::string & file) : BamSequence(file){ }
  InsertionSequence(const std::vector<std::string> & files) : BamSequence(files){ }
  ~InsertionSequence() {}

  // TODO: Copy Constructor
  
  // Overide some of the defaults
  // virtual char begin(); // TODO: do we need to overide this ?
  virtual char next();
  //virtual char retrieve(int n);

 protected:
  virtual char calc_position(int i);
  
};
#endif //_INSERTION_SEQUENCE_H
