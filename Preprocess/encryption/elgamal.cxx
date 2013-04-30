#include "elgamal.h"
#include <iostream>

#define DEBUG true

Elgamal::Elgamal(unsigned int security): rr(gmp_randinit_default) {
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
  
  std::cout << res << std::endl;
  
  sk->x = rr.get_z_range(sk->q);
  
  mpz_powm(sk->h.get_mpz_t(), sk->g.get_mpz_t(), sk->x.get_mpz_t(), sk->p.get_mpz_t());

  *pk = *sk;
}

void Elgamal::encrypt(const std::string msg, const PublicKey * pk,Encryption * e) {
  
  std::cout << "Encrypting";
}

Elgamal_Encryption::Elgamal_Encryption(){

}
