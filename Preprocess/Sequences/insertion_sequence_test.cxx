#include "gtest/gtest.h"
#include "InsertionSequence.h"
#include "BamSequence.h"


std::string DATA = "data/test/";

TEST(DISABLED_InsertionSequenceTest, FiltersInsertions){
  InsertionSequence s(DATA + "test_seq.bam");
  
  EXPECT_NE('T', s.current()); // TODO: Why is this needed (but not in pileup test ???
  EXPECT_NE('T', s.next());
  EXPECT_NE('G', s.next());
  EXPECT_NE('C', s.next());
  EXPECT_NE(9996, s.Position());
}
