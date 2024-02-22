#include <iostream>
#include <vector>
#include "../src/matrix_operation_abstract.hpp"

int main(int argc, char* argv[]){

    time_t start, end;

    std::string filename1 = "./test/matAhun.txt";
    std::string filename2 = "./test/matBhun.txt";

    MO::matrixData matA(filename1);
    MO::matrixData matB(filename2);

    MO::matrixData matAt = matA.transpose();
    MO::matrixData matBt = matB.transpose();

    std::cout << "Start multiplying\n";

    MO::matrixData res1 = matA * matB;
    MO::matrixData res2 = matBt * matAt;

    std::cout << "Start transposing res1\n";

    res1 = res1.transpose();

    std::cout << "result\n";

    std::cout << (res1 == res2) << "\n"; // 
   
    res1.printMat(6);

    return 0;
}