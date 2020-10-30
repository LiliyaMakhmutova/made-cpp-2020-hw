#include "matrix.h"

using namespace task;

task::Matrix::Matrix()
{
	v = new double*[1];
	v[0] = new double[1];
	v[0][0] = 1.0;
	row = 1;
	col = 1;	
}

task::Matrix::Matrix(size_t rows, size_t cols) : row(rows), col(cols)
{
	v = new double* [rows];
	for (int i = 0; i < rows; i++)
	{
		v[i] = new double [cols];
	}

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			if(i == j)			
			{ 
				v[i][j] = 1.0;
			}
			else
			{
				v[i][j] = 0.0;
			}
		}
	}
}

task::Matrix::Matrix(const Matrix& copy) 
{
	auto size = copy.getSize();	
	size_t rows = size.first;
	size_t cols = size.second;

	v = new double* [rows];
	for (int i = 0; i < rows; i++)
	{
		v[i] = new double[cols];
	}

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			v[i][j] = copy[i][j];
		}
	}

	row = rows;
	col = cols;
}

task::Matrix::~Matrix()
{
	for (int i = 0; i < row; i++)
	{
		delete[] v[i];
	}
	delete[] v;
}

Matrix& task::Matrix::operator=(const Matrix& a)
{
	if (this != &a)
	{
		auto size = a.getSize();
		size_t rows = size.first;
		size_t cols = size.second;

		double** tmp = new double* [rows];
		for (int i = 0; i < rows; i++)
		{
			tmp[i] = new double[cols];
		}

		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < cols; j++)
			{
				tmp[i][j] = a[i][j];
			}
		}

		for (int i = 0; i < row; i++)
		{
			delete[] v[i];
		}
		delete[] v;

		row = rows;
		col = cols;
		v = tmp;
	}
	return *this;
}


double& task::Matrix::get(size_t row, size_t col)
{
	if (row >= this->row || col >= this->col)
	{
		throw OutOfBoundsException();
	}
	return v[row][col];
}

const double& task::Matrix::get(size_t row, size_t col) const
{
	if (row >= this->row || col >= this->col)
	{
		throw OutOfBoundsException();
	}
	return v[row][col];
}

void task::Matrix::set(size_t row, size_t col, const double& value)
{
	if (row >= this->row || col >= this->col)
	{
		throw OutOfBoundsException();
	}
	v[row][col] = value;
}

void task::Matrix::resize(size_t new_rows, size_t new_cols)
{
	double** tmp = new double* [new_rows];

	for (int i = 0; i < new_rows; i++)
	{
		tmp[i] = new double[new_cols];
	}

	for (int i = 0; i < new_rows; i++)
	{
		for (int j = 0; j < new_cols; j++)
		{
			if (i < row && j < col) 
			{
				tmp[i][j] = v[i][j];
			}
			else
			{
				tmp[i][j] = 0.0;
			}			
		}
	}

	for (int i = 0; i < row; i++)
	{
		delete[] v[i];
	}
	delete[] v;

	row = new_rows;
	col = new_cols;
	v = tmp;
}

task::Matrix::MutableRow task::Matrix::operator[](size_t row)
{
	return MutableRow(this, row);
}

task::Matrix::ImmutableRow task::Matrix::operator[](size_t row) const
{
	return ImmutableRow(this, row);
}

Matrix& task::Matrix::operator+=(const Matrix& a)
{
	*this = *this + a;
	return *this;
}

Matrix& task::Matrix::operator-=(const Matrix& a)
{
	*this = *this - a;
	return *this; 
}

Matrix& task::Matrix::operator*=(const Matrix& a)
{
	*this = *this * a;
	return *this;
}

Matrix& task::Matrix::operator*=(const double& number)
{
	*this = *this * number;
	return *this;
}

Matrix task::Matrix::operator+(const Matrix& a) const
{
	auto size = a.getSize();
	size_t rows = size.first;
	size_t cols = size.second;

	if (rows != this->row || cols >= this->col)
	{
		throw SizeMismatchException();
	}

	Matrix new_mat(*this);
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			new_mat[i][j] += a[i][j];
		}
	}
	return new_mat;
}

Matrix task::Matrix::operator-(const Matrix& a) const
{
	auto size = a.getSize();
	size_t rows = size.first;
	size_t cols = size.second;

	if (rows != this->row || cols >= this->col)
	{
		throw SizeMismatchException();
	}

	Matrix new_mat(*this);
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			new_mat[i][j] -= a[i][j];
		}
	}
	return new_mat;
}

