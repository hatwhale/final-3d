/*
класс GeomTool является вспомогательным для рендеринга, и реализует:
- заполнение словаря пикселей {x : sort_dict{y : b}} для линии (метод DrawLine);
- заполнение словаря пикселей {x : sort_dict{y : b}}  для многоугольника (метод DrawPolygon)
(с учетом отсечения пикселей, находящихся за пределами экрана);
где (x, y) - координаты пикселя, b - булево значение,
для линии показывающее нахождение пикселя в пределах экрана
(это необходимо для отсечения пикселей для многоугольника);
для многоугольника показывающее нахождение пикселя на границе многоугольника
(это необходимо для рендеринга "каркасных" моделей)

при помощи вспомогательного метода AddPixel,
реализующего добавления пикселя (x, y) в dict{x : sort_dict{y : b}}
с учетом его нахождения в пределах экрана и на границе (параметр boundary)
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

