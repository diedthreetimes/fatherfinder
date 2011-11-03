#include "gtest/gtest.h"
#include "BamSequence.h"
#include "StringSequence.h"

// Tests factorial of positive numbers.
TEST(BamSequenceTest, ArgumentsConstructor) {
  
}

TEST(BamSequenceTest, SingleFile){

}

TEST(BamSequenceTest, MultipleFile){

}

// This is private test it later
//TEST(BamSequenceTest, pileup){
//
//}

TEST(BamSequenceTest, next){
  // Make sure we can't walk off the string
  BamSequence e("empty.bam");

  bool res = true;
  if(e.next()){
    res = false;
  }

  EXPECT_EQ(true, res);

  StringSequence s("abcd.bam");
  EXPECT_EQ('b', s.next());
  EXPECT_EQ('c', s.next());
  EXPECT_EQ('d', s.next());
  EXPECT_EQ('\0', s.next());
  EXPECT_EQ('\0', s.next());
  //EXPECT_EQ(false, !!s.next()); //Passes but annoying warning
}


TEST(BamSequenceTest, IsMatch){
  BamSequence s1("test_seq.bam");//"ATTACGACTAGGTA"); // TODO: Load from file
  StringSequence s2("AGGTA");
  
  EXPECT_FALSE( s1.isMatch(s2) );
  
  while(!s1.isMatch(s2))
    s1.next();

  EXPECT_TRUE( s1.isMatch(s2) );

  EXPECT_EQ('A', s1.current());

  do{
    EXPECT_EQ(s2.current(), s1.current());
    s1.next();
  }while(s2.next());
	      
}


// This isn't implemented for bams
TEST(BamSequenceTest, concat){
  BamSequence s("empty.bam");
  caught = false
  try{
    s += 'c';
  }catch( BamSequence::NotImplementedException e& ){
    caught = true;
  }
}
