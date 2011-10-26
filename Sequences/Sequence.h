// ***************************************************************************                         
// Sequence.h (c) 2011 Sky Faber                                               
// SPROUT Lab, Department of Computer Science, UC: Irvine                                              
// ---------------------------------------------------------------------------         
// Last modified:  October 2011 (SF)                                       
// ---------------------------------------------------------------------------
// Provides a standard (abstract) api for sequencial genomic access                                       
// *************************************************************************** 
#ifdef  _SEQUENCE_H
#define _SEQUENCE_H
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
  virtual void rewind(int n) =0;  
  //TODO: hasNext()

  // ------------------------------
  // Compare sequences
  // ------------------------------

  // Does other match self as rooted at the current BP?
  //
  // The following is considered a match 
  // other:
  // AGGT
  //
  // self:    
  // ATTACGACTAGGTA
  //          ^ <-current  
  virtual bool isMatch(Sequence* other)=0;

  //TODO: Implement current/hasNext using next and a private next_item +more
}

#endif //_SEQUENCE_H
