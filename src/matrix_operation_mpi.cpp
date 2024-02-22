#define MPI_OPERATION
#include "matrix_operation_abstract.hpp"
#include <mpi.h>
#include <iostream>

#define MPI_Error_Check(x) {const int err=x; if(x!=MPI_SUCCESS) { fprintf(stderr, "MPI ERROR %d at %d.", err, __LINE__);}} << status.MPI_ERROR

namespace MO{
    
    void matrixData::MPI_sync_distribute(int rank, int root, matrixData& mat, MPI_Comm comm){
        int row_and_col[2];
        if(root == rank){
            row_and_col[0] = (int) mat.row();
            row_and_col[1] = (int) mat.col();
        }
        MPI_Bcast((void*) row_and_col, 2, MPI_INT, root, comm);
        if(root != rank){
            mat.reserve(row_and_col[0], row_and_col[1]);
        }
        MPI_Bcast(&(mat[0]), mat.row() * mat.col(), MPI_FLOAT, root, comm);
    }


    void matrixData::MPI_async_distribute(int rank, int root, int nprocs, matrixData& mat, MPI_Comm comm){ 
        int row_and_col[2];
        if(root == rank){
            MPI_Request request_stat;
            row_and_col[0] = (int) mat.row();
            row_and_col[1] = (int) mat.col();
            
            for(int i = 0;i < nprocs;i++){
                if(i != root){
                    MPI_Isend((void*) row_and_col, 2, MPI_INT, i, i, comm, &request_stat);
                    MPI_Isend(&(mat[0]), mat.row() * mat.col(), MPI_FLOAT, i, nprocs + i, comm, &request_stat);
                }
            }
        }else{
            MPI_Status status;
            MPI_Recv((void*) row_and_col, 2, MPI_INT, 0, rank, comm, &status);
            mat.reserve(row_and_col[0], row_and_col[1]);
            MPI_Recv(&(mat[0]), mat.row() * mat.col(), MPI_FLOAT, 0, nprocs + rank, comm, &status);
        }
    }

    // TODO: This function should be able to tell how many process and which process should run the task
    // TODO: if root is not at the actualnprocs - 1 then it's break.
    matrixData matrixData::MPI_distribute_multiply(
        matrixData& matA, matrixData& matB, int root, int rank, int nprocs, MPI_Comm comm
    ){
        int actualnprocs = nprocs;
        std::vector<float> new_arr;
        int new_size;
        int part_size = 0;
        MPI_Request r_status;
        MPI_Status status;
        MO::matrixData res;

        MPI_async_distribute(rank, root, nprocs, matA, comm);
        MPI_async_distribute(rank, root, nprocs, matB, comm);

        // why, I don't know.
        while(actualnprocs / 2 > matA.row() * matB.col()){
            actualnprocs /= 2;
        }

        new_size = matA.row() * matB.col();

        part_size = new_size / actualnprocs;

        new_arr.reserve(part_size); 

        for(int index = 0;index < part_size;index++){
            int i = (part_size * rank + index) / matB.col();
            int j = (part_size * rank + index) % matB.col();
            float sum = 0;
            #pragma omp parallel for reduction(+:sum)
                for(int k = 0;k < matA.col();k++){
                    sum += matA[i * matA.col() + k] * matB[k * matB.col() + j];
                }
            new_arr[index] = sum;
        }

        MPI_Barrier(comm);

        std::vector<float> buf(part_size * actualnprocs);

        MPI_Gather(&(new_arr[0]), part_size, MPI_FLOAT, &(buf[0]), part_size, MPI_FLOAT, root, comm);

        // take care of left over.
        if((new_size % actualnprocs != 0) && (rank == root)){
            for(int index = 0;index < new_size % actualnprocs;index++){    
                int i = (part_size * actualnprocs + index) / matB.col();
                int j = (part_size * actualnprocs + index) % matB.col();
                float sum = 0;
                #pragma omp parallel for reduction(+:sum)
                    for(int k = 0;k < matA.col();k++){
                        sum += matA[i * matA.col() + k] * matB[k * matB.col() + j];
                    }
                buf.push_back(sum);
            }
        }

        if(rank == root){
            res = MO::matrixData(buf, matA.row(), matB.col());
        }

        MPI_Barrier(comm);
        return res;
    }

    matrixData matrixData::MPI_multiply(
        matrixData& matA, matrixData& matB, int root, int rank, int nprocs, MPI_Comm comm
    ){
        int actualnprocs = nprocs;
        std::vector<float> new_arr;
        int new_size;
        int part_size = 0;
        MPI_Request r_status;
        MPI_Status status;
        MO::matrixData res;

        // why, I don't know.
        while(actualnprocs / 2 > matA.row() * matB.col()){
            actualnprocs /= 2;
        }

        new_size = matA.row() * matB.col();

        part_size = new_size / actualnprocs;

        new_arr.reserve(part_size); 

        for(int index = 0;index < part_size;index++){
            int i = (part_size * rank + index) / matB.col();
            int j = (part_size * rank + index) % matB.col();
            float sum = 0;
            #pragma omp parallel for reduction(+:sum)
                for(int k = 0;k < matA.col();k++){
                    sum += matA[i * matA.col() + k] * matB[k * matB.col() + j];
                }
            new_arr[index] = sum;
        }

        MPI_Barrier(comm);

        std::vector<float> buf(part_size * actualnprocs);

        MPI_Gather(&(new_arr[0]), part_size, MPI_FLOAT, &(buf[0]), part_size, MPI_FLOAT, root, comm);

        // take care of left over.
        if((new_size % actualnprocs != 0) && (rank == root)){
            for(int index = 0;index < new_size % actualnprocs;index++){    
                int i = (part_size * actualnprocs + index) / matB.col();
                int j = (part_size * actualnprocs + index) % matB.col();
                float sum = 0;
                #pragma omp parallel for reduction(+:sum)
                    for(int k = 0;k < matA.col();k++){
                        sum += matA[i * matA.col() + k] * matB[k * matB.col() + j];
                    }
                buf.push_back(sum);
            }
        }

        if(rank == root){
            res = MO::matrixData(buf, matA.row(), matB.col());
        }

        MPI_Barrier(comm);
        return res;
    }
}