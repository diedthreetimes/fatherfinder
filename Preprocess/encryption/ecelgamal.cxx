#include "ecelgamal.h"
#include <iostream>
#include <openssl/err.h>

#define DEBUG true

enum { SERIAL_E, SERIAL_PK, SERIAL_SK };

// How to print an OpenSSL error
// ERR_load_crypto_strings();
// std::cout << ERR_reason_error_string(ERR_get_error()) << std::endl;


//TODO: Convert c style error codes to c++ error handleing (throw)
ECElgamal::ECElgamal(unsigned int security) {
  if(security != 160){
    std::cerr << "Only 160 bit curves are supported" << std::endl;
    exit(-1);
  }
  this->security = security;
}

void ECElgamal::GenerateKeys(PublicKey * pk, SecretKey * sk){
  // This may not be the best stratdegy for typing the public keys, but this works
  // This allows the type of the encrypion algorithim to not be known, however perhaps typdefs
  //   are more suitable
  GenerateKeys((ECElgamal_PublicKey *)pk, (ECElgamal_SecretKey *) sk);
}

void ECElgamal::GenerateKeys(ECElgamal_PublicKey * pk, ECElgamal_SecretKey * sk){
  EC_KEY *key;
  
  if(NULL== (key = EC_KEY_new_by_curve_name(NID_secp160r1))){
    std::cerr << "Curve not supported or not found" << std::endl;
    exit(-1);
  }
  
  if(1 != EC_KEY_generate_key(key)){
    std::cerr << "Key generation failed" << std::endl;
    exit(-1);
  }
  
  
  sk->key = key;
  
  pk->init(*sk);
}

void ECElgamal_Encryption::encrypt(const std::string msg, const PublicKey * pk) {
  encrypt(msg.c_str(),msg.length(), (ECElgamal_PublicKey *)pk);
}
void ECElgamal_Encryption::encrypt(const char msg, const PublicKey * pk) {
  encrypt(&msg, 1, (ECElgamal_PublicKey *)pk);
}

void ECElgamal_Encryption::encrypt(const char * msg, const unsigned int length, const ECElgamal_PublicKey * pk) {
  group = pk->group; // save for later
  
  BIGNUM * m = NULL;
  BIGNUM * r = NULL;
  BIGNUM *order = NULL;
  BN_CTX * ctx = NULL;

  c1 = EC_POINT_new(group);
  c2 = EC_POINT_new(group);
  r = BN_new();
  //  m = BN_new();

  if(!(order = BN_new())) std::cerr << "can't init" << std::endl;
  //  if((ctx = BN_CTX_new()) == NULL) std::cerr << "Cant make context" << std::endl;

  if(!EC_GROUP_get_order(group, order, ctx)) std::cerr << "can't get order" << std::endl;

  char * buffer = new char[length+4];

  // OpenSSL has BS endianess
  unsigned char bytes[4];
  bytes[0] = (length >> 24) & 0xFF;
  bytes[1] = (length >> 16) & 0xFF;
  bytes[2] = (length >>  8) & 0xFF;
  bytes[3] =  length        & 0xFF;

  memcpy(buffer, bytes, 4);
  memcpy(buffer+4, msg, length);
  
  if((m =  BN_mpi2bn((unsigned char *)(buffer), length+4, m)) == NULL)
    std::cerr << "BN msg conversion failed" << std::endl;  

  if(m == NULL)
    std::cerr << "BN m is still nULL" << std::endl;
  
  if(1!= BN_rand_range(r, order)) std::cerr << "rand failed" << std::endl;
  
  if(!EC_POINT_mul(group, c1, r, NULL, NULL, NULL))
    std::cerr << "g^r failed" << std::endl;

  if(!EC_POINT_mul(group,c2, m, pk->h, r, NULL))
    std::cerr << "h^r * g^m failed" << std::endl;		   
  BN_free(r);
  BN_free(m);
}

bool ECElgamal_Encryption::isZero(const SecretKey * sk){
  EC_POINT * res = EC_POINT_new(this->group);
 
  
  const BIGNUM * priv_key = EC_KEY_get0_private_key(((ECElgamal_SecretKey *)sk)->key);
  if(1!=EC_POINT_mul(group,res,NULL,c1,priv_key,NULL))
    std::cerr << "Mult error in isZero" << std::endl;

  bool ans = (EC_POINT_cmp(group,res,c2,NULL) == 0);
  EC_POINT_free(res);
  return ans;
}

