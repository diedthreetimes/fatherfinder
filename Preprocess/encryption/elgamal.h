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
};


class Elgamal_PublicKey : public PublicKey {
  public:
    Elgamal_PublicKey() {}

    Elgamal_PublicKey(Elgamal_SecretKey sk): h(sk.h), g(sk.g), q(sk.q), p(sk.p){ }


    mpz_class h,g,q,p;
};


class Elgamal_Encryption : public Encryption {
 public:
  Elgamal_Encryption();
};

class Elgamal : public EncryptionScheme {
 public:
  Elgamal(unsigned int security);

  // TODO: These should probably return status instead of void
  virtual void GenerateKeys(PublicKey * pk, SecretKey * sk);
  virtual void GenerateKeys(Elgamal_PublicKey * pk, Elgamal_SecretKey * sk);
  virtual void encrypt(const std::string msg, const PublicKey * pk, Encryption * e);

private:
  gmp_randclass rr;
  unsigned int security;
};



#endif
