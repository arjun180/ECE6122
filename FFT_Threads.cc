// Distributed two-dimensional Discrete FFT transform
// Arjun Chakraborty
// ECE8893 Project 1


#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <signal.h>
#include <math.h>
#include <mpi.h>
#include <stdlib.h>

#include "Complex.h"
#include "InputImage.h"


using namespace std;

//  The resultant files are MyAfter1D.txt and MyAfter2D.txt
//  1D transform 
void Transform1D(Complex* dat_ptr, int w,Complex* H,int numrows ) {


  for (int i = 0; i < numrows; i++) {
    
    for (int j = 0; j < w; j++) {
      
      H[i*w + j] = Complex(0,0);
      
      for (int p = 0; p < w; p++) {
       
        //  Computation of W
         Complex W = Complex(cos(2*M_PI*j*p/w), -sin(2*M_PI*j*p/w));  
        
        H[i*w + j] = H[i*w + j] + dat_ptr[i*w + p] * W;
     
      }
    }
  }

}


Complex* Transform2D(const char* inputFN,int rank,int numtasks) { 
  InputImage image(inputFN);  // Create the helper object for reading the image
  

  //  Computation of width and height
  int h = image.GetHeight();  
  int w = image.GetWidth();

  int numrows  = h/numtasks;

 
  Complex* D_rank = new Complex[numrows * h];
  Complex* Dbuf = new Complex[numrows * w];
  Complex* ret1D = new Complex[numrows * h];
  Complex* res1D = new Complex[numrows * h];
  Complex* dat_ptr = new Complex[ h * w];
  Complex* H = new Complex[w * h];
  Complex dat_new[w][h];
  Complex* dat_final = new Complex[w * h];
  

  if (rank==0){
    
    dat_ptr = image.GetImageData();

    // Converting 1D array into 2D array
    for (int i = 0; i < w; i++) {
      for (int j = 0; j < h; j++) {
        dat_new[i][j] = dat_ptr[ i * w + j ];
      }
    }

    int rc, offsetRows;

    // Using MPI send and sendinf from CPU 0 to all other CPUs.
    for (int i = 1; i < numtasks; i++){
      offsetRows = i * numrows;
      //cout<<"offset: "<<offsetRows<<"  sending to: "<<i<<endl;
      rc = MPI_Send(&dat_new[offsetRows][0], 2*numrows*h, MPI_COMPLEX, i , 0, MPI_COMM_WORLD);
    }

    
    //  Failure message
    if (rc != MPI_SUCCESS){
      cout<<"Send Failed rank: "<<rank<<"RC: "<<rc<<endl;
      MPI_Finalize();
      exit(1);
    }
    
    //  Putiing it all into a matrix for saving.
    for (int i = 0; i < numrows; i++){
      for (int j = 0; j < h; j++){
        D_rank[ i* w +j ]= dat_new[i][j];

      }
    }

    //  Performing 1D transform

    Transform1D(D_rank,w,ret1D,numrows);



    cout<<"The first: "<<ret1D[0]<<" Rank: " <<rank<<endl;

    // Taking the tranform 1D into a single matrix

    for (int i = 0; i < numrows; i++){
        for (int j = 0; j < h; j++){
              H[i * w + j]=ret1D[i * w + j];
        }      
    }


  }

  else{

    MPI_Status status;
    int rc;

    //cout<<"Rank: "<<rank<<"numrows: "<<numrows<<endl;

    
    //  If not rank=0, then use of MPI receive
    rc = MPI_Recv(Dbuf, 2*numrows*h, MPI_COMPLEX, 0, 0, MPI_COMM_WORLD,&status); 
    
    //cout<<"Recv rank: "<<rank<<"   RC: "<<rc<<endl;

    if (rc != MPI_SUCCESS){
      cout<<"Recv Failed rank: "<<rank<<"RC: "<<rc<<endl;
      MPI_Finalize();
      exit(1);
    }

   //  Doing the Transform
    Transform1D(Dbuf,w,res1D,numrows);


    //  Use of MPI send
    rc = MPI_Send(&res1D[0], 2*numrows*h, MPI_COMPLEX, 0 , 0, MPI_COMM_WORLD);

    if (rc != MPI_SUCCESS){
      cout<<"Send Failed rank: "<<rank<<"RC: "<<rc<<endl;
      MPI_Finalize();
      exit(1);
    }
  }

  
  if(rank == 0) {

    int offsetRows = 0;

    for (int i = 1; i < numtasks; i++) {
      
      int rc;
      MPI_Status status;

      // CPU 0 receives all the tranformed
      rc = MPI_Recv(&res1D[0], 2*numrows*h, MPI_COMPLEX, i, 0, MPI_COMM_WORLD,&status);

      cout<<"Recv first: "<<res1D[0]<<" Rank: " <<i<<endl;

      if (rc != MPI_SUCCESS){
        cout<<"Receive Failed rank: "<<rank<<"RC: "<<rc<<endl;
        MPI_Finalize();
        exit(1);
      }

      offsetRows = i * numrows;

      //cout<<"offsetRows: "<<offsetRows<<"numrows: "<<numrows<<endl;
      for (int i = 0; i < (numrows); ++i) {
        for (int j = 0; j < w; ++j) {
           H[ (i+ offsetRows)*w +j ] = res1D[ i*w + j ];
        }
      }

    }

    

// FFT1D complete

image.SaveImageData("MyAfter1d.txt",H,w,h);

  
  }


 // zzzzzzzzzaaannnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn THIS IS 2D FFT nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn
// Transposing the result

// 2D FFT starts with rank=0 condition

if (rank==0){

// Transposing the resultant matrix
  for (int i = 0; i < h; i++){
    for (int j = 0; j < w; j++){
      dat_ptr[j * w + i] = H[i * w + j];

    }
  }
    
    

    // Converting 1D array into 2D array
    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
        dat_new[i][j] = dat_ptr[ i * w + j ];
      }
    }

    int rc, offsetRows;

    for (int i = 1; i < numtasks; i++){
      offsetRows = i * numrows;
      
      //cout<<"offset: "<<offsetRows<<"  sending to: "<<i<<endl;
      // CPU sends to all in loop starting from rank 1
      rc = MPI_Send(&dat_new[offsetRows][0], 2*numrows*h, MPI_COMPLEX, i , 0, MPI_COMM_WORLD);
    }

    if (rc != MPI_SUCCESS){
      cout<<"Send Failed rank: "<<rank<<"RC: "<<rc<<endl;
      MPI_Finalize();
      exit(1);
    }
    
    //  D_rank contains the matrix on which 1D transform is to be performed.

    for (int i = 0; i < numrows; i++){
      for (int j = 0; j < h; j++){
        D_rank[ i* w +j ]= dat_new[i][j];

      }
    }


   //  1D transform occurs
    Transform1D(D_rank,w,ret1D,numrows);



    cout<<"The first: "<<ret1D[0]<<" Rank: " <<rank<<endl;

    //  It is put into a matrix after 1D transform occurs
    for (int i = 0; i < numrows; i++){
        for (int j = 0; j < h; j++){
              H[i * w + j]=ret1D[i * w + j];
        }      
    }


  }

  else{

    
    MPI_Status status;
    int rc;

    //cout<<"Rank: "<<rank<<"numrows: "<<numrows<<endl;

    rc = MPI_Recv(Dbuf, 2*numrows*h, MPI_COMPLEX, 0, 0, MPI_COMM_WORLD,&status); 
    
    //cout<<"Recv rank: "<<rank<<"   RC: "<<rc<<endl;

    if (rc != MPI_SUCCESS){
      cout<<"Recv Failed rank: "<<rank<<"RC: "<<rc<<endl;
      MPI_Finalize();
      exit(1);
    }

    Transform1D(Dbuf,w,res1D,numrows);


    rc = MPI_Send(&res1D[0], 2*numrows*h, MPI_COMPLEX, 0 , 0, MPI_COMM_WORLD);

    if (rc != MPI_SUCCESS){
      cout<<"Send Failed rank: "<<rank<<"RC: "<<rc<<endl;
      MPI_Finalize();
      exit(1);
    }
  }

  if(rank == 0) {

    int offsetRows = 0;

    for (int i = 1; i < numtasks; i++) {
      
      int rc;
      MPI_Status status;

      rc = MPI_Recv(&res1D[0], 2*numrows*h, MPI_COMPLEX, i, 0, MPI_COMM_WORLD,&status);

      cout<<"Recv first: "<<res1D[0]<<" Rank: " <<i<<endl;

      if (rc != MPI_SUCCESS){
        cout<<"Receive Failed rank: "<<rank<<"RC: "<<rc<<endl;
        MPI_Finalize();
        exit(1);
      }

      offsetRows = i * numrows;

      //cout<<"offsetRows: "<<offsetRows<<"numrows: "<<numrows<<endl;
      for (int i = 0; i < (numrows); ++i) {
        for (int j = 0; j < w; ++j) {
           H[ (i+ offsetRows)*w +j ] = res1D[ i*w + j ];
        }
      }

    }

      // Transposing the resultant matrix
  for (int i = 0; i < h; i++){
    for (int j = 0; j < w; j++){
      dat_final[j * w + i] = H[i * w + j];

    }
  }


// FFT1D complete

image.SaveImageData("MyAfter2d.txt",dat_final,w,h);

  
  }
