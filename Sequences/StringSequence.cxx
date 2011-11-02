// ***************************************************************************                         
// StringSequence.h (c) 2011 Sky Faber                                               
// SPROUT Lab, Department of Computer Science, UC: Irvine                                              
// ---------------------------------------------------------------------------         
// Last modified:  October 2011 (SF)                                       
// ---------------------------------------------------------------------------
// Provides a concrete string impelmentation of the Sequence api
// *************************************************************************** 
#include "StringSequence.h"

char StringSequence::current()
{
  return m_seq[m_idx];
}

char StringSequence::next()
{
  m_idx++;
  if(m_idx == m_seq.size() + 1)
    m_idx--;

  return current();
}

char StringSequence::rewind(int n)
{
  if( n < 0 || m_idx < n )
    m_idx = 0;
  else{
    m_idx = m_idx - n;
  }

  return current();
}

// TODO: Make this work for a generic seuqence
Sequence& StringSequence::concat(const Sequence& other)
{
  m_seq += ((StringSequence *)&other)->m_seq;
}

Sequence& StringSequence::concat(const char& c)
{
  m_seq += c;
}

