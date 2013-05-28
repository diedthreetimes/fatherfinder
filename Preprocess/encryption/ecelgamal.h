// ***********************************************************************************                         
// Sequence.h (c) 2013 Sky Faber                                               
// SPROUT Lab, Department of Computer Science, UC: Irvine                                              
// -----------------------------------------------------------------------------------         
// Last modified:  April 2013 (SF)                                       
// -----------------------------------------------------------------------------------
// Additive Elgamal implementation of the encryption api
// *********************************************************************************** 

#ifndef _ECELGAMAL_H
#define _ECELGAMAL_H
#include "encryption.h"

#include <openssl/obj_mac.h>
#include <openssl/ec.h>
#include <gmpxx.h>

#include <iostream>


class ECElgamal_SecretKey: public SecretKey {
public:
  // As it is now copying this will cause a multiple free error;
  // TODO: Provide a copy constructor
  ECElgamal_SecretKey(){key = NULL;}
  ~ECElgamal_SecretKey(){ if(key != NULL) EC_KEY_free(key); } 
public:
  EC_KEY *key;

  virtual int serialize(char * buffer, int size);
  virtual int deserialize(const char * buffer, const int length);
};


class ECElgamal_PublicKey : public PublicKey {
  public:
    ECElgamal_PublicKey();
    virtual ~ECElgamal_PublicKey();

    ECElgamal_PublicKey(const ECElgamal_SecretKey &sk);
    ECElgamal_PublicKey(const ECElgamal_PublicKey &pk);
    
    void init(const ECElgamal_SecretKey &sk);

    EC_GROUP *group;
    EC_POINT *h;
    

    virtual int serialize(char * buffer, int size);
    virtual int deserialize(const char * buffer, const int length);

};


class ECElgamal_Encryption : public Encryption {
 public:
  ECElgamal_Encryption();
  virtual ~ECElgamal_Encryption();
  ECElgamal_Encryption(ECElgamal_Encryption & o)  { std::cerr << "NEED TO IMPLEMENT CONSTRUCTOR" << std::endl;}
  ECElgamal_Encryption(EC_POINT * _c1, EC_POINT * _c2)  { std::cerr << "NEED TO IMPLEMENT COPY" << std::endl;}

  virtual void encrypt(const std::string msg, const PublicKey * pk);
  virtual void encrypt(const char msg, const PublicKey * pk);
  virtual bool isZero(const SecretKey * sk);
  virtual std::string decrypt(const Encryption * e, const PublicKey * pk){
    throw "Additive ECElgamal does not support decryption";
  }

  virtual Encryption * plus(Encryption * o);
  virtual Encryption * mult(mpz_class o);

  virtual int serialize(char * buffer, int size);
  virtual int deserialize(const char * buffer, const int length, const PublicKey * pk);

  EC_POINT * c1, *c2; // TODO: Make this protected and the test case a friend
  
 private:
  EC_GROUP * group;
  virtual void encrypt(const char * msg, const unsigned int length, const ECElgamal_PublicKey * pk);
};

class ECElgamal : public EncryptionScheme {
 public:
  ECElgamal(unsigned int security);

  // TODO: These should probably return status instead of void
  virtual void GenerateKeys(PublicKey * pk, SecretKey * sk);
  virtual void GenerateKeys(ECElgamal_PublicKey * pk, ECElgamal_SecretKey * sk);

private:
  unsigned int security;
};



#endif
