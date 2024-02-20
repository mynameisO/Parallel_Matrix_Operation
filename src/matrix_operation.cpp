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

    float& matrixData::operator[](int index){
        return arr[index];
    }

    float& matrixData::operator()(int x, int y){
        return arr[x * Row + y];
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

// TODO: finish this shit.
    matrixData matrixData::operator*(matrixData& mat){
        matrixData res(Row, mat.col());
        for(int i = 0;i < Row;i++){
            for(int j = 0;j < mat.col();j++){
                res[i * mat.col() + j] = 0;
                for(int k = 0;k < mat.row();k++){
                    
                }
            }
        }
    }

    void matrixData::transpose(){
        std::vector<float> new_arr(Row * Col);
        #pragma omp parallel for
            for(int i = 0;i < Row * Col;i++){
                // derive from: offset = (i * C) / (R * C), and the first column of each row = (i * C) % (R * C)
                new_arr[i] = arr[(i / Row) + ((i * Col) % (Row * Col))];
            }
        arr = new_arr;

        // Row and Col swap
        int tmp = Row;
        Row = Col;
        Col = tmp;
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