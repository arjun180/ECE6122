// ECE4122/6122 RSA Encryption/Decryption assignment
// Fall Semester 2015

#include <iostream>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "RSA_Algorithm.h"

using namespace std;
//gmp_randclass k (gmp_randinit_default);
mpz_class p,q,phi,one,gcd,M,C,decrypt;

int main() {
 
  // int flag = 1;



  // Instantiate the one and only RSA_Algorithm object
 
  RSA_Algorithm RSA;

  // int flag = 1;
  
  for (int i = 32; i <= 1024; i=i*2){

    for (int l = 0; l < 10; ++l){
    
      while(1){
    
      // Generating random prime number p


      p = RSA.rng.get_z_bits(i);
      while (mpz_probab_prime_p(p.get_mpz_t(),100) == 0) {
        p = RSA.rng.get_z_bits(i);
      }

      // if(flag){
      //cout<<"The P: "<<p.get_mpz_t()<<endl;
      //cout<<"P has been generated correctly"<<endl;
      // }

      // Generating random number q

      q = RSA.rng.get_z_bits(i);
      while (mpz_probab_prime_p(q.get_mpz_t(),100) == 0) {
        q = RSA.rng.get_z_bits(i);
      }

      //cout<<"The Q: "<<q.get_mpz_t()<<endl;
      //cout<<"Q has been generated correctly"<<endl;    


      //  Producing n by multiplying p and q

      mpz_mul(RSA.n.get_mpz_t(),p.get_mpz_t(),q.get_mpz_t());

      // cout<<"The N: "<<n.get_mpz_t()<<endl;

      // Producing (p-1) and (q-1) and then multiplying them to produce phi

      mpz_set_ui(one.get_mpz_t(),1);
      mpz_sub(p.get_mpz_t(),p.get_mpz_t(),one.get_mpz_t());
      mpz_sub(q.get_mpz_t(),q.get_mpz_t(),one.get_mpz_t());  
      
      mpz_mul(phi.get_mpz_t(),p.get_mpz_t(),q.get_mpz_t());

      // cout<<"The phi: "<<phi.get_mpz_t()<<endl;


      // generating d

      // int flag_inside = 1;
      

      while(1){
        RSA.d = RSA.rng.get_z_bits(2*i);
        mpz_gcd(gcd.get_mpz_t(),RSA.d.get_mpz_t(),phi.get_mpz_t());

        if (gcd==1 && RSA.d<phi){
          //cout<<"Gcd is 1 now.Please break"<<endl;
          break;
        }
      }

    //cout<<"D is generated: "<<d.get_mpz_t()<<endl;
    // RSA.PrintND();
    //  Generating e

    int chk_Final =0;

    chk_Final = mpz_invert(RSA.e.get_mpz_t(),RSA.d.get_mpz_t(),phi.get_mpz_t());

    //cout<<"the check final is"<<e.get_mpz_t();
   
    if (!chk_Final){
      //cout<<"Chk_final is not correct.GO back and restart"<<endl;
    } else{

      RSA.PrintNDE();

      //  Compute the ciphertext message. Generating and then decrypting
        while(1){
          M = RSA.rng.get_z_bits(2*i);

          if (M<RSA.n) {
            break;
          }

        }
  
        // Computation

        mpz_powm(C.get_mpz_t(),M.get_mpz_t(),RSA.d.get_mpz_t(),RSA.n.get_mpz_t());

        mpz_powm(decrypt.get_mpz_t(),C.get_mpz_t(),RSA.e.get_mpz_t(),RSA.n.get_mpz_t());
        if(M == decrypt){
          RSA.PrintM(decrypt);
        } else {
          cout<<"Can't decrypt"<<endl;
        }
        RSA.PrintC(C);


        break;







      }


 

        }





      }

    }
  // Loop from sz = 32 to 1024 inclusive
  // for each size choose 10 different key pairs
  // For each key pair choose 10 differnt plaintext 
  // messages making sure it is smaller than n.
  // If not smaller then n then choose another
  // For eacm message encrypt it using the public key (n,e).
  // After encryption, decrypt the ciphertext using the private
  // key (n,d) and verify it matches the original message.

  // your code here
}