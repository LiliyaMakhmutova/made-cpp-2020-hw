#pragma once
#include <vector>
#include <iostream>

namespace task {

    const double EPS = 1e-6;


    class OutOfBoundsException : public std::exception {};
    class SizeMismatchException : public std::exception {};


    class Matrix {
        class MutableRow;
        class ImmutableRow;
    public:        
        Matrix();
        Matrix(size_t rows, size_t cols);
        Matrix(const Matrix& copy);
        ~Matrix();
        Matrix& operator=(const Matrix& a);

        double& get(size_t row, size_t col);
        const double& get(size_t row, size_t col) const;
        void set(size_t row, size_t col, const double& value);
        void resize(size_t new_rows, size_t new_cols);

        MutableRow operator[](size_t row);
        ImmutableRow operator[](size_t row) const;

        Matrix& operator+=(const Matrix& a);
        Matrix& operator-=(const Matrix& a);
        Matrix& operator*=(const Matrix& a);
        Matrix& operator*=(const double& number);

        Matrix operator+(const Matrix& a) const;
        Matrix operator-(const Matrix& a) const;
        Matrix operator*(const Matrix& a) const;
        Matrix operator*(const double& a) const;

        Matrix operator-() const;
        Matrix operator+() const;

        double det() const;
        void transpose();
        Matrix transposed() const;
        double trace() const;

        std::vector<double> getRow(size_t row);
        std::vector<double> getColumn(size_t column);

        bool operator==(const Matrix& a) const;
        bool operator!=(const Matrix& a) const;

        std::pair<size_t, size_t> getSize() const; 
    private: 
        double** v;
        size_t row;
        size_t col;

        bool are_equal(double a, double b) const;

        class MutableRow
        {
        public:
            friend class Matrix;
            double& operator[](size_t col)
            {
                return m->get(row, col);
            }
        private:
            MutableRow(Matrix* m, size_t row) : m(m), row(row) {}
            Matrix* const m;
            const size_t row;
        };
        class ImmutableRow
        {
            friend class Matrix;
        public:
            const double& operator[](size_t col) const
            {
                return m->get(row, col);
            }
        private:
            ImmutableRow(const Matrix* m, size_t row) : m(m), row(row) {}
            const Matrix* const m;
            const size_t row;
        };

    };


    Matrix operator*(const double& a, const Matrix& b);

    std::ostream& operator<<(std::ostream& output, const Matrix& matrix);
    std::istream& operator>>(std::istream& input, Matrix& matrix);

    

}  // namespace task