Encryption * ECElgamal_Encryption::plus(Encryption * o){
  ECElgamal_Encryption * _o = (ECElgamal_Encryption *) o;
  
  EC_POINT * res = EC_POINT_new(this->group);
  if(1!= EC_POINT_add(group,res,c1,_o->c1, NULL))
    return NULL;

  if(!EC_POINT_copy(this->c1, res)){
    std::cerr << "Error" << std::endl;
    return NULL;
  }


  if(1!= EC_POINT_add(group,res,c2,_o->c2, NULL))
    return NULL;

  if(!EC_POINT_copy(this->c2, res)){
    std::cerr << "Error" << std::endl;
    return NULL;
  }


  EC_POINT_free(res);  
    
  return this;
}
Encryption * ECElgamal_Encryption::mult(mpz_class o){

  bool negative = false;
  if(o < 0){
    o= o*-1;
    negative = true;
  }
    
  std::string tmp = o.get_str(16);
  
  //BIGNUM * a = hex2bn(tmp.c_str()); // TODO: Try to do BN_hex2b(&a, tmp.c_str());
  BIGNUM *a = NULL;
  if(0 == BN_hex2bn(&a, tmp.c_str()) ){
    std::cerr << "hex2bn fail here" << std::endl;
  }

  if(negative)
    ((a)->neg = (!((a)->neg)) & 1);

  if(a == NULL)
    std::cerr << "Not loaded" << std::endl;
  
  EC_POINT * res = EC_POINT_new(this->group);
  
  if(res == NULL){
    std::cerr << "Error" << std::endl;
    return NULL;
  }

  
  EC_POINT_mul(group, res, NULL, this->c1, a, NULL);
  // TODO: We may need to do a free and new here first
  if(!EC_POINT_copy(this->c1, res)){
    std::cerr << "Error" << std::endl;
    return NULL;
  }
   

  EC_POINT_mul(group, res, NULL, this->c2, a, NULL);
  // TODO: We may need to do a free and new here first
  if(!EC_POINT_copy(this->c2, res)){
    std::cerr << "Error" << std::endl;
    return NULL;
  }
  
  BN_free(a);
  EC_POINT_free(res);

  return this;
}

int ECElgamal_Encryption::serialize(char * buffer, int size){
  int count1, count2, count;
  count1 = EC_POINT_point2oct(group, c1, POINT_CONVERSION_COMPRESSED, NULL, 0, NULL);
  count2 = EC_POINT_point2oct(group, c2, POINT_CONVERSION_COMPRESSED, NULL, 0, NULL);
  count = count1 + count2;
  
  if(count + 2 > size){
    return -1;
  }

  int offset =0;
  *(buffer+offset) = (char)count1;
  offset += 1;
  *(buffer+offset) = (char)count2;
  offset += 1;
  
  EC_POINT_point2oct(group,c1,POINT_CONVERSION_COMPRESSED,(unsigned char*)buffer+offset,count1, NULL);
  offset += count1;
  EC_POINT_point2oct(group,c2,POINT_CONVERSION_COMPRESSED,(unsigned char*)buffer+offset,count2, NULL);
  offset += count2;

  return offset;
}
int ECElgamal_Encryption::deserialize(const char * buffer, const int length, const PublicKey * pk){
  group = ((ECElgamal_PublicKey *)pk)->group; // save for later (pk must remain in memory)

  
  char length1, length2;
  unsigned int offset = 0;
  length1 = *(buffer+offset);
  offset += 1;
  length2 = *(buffer+offset);
  offset += 1;

  // We need more data then given to us;
  if(length1 + length2 + 2 > length){
    return -1;
  }  

  if(c1 == NULL)
    c1 = EC_POINT_new(group);
  if(c2 == NULL)
    c2 = EC_POINT_new(group);

  if(!EC_POINT_oct2point(group,c1,(unsigned char *)buffer+offset,length1,NULL)){
    std::cerr << "Deser error" << std::endl;
    return -1;
  }
  offset+= length1;
  if(!EC_POINT_oct2point(group,c2,(unsigned char *)buffer+offset,length2,NULL)){
    std::cerr << "Deser error" << std::endl;
    return -1;
  }
  offset+= length2;
  
  return offset;
}

