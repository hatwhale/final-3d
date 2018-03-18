#include "matrix.h"

//matrix creation:

//empty matrix (0, m)
Matrix::Matrix(unsigned int columns) {
	this->matrix = gcnew List<List<double>^>();
	this->rows = 0;
	this->columns = columns;
}
//empty matrix (n, m)
Matrix::Matrix(unsigned int n, unsigned int m) {
	this->matrix = gcnew List<List<double>^>(n);
	this->rows = 0;
	this->columns = m;
	for(unsigned int i = 0; i < n; i++) {
		List<double>^ row = gcnew List<double>(m);
		for(unsigned int j = 0; j < m; j++)
			row->Add(0);
		this->Add(row);
	}
}
//copy matrix
Matrix::Matrix(Matrix^ const M) {
	this->matrix = gcnew List<List<double>^>(M->shape(0));
	this->rows = 0;
	this->columns = M->shape(1);
	for(unsigned int row = 0; row < M->shape(0); row++) {
		List<double>^ row_vector = gcnew List<double>(columns);
		for(unsigned int col = 0; col < columns; col++)
			row_vector->Add(M->Get(row, col));
		this->Add(row_vector);
	}
}
Matrix::~Matrix() {
	for each(List<double>^ row in matrix)
		row->Clear();
	delete [] matrix;
}

//row addition
void Matrix::Add(List<double>^ vector) {
	this->matrix->Add(vector);
	this->rows++;
}
//remove row (i, )
void Matrix::Remove(int row) {
	row += row < 0 ? this->rows : 0;
	matrix->RemoveAt(row);
	this->rows--;
}
//get element (i, j)
double Matrix::Get(int row, int col) {
	row += row < 0 ? this->rows : 0;
	col += col < 0 ? this->columns : 0;
	return matrix[row][col];
}
//get row (i, )
List<double>^ Matrix::Get(int row) {
	row += row < 0 ? this->rows : 0;
	return matrix[row];
}
//set element (i, j)
void Matrix::Set(double value, int row, int col) {
	row += row < 0 ? this->rows : 0;
	col += col < 0 ? this->columns : 0;
	matrix[row][col] = value;
}
//set row (i, )
void Matrix::Set(List<double>^ vector, int row) {
	row += row < 0 ? this->rows : 0;
	for(unsigned int col = 0; col < this->columns; col++)
		matrix[row][col] = vector[col];
}
//find a vector among the rows
int Matrix::Find(List<double>^ vector, double epsilon) {
	int find_flag = -1;
	if(vector->Count != this->columns)
		return find_flag;
	for(unsigned int row = 0; row < this->rows; row++) {
		int check = 0;
		for(unsigned int col = 0; col < this->columns; col++) {
			check += int(System::Math::Abs(matrix[row][col] - vector[col]) > epsilon);
		}
		if(check == 0) {
			find_flag = row;
			break;
		}
	}
	return find_flag;
}

//get matrix dimensions
unsigned int Matrix::shape(int i) {
	return i == 0 ? rows : columns;
}

//left-dot multiplication: v * M
List<double>^ Matrix::left_dot (List<double>^ vector) {
	List<double>^ row_vector = gcnew List<double>();
	for(unsigned int col = 0; col < this->columns; col++) {
		double inner_product = 0;
		for(unsigned int inner = 0; inner < this->rows; inner++)
			inner_product += this->Get(inner, col) * vector[inner];
		row_vector->Add(inner_product);
	}
	return row_vector;
}
//matrix multiplication: (n, m) -> (n, m) * (m, m) = (n, m)
Matrix^ Matrix::operator *= (Matrix^ const M) {
	for(unsigned int row = 0; row < this->rows; row++) {
		List<double>^ new_row_vector = M->left_dot(this->Get(row));
		this->Set(new_row_vector, row);
		delete new_row_vector;
	}
	return this;
}

//transpose of square matrix
Matrix^ Matrix::T() {
	for(unsigned int row = 1; row < this->rows; row++)
		for(unsigned int col = 0; col < row; col++) {
			double row_col = this->Get(row, col);
			double col_row = this->Get(col, row);
			this->Set(col_row, row, col);
			this->Set(row_col, col, row);
		}
	return this;
}

//write matrix to the file
void Matrix::ToFile(StreamWriter^ stream_writer, unsigned int dim) {
	for(unsigned int row = 0; row < this->rows; row++) {
		for(unsigned int col = 0; col < dim; col++)
			stream_writer->Write("{0, -5:}", (this->Get(row, col)).ToString() + " ");
		stream_writer->WriteLine();
	}
}