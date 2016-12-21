// Threaded two-dimensional Discrete FFT transform
// YOUR NAME HERE
// ECE8893 Project 2


#include <iostream>
#include <string>
#include <math.h>
#include <signal.h>
#include <pthread.h>
#include "Complex.h"
#include "InputImage.h"

// You will likely need global variables indicating how
// many threads there are, and a Complex* that points to the
// 2d image being transformed.

using namespace std;
Complex* dat_ptr ;


int N; //N=1024
int h,w;//Height and width
int elementCount = 0;
int nThreads=16;
Complex* dat_result = new Complex[1024*1024];
Complex* dat_transpose = new Complex[1024*1024];
Complex* dat_new2d = new Complex[1024*1024];
Complex* dat_original = new Complex[1024*1024];


pthread_mutex_t startCountMutex;
pthread_mutex_t exitMutex;
pthread_mutex_t elementCountMutex;
pthread_cond_t exitCond;
int startCount;




// Function to reverse bits in an unsigned integer
// This assumes there is a global variable N that is the
// number of points in the 1D transform.
unsigned ReverseBits(unsigned v)
{ //  Provided to students
  unsigned n = 1024; // Size of array (which is even 2 power k value)
  unsigned r = 0; // Return value
   
  for (--n; n > 0; n >>= 1)
    {
      r <<= 1;        // Shift return value
      r |= (v & 0x1); // Merge in next bit
      v >>= 1;        // Shift reversal value
    }
  return r;
}

// GRAD Students implement the following 2 functions.
// Undergrads can use the built-in barriers in pthreads.

// Call MyBarrier_Init once in main
void MyBarrier_Init()// you will likely need some parameters)
{
  pthread_mutex_init(&exitMutex,0);
  pthread_mutex_init(&startCountMutex,0);
  pthread_cond_init(&exitCond, 0);
  pthread_mutex_lock(&exitMutex);
  pthread_cond_wait(&exitCond,&exitMutex);
}

// Each thread calls MyBarrier after completing the row-wise DFT
void MyBarrier() // Again likely need parameters
{
    pthread_mutex_lock(&startCountMutex);
    startCount--;
    cout<<"Start count: "<<startCount<<endl;
    if (startCount == 0)
    { // Last to exit, notify main
    cout<<"Last thread reached"<<endl;
    pthread_mutex_unlock(&startCountMutex);
    pthread_mutex_lock(&exitMutex);
    pthread_cond_signal(&exitCond);
    pthread_mutex_unlock(&exitMutex);
    }

    else
    {
      pthread_mutex_unlock(&startCountMutex);
    }
}
                    

void Transform1D(Complex* h, int N)
{

  // Implement the efficient Danielson-Lanczos DFT here.
  // "h" is an input/output parameter
  // "N" is the size of the array (assume even power of 2) 

 Complex* x = new Complex[N];
 // Complex* dat_final = new Complex[N];


 for (int i = 0; i < N; ++i)
 {
   x[ReverseBits(i)] = h[i];
 }
 
 
 for (int i = 1; i <=log2(N); ++i){
   int Num_blocks = N/pow(2,i);
   

   for (int j=0; j<Num_blocks;++j){
    Complex W(0.0,0.0);

    // int k_w =0 
    int n_w = N/Num_blocks;
    Complex* temp = new Complex[n_w] ;
    
    for (int k = 0; k < n_w; ++k){
      //int ind2 = pow(2,i)*j+k;
      // cout<<"K: "<<ind2<<endl;
      
      Complex W = Complex((double)cos(2*M_PI*k/n_w),(double)(-1.0)*sin(2*M_PI*k/n_w)); 

      //int index = ind2+ pow(2,i-1);

      // cout<< "index:  "<<index<<endl;

      
      if (k < n_w/2)
      temp[k]  = x[j*n_w+k] + W*x[j*n_w+k+n_w/2];
      else
      temp[k]  = x[j*n_w+k-n_w/2] + (W*x[j*n_w+k]);
      
      // k_w++;


    }
    for (int u = 0; u < n_w; u++)
    {
      x[j*n_w + u] = temp[u];
    }

    delete temp;
   } 
 }

  for (int i = 0; i < N; i++) {
    h[i] = x[i];
  }

}

void* Transform2DTHread(void* v)
{ // This is the thread startign point.  "v" is the thread number
  // Calculate 1d DFT for assigned rows
  // wait for all to complete
  // Calculate 1d DFT for assigned columns
  // Decrement active count and signal main if all complete
  

  unsigned long myID = (unsigned long) v;
  
  

  int rowsPerThread = h/nThreads;

  Complex* dat_test = new Complex[N];
  
  int startrow = myID*rowsPerThread;


  for (int i = startrow; i < startrow + rowsPerThread; i++){
   
    for (int j = 0; j < w; j++){
      dat_test[j] = dat_ptr[i*w+j];   
    }

    Transform1D(dat_test,N);
    
    double a = 0.0001;
    Complex val = Complex(0.0,0.0);

    for (int j = 0; j<w; j++) {
      if(dat_test[j].Mag().real < a ){
        dat_test[j]= val;
      }
      dat_result[i*w+j]=dat_test[j];
    }
    
}
   
  MyBarrier(); 
  
return 0;




}


// ///////////////////////////////////////////////// Inverse functions//////////////////////////////////////////////////////////////


