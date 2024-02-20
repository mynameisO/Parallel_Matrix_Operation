#ifndef MATRIX_CLASS_H
#define MATRIX_CLASS_H
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
            matrixData(size_t row, size_t col);

            size_t row(){ return Row; };
            size_t col(){ return Col; };

            float& operator[](int index);
            float& operator()(int x, int y);
            matrixData operator+(matrixData& mat);
            matrixData operator-(matrixData& mat);
            matrixData operator*(matrixData& mat);

#ifdef MPI_OPERATION

            //block comm
            void MPI_block_distribute_matrix(int root, int nprocs);
            void MPI_block_p2p_comm(int sender, int receiver);

            //non block comm
            void MPI_non_block_distribute_matrix(int root, int nprocs);
            void MPI_non_block_recv_matrix(int root, int nprocs);
            bool MPI_test_nb_recv(int root, int nprocs);
            void MPI_non_block_p2p_comm(int sender, int receiver);

            static matrixData MPI_multiply(matrixData& matA, matrixData& matB);
            static matrixData MPI_distribute_and_multiply(matrixData& matA, matrixData& matB, int root, int nprocs);

#endif

            void transpose();

            void printMat(int precision);
    };
}

#endif