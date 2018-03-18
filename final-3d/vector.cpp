#include "vector.h"

//2-dimensional vector
Vector::Vector(double x, double y) : List<double>(){
	this->Add(x);
	this->Add(y);
}
//3-dimensional vector
Vector::Vector(double x, double y, double z) : List<double>(){
	this->Add(x);
	this->Add(y);
	this->Add(z);
}
//4-dimensional vector
Vector::Vector(double x, double y, double z, double w) : List<double>(){
	this->Add(x);
	this->Add(y);
	this->Add(z);
	this->Add(w);
}
//list to vector
Vector::Vector(List<double>^ vector) : List<double>() {
	for each(double element in vector)
		this->Add(element);

}
//range of list to vector
Vector::Vector(List<double>^ vector, int count) : List<double>() {
	for(int i = 0; i < count && i < vector->Count; i++)
		this->Add(vector[i]);
}
//color to vector
Vector::Vector(System::Drawing::Color^ color) : List<double>() {
	this->Add(color->R);
	this->Add(color->G);
	this->Add(color->B);
}
//delete vector
Vector::~Vector() {
	this->Clear();
}

Vector^ Vector::operator += (List<double>^ const vector) {
	for(int i = 0; i < vector->Count; i++)
		this[i] += vector[i];
	return this;
}
Vector^ Vector::operator -= (List<double>^ const vector) {
	for(int i = 0; i < vector->Count; i++)
		this[i] -= vector[i];
	return this;
}
Vector^ Vector::operator *= (List<double>^ const vector) {
	for(int i = 0; i < vector->Count; i++)
		this[i] *= vector[i];
	return this;
}
Vector^ Vector::operator *= (double k) {
	for(int i = 0; i < this->Count; i++)
		this[i] *= k;
	return this;
}
Vector^ Vector::operator /= (double k) {
	for(int i = 0; i < this->Count; i++)
		this[i] /= k;
	return this;
}

Vector^ Vector::operator + (List<double>^ vector) { return (gcnew Vector(this) += vector); }
Vector^ Vector::operator - (List<double>^ vector) { return (gcnew Vector(this) -= vector); }
Vector^ Vector::operator * (List<double>^ vector) { return (gcnew Vector(this) *= vector); }
Vector^ Vector::operator * (double k) { return (gcnew Vector(this) *= k); }
Vector^ Vector::operator / (double k) { return (gcnew Vector(this) /= k); }

Vector^ Vector::operator - () { return (gcnew Vector(this) * 0 -= this); }

bool Vector::operator == (double k) {
	for(int i = 0; i < this->Count; i++)
		if(this[i] != k)
			return false;
	return true;
}

Vector^ operator * (double k, Vector^ const vector) {
	return vector * k;
}
Vector^ operator / (double k, Vector^ const vector) {
	Vector^ k_inv_vector = gcnew Vector();
	for(int i = 0; i < vector->Count; i++)
		k_inv_vector->Add(k / vector[i]);
	return k_inv_vector;
}
