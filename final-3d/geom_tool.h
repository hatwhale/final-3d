/*
����� GeomTool �������� ��������������� ��� ����������, � ���������:
- ���������� ������� �������� {x : sort_dict{y : b}} ��� ����� (����� DrawLine);
- ���������� ������� �������� {x : sort_dict{y : b}}  ��� �������������� (����� DrawPolygon)
(� ������ ��������� ��������, ����������� �� ��������� ������);
��� (x, y) - ���������� �������, b - ������ ��������,
��� ����� ������������ ���������� ������� � �������� ������
(��� ���������� ��� ��������� �������� ��� ��������������);
��� �������������� ������������ ���������� ������� �� ������� ��������������
(��� ���������� ��� ���������� "���������" �������)

��� ������ ���������������� ������ AddPixel,
������������ ���������� ������� (x, y) � dict{x : sort_dict{y : b}}
� ������ ��� ���������� � �������� ������ � �� ������� (�������� boundary)
*/

#pragma once

#include "data_structures.h"

using namespace System::Drawing;
using namespace System::Linq;

ref class GeomTool
{
public:
	//add pixel (x, y) to dict{x : sort_dict{y : b}}
	void AddPixel(Dictionary<int, SortedDictionary<int, bool>^>^, int, int, bool);
	//add line's pixels to dict{x : sort_dict{y : b}}
	void DrawLine(Dictionary<int, SortedDictionary<int, bool>^>^, Point^, Point^, RenderData^);
	//add polygon's pixels to dict{x : sort_dict{y : b}}
	Dictionary<int, SortedDictionary<int, bool>^>^ DrawPolygon(array<Point>^, RenderData^);
};

