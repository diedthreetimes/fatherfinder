#include "gtest/gtest.h"
#include "BamSequence.h"
#include "StringSequence.h"

#define DATA test/data/


TEST(BamSequenceTest, SingleFileConstructor){
  BamSequence s("test_seq.bam");

  s.PrintSelf(std::cout);

  EXPECT_NE('\0', s.current());
}

TEST(BamSequenceTest, DISABLED_MultipleFileConstructor){
  std::vector<std::string> files;
  files.push_back("test_seq.bam");
  BamSequence s(files);
}

// The easiest way to test this is just compare it to bamtools output
TEST(BamSequenceTest, DISABLED_pileup){
  std::vector<std::string> files;
  files.push_back("test_seq.bam");
  BamSequence s(files);

  std::string result;
  result += s.current();
  for(int i = 0; i < 60; i++){
    result += s.next();
  }
  
  // TODO: Fill this up with the actual data
  std::string expected = "";
  expected += "12345678901234567890";
  expected += "12345678901234567890";
  expected += "12345678901234567890";

  EXPECT_EQ(expected, result);
}

TEST(BamSequenceTest, DISABLED_rewind){

}

TEST(BamSequenceTest, DISABLED_next){
  // Make sure we can't walk off the string
  // BamSequence e("empty.bam");

  // bool res = true;
  // if(e.next()){
  //   res = false;
  // }

  // EXPECT_EQ(true, res);

  BamSequence s("DATAabcd.bam");
  EXPECT_EQ('b', s.next());
  EXPECT_EQ('c', s.next());
  EXPECT_EQ('d', s.next());
  EXPECT_EQ('\0', s.next());
  EXPECT_EQ('\0', s.next());
  //EXPECT_EQ(false, !!s.next()); //Passes but annoying warning
}


TEST(BamSequenceTest, DISABLED_IsMatch){
  BamSequence s1("DATAtest_seq.bam");//"ATTACGACTAGGTA");
  StringSequence s2("AGGTA");
  
  EXPECT_FALSE( s1.isMatch(s2) );
  
  while(!s1.isMatch(s2) && s1.next());

  EXPECT_TRUE( s1.isMatch(s2) );

  EXPECT_EQ('A', s1.current());

  do{
    EXPECT_EQ(s2.current(), s1.current());
    s1.next();
  }while(s2.next());
	      
}

// This isn't implemented for bams
TEST(BamSequenceTest, concat){
  BamSequence s("DATAtest_seq.bam");
  
  EXPECT_THROW(s += 'c', BamSequence::NotImplementedException);
}
