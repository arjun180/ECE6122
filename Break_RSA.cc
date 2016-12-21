#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>

#include "RSA_Algorithm.h"

using namespace std;
mpz_class n32Attack,d32Attack,C32Attack,M32Attack,MN32Attack;
mpz_class p32Attack = 1,q32Attack;
mpz_class e32Attack;

void g (mpz_class& x,mpz_class& N) {
  x = (x * x + 1) % N;
}

//////////////////////////////////////////////////
unsigned char val(char ch){
  if('0' <= ch && ch <= '9'){ 
    return ch      - '0'; 
  }
  if('a' <= ch && ch <= 'f'){
    return ch + 10 - 'a'; 
  }
  if('A' <= ch && ch <= 'F'){
    return ch + 10 - 'A'; 
  }    
}

string decode(string const & s){
  if ((s.size() % 2) != 0){ 
   cout<<"invalid"<<endl; 
  }
  string res;
  res.reserve(s.size() / 2);
  for (size_t i = 0; i < s.size() / 2; ++i){
    unsigned char temp = val(s[2 * i]) * 16 + val(s[2 * i + 1]);
    res += temp;
  }
  return res;
}




//////////////////////////////////////////////////

int main(int argc, char** argv)
{ // Arguments are as follows:
  //argv[1] = n;
  //argv[2] = e;  // n and e are the public key
  //argv[3] = first 6 characters of encrypted message
  //argv[4] = next 6 characters .. up to argv[12] are the lsat 6 characters
  // The number of arguments will always be exacty 12, and each argument past the
  // public key contain 6 ascii characters of the encrypted message.
  // Each of the 32bit values in the argv[] array are right justified in the
  // low order 48 bits of each unsigned long.  The upper 16 bits are always
  // zero, which insures the each unsigned long is < n (64 bits) and therefore
  // the RSA encryption will work.

  // Below is an example of the BreakRSA and command line arguments:

// ./BreakRSA  2966772883822367927 2642027824495698257  817537070500556663 1328829247235192134 
// 1451942276855579785 2150743175814047358 72488230455769594 1989174916172335943 962538406513796755 
// 1069665942590443121 72678741742252898 1379869649761557209

//   The corect output from the above is:
//   HelloTest  riley CekwkABRIZFlqmWTanyXLogFgBUENvzwHpEHRCZIKRZ
//
//   The broken (computed) private key for the above is 4105243553



  // Our one and only RSA_Algorithm object
  RSA_Algorithm rsa;
  
  // First "break" the keys by factoring n and computing d
  // Set the keys in the rsa object afer calculating d
  rsa.n = mpz_class(argv[1]);
  rsa.e = mpz_class(argv[2]);
  // /////////////////////////////////////////////////////////////////

            n32Attack = rsa.n;
            d32Attack = rsa.e;
            
            p32Attack = 1;

          mpz_class x_fixed = 2;
          mpz_class cycle_size = 2;
          mpz_class x = 2;
          while (p32Attack == 1) 
          {
            mpz_class count = 1;
            while (count <= cycle_size && p32Attack == 1) 
            {
              g(x,n32Attack);
              count = count + 1;
              mpz_class temp = x-x_fixed;
              mpz_gcd (p32Attack.get_mpz_t(), temp.get_mpz_t(), n32Attack.get_mpz_t());
            }
           
            if (p32Attack != 1)
              break;
           
            cycle_size = 2 * cycle_size;
            x_fixed = x;
          }
          q32Attack = n32Attack/p32Attack;

          cout<<"q is:"<<q32Attack.get_mpz_t()<<endl;
          mpz_class phinew = (p32Attack-1)*(q32Attack-1);
          cout<<"phi new is: "<<phinew.get_mpz_t();
          mpz_invert(e32Attack.get_mpz_t(), d32Attack.get_mpz_t(), phinew.get_mpz_t());
          cout<<" is : "<<e32Attack.get_mpz_t()<<endl;



//////////////////////////////////////////////////////////////////////////
  // Set rsa.d to the calculated private key d
  rsa.d = e32Attack;

  for (int i = 3; i < 13; ++i)
    { // Decrypt each set of 6 characters
      mpz_class c(argv[i]);
      mpz_class m = rsa.Decrypt(c,rsa.d,rsa.n);
      //  use the get_ui() method in mpz_class to get the lower 48 bits of the m
      unsigned long ul = m.get_ui();

      std::stringstream ss;
      std::string inp;

      ss << std::hex << std::setfill('0')<<std::setw(12)<<ul;
      ss >> inp;

      std::string asc = decode(inp);

      std::cout <<asc;



      
      // Now print the 6 ascii values in variable ul.  As stated above the 6 characters
      // are in the low order 48 bits of ui.
    }
  cout << endl;
}