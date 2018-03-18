/*
����� Model ��������� ������������� �������� �������, ���� ��������� �����:
- ��� ������ (���������� �������);
- ������� ������ (������ ������� ������������ ����� ������������� ���������);
- ������ ������ (����� �� ����������� ����������, ������ ����� �������� ��� ������� ������ � ������� ������);
- ����� ������ ������ (�����������, ��� ������� �������� ��������-������ ������);
- ������� ���������� ���������, �������������� �������� ������;
- ������ "���������� ������", ��������������� ������ ������;
- ������� ���� (��������� �� ������������� ��������� ����������-������ ������ - ����������������� ������� ������);
- ������� �������� ������ (����������������� ������� ������ ������� ����� ���������� �� ������� ����� �������� �������� ������);
- ������� ������ ��� �������� (�.�. ������� ������ ����� ������������ ��������������);
- ������� ���� ��� �������� (�.�. ������� ���� ������ � ��������� ����� ������������ ��������������);
- �������������� ������, ���������� ���������� MaterialData;
- �������������� ��������, ���������� �� ������, ���������� ���������� TextureData;

������������, �����������:
- ��������� ������ �� ����� �� ������ ������ � ������ (��������������, ��� ������� ������, �� ����������);
- ������������ ��������� ����;
- ������������ ���� ��������, � ������:
-- �������, ��� ������ ��� ���� ������ ��� ��������;
-- ����� � �� �����������;
-- ��� � ��� �����������;
��������� ��������������� ������� ������ � ������ ������
� �������� ������� ���� � �������� ������, � ����� ������ ������ ������;

� ��������� ������:
- ��������� ������� �����, ������� ������� � ������� ������� ����� �� �������;
- ��������� ������� �����, ������� ������� � ������� ����� ����� �������� �� �������;
- ��������� ������� ������� ������� � ������� �������� ������ ��������� ����� �� �������;
- ��������� ���� ������, ���������� ������ � ������ ������ ������, � ����� ������ ������;
- �������� ������������ �������� � ������ �� �����;
- �����������, �������������� �� ������;
- ��������� ����������� ������� ������ ������ ������;
- ���������� ������ � ���� (���������� ������� ������ � ������ ������);
- ��������� ������� �������������� ������� ������ � ������� ����;
- ���������� ������� ������ � ������� ���� ����� ������������ ��������������.
*/

#pragma once

#include "algebra_tool.h"
#include "data_structures.h"

using namespace System::Drawing;
using namespace System::Collections::Generic;

const double EPS = 1e-7;

private ref class Model {
private:
	System::String^ type;

	Matrix^ vertices;
	List<List<unsigned int>^>^ faces;
	List<double>^ center;

	Matrix^ tex_vertices;
	List<List<unsigned int>^>^ tex_faces;

	Matrix^ body_matrix;
	Matrix^ vertex_norms;

	Matrix^ project_vertices;
	Matrix^ project_body_matrix;

	//model initialization and constructors
	void ModelInitialize(System::String^); //model initialization
	void BodyMatrixCalculate(); //body matrix calculation
	void BodyMatrixCalculate(System::String^); //body matrix calculation for solids of revolution
	void VertexNormsCalculate(); //vertex normals calculation
	void LoadModelConstruct(System::String^); //load model from the file
	void PlatonicSolidConstruct(System::String^, double);  //platonic solids: cube, tetrahedron, octahedron, icosahedron, dodecahedron
	void CylinderConstruct(double, double, System::String^); //cylinder creation for solids of revolution
	void SphereDConstruct(System::String^, double, double); //sphere and its derivatives: kinder, surprise, pear, almost_thorus, moon, drop, fruit, garlic
	void ThorusDConstruct(System::String^, double, double, double); //thorus and its derivatives: thorus, spiral, spring, conical_spring
public:
	TextureData^ tex_data;
	MaterialData^ mat_data;

	//model creations
	Model(System::String^);
	Model(System::String^, Color);
	Model(System::String^, double);
	Model(System::String^, double, Color);
	Model(System::String^, double, double);
	Model(System::String^, double, double, Color);
	Model(System::String^, double, double, double);
	Model(System::String^, double, double, double, Color);
	~Model();

	Vector^ Center() { return gcnew Vector(center, DIM); }
	unsigned int FacesCount() { return faces->Count; }
	unsigned int VerticesCount() { return vertices->shape(0); }

	void ApplyTexture(System::String^); //load a texture from the file
	bool Texturized() { return tex_data->defined; }

	System::String^ GetType() { return this->type; } //get type

	Color^ GetColor() { return mat_data->color; } //get deafult model color
	Color^ GetColor(List<double>^); //get color from texture

	//get vertices and normals

	//get vertex (face_num, vertex_num)
	List<double>^ Get(int face_num, int vertex_num) { return vertices->Get(faces[face_num][vertex_num]); }
	//get normal vector of face
	List<double>^ GetFaceNorm(int face_num) { return body_matrix->Get(face_num); }
	//get normal vector of vertex
	List<double>^ GetVertexNorm(int vertex_num) { return vertex_norms->Get(vertex_num); }
	//get project vertex (face_num, vertex_num)
	List<double>^ GetProject(int face_num, int vertex_num) { return project_vertices->Get(faces[face_num][vertex_num]); }
	//get normal vector of project face
	List<double>^ GetProjectNorm(int face_num) { return project_body_matrix->Get(face_num); }

	Matrix^ Get(int); //get face matrix (with row-vertices)
	Matrix^ GetTexFace(int); //get face matrix from texture's faces
	Matrix^ GetVertexNorms(int); //get vertex normals' matrix of face
	Matrix^ GetProject(int); //get project face matrix (with row-vertices)

	//save model to the file
	void ToFile(System::String^);

	//verticies & body_matrix transformations
	void Transform(Vector^, System::String^);
	void scale(double); //(r, r, r)-scale

	//projections
	Matrix^ VerticesProjectCalculate(Matrix^); //calculate projection of verticies
	Matrix^ BodyMatrixProjectCalculate(Matrix^); //calculate "projection" of normals
};