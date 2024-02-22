#include <iostream>
#include <vector>

#define MPI_OPERATION
#include "../src/matrix_operation_abstract.hpp"
#include <mpi.h>

int myrank;
int nprocs;

int main(int argc, char* argv[]){

    MPI_Init(NULL, NULL);

    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

    MO::matrixData matA;
    MO::matrixData matAt;
    MO::matrixData matB;
    MO::matrixData matBt;

    if(myrank == 0){
        std::string filename1 = "./test/matAhun.txt";
        std::string filename2 = "./test/matBhun.txt";
        matA = MO::matrixData(filename1);
        matB = MO::matrixData(filename2);
        matAt = matA.transpose();
        matBt = matB.transpose();
    }

    MO::matrixData::MPI_async_distribute(myrank, 0, nprocs, matA, MPI_COMM_WORLD);
    MO::matrixData::MPI_async_distribute(myrank, 0, nprocs, matB, MPI_COMM_WORLD);
    MO::matrixData::MPI_async_distribute(myrank, 0, nprocs, matAt, MPI_COMM_WORLD);
    MO::matrixData::MPI_async_distribute(myrank, 0, nprocs, matBt, MPI_COMM_WORLD);

    MO::matrixData res1 = MO::matrixData::MPI_multiply(matA, matB, 0, myrank, nprocs, MPI_COMM_WORLD);
    MO::matrixData res2 = MO::matrixData::MPI_multiply(matBt, matAt, 0, myrank, nprocs, MPI_COMM_WORLD);

    if(myrank == 0){
        res1 = res1.transpose();
        std::cout << (res1 == res2) << "\n\n";
        res2.printMat(5);
    }
    
    MPI_Barrier(MPI_COMM_WORLD);

    return 0;
}