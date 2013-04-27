#include "gtest/gtest.h"
#include "elgamal.h"

TEST(ElgamalTest, Encrypt){
  EncryptionScheme enc = Elgamal();
  PublicKey pk;
  SecretKey sk;

  uint security = 1024;

  enc.GenerateKeys(security,pk,sk);

  
  msg = "";
  enc.encrypt(msg,pk);
  
  
}

TEST(ElgamalTest, TestZero){

}

TEST(ElgamalTest, TestMult){

}

TEST(ElgamalTest, TestAdd){

}
