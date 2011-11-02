#include "gtest/gtest.h"
#include "StringSequence.h"

// Tests factorial of positive numbers.
TEST(StringSequenceTest, ArgumentsConstructor) {
  StringSequence s1("hi");
  EXPECT_EQ('h', s1.current());
  EXPECT_EQ('i', s1.next());

  StringSequence s2("");
  EXPECT_EQ('\0', s2.current());
}

TEST(StringSequenceTest, next){
  // Make sure we can't walk off the string
  StringSequence e("");
  
  bool res = true;
  if(e.next()){
    res = false;
  }

  EXPECT_EQ(true, res);

  StringSequence s("abcd");
  EXPECT_EQ('b', s.next());
  EXPECT_EQ('c', s.next());
  EXPECT_EQ('d', s.next());
  EXPECT_EQ('\0', s.next());
  EXPECT_EQ('\0', s.next());
  //EXPECT_EQ(false, !!s.next()); //Passes but annoying warning
}

//TODO: Finish testing this class and method
TEST(StringSequenceTest, IsMatch){
  StringSequence s1("ATTACGACTAGGTA");
  StringSequence s2("AGGTA");
  
  
  EXPECT_FALSE( s1.isMatch(s2) );

  while(!s1.isMatch(s2))
    s1.next();

  EXPECT_TRUE( s1.isMatch(s2) );

  do{
    EXPECT_EQ(s2.current(), s1.current());
  }while(s2.next());
	      
}
