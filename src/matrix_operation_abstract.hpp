#ifndef MATRIX_CLASS_H
#define MATRIX_CLASS_H
#ifdef MPI_OPERATION
#include <mpi.h>
#endif
#include <vector>
#include <string>

enum matOpStatus{
    SUCCESS = 0,
    WRONG_DIMENSION = 1
};

namespace MO{
    class matrixData{
        private:
            std::vector<float> arr;
            size_t Row;
            size_t Col;
        public:
            matrixData(std::string& filename);
            matrixData(float** array, int row, int col);
            matrixData(std::vector<float> array, int row, int col);
            matrixData(size_t row, size_t col);
            matrixData();

            size_t row(){ return Row; };
            size_t col(){ return Col; };
            
            void reserve(int row, int col);

            float& operator[](int index);
            float& operator()(int x, int y);
            matrixData operator+(matrixData& mat);
            matrixData operator-(matrixData& mat);
            matrixData operator*(matrixData& mat);
            bool operator==(matrixData& mat);

            matrixData transpose(bool inplace = false);

#ifdef MPI_OPERATION
            static void MPI_sync_distribute(int rank, int root, matrixData& mat, MPI_Comm comm);
            static void MPI_async_distribute(int rank, int root, int nprocs, matrixData& mat, MPI_Comm comm);
            

            // TODO: This function should be able to tell how many process and which process should run the task
            static matrixData MPI_distribute_multiply(matrixData& matA, matrixData& matB, int root, int rank, int nprocs, MPI_Comm comm);
            static matrixData MPI_multiply(matrixData& matA, matrixData& matB, int root, int rank, int nprocs, MPI_Comm comm);
#endif

            void printMat(int precision);
    };
}

#endif