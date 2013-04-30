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

#include <string>
class Key {
};

class PublicKey : public Key{

};

class SecretKey : public Key {

};

class Encryption {

 public:
  virtual ~Encryption() {}
  
  // public:

};


class EncryptionScheme {

  public:
    virtual ~EncryptionScheme(){}

  public:
    virtual void GenerateKeys(PublicKey * pk, SecretKey * sk) = 0;
    virtual void encrypt(const std::string msg, const PublicKey * pk, Encryption * e) = 0;

};

#endif
