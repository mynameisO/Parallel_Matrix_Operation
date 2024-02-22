#include "matrix_operation_abstract.hpp"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstring>

namespace MO{
    matrixData::matrixData(std::string& filename){
        std::string line, word;
        std::stringstream ss;
        std::ifstream finput(filename.c_str());
        int i = 0; 
        
        getline(finput, line);
        ss << line; 
        getline(ss, word, ' ');  
        Row = std::stoi(word); 
        getline(ss, word, ' ');       
        Col = std::stoi(word);
        
        arr.reserve(Row * Col);

        try{
            while(getline(finput, line)){
                std::stringstream ssline;
                ssline << line;
                while(getline(ssline, word, ' ')){
                    arr[i] = std::stof(word);
                    if(i >= Row * Col){
                        throw (i);
                    }
                    i++;
                }
            }
        }catch(int index_error){
            std::cout << "Input wrong dimension" << "\n";
            exit(-1);
        }

        finput.close();
    }

    matrixData::matrixData(size_t row, size_t col)
    :Row(row), Col(col)
    {
        arr.reserve(row * col);
    }

    matrixData::matrixData(float** array, int row, int col)
    :Row(row), Col(col)
    {
        arr.reserve(row * col);
        for(int i = 0;i < row;i++){
            std::memcpy(&(arr.at(i * col)), array[i], col * sizeof(float));
        }
    }

    matrixData::matrixData(std::vector<float> array, int row, int col):Row(row), Col(col){
        arr = array;
    }

    matrixData::matrixData():Row(0), Col(0){}

    void matrixData::reserve(int row, int col){
        arr.reserve(row * col);
        Row = row;
        Col = col;
    }

    float& matrixData::operator[](int index){
        return arr[index];
    }

    float& matrixData::operator()(int x, int y){
        return arr[x * Col + y];
    }    

    matrixData matrixData::operator+(matrixData& mat){
        if(mat.row() != Row || mat.col() != Col){
            std::cout << "Both operand need to have the same dimension." << "\n";
            exit(-1);
        }

        matrixData res(mat.row(), mat.col());   
        #pragma omp parallel for 
            for(int i = 0;i < mat.row() * mat.col();i++){
                res[i] = arr[i] + mat[i];
            }

        return res;
    }

    matrixData matrixData::operator-(matrixData& mat){
        if(mat.row() != Row || mat.col() != Col){
            std::cout << "Both operand need to have the same dimension." << "\n";
            exit(-1);
        }

        matrixData res(mat.row(), mat.col());   
        #pragma omp parallel for
            for(int i = 0;i < mat.row() * mat.col();i++){
                res[i] = arr[i] - mat[i];
            }

        return res;
    }

    matrixData matrixData::operator*(matrixData& mat){
        if(mat.row() != Col){
            std::cout << Col << " " << mat.row() << "\n";
            std::cout << "Number of Col Operand A need to be equal with Number of Row of Operand B.\n";
        }

        matrixData res(Row, mat.col());
        for(int i = 0;i < Row;i++){
            for(int j = 0;j < mat.col();j++){
                float sum = 0;
                #pragma omp parallel for reduction(+:sum)
                    for(int k = 0;k < Col;k++){
                        sum += arr[i * Col + k] * mat[k * mat.col() + j];
                    }
                res[i * mat.col() + j] = sum;
            }
        }
        return res;
    }

    bool matrixData::operator==(matrixData& mat){
        if(mat.row() != Row || mat.col() != Col){
            return false; 
        }

        for(int i = 0;i < Row * Col;i++)
            if((mat[i] - arr[i]) >= 0.005f){ 
                std::cout << std::setprecision(6) << std::fixed;
                std::cout << mat[i] << " " << arr[i] << " " << "\n";
                return false; 
            }
        
        return true;
    }

    // TODO: inplace is not working. fix it.
    matrixData matrixData::transpose(bool inplace){
        matrixData res(Col, Row);
        #pragma omp parallel for
            for(int i = 0;i < Row * Col;i++){
                // derive from: offset = (i * C) % (R * C), and the column = (i * C) / (R * C)
                res[i] = arr[(i / Row) + ((i * Col) % (Row * Col))];
            }
        return res;
    }

    void matrixData::printMat(int precision){
        std::cout << Row << " " << Col << "\n";
        std::cout << std::setprecision(precision) << std::fixed;
        for(int i = 0;i < Row * Col;i++){
            if((i != 0) && (i % Col <= 0)){
                std::cout << "\n";
            }
            std::cout << arr[i]  << " ";
        }
        std::cout << "\n";
    }

}