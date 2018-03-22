/*
Класс Matrix реализует n-мерную матрицу, как упорядоченный набор векторов-строк
(это полезно как для хранения вершин в полигональном описании объектов,
так и для левого умножения на вектор-строку и умножения матрицы справа),
с определенными на ней следующими операциями:
- добавление вектора-строки в конец матрицы;
- удаление вектора-строки по индексу;
- получение вектора-строки и элемента по индексу;
- изменение вектора-строки и элемента по индексу;
- нахождение размерности матрицы;
- левое умножение вектора на матрицу;
- перегруженный оператор (n, m) *= (m, m)
- транспонирование квадратной матрицы;
(умножение на квадратную матрицу - необходимо для реализации афинных преобразований);
- запись в файл;
*/

#pragma once

using namespace System::Collections::Generic;
using namespace System::IO;

ref class Matrix {
private:
	List<List<double>^> ^matrix;
	unsigned int rows, columns;
public:
	//matrix creation:
	Matrix(unsigned int); //empty matrix (0, m)
	Matrix(unsigned int, unsigned int); //empty matrix (n, m)
	Matrix(Matrix^ const); //copy matrix
	~Matrix();

	void Add(List<double>^); //row addition
	void Remove(int); //remove row (i, )
	double Get(int, int); //get element (i, j)
	List<double>^ Get(int); //get row (i, )
	void Set(double, int, int); //set element (i, j)
	void Set(List<double>^, int); //set row (i, )
	int Find(List<double>^, double); //find a vector among the rows

	unsigned int shape(int); //get matrix dimensions

	List<double>^ left_dot(List<double>^); //left-dot multiplication: v * M
	Matrix^ operator *= (Matrix^ const); //matrix multiplication: (n, m) -> (n, m) * (m, m)
	Matrix^ operator * (Matrix^ const M) { return gcnew Matrix(this) *= M; }

	Matrix^ T();//transpose of square matrix

	void ToFile(StreamWriter^, unsigned int); //write matrix to the file
};
