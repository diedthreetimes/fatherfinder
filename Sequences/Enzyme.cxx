// ***************************************************************************                         
// Enzyme.cpp (c) 2011 Sky Faber                                               
// SPROUT Lab, Department of Computer Science, UC: Irvine                                              
// ---------------------------------------------------------------------------         
// Last modified:  October 2011 (SF)                                       
// ---------------------------------------------------------------------------
// This is a sequence with special attributes to represent a digestion enczme
//    namely name and "cut"
// *************************************************************************** 
#include "Enzyme.h"


// Here cut is how to break s into 2 pieces
// If cut > s.size and out_of_range exception will be thrown
// cut is zero based
Enzyme::Enzyme(std::string s, const char name [], int cut) : Super(s)
{
  m_name = name;

  m_last = s.substr(cut);
  m_first = s.substr(0, cut);
}
