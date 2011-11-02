// ***************************************************************************                         
// Sequence.h (c) 2011 Sky Faber                                               
// SPROUT Lab, Department of Computer Science, UC: Irvine                                              
// ---------------------------------------------------------------------------         
// Last modified:  October 2011 (SF)                                       
// ---------------------------------------------------------------------------
// Provides a standard (abstract) api for sequencial genomic access                                       
// *************************************************************************** 
#include "Sequence.h"

// length (instantiate a trivial counting using rewind and next) (and possibly a member)

//TODO: Make this leave 'other' where it is
bool Sequence::isMatch(Sequence& other)
{
  char o_cur = other.begin();
  char cur = this->current();

  if( !o_cur || !cur ){
    return false;
  }
  
  int moved = 0;
  bool match = true;
  // Walk along both strings, breaking when we have no match
  do {
    if( o_cur != cur ){
      match = false;
      break;
    }

    moved++;
    cur = this->next();
  } while( o_cur = other.next() );
  // If we've made it without any of them not matching match will be true

  // Rewind both strings
  this->rewind(moved);
  other.begin();
  
  return match;
}
