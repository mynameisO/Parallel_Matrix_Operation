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

            void printMat(int precision);
    };
}

#endif