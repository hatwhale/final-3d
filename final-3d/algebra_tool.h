/*
Класс AlgebraTool реализует:
- создание скалярной матрицы;
- создание матриц афинных преобразований и проективных матриц;
- создание обратных+транспонированных матриц преобразований
(для преобразования матриц тела и векторов нормалей);

- операции с матрицами и векторами:
-- нахождение среднего n векторов (~ (1/n, 1/n, ...) x (n, m));
-- скалярное произведение двух векторов;
-- векторное произведение двух трехмерных векторов;
-- нахождение длины вектора;
-- нормализация вектора;
-- нахождение барицентрических координат точки в треугольнике;
-- перегруженный оператор v * M  - левое умножение вектора на матрицу;

- создание цвета из трехмерного вектора
*/

#pragma once

#include "matrix.h"
#include "vector.h"

using namespace System::Collections::Generic;

const int DIM = 3;

ref class AlgebraTool
{
private:
	static Matrix^ Translate(Vector^); //vector translation matrix creation
	static Matrix^ Rotate(Vector^); //rotation matrix creation
	static Matrix^ Scale(Vector^); //scale matrix creation
	static Matrix^ Project(Vector^); //orthographic|perspective projection matrix creation
	static Matrix^ InvProjectT(Vector^); //inverse&transponse projection matrix

	static Matrix^ LookAt(Vector^); //look-at matrix
public:
	//matrix creations
	static Matrix^ ScalarMatrix(int, double); //scalar matrix (n, n) creation

	static Matrix^ Transform(Vector^, System::String^); //transformation matrix
	static Matrix^ InvTransformT(Vector^, System::String^); //inverse&transponse matrix for normals' transformation

	//operations with matrices
	static Vector^ Barycenter(Matrix^ const); //Barycenter (mean rows) calculation

	//operations with matrices and vectors
	static double Dot(List<double>^, Matrix^ const, int); //scalar product of vector & matrix column

	//operations on vectors
	static double Dot(List<double>^, List<double>^); //Dot product
	static Vector^ Cross(List<double>^, List<double>^); //vector product

	static double Dot(List<double>^, List<double>^, List<double>^); //scalar product of ca & cb
	static Vector^ Cross(List<double>^, List<double>^, List<double>^); //vector product of ca & cb

	static double Length(List<double>^); //vector Length
	static Vector^ Normalize(Vector^); //vector normalization

	static Vector^ RenderCoord(Matrix^, int, int); //barycentric coordinates in project space
	static Vector^ WorldCoord(Matrix^, Vector^); //barycentric coordinates in world space: correction of distortions
};

//left-Dot multiplication: v * M
Vector^ operator * (List<double>^, Matrix^ const);