int ECElgamal_PublicKey::serialize(char * buffer, int size){
  return -1;
 // //TODO: Remove redundant calls to mpz_sizeinbase
 //  int count;
 //  count  = (mpz_sizeinbase(h.get_mpz_t(), 2) + 7) / 8;
 //  count += (mpz_sizeinbase(g.get_mpz_t(), 2) + 7) / 8;
 //  count += (mpz_sizeinbase(q.get_mpz_t(), 2) + 7) / 8;
 //  count += (mpz_sizeinbase(p.get_mpz_t(), 2) + 7) / 8;

 //  if(count + 4 > size){
 //    std::cout << count << std::endl;
 //    std::cout << size << std::endl;
 //    return -1;
 //  }
  
 //  unsigned int offset = 0;
 //  *(buffer+offset) = (char)((mpz_sizeinbase(h.get_mpz_t(), 2) + 7) / 8);
 //  offset += 1;
 //  *(buffer+offset) = (char)((mpz_sizeinbase(g.get_mpz_t(), 2) + 7) / 8);
 //  offset += 1;
 //  *(buffer+offset) = (char)((mpz_sizeinbase(q.get_mpz_t(), 2) + 7) / 8);
 //  offset += 1;
 //  *(buffer+offset) = (char)((mpz_sizeinbase(p.get_mpz_t(), 2) + 7) / 8);
 //  offset += 1;
  
 //  size_t tmp;
 //  mpz_export(buffer+offset, &tmp, 1, 1, 0, 0, h.get_mpz_t());
 //  offset+= tmp;
 //  mpz_export(buffer+offset, &tmp, 1, 1, 0, 0, g.get_mpz_t());
 //  offset+= tmp;
 //  mpz_export(buffer+offset, &tmp, 1, 1, 0, 0, q.get_mpz_t());
 //  offset+= tmp;
 //  mpz_export(buffer+offset, &tmp, 1, 1, 0, 0, p.get_mpz_t());
 //  offset+= tmp;

  // return offset;
}
int ECElgamal_SecretKey::serialize(char * buffer, int size){
  int count; 
  count = i2d_ECPrivateKey(key, NULL);

  if(count + 4 > size){
    return -1;
  }

  unsigned int offset = 0;
  memcpy(buffer+offset, &count, 4);
  offset += 4;

  unsigned char * tmp;
  tmp = ((unsigned char *)buffer+offset);
  
  i2d_ECPrivateKey(key, &tmp);
  offset+= count;
  
  return offset;
}

int ECElgamal_PublicKey::deserialize(const char * buffer, const int length){
  std::cerr << "Deserialize not implemented for ECElgamal_PK" << std::endl;
  return -1;
}
int ECElgamal_SecretKey::deserialize(const char * buffer, const int length){

  unsigned int offset = 0;
  int count = *((int*)(buffer+offset));
  offset += 4;
  
  if(count + offset > length){
    return -1;
  }

  const unsigned char ** tmp;
  *tmp = (unsigned char *)buffer+offset;
  key = d2i_ECPrivateKey(&key, tmp,length);
  offset+= count;

  
  return offset;
}
ECElgamal_PublicKey::ECElgamal_PublicKey() {
  group = NULL; 
  h = NULL;
}
ECElgamal_PublicKey::ECElgamal_PublicKey(const ECElgamal_PublicKey &pk){
  std::cout << "NOT IMPLEMENTED" << std::endl;
}
ECElgamal_PublicKey::ECElgamal_PublicKey(const ECElgamal_SecretKey & sk){
  init(sk);
}
void ECElgamal_PublicKey::init(const ECElgamal_SecretKey & sk){

  if(sk.key == NULL) {
    std::cerr << "COPY erorr" << std::endl;
    exit(-1);
  }
  
  group = NULL;
  // Copy the group
  const EC_METHOD *meth = EC_GROUP_method_of(EC_KEY_get0_group(sk.key));
  this->group = EC_GROUP_new(meth);
  if(this->group == NULL)
    std::cerr << "Group init error" << std::endl;
  if(!EC_GROUP_copy(this->group, EC_KEY_get0_group(sk.key)))
    std::cerr << "Group copy error" << std::endl;

  if(this->group == NULL)
    std::cout << "Still null :(" << std::endl;

  // Copy the public key  
  this->h = EC_POINT_new(this->group);
  if(this->h == NULL)
    std::cerr << "PK create error" << std::endl;
  if(!EC_POINT_copy(this->h, EC_KEY_get0_public_key(sk.key)))
    std::cerr << "PK copy error" << std::endl;
}
ECElgamal_PublicKey::~ECElgamal_PublicKey(){
  if(group != NULL)
    EC_GROUP_free(group);
  if(h != NULL)
    EC_POINT_free(h);
}

ECElgamal_Encryption::ECElgamal_Encryption(){
  //c1 = EC_POINT_new(group); 
  //c2 = EC_POINT_new(group);
  c1 = NULL;
  c2 = NULL;
}

ECElgamal_Encryption::~ECElgamal_Encryption(){
  if(c1) EC_POINT_free(c1); 
  if(c2) EC_POINT_free(c2);
}
