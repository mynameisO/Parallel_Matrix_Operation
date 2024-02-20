#include <iostream>
#include "../src/matrix_operation_abstract.hpp"

int main(int argc, char* argv[]){

    std::string matfile1 = "./test/matAlarge.txt";
    std::string matfile2 = "./test/matBlarge.txt";

    MO::matrixData matA(matfile1);
    MO::matrixData matB(matfile2);

    MO::matrixData matC = matA + matB;

    matA.transpose();
    matA.transpose();

    matA.printMat(1);

    return 0;
}