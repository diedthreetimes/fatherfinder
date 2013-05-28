// ***********************************************************************************                         
// Sequence.h (c) 2013 Sky Faber                                               
// SPROUT Lab, Department of Computer Science, UC: Irvine                                              
// -----------------------------------------------------------------------------------         
// Last modified:  April 2013 (SF)                                       
// -----------------------------------------------------------------------------------
// Additive Elgamal implementation of the encryption api
// *********************************************************************************** 

#ifndef _ELGAMAL_H
#define _ELGAMAL_H
#include "encryption.h"
#include <gmpxx.h>


class Elgamal_SecretKey: public SecretKey {
public:
  Elgamal_SecretKey() {}

public:
  mpz_class h,g,q,x,p;

  virtual int serialize(char * buffer, int size);
  virtual int deserialize(const char * buffer, const int length);
};


class Elgamal_PublicKey : public PublicKey {
  public:
    Elgamal_PublicKey() {}

    Elgamal_PublicKey(Elgamal_SecretKey sk): h(sk.h), g(sk.g), q(sk.q), p(sk.p){ }
    virtual ~Elgamal_PublicKey() {}


    mpz_class h,g,q,p;

    virtual int serialize(char * buffer, int size);
    virtual int deserialize(const char * buffer, const int length);

};


class Elgamal_Encryption : public Encryption {
 public:
  Elgamal_Encryption() {}
  Elgamal_Encryption(Elgamal_Encryption & o) : c1(o.c1), c2(o.c2) {}
  Elgamal_Encryption(mpz_class _c1, mpz_class _c2) : c1(_c1), c2(_c2) {}

  virtual void encrypt(const std::string msg, const PublicKey * pk);
  virtual void encrypt(const char msg, const PublicKey * pk);
  virtual bool isZero(const SecretKey * sk);
  virtual std::string decrypt(const Encryption * e, const PublicKey * pk){
    throw "Additive Elgamal does not support decryption";
  }

  virtual Encryption * plus(Encryption * o);
  virtual Encryption * mult(mpz_class o);

  virtual int serialize(char * buffer, int size);
  virtual int deserialize(const char * buffer, const int length, const PublicKey * pk);

  mpz_class c1, c2; // TODO: Make this protected and the test case a friend
  
 private:
  mpz_class p;
  virtual void encrypt(const char * msg, const unsigned int length, const Elgamal_PublicKey * pk);
};

class Elgamal : public EncryptionScheme {
 public:
  Elgamal(unsigned int security);

  // TODO: These should probably return status instead of void
  virtual void GenerateKeys(PublicKey * pk, SecretKey * sk);
  virtual void GenerateKeys(Elgamal_PublicKey * pk, Elgamal_SecretKey * sk);
  
  static gmp_randclass rr;
private:
  unsigned int security;
};



#endif