Matrix task::Matrix::operator*(const Matrix& a) const
{
	auto size = a.getSize();
	size_t rows = size.first;
	size_t cols = size.second;

	if (col != rows)
	{
		throw SizeMismatchException();
	}

	Matrix mult(row, cols);

	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			mult[i][j] = 0;
		}
	}
		

	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			for (int k = 0; k < col; k++)
			{
				mult[i][j] += v[i][k] * a[k][j];
			}
		}
	}

	return mult;
}

Matrix task::Matrix::operator*(const double& a) const
{
	Matrix new_mat(*this);
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			new_mat[i][j] *= a;
		}
	}
	return new_mat;
}

Matrix task::Matrix::operator-() const
{
	Matrix new_mat(*this);
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			new_mat[i][j] *= -1;
		}
	}
	return new_mat;
}

Matrix task::Matrix::operator+() const
{
	return Matrix(*this);
}

double task::Matrix::det() const
{	
	if (col != row)
	{
		throw SizeMismatchException();
	}

	double determinant = 0.0;
	Matrix submatrix(*this);
	size_t n = row;

	if (n == 2)
	{
		return ((v[0][0] * v[1][1]) - (v[1][0] * v[0][1]));
	}		
	else 
	{
		for (int x = 0; x < n; x++) 
		{
			int sub_i = 0;
			for (int i = 1; i < n; i++) 
			{
				int sub_j = 0;
				for (int j = 0; j < n; j++) 
				{
					if (j == x)
					{
						continue;
					}						
					submatrix[sub_i][sub_j] = v[i][j];
					sub_j++;
				}
				sub_i++;
			}
			determinant += (pow(-1, x) * v[0][x] * submatrix.det());
		}
	}
	return determinant;
}

void task::Matrix::transpose()
{
	Matrix copy(*this);

	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++) 
		{
			v[j][i] = copy[i][j];
		}
	}
}

Matrix task::Matrix::transposed() const
{
	Matrix copy(*this);

	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			copy[j][i] = v[i][j];
		}
	}

	return copy;
}

double task::Matrix::trace() const
{
	if (col != row)
	{
		throw SizeMismatchException();
	}

	double result = 0.0;
	for (int i = 0; i < row; i++)
	{
		result += v[i][i];
	}

	return result;
}

std::vector<double> task::Matrix::getRow(size_t row)
{
	if (row >= this->row)
	{
		throw OutOfBoundsException();
	}

	std::vector<double> result(col);
	for (int i = 0; i < col; i++)
	{
		result[i] = v[row][i];
	}
	return result;
}

std::vector<double> task::Matrix::getColumn(size_t column)
{
	if (column >= this->col)
	{
		throw OutOfBoundsException();
	}

	std::vector<double> result(row);
	for (int i = 0; i < row; i++)
	{
		result[i] = v[i][column];
	}
	return result;
}

bool task::Matrix::operator==(const Matrix& a) const
{	
	auto size = a.getSize();
	size_t rows = size.first;
	size_t cols = size.second;
	if (rows != row || cols != col)
	{
		return false;
	}

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			if (!are_equal(a[i][j], v[i][j]))
			{
				return false;
			}
		}
	}
	return true;
}

bool task::Matrix::operator!=(const Matrix& a) const
{
	return !operator==(a);
}

std::pair<size_t, size_t> task::Matrix::getSize() const
{
	return std::pair<size_t, size_t>(row, col);
}

bool task::Matrix::are_equal(double a, double b) const
{
	double diff = a - b;
	return (diff < EPS) && (diff > -EPS);
}

Matrix task::operator*(const double& a, const Matrix& b)
{
	Matrix new_mat(b);
	new_mat *= a;
	return new_mat;
}

std::ostream& task::operator<<(std::ostream& output, const Matrix& matrix)
{
	auto size = matrix.getSize();
	size_t rows = size.first;
	size_t cols = size.second;

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; ++j)
		{
			output << matrix[i][j] << " ";
		}
		output << "\n";
	}
	return output;
}

std::istream& task::operator>>(std::istream& input, Matrix& matrix)
{
	size_t rows, cols;
	input >> rows >> cols;
	matrix.resize(rows, cols);

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; ++j)
		{
			input >> matrix[i][j];
		}
	}
	return input;
}
