// ***************************************************************************                         
// Sequence.h (c) 2011 Sky Faber                                               
// SPROUT Lab, Department of Computer Science, UC: Irvine                                              
// ---------------------------------------------------------------------------         
// Last modified:  October 2011 (SF)                                       
// ---------------------------------------------------------------------------
// Provides a standard (abstract) api for sequencial genomic access                                       
// *************************************************************************** 
#ifndef  _SEQUENCE_H
#define _SEQUENCE_H
#include <ostream>
class Sequence {
  
// constructor / destructor
public:
  virtual ~Sequence() {}
  
// public interface
public:
  // ------------------------------
  // Navigate a sequence
  // ------------------------------
  
  // Returns the current base pair 
  virtual char current(void)=0;
  // Move to the next base pair in the sequence return null if none
  virtual char next(void)=0;
  // Move back n steps in the sequence. (Or -1 to return to the beggining)
  virtual char rewind(int n) =0;  
  // Move to the begining of the sequence;
  virtual char begin(){ return rewind(-1); }
  // Return the length of this sequence
  virtual int length()=0;
  //TODO: hasNext()
  //TODO: skip()
  //TODO: (think more about this, and maybe implement end) FIX sequences to start at pos -1 thus begin works better
  
  // ------------------------------
  // Mutate Sequences
  // ------------------------------
  // Add various data types to the end of this sequence
  virtual Sequence& concat(const Sequence & other)= 0;
  virtual Sequence& operator += (Sequence & other){ return this->concat(other); }
  virtual Sequence& concat(const char & other)= 0;
  virtual Sequence& operator += (const char& other){ return this->concat(other); }
  virtual Sequence& concat(const std::string o){ for(int i=0;i<o.size();i++) (*this)+=o[i]; }
  virtual Sequence& operator += (const std::string other){ return this->concat(other); }

  // ------------------------------
  // Compare sequences
  // ------------------------------

  // Does other match self as rooted at the current BP? //TODO: reword
  //
  // The following is considered a match 
  // other:
  // AGGT
  //
  // self:    
  // ATTACGACTAGGTA
  //          ^ <-current
  // Note: this has the affect of resetting other //TODO: fix this
  virtual bool isMatch(Sequence& other);

  // ------------------------------
  // Debug
  // ------------------------------
  virtual void PrintSelf(std::ostream &s){};
  // TODO: Overide caster to use PrintSelf in 'cout <<'

  //TODO: Implement current/hasNext using next and a private next_item +more
};

#endif //_SEQUENCE_H
