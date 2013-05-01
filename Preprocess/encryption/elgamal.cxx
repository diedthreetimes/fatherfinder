#include "elgamal.h"
#include <iostream>

#define DEBUG true

enum { SERIAL_E, SERIAL_PK, SERIAL_SK };

// This should be global, although we reseed every time Elgamal is called
gmp_randclass Elgamal::rr( gmp_randinit_default );

Elgamal::Elgamal(unsigned int security) {
  rr.seed(time(NULL));
  this->security = security;
}

void Elgamal::GenerateKeys(PublicKey * pk, SecretKey * sk){
  // This may not be the best stratdegy for typing the public keys, but this works
  // This allows the type of the encrypion algorithim to not be known, however perhaps typdefs
  //   are more suitable
  GenerateKeys((Elgamal_PublicKey *)pk, (Elgamal_SecretKey *) sk);
}

void Elgamal::GenerateKeys(Elgamal_PublicKey * pk, Elgamal_SecretKey * sk){
  
  // First Generate the keys
  do {
    do {
      sk->q = rr.get_z_bits(security/2);
    }while(mpz_probab_prime_p(sk->q.get_mpz_t(),100) == 0);

    sk->p = (2*sk->q)+1;
  } while(mpz_probab_prime_p(sk->p.get_mpz_t(),100) == 0);

  // Find the generator
  mpz_class res;
  do {
    sk->g = rr.get_z_range(sk->p);
    if(sk->g == 1)
      continue;
    
    mpz_powm(res.get_mpz_t(), sk->g.get_mpz_t(), sk->q.get_mpz_t(), sk->p.get_mpz_t());
  } while(res != 1);
  
  
  sk->x = rr.get_z_range(sk->q);
  
  mpz_powm(sk->h.get_mpz_t(), sk->g.get_mpz_t(), sk->x.get_mpz_t(), sk->p.get_mpz_t());

  *pk = *sk;
}

void Elgamal_Encryption::encrypt(const std::string msg, const PublicKey * pk) {
  encrypt(msg.c_str(),msg.length(), (Elgamal_PublicKey *)pk);
}
void Elgamal_Encryption::encrypt(const char msg, const PublicKey * pk) {
  encrypt(&msg, 1, (Elgamal_PublicKey *)pk);
}

void Elgamal_Encryption::encrypt(const char * msg, const unsigned int length, const Elgamal_PublicKey * pk) {
  p = pk->p; // save for later

  mpz_class m;
  mpz_import(m.get_mpz_t(), length, 1, 1, 0, 0, msg);
  
  mpz_class r = Elgamal::rr.get_z_range(pk->p);
  
  // c1 = g^r
  mpz_powm(c1.get_mpz_t(), pk->g.get_mpz_t(), r.get_mpz_t(), pk->p.get_mpz_t());
  
  // c2 = h^r * g^m mod p
  mpz_class tmp; // g^m portion

  mpz_powm(c2.get_mpz_t(), pk->h.get_mpz_t(), r.get_mpz_t(), pk->p.get_mpz_t());
  mpz_powm(tmp.get_mpz_t(), pk->g.get_mpz_t(), m.get_mpz_t(), pk->p.get_mpz_t());
  c2 = ((c2 * tmp) % pk->p);
}

bool Elgamal_Encryption::isZero(const SecretKey * sk){
  mpz_class res;
  mpz_powm(res.get_mpz_t(), c1.get_mpz_t(), ((Elgamal_SecretKey *)sk)->x.get_mpz_t(), ((Elgamal_SecretKey *)sk)->p.get_mpz_t());
  return res == c2;
}

Encryption * Elgamal_Encryption::plus(Encryption * o){
  Elgamal_Encryption * _o = (Elgamal_Encryption *) o;
  
  c1 = (c1 * _o->c1) % p;
  c2 = (c2 * _o->c2) % p;

  return this;
}
Encryption * Elgamal_Encryption::mult(mpz_class o){
  mpz_class tmp;
  mpz_powm(tmp.get_mpz_t(), c1.get_mpz_t(), o.get_mpz_t(), p.get_mpz_t());
  c1 = tmp;

  mpz_powm(tmp.get_mpz_t(), c2.get_mpz_t(), o.get_mpz_t(), p.get_mpz_t());
  c2 = tmp;

  return this;
}

int Elgamal_Encryption::serialize(char * buffer, int size){
  //TODO: Remove redundant calls to mpz_sizeinbase
  int count;
  count  = (mpz_sizeinbase(c1.get_mpz_t(), 2) + 7) / 8;
  count += (mpz_sizeinbase(c2.get_mpz_t(), 2) + 7) / 8;

  if(count + 2 > size){
    return -1;
  }
  
  unsigned int offset = 0;
  *buffer = (char)((mpz_sizeinbase(c1.get_mpz_t(), 2) + 7) / 8);
  offset += 1;

  *(buffer+offset) = (char)((mpz_sizeinbase(c2.get_mpz_t(), 2) + 7) / 8);
  offset += 1;
  
  size_t tmp;
  mpz_export(buffer+offset, &tmp, 1, 1, 0, 0, c1.get_mpz_t());
  offset+= tmp;

  mpz_export(buffer+offset, &tmp, 1, 1, 0, 0, c2.get_mpz_t());
  offset+= tmp;

  return offset;
}
bool Elgamal_Encryption::deserialize(const char * buffer, const int length, const PublicKey * pk){
  p = ((Elgamal_PublicKey *)pk)->p;

  char length1, length2;
  unsigned int offset = 0;
  length1 = *(buffer+offset);
  offset += 1;
  length2 = *(buffer+offset);
  offset += 1;

  // We need more data then given to us;
  if(length1 + length2 + 2 > length){
    return false;
  }  

  mpz_import(c1.get_mpz_t(), length1, 1, 1, 0, 0, buffer+offset);
  offset+= length1;

  mpz_import(c2.get_mpz_t(), length2, 1, 1, 0, 0, buffer+offset);   
  return true;
}

int Elgamal_PublicKey::serialize(char * buffer, int size){
 //TODO: Remove redundant calls to mpz_sizeinbase
  int count;
  count  = (mpz_sizeinbase(h.get_mpz_t(), 2) + 7) / 8;
  count += (mpz_sizeinbase(g.get_mpz_t(), 2) + 7) / 8;
  count += (mpz_sizeinbase(q.get_mpz_t(), 2) + 7) / 8;
  count += (mpz_sizeinbase(p.get_mpz_t(), 2) + 7) / 8;

  if(count + 4 > size){
    return -1;
  }
  
  unsigned int offset = 0;
  *(buffer+offset) = (char)((mpz_sizeinbase(h.get_mpz_t(), 2) + 7) / 8);
  offset += 1;
  *(buffer+offset) = (char)((mpz_sizeinbase(g.get_mpz_t(), 2) + 7) / 8);
  offset += 1;
  *(buffer+offset) = (char)((mpz_sizeinbase(q.get_mpz_t(), 2) + 7) / 8);
  offset += 1;
  *(buffer+offset) = (char)((mpz_sizeinbase(p.get_mpz_t(), 2) + 7) / 8);
  offset += 1;

  
  size_t tmp;
  mpz_export(buffer+offset, &tmp, 1, 1, 0, 0, h.get_mpz_t());
  offset+= tmp;
  mpz_export(buffer+offset, &tmp, 1, 1, 0, 0, g.get_mpz_t());
  offset+= tmp;
  mpz_export(buffer+offset, &tmp, 1, 1, 0, 0, q.get_mpz_t());
  offset+= tmp;
  mpz_export(buffer+offset, &tmp, 1, 1, 0, 0, p.get_mpz_t());
  offset+= tmp;

  return offset;
}
int Elgamal_SecretKey::serialize(char * buffer, int size){
   //TODO: Remove redundant calls to mpz_sizeinbase
  int count;
  count  = (mpz_sizeinbase(h.get_mpz_t(), 2) + 7) / 8;
  count += (mpz_sizeinbase(g.get_mpz_t(), 2) + 7) / 8;
  count += (mpz_sizeinbase(q.get_mpz_t(), 2) + 7) / 8;
  count += (mpz_sizeinbase(p.get_mpz_t(), 2) + 7) / 8;
  count += (mpz_sizeinbase(x.get_mpz_t(), 2) + 7) / 8;

  if(count + 5 > size){
    return -1;
  }
  
  unsigned int offset = 0;
  *(buffer+offset) = (char)((mpz_sizeinbase(h.get_mpz_t(), 2) + 7) / 8);
  offset += 1;
  *(buffer+offset) = (char)((mpz_sizeinbase(g.get_mpz_t(), 2) + 7) / 8);
  offset += 1;
  *(buffer+offset) = (char)((mpz_sizeinbase(q.get_mpz_t(), 2) + 7) / 8);
  offset += 1;
  *(buffer+offset) = (char)((mpz_sizeinbase(p.get_mpz_t(), 2) + 7) / 8);
  offset += 1;
  *(buffer+offset) = (char)((mpz_sizeinbase(x.get_mpz_t(), 2) + 7) / 8);
  offset += 1;

  
  size_t tmp;
  mpz_export(buffer+offset, &tmp, 1, 1, 0, 0, h.get_mpz_t());
  offset+= tmp;
  mpz_export(buffer+offset, &tmp, 1, 1, 0, 0, g.get_mpz_t());
  offset+= tmp;
  mpz_export(buffer+offset, &tmp, 1, 1, 0, 0, q.get_mpz_t());
  offset+= tmp;
  mpz_export(buffer+offset, &tmp, 1, 1, 0, 0, p.get_mpz_t());
  offset+= tmp;
  mpz_export(buffer+offset, &tmp, 1, 1, 0, 0, x.get_mpz_t());
  offset+= tmp;

  
  return offset;
}

