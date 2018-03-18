/*
Класс Vector реализует n-мерный вектор, наследуемый от List<double>,
с расширенным набором конструкторов и определенным на нем "базовым" набором операторов
(перегруженные операторы и конструкторы упрощают работу с математическими векторами,
делая ее нагляднее в ряде случаев: например, при закраске отображаемых моделей)
*/

#pragma once

using namespace System::Collections::Generic;

ref class Vector : List<double> 
{
public:
	//constructos
	Vector() : List<double>() {} //empty constructor
	Vector(double, double); //2-dimensional vector constructor
	Vector(double, double, double); //3-dimensional vector constructor
	Vector(double, double, double, double); //4-dimensional vector constructor
	Vector(List<double>^); //copy constructor: list to vector
	Vector(List<double>^, int); //copy constructor: range of list to vector
	Vector(System::Drawing::Color^); //copy constructor: color to vector
	virtual ~Vector();

	//operators
	Vector^ operator += (List<double>^ const); //element-wise addition
	Vector^ operator -= (List<double>^ const); //element-wise subtraction
	Vector^ operator *= (List<double>^ const); //element-wise multiplication
	Vector^ operator *= (double); //scalar multiplication

	Vector^ operator /= (double); //defined through 'vector *= double'
	Vector^ operator + (List<double>^ const); //defined through 'vector += list'
	Vector^ operator - (List<double>^ const); //defined through 'vector -= list'
	Vector^ operator * (List<double>^ const); //defined through 'vector *= list'
	Vector^ operator * (double); //defined through 'vector *= double'
	Vector^ operator / (double); //defined through 'vector /= double'

	Vector^ operator - (); //negation, defined through 'vector -= vector'

	bool operator == (double); //element-wise equality
};

Vector^ operator * (double, Vector^ const); //defined through 'vector * double'
Vector^ operator / (double, Vector^ const); //element-wise inversion and multiplication 'double / vector'