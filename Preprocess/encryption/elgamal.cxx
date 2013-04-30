#include "elgamal.h"
#include <iostream>

#define DEBUG true

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

  mpz_powm(tmp.get_mpz_t(),c2.get_mpz_t(), o.get_mpz_t(), p.get_mpz_t());
  c2 = tmp;

  return this;
}
