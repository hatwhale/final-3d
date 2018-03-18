/*
класс RenderTool реализует рендеринг изображения, имея следующие члены:
- проективная матрица (для проективного преобразования матриц вершин моделей);
- обратная транспонированная проективная матрица (для проективного преобразования матриц тел моделей);
- буфер кадра (bitmap, на который выводится изображение);
- z-буфер (bitmap, в который сохраняется глубина);
- вспомогательный буфер, используемый для рендеринга полигонов;
- ширина и высота буферов (общие для всех);
- вспомогательные черные и белые кисти и ручки (brush и pen);
- параметры рендеринга, задаваемые структурой RenderData;
- параметры освещения, задаваемые структурой LightData;
- инструмент для матричных и векторных операций, задаваемый классом AlgebraTool;

при помощи следующих методов:
- Render:
	очищает буферы,
	запускает цикл по списку моделей,
	для каждой из моделей вычисляет матрицу вершин и матрицу тела после проекции,
	запускает цикл по списку граней для каждой из моделей,
	выполняет проверку на видимость грани (early depth test) при истинности параметра,
	для каждой грани вызывает метод RenderFace;
- RenderFace:
	для каждого из типов освещения, закраски и затенения:
		- освещение: активное или неактивное
		- закраска: заполнение полигонов или отображение каркаса (освещение не влияет на каркас)
		- затенение: плоская модель, по гуро и по фонгу
	заполняет буфер кадра, для каждого пикселя вызывая метод RenderPixel;
	(исключение составляет 2 случая:
	когда освещение активно, полигоны заполняются, модель затенения плоская и буфер глубины отключен;
	когда полингоны не заполняются и буфер глубины отключен;
- RenderPixel:
	в зависимости от использования буфера глубины либо просто заполняет пиксель и ставит метку модели
	(ее порядковый номерв списке), либо выполняет тест на глубину;

- GetModelNum по пикселю возвращает номер модели, который хранится в буфере глубины.
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