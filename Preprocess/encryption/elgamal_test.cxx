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

  e->encrypt('T',pk);
  e1->encrypt('T',pk);
  e1->mult(-1);
  e->plus(e1);
  EXPECT_TRUE(e->isZero(sk));
  
  delete pk, sk, enc, e, e1;
}

TEST(ElgamalTest, Serialize){
  EncryptionScheme * enc = new Elgamal(1024);
  PublicKey* pk = new Elgamal_PublicKey();
  SecretKey* sk = new Elgamal_SecretKey();
  Elgamal_Encryption* e = new Elgamal_Encryption();
  Elgamal_Encryption* e1 = new Elgamal_Encryption();

  enc->GenerateKeys(pk,sk);
  
  std::string msg = "hi there";
  e->encrypt(msg,pk);
  EXPECT_FALSE(e->isZero(sk));
  
  char *buffer = new char[512];
  int length = e->serialize(buffer, 512);
  EXPECT_NE( length, -1 );

  EXPECT_EQ(-1, e1->deserialize(buffer, 2, pk) );

  ASSERT_EQ( length, e1->deserialize(buffer, length, pk) );
  
  EXPECT_EQ(e->c1, e1->c1);
  EXPECT_EQ(e->c2, e1->c2);

  delete [] buffer;
  
  buffer = new char[10];
  length = e->serialize(buffer, 10);
  EXPECT_EQ( length, -1);
  
  delete [] buffer;

  // Test serializing the keys
  Elgamal_PublicKey* pk1 = new Elgamal_PublicKey();
  Elgamal_SecretKey* sk1 = new Elgamal_SecretKey();

  buffer = new char [255];
  length = pk->serialize(buffer, 512);
  
  ASSERT_NE( length, -1 );
 
  EXPECT_EQ(length, pk1->deserialize(buffer, length));
  EXPECT_EQ(((Elgamal_PublicKey *)pk)->p, pk1->p);  
  EXPECT_EQ(((Elgamal_PublicKey *)pk)->g, pk1->g);
  EXPECT_EQ(((Elgamal_PublicKey *)pk)->q, pk1->q);
  EXPECT_EQ(((Elgamal_PublicKey *)pk)->h, pk1->h);
  
  delete [] buffer;
  
  buffer = new char [512];
  length = sk->serialize(buffer, 512);
  
  ASSERT_NE( length, -1 );
 
  EXPECT_EQ(length, sk1->deserialize(buffer, length));
  EXPECT_EQ(((Elgamal_SecretKey *)sk)->p, sk1->p);  
  EXPECT_EQ(((Elgamal_SecretKey *)sk)->q, sk1->q);  
  EXPECT_EQ(((Elgamal_SecretKey *)sk)->g, sk1->g);
  EXPECT_EQ(((Elgamal_SecretKey *)sk)->h, sk1->h);
  EXPECT_EQ(((Elgamal_SecretKey *)sk)->x, sk1->x);

  delete pk, sk, enc, e, e1, pk1, sk1;
}
