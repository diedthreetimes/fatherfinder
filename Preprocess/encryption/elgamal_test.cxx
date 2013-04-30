#include "gtest/gtest.h"
#include "elgamal.h"

TEST(ElgamalTest, GenerateKeys){
  EncryptionScheme * enc = new Elgamal(512);
  Elgamal_PublicKey* pk = new Elgamal_PublicKey();
  Elgamal_SecretKey* sk = new Elgamal_SecretKey();
  Encryption* e = new Elgamal_Encryption();

  enc->GenerateKeys(pk,sk);

  EXPECT_EQ(sk->g, pk->g);
  EXPECT_EQ(sk->p, pk->p);
  EXPECT_EQ(sk->q, pk->q);

  EXPECT_NE(sk->g, 1);
  EXPECT_NE(sk->p, 1);
  EXPECT_NE(sk->q, 1);
  EXPECT_NE(sk->x, 1);

  // Check that it is a safe prime
  EXPECT_EQ((sk->p-1)/2, sk->q);
    
  mpz_class res;
  // Check that h is computed correctly
  mpz_powm(res.get_mpz_t(), pk->g.get_mpz_t(), sk->x.get_mpz_t(), pk->p.get_mpz_t());
  EXPECT_EQ(res, pk->h);

  // Check that g is a generator
  mpz_powm(res.get_mpz_t(), sk->g.get_mpz_t(), sk->q.get_mpz_t(), sk->p.get_mpz_t());
  EXPECT_EQ(res, 1);

  delete pk, sk, enc;
}

TEST(ElgamalTest, Encrypt){
  EncryptionScheme * enc = new Elgamal(512);
  PublicKey* pk = new Elgamal_PublicKey();
  SecretKey* sk = new Elgamal_SecretKey();
  Encryption* e = new Elgamal_Encryption();

  enc->GenerateKeys(pk,sk);
  
  std::string msg = "hi there";
  e->encrypt(msg,pk);
  EXPECT_FALSE(e->isZero(sk));
  
  e->encrypt((char)0,pk);

  EXPECT_TRUE(e->isZero(sk));
  
  
  delete pk, sk, enc, e;
}

TEST(ElgamalTest, TestMultAdd){
  EncryptionScheme * enc = new Elgamal(512);
  PublicKey* pk = new Elgamal_PublicKey();
  SecretKey* sk = new Elgamal_SecretKey();
  Encryption* e = new Elgamal_Encryption();
  Encryption* e1 = new Elgamal_Encryption();

  enc->GenerateKeys(pk,sk);

  e->encrypt(5,pk);
  e1->encrypt(5,pk);
  
  e1->mult(-1);
  e->plus(e1);
  EXPECT_TRUE(e->isZero(sk));
  
  char msg = 18;
  e->encrypt(msg,pk);
  EXPECT_FALSE(e->isZero(sk));
  
  e1->encrypt(2,pk);
  e1->plus(e);
  
  e->encrypt(20,pk);
  e->mult(-1);

  e->plus(e1);
  EXPECT_TRUE(e->isZero(sk));

  
  delete pk, sk, enc, e, e1;
}