return dat_final;

}



/////////////////////////////////////////// INVERSE FFT //////////////////////////////////////////////////////////////////////////////////////


//  Inverse FFT follows the same procedure. The formula for W changes. The resultant txt file MyAfterInverse.txt

void inverse1D(Complex* dat_ptr, int w,Complex* H,int numrows ) {


  for (int i = 0; i < numrows; i++) {
    
    for (int j = 0; j < w; j++) {
      
      H[i*w + j] = Complex(0,0);
      
      for (int p = 0; p < w; p++) {
       
        Complex W = Complex (0,0);
        W.real = (double)cos(2*M_PI*j*p/w)/w;
        W.imag = (double)sin(2*M_PI*j*p/w)/w;
        
        H[i*w + j] = H[i*w + j] + dat_ptr[i*w + p] * W;
     
      }
    }
  }

}


void inverse2D(const char* inputFN,Complex* res,int rank,int numtasks,int h,int w) { 
   InputImage image(inputFN);  // Create the helper object for reading the image
  
  // int h = image.GetHeight();
  // int w = image.GetWidth();

  int numrows  = h/numtasks;

  Complex* D_rank = new Complex[numrows * h];
  Complex* Dbuf = new Complex[numrows * w];
  Complex* ret1D = new Complex[numrows * h];
  Complex* res1D = new Complex[numrows * h];
  Complex* dat_ptr = new Complex[ h * w];
  Complex* H = new Complex[w * h];
  Complex dat_new[w][h];
  Complex* dat_final = new Complex[w * h];
  

  if (rank==0){
    
    

    // Converting 1D array into 2D array
    for (int i = 0; i < w; i++) {
      for (int j = 0; j < h; j++) {
        dat_new[i][j] = res[ i * w + j ];
      }
    }

    int rc, offsetRows;

    for (int i = 1; i < numtasks; i++){
      offsetRows = i * numrows;
      //cout<<"offset: "<<offsetRows<<"  sending to: "<<i<<endl;
      rc = MPI_Send(&dat_new[offsetRows][0], 2*numrows*h, MPI_COMPLEX, i , 0, MPI_COMM_WORLD);
    }

    if (rc != MPI_SUCCESS){
      cout<<"Send Failed rank: "<<rank<<"RC: "<<rc<<endl;
      MPI_Finalize();
      exit(1);
    }
    
    for (int i = 0; i < numrows; i++){
      for (int j = 0; j < h; j++){
        D_rank[ i* w +j ]= dat_new[i][j];

      }
    }


    inverse1D(D_rank,w,ret1D,numrows);



    cout<<"The first: "<<ret1D[0]<<" Rank: " <<rank<<endl;

    for (int i = 0; i < numrows; i++){
        for (int j = 0; j < h; j++){
              H[i * w + j]=ret1D[i * w + j];
        }      
    }


  }

  else{

    MPI_Status status;
    int rc;

    //cout<<"Rank: "<<rank<<"numrows: "<<numrows<<endl;

    rc = MPI_Recv(Dbuf, 2*numrows*h, MPI_COMPLEX, 0, 0, MPI_COMM_WORLD,&status); 
    
    //cout<<"Recv rank: "<<rank<<"   RC: "<<rc<<endl;

    if (rc != MPI_SUCCESS){
      cout<<"Recv Failed rank: "<<rank<<"RC: "<<rc<<endl;
      MPI_Finalize();
      exit(1);
    }

    inverse1D(Dbuf,w,res1D,numrows);


    rc = MPI_Send(&res1D[0], 2*numrows*h, MPI_COMPLEX, 0 , 0, MPI_COMM_WORLD);

    if (rc != MPI_SUCCESS){
      cout<<"Send Failed rank: "<<rank<<"RC: "<<rc<<endl;
      MPI_Finalize();
      exit(1);
    }
  }

  if(rank == 0) {

    int offsetRows = 0;

    for (int i = 1; i < numtasks; i++) {
      
      int rc;
      MPI_Status status;

      rc = MPI_Recv(&res1D[0], 2*numrows*h, MPI_COMPLEX, i, 0, MPI_COMM_WORLD,&status);

      cout<<"Recv first: "<<res1D[0]<<" Rank: " <<i<<endl;

      if (rc != MPI_SUCCESS){
        cout<<"Receive Failed rank: "<<rank<<"RC: "<<rc<<endl;
        MPI_Finalize();
        exit(1);
      }

      offsetRows = i * numrows;

      //cout<<"offsetRows: "<<offsetRows<<"numrows: "<<numrows<<endl;
      for (int i = 0; i < (numrows); ++i) {
        for (int j = 0; j < w; ++j) {
           H[ (i+ offsetRows)*w +j ] = res1D[ i*w + j ];
        }
      }

    }


// FFT1D complete

image.SaveImageData("my1d.txt",H,w,h);

  
  }


 // zzzzzzzzzaaannnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn THIS IS 2D FFT Inverse nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn
// Transposing the result



if (rank==0){

// Transposing the resultant matrix
  for (int i = 0; i < h; i++){
    for (int j = 0; j < w; j++){
      dat_ptr[j * w + i] = H[i * w + j];

    }
  }
    
    

    // Converting 1D array into 2D array
    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
        dat_new[i][j] = dat_ptr[ i * w + j ];
      }
    }

    int rc, offsetRows;

    for (int i = 1; i < numtasks; i++){
      offsetRows = i * numrows;
      //cout<<"offset: "<<offsetRows<<"  sending to: "<<i<<endl;
      rc = MPI_Send(&dat_new[offsetRows][0], 2*numrows*h, MPI_COMPLEX, i , 0, MPI_COMM_WORLD);
    }

    if (rc != MPI_SUCCESS){
      cout<<"Send Failed rank: "<<rank<<"RC: "<<rc<<endl;
      MPI_Finalize();
      exit(1);
    }
    
    for (int i = 0; i < numrows; i++){
      for (int j = 0; j < h; j++){
        D_rank[ i* w +j ]= dat_new[i][j];

      }
    }


    inverse1D(D_rank,w,ret1D,numrows);



    cout<<"The first: "<<ret1D[0]<<" Rank: " <<rank<<endl;

    for (int i = 0; i < numrows; i++){
        for (int j = 0; j < h; j++){
              H[i * w + j]=ret1D[i * w + j];
        }      
    }


  }

  else{

    MPI_Status status;
    int rc;

    //cout<<"Rank: "<<rank<<"numrows: "<<numrows<<endl;

    rc = MPI_Recv(Dbuf, 2*numrows*h, MPI_COMPLEX, 0, 0, MPI_COMM_WORLD,&status); 
    
    //cout<<"Recv rank: "<<rank<<"   RC: "<<rc<<endl;

    if (rc != MPI_SUCCESS){
      cout<<"Recv Failed rank: "<<rank<<"RC: "<<rc<<endl;
      MPI_Finalize();
      exit(1);
    }

    inverse1D(Dbuf,w,res1D,numrows);


    rc = MPI_Send(&res1D[0], 2*numrows*h, MPI_COMPLEX, 0 , 0, MPI_COMM_WORLD);

    if (rc != MPI_SUCCESS){
      cout<<"Send Failed rank: "<<rank<<"RC: "<<rc<<endl;
      MPI_Finalize();
      exit(1);
    }
  }

  if(rank == 0) {

    int offsetRows = 0;

    for (int i = 1; i < numtasks; i++) {
      
      int rc;
      MPI_Status status;

      rc = MPI_Recv(&res1D[0], 2*numrows*h, MPI_COMPLEX, i, 0, MPI_COMM_WORLD,&status);

      cout<<"Recv first: "<<res1D[0]<<" Rank: " <<i<<endl;

      if (rc != MPI_SUCCESS){
        cout<<"Receive Failed rank: "<<rank<<"RC: "<<rc<<endl;
        MPI_Finalize();
        exit(1);
      }

      offsetRows = i * numrows;

      //cout<<"offsetRows: "<<offsetRows<<"numrows: "<<numrows<<endl;
      for (int i = 0; i < (numrows); ++i) {
        for (int j = 0; j < w; ++j) {
           H[ (i+ offsetRows)*w +j ] = res1D[ i*w + j ];
        }
      }

    }

      // Transposing the resultant matrix
  for (int i = 0; i < h; i++){
    for (int j = 0; j < w; j++){
      dat_final[j * w + i] = H[i * w + j];

    }
  }


// FFT1D complete

image.SaveImageData("MyAfterInverse.txt",dat_final,w,h);

  
  }
  

}




///////////////////////////////////////////////// END oF Inverse FFT////////////////////////////////////////////////////////////////////////////////

  


int main(int argc, char** argv){
  string fn("Tower.txt"); // default file name
  if (argc > 1) fn = string(argv[1]);  // if name specified on cmd line
  InputImage im(fn.c_str());

  int h = im.GetHeight();
  int w = im.GetWidth();
  
  int rank,numtasks,rc;
  

  // Complex* W_ptr = new Complex[ h * w ];
  // W_ptr = computeW(h,w);

  rc = MPI_Init(&argc,&argv);

  if (rc != MPI_SUCCESS){
    cout<<"Init failed: "<<rank<<"RC: "<<rc<<endl;
    MPI_Finalize();
    exit(1);
  }

  MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank); 

  
 
  // MPI initialization here
  
  Complex* res = new Complex[w*h];
  res = Transform2D(fn.c_str(),rank,numtasks);
  
  inverse2D(fn.c_str(),res,rank,numtasks,h,w);
   // Perform the transform.


  // Finalize MPI here

  MPI_Finalize();
}  