// ***********************************************************************************                         
// Sequence.h (c) 2013 Sky Faber                                               
// SPROUT Lab, Department of Computer Science, UC: Irvine                                              
// -----------------------------------------------------------------------------------         
// Last modified:  April 2013 (SF)                                       
// -----------------------------------------------------------------------------------
// Provides a standard (abstract) api for public key additively homomorphic encrytpion
// *********************************************************************************** 

#ifndef _ENCRYPTION_H
#define _ENCRYPTION_H

#include <gmpxx.h>
#include <string>
class Key {
 public:
  virtual int serialize(char * buffer, int size) = 0;
  virtual int deserialize(const char * buffer, const int length) = 0;
};

class PublicKey : public Key{
 public:
  virtual ~PublicKey() { }
};

class SecretKey : public Key{
 public:
  virtual ~SecretKey() {}
};

class Encryption {

 public:
  virtual ~Encryption() {}
  
  virtual void encrypt(const char msg, const PublicKey * pk) = 0;
  virtual void encrypt(const std::string msg, const PublicKey * pk) = 0;
  virtual bool isZero(const SecretKey * sk) = 0; 
  virtual std::string decrypt(const Encryption * e, const PublicKey * pk) = 0;

  // These should be subclassed as a "additive" encryption
  // Homomorphic functions (these are all mutable)
  virtual Encryption * plus(Encryption * o) = 0;
  virtual Encryption * mult(mpz_class o) = 0;

  virtual int serialize(char * buffer, int size) = 0;
  virtual int deserialize(const char * buffer, const int length, const PublicKey * pk) = 0;
  
};


class EncryptionScheme {

  public:
    virtual ~EncryptionScheme(){}

  public:
    virtual void GenerateKeys(PublicKey * pk, SecretKey * sk) = 0;
};

#endif
