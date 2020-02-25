#include<mpi.h>
#include <stdio.h> 
#include<stdlib.h>   


// we need a special format of matrix for our algorithm , let's call it formal "S"
// the first matrix would be in column major format and the second one in a row major format 
//so we make two functions to  change a column major to row major and vice versa
//this makes sure that we can work on any format of the input 
//after processing the input with those two functions , our multipliaction algorithm requires us to have those in format "S"


void createMatrix(float *A,int n , float a ) // generate n elements for a matrix 
{
	int i;
	for(i=0;i<n;i++)
		{A[i]= (float)rand()/(float)(RAND_MAX/a);}
 // floats wil be all less than a

}

void rtoC(float* A , float* B , int m , int n){
//converts row major A into column major B , size is m*n

for (int i=0;i< m ;i++){

for (int j=0; j< n; j++){
 B[j*m + i] = A[i*n + j] ;

}
}


}

void ctor(float* A , float* B , int m , int n){
//converts column major A into row major B , size is m*n

for (int i=0;i< m ;i++){

for (int j=0; j< n; j++){
  B[i*n + j] =A[j*m + i]  ;

}
}


} 



void matrix_Multiply(float *A, float *B, float *C, int m, int n, int p){

// A is column major and B is row major 
// Output c is row major 
//A is m*n and B is n*p

	int i, j, k;
	for (i = 0; i < m; i++){
		for (j = 0; j < p; j++){
			C[i*p + j] = 0;
			for (k = 0; k < n; k++)
				C[i*p + j] += A[ k*m + i] * B[k*p + j];
		}
	}
}		


//checks if two matrices are equal  
int matrixEqual (float *A , float *B , int p ){ // returns 0 for equal or 1 for unequal
// p is total number of elements in them

int i;
 for (i = 0; i < p; i++){
  
   if (A[i] == B[i]){}
else {return 1;}

}

return 0;
}


int main (int argc, char**argv){
 
int rank,size,n ,count;
 

n=4; // size of matrix 
//count = atoi(argv[2]); // number of cores 

MPI_Init(&argc,&argv);
MPI_Comm_size(MPI_COMM_WORLD,&size);
MPI_Comm_rank(MPI_COMM_WORLD,&rank);
count = size;

int partSize = 32/count ;

float *partMatrix1 =(float*)malloc((partSize*n)*sizeof(float));  // space to store and mutilply partial matrices
float *partMatrix2 =(float*)malloc((partSize*n)*sizeof(float));
float *partProduct=(float*)malloc((n*n)*sizeof(float));
float *wholeMatrix1;
float *wholeMatrix2 ;
float *wholeProduct1 ;
float *wholeProduct2;
double startTime ;
double endTime ;
double timeSerial;
double *elapsed ;
double *timeParallel;

printf("The rank is %d\n\n",rank);
if(rank ==0)
	{
	wholeMatrix1=(float*)malloc((n*32)*sizeof(float));
	wholeMatrix2=(float*)malloc((n*32)*sizeof(float));
	 wholeProduct1=(float*)malloc((n*n)*sizeof(float));

         for(int i=0;i<(n*n);i++){ wholeProduct1[i] = 0.000;}


         wholeProduct2=(float*)malloc((n*n)*sizeof(float));
     for(int i=0;i<(n*n);i++){ wholeProduct2[i] = 0.000;}

        createMatrix(wholeMatrix1 ,(n*32),10000.00);
	createMatrix(wholeMatrix2 ,(n*32),10000.00);	
       
       startTime = MPI_Wtime();
      matrix_Multiply(wholeMatrix1 , wholeMatrix2 ,wholeProduct2, n , partSize , n);
         endTime = MPI_Wtime();
         timeSerial = endTime - startTime;
for(int q=0; q<(n*n);q++){
	printf("The integer is: %f\n", wholeProduct2[q]);}

     
	

}

MPI_Barrier(MPI_COMM_WORLD);

startTime = MPI_Wtime();
MPI_Scatter(wholeMatrix1,(partSize*n),MPI_FLOAT,partMatrix1,(partSize*n),MPI_FLOAT,0,MPI_COMM_WORLD);
MPI_Scatter(wholeMatrix2,(partSize*n),MPI_FLOAT,partMatrix2,(partSize*n),MPI_FLOAT,0,MPI_COMM_WORLD);

matrix_Multiply(partMatrix1 , partMatrix2 ,partProduct, n , partSize , n);


MPI_Reduce(partProduct,wholeProduct1,(n*n),MPI_FLOAT,MPI_SUM,0,MPI_COMM_WORLD);

endTime = MPI_Wtime();

elapsed[0] = endTime - startTime;
MPI_Reduce(elapsed,timeParallel,1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD);

if(rank==0){ 

for(int q=0; q<(n*n);q++){
	printf("The integer is: %f\n", wholeProduct1[q]);
}
printf("Hello C Language");  
for(int q=0; q<(n*n);q++){
	printf("The integer is: %f\n", wholeProduct2[q]);
}
 int p = matrixEqual(wholeProduct1 , wholeProduct2 , (n*n));
printf("The integer is: %d\n", p);

printf("Serial time collective: %f\n", timeSerial);
printf("Parallel time collective: %f\n", timeParallel[0]);



}

MPI_Finalize();
return 0;
}
