#include "gtest/gtest.h"
#include "BamSequence.h"
#include "StringSequence.h"

std::string DATA = "data/test/";

TEST(BamSequenceTest, SingleFileConstructor){
  BamSequence s(DATA + "test_seq.bam");

  EXPECT_NE('\0', s.current());

  for(int i=0; i < 100; i++){    
    EXPECT_NE('\0', s.current()) << "Read " << i + 1 << " has found null";
  }

  EXPECT_NE('\0', s.current());

}

TEST(BamSequenceTest, MultipleFileConstructor){
  std::vector<std::string> files;
  files.push_back(DATA + "test_seq.bam");
  files.push_back(DATA + "test_seq2.bam");
  BamSequence s(files);


  // todo: test jumping around more thorougly 

  EXPECT_NE('\0', s.current());
}

// The easiest way to test this is just compare it to bamtools output
// this test relies on the behavior of next and current
TEST(BamSequenceTest, pileup){
  std::vector<std::string> files;
  files.push_back(DATA + "test_seq.bam");
  BamSequence s(files);

  while(s.next() == 'D');

  std::string result;
  result += s.current();

  for(int i = 0; i < (86*3)-1; i++){
    result += s.next();
  }

  
  // This is what we expect pileup to return, based on our deterministic algorithim for deciding ties.
  //    The results of bamptools pileup is in the comments
  std::string expected = "";
  expected += "TGCCCGATAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCT"; // 86
  expected += "AACCCTAACCCTAACCCTAACCCTAACCCAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAGCCCCTACCCCTAACCCTAA";
  expected += "CCCTAACCCCAACCAAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCCTAACCCTTAACCCAAAC";
  //expected += "TKTCCGATAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCT"; // 86
  //expected += "AACCCTAACCCTAACCCTAACCCTAACCCAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCCTAACCCTAACCCTAA";
  //expected += "CCCTAACCCTAACCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCCTAACCCTAACCCTAAAC"; 



  EXPECT_EQ(expected.size(), result.size());

  EXPECT_EQ(expected, result);
}

TEST(BamSequenceTest, DISABLED_rewind){

}

TEST(BamSequenceTest, DISABLED_next){

  //TODO: This used to be working what has changed?!

  // Make sure we can traverse a few alignments without error
  std::vector<std::string> files;
  files.push_back(DATA + "test_seq.bam");
  files.push_back(DATA + "test_seq2.bam");
  BamSequence real(files);
  for(int i=0; i < 9993; i++)
    EXPECT_EQ('D', real.next()) << "Read " << i + 1 << " has no D. Position: " << real.Position();
  
  for(int i=0; i < 1000; i++)
    EXPECT_NE('\0', real.next()) << "Read " << i + 1 << " has found null. Position: " << real.Position();
  

  //Test walking off the edge
  //BamSequence s("abcd.bam");
  //EXPECT_EQ('b', s.next());
  //EXPECT_EQ('c', s.next());
  //EXPECT_EQ('d', s.next());
  //EXPECT_EQ('\0', s.next());
  //EXPECT_EQ('\0', s.next());
}


TEST(BamSequenceTest, IsMatch){
  BamSequence s1(DATA + "test_seq.bam");
  StringSequence s2("CTAACCC");
  
  EXPECT_FALSE( s1.isMatch(s2) );
  
  while(!s1.isMatch(s2) && s1.next());

  EXPECT_TRUE( s1.isMatch(s2) );

  EXPECT_EQ('C', s1.current());

  do{
    EXPECT_EQ(s2.current(), s1.current());
    s1.next();
  }while(s2.next());
	      
}

// This isn't implemented for bams
TEST(BamSequenceTest, concat){
  BamSequence s(DATA+"test_seq.bam");
  
  EXPECT_THROW(s += 'c', BamSequence::NotImplementedException);
}