bool Elgamal_PublicKey::deserialize(const char * buffer, const int length){
  char lengths [4];
  unsigned int offset = 0;
  
  int total = 0;
  for(int i=0; i < 4; i++){
    lengths[i] = *(buffer+offset);
    offset += 1;

    total += lengths[i];
  }

  // We need more data then given to us;
  if(total + offset > length){
    return false;
  }  

  mpz_import(h.get_mpz_t(), lengths[0], 1, 1, 0, 0, buffer+offset);
  offset+= lengths[0];
  mpz_import(g.get_mpz_t(), lengths[1], 1, 1, 0, 0, buffer+offset);
  offset+= lengths[1];
  mpz_import(q.get_mpz_t(), lengths[2], 1, 1, 0, 0, buffer+offset);
  offset+= lengths[2];
  mpz_import(p.get_mpz_t(), lengths[3], 1, 1, 0, 0, buffer+offset);
  offset+= lengths[3];

  return true;
}
bool Elgamal_SecretKey::deserialize(const char * buffer, const int length){
  char lengths [5];
  unsigned int offset = 0;
  
  int total = 0;
  for(int i=0; i < 5; i++){
    lengths[i] = *(buffer+offset);
    offset += 1;

    total += lengths[i];
  }

  // We need more data then given to us;
  if(total + offset > length){
    return false;
  }  

  mpz_import(h.get_mpz_t(), lengths[0], 1, 1, 0, 0, buffer+offset);
  offset+= lengths[0];
  mpz_import(g.get_mpz_t(), lengths[1], 1, 1, 0, 0, buffer+offset);
  offset+= lengths[1];
  mpz_import(q.get_mpz_t(), lengths[2], 1, 1, 0, 0, buffer+offset);
  offset+= lengths[2];
  mpz_import(p.get_mpz_t(), lengths[3], 1, 1, 0, 0, buffer+offset);
  offset+= lengths[3];
  mpz_import(x.get_mpz_t(), lengths[4], 1, 1, 0, 0, buffer+offset);
  offset+= lengths[4];

  return true;
}