void inverseTransform1D(Complex* h, int N)
{

  // Implement the efficient Danielson-Lanczos DFT here.
  // "h" is an input/output parameter
  // "N" is the size of the array (assume even power of 2) 

 Complex* x = new Complex[N];
 // Complex* dat_final = new Complex[N];


 for (int i = 0; i < N; ++i)
 {
   x[ReverseBits(i)] = h[i];
 }
 
 
 for (int i = 1; i <=log2(N); ++i){
   int Num_blocks = N/pow(2,i);
   

   for (int j=0; j<Num_blocks;++j){
    Complex W(0.0,0.0);

    // int k_w =0 
    int n_w = N/Num_blocks;
    Complex* temp = new Complex[n_w] ;
    
    for (int k = 0; k < n_w; ++k){
      //int ind2 = pow(2,i)*j+k;
      // cout<<"K: "<<ind2<<endl;
      
      Complex W = Complex((double)cos(2*M_PI*k/n_w),(double)(1.0)*sin(2*M_PI*k/n_w)); 

      //int index = ind2+ pow(2,i-1);

      // cout<< "index:  "<<index<<endl;

      
      if (k < n_w/2)
      temp[k]  = x[j*n_w+k] + W*x[j*n_w+k+n_w/2];
      else
      temp[k]  = x[j*n_w+k-n_w/2] + (W*x[j*n_w+k]);
      
      // k_w++;


    }
    for (int u = 0; u < n_w; u++)
    {
      x[j*n_w + u] = temp[u];
    }

    delete temp;
   } 
 }

  for (int i = 0; i < N; i++) {
    h[i].real = (double)x[i].real/1024.0;
    h[i].imag = (double)x[i].imag/1024.0;
  }

}

void* inverseTransform2DTHread(void* v)
{ // This is the thread startign point.  "v" is the thread number
  // Calculate 1d DFT for assigned rows
  // wait for all to complete
  // Calculate 1d DFT for assigned columns
  // Decrement active count and signal main if all complete
  

  unsigned long myID = (unsigned long) v;
  
  

  int rowsPerThread = h/nThreads;

  Complex* dat_test = new Complex[N];
  
  int startrow = myID*rowsPerThread;


  for (int i = startrow; i < startrow + rowsPerThread; i++){
   
    for (int j = 0; j < w; j++){
      dat_test[j] = dat_new2d[i*w+j];   
    }

    inverseTransform1D(dat_test,N);
    
    double a = 0.0001;
    Complex val = Complex(0.0,0.0);

    for (int j = 0; j<w; j++) {
      if(dat_test[j].Mag().real < a ){
        dat_test[j]= val;
      }
      dat_result[i*w+j]=dat_test[j];
    }
    
}
   
  MyBarrier(); 
  
return 0;




}









/////////////////////////////////////////////////////////////////////////////////////////////////

void Transform2D(const char* inputFN) { // Do the 2D transform here.

  // Create the global pointer to the image array data
  // Create 16 threads
  // Wait for all threads complete
  // Write the transformed data
  
  InputImage image(inputFN);  // Create the helper object for reading the image
  dat_ptr = image.GetImageData();

  h = image.GetHeight();
  w = image.GetWidth();
  N = h;
  
  startCount = nThreads;

  for (int i = 0; i <nThreads; ++i){
    pthread_t pt;
    pthread_create(&pt,0,Transform2DTHread,(void*)i);
  }
  
  MyBarrier_Init();
   

  image.SaveImageData("MyAfter1d.txt",dat_result,N,N);


// ////////////////////////////// Starting Transpose////////////////////////////////////////////////////////////////


// Transposing the resultant matrix
  for (int i = 0; i < h; i++){
    for (int j = 0; j < w; j++){
      dat_ptr[j * w + i] = dat_result[i * w + j];

    }
  }



startCount = nThreads;

for (int i = 0; i <nThreads; ++i){
    pthread_t pt1;
    pthread_create(&pt1,0,Transform2DTHread,(void*)i);
  }
  
   MyBarrier_Init();
  

  double a = 0.0001;
  Complex val = Complex(0.0,0.0);

   // Transposing the resultant matrix
  for (int i = 0; i < h; i++){
    
    for (int j = 0; j < w; j++){

      dat_new2d[j * w + i] = dat_result[i * w + j];

       
    }
  }



  image.SaveImageData("MyAfter2d.txt",dat_new2d,N,N);


// //////////////////////////////////// Starting 1D inverse//////////////////////////////////////////////////////////////////

startCount = nThreads;

for (int i = 0; i <nThreads; ++i){
    pthread_t pt2;
    pthread_create(&pt2,0,inverseTransform2DTHread,(void*)i);
  }
  
   MyBarrier_Init();

// image.SaveImageData("Myrandom.txt",dat_result,N,N);

// Transposing the resultant matrix
  for (int i = 0; i < h; i++){
    for (int j = 0; j < w; j++){
      dat_new2d[j * w + i] = dat_result[i * w + j];
    }
  }


//////////////////////////////////////Starting 2D inverse//////////////////////////////////////////////////

startCount = nThreads;

for (int i = 0; i <nThreads; ++i){
    pthread_t pt3;
    pthread_create(&pt3,0,inverseTransform2DTHread,(void*)i);
  }
  
   MyBarrier_Init();




// Transposing the resultant matrix
  for (int i = 0; i < h; i++){
    for (int j = 0; j < w; j++){
      dat_original[j * w + i] = dat_result[i * w + j];

    }
  }


 image.SaveImageData("Myoriginal.txt",dat_original,N,N);

  
}






int main(int argc, char** argv)
{
  string fn("Tower.txt"); // default file name
  if (argc > 1) fn = string(argv[1]);  // if name specified on cmd line
  
  Transform2D(fn.c_str()); // Perform the transform.
}  
  

  
