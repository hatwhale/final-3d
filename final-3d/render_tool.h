/*
����� RenderTool ��������� ��������� �����������, ���� ��������� �����:
- ����������� ������� (��� ������������ �������������� ������ ������ �������);
- �������� ����������������� ����������� ������� (��� ������������ �������������� ������ ��� �������);
- ����� ����� (bitmap, �� ������� ��������� �����������);
- z-����� (bitmap, � ������� ����������� �������);
- ��������������� �����, ������������ ��� ���������� ���������;
- ������ � ������ ������� (����� ��� ����);
- ��������������� ������ � ����� ����� � ����� (brush � pen);
- ��������� ����������, ���������� ���������� RenderData;
- ��������� ���������, ���������� ���������� LightData;
- ���������� ��� ��������� � ��������� ��������, ���������� ������� AlgebraTool;

��� ������ ��������� �������:
- Render:
	������� ������,
	��������� ���� �� ������ �������,
	��� ������ �� ������� ��������� ������� ������ � ������� ���� ����� ��������,
	��������� ���� �� ������ ������ ��� ������ �� �������,
	��������� �������� �� ��������� ����� (early depth test) ��� ���������� ���������,
	��� ������ ����� �������� ����� RenderFace;
- RenderFace:
	��� ������� �� ����� ���������, �������� � ���������:
		- ���������: �������� ��� ����������
		- ��������: ���������� ��������� ��� ����������� ������� (��������� �� ������ �� ������)
		- ���������: ������� ������, �� ���� � �� �����
	��������� ����� �����, ��� ������� ������� ������� ����� RenderPixel;
	(���������� ���������� 2 ������:
	����� ��������� �������, �������� �����������, ������ ��������� ������� � ����� ������� ��������;
	����� ��������� �� ����������� � ����� ������� ��������;
- RenderPixel:
	� ����������� �� ������������� ������ ������� ���� ������ ��������� ������� � ������ ����� ������
	(�� ���������� ������ ������), ���� ��������� ���� �� �������;

- GetModelNum �� ������� ���������� ����� ������, ������� �������� � ������ �������.
*/

#pragma once

#include "algebra_tool.h"
#include "geom_tool.h"
#include "data_structures.h"
#include "model.h"

using namespace System::Drawing;

ref class RenderTool
{
private:
	AlgebraTool^ tool;
	RenderTool^ shadow_tool;
	GeomTool^ geom_tool;

	double view_z;

	Matrix^ render_matrix;
	Matrix^ body_render_matrix;

	int Width, Height;
	Point^ origin;

	Imaging::BitmapData^ bitmap_data;
	array<System::Byte>^ color_values;
	array<System::Byte>^ z_buffer;

	double LightCoef(Vector^, Vector^, MaterialData^); //light coefficient calculation
public:
	LightData^  light_data;
	RenderData^ render_data;
	Bitmap^ render_bitmap;

	void RenderMatricesCalculate(); //render matrices calculation
	RenderTool(int, int, RenderData^, LightData^); //render tool initialization

	//set pixel, considering the depth test
	void RenderPixel(int, int, Color^, int, unsigned int);
	void RenderPixel(int, int, Color^, Matrix^, unsigned int);
	void RenderPixel(int, int, Color^, Matrix^, Vector^, unsigned int);

	//check shadows
	bool Shaded(Matrix^, Vector^, unsigned int, System::String^);

	//get model number from pixel: save it in z-buffer
	int GetModelNum(int, int);

	//render face of model with different types of lighting, filling and shading
	void RenderFace(Model^, unsigned int, unsigned int);
	//render list of models
	void Render(List<Model^>^);
};

Color^ operator * (double, Color^);