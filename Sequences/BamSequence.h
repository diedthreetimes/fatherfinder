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
#include "Sequence.h"

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
  BamSequence(std::string file);
  BamSequence(std::string files []);
  virtual ~BamSequence();

  // TODO: Copy Constructor
  
  // Implement the virtual functions
  virtual char current();
  virtual char next();
  virtual char rewind(int n);

  virtual Sequence& concat(const Sequence& other){ throw notImplemented; };
  virtual Sequence& concat(const char& c) {throw notImplemented; };
  
  virtual void PrintSelf(std::ostream& s){
  }

 // Data members
 private:
 
  
};
#endif //_BAM_SEQUENCE_H
