/*
Структура EventData отвечает за хранение данных о событии;
структура RenderData отвечает за хранение параметров рендеринга;
структура LightData отвечает за хранение параметров освещения;
структура MaterialData отвечает за хранение характеристик модели;
структура TextureData отвечает за хранение текстуры и ее характеристик,
в ней также реализована закрузка текстуры из файла.
*/

#pragma once

using namespace System::Collections::Generic;
using namespace System::Drawing;

//events characteristics
public ref struct EventData {
	System::String ^Type; //type of event
	bool MouseHold; //mouse-hold flag
	Point MouseDown; //mouse click position
	double scale; //scale of objects
	unsigned int model_num; //number of current model for affine transformations
	double speed; //speed of 'peg-top' and 'carousel' events
	EventData() {
		MouseHold = false;
		Type = "rotate";
		scale = 1;
		model_num = 1;
		speed = 1.0;
	}
};

//rendering characteristics
public ref struct RenderData {
	Rectangle^ box; //render box
	List<double>^ camera_pos; //point of view (camera position)
	System::String^ projection; //type of projection: perspective or orthographic
	double view_z, near_z, far_z;
	bool early_depth_test; //early-depth-test flag (early discarding invisible faces by their normals or not)
	bool depth_test; //depth-test flag (using z-buffer or not)
	bool fill; //fill-polygons flag (render lines or filled polygons)
	System::String^ shading; //type of shading: flat, gourand or fong
	bool depth_render_mode; //render mode (render z-buffer or frame buffer)
	bool shadow_test; //render shadow map or not
	RenderData(Rectangle^ box, List<double>^ position, bool depth_flag) {
		this->box = box;
		camera_pos = position;
		projection = "perspective";
		view_z = -500; near_z = -400; far_z = 400;
		early_depth_test = depth_flag;
		depth_test = depth_flag;
		fill = false;
		shading = "flat";
		depth_render_mode = depth_flag;
		shadow_test = false;
	}
};

//light characteristics
public ref struct LightData {
	bool active; //light-activation flag
	bool move; //light-moving flag
	List<double>^ light_pos; //position of light source
	double amb; //ambient coefficient of light
	double diff; //diffuse coefficient of light
	double spec; //specular coefficient of light
	LightData(List<double>^ position) {
		active = false;
		move = false;
		light_pos = position;
		diff = 1.0;
		amb = 0.1 * diff;
		spec = 1.0;
	}
};

//material characteristics
public ref struct MaterialData {
	Color^ color; //model color
	double diff; //diffuse coefficient of material
	double amb; //ambient coefficient of material
	double spec; //specular coefficient of material
	double sheen; //sheen coefficient of material
	MaterialData() {
		this->color = Color::Green;
		diff = 1.0;
		amb = diff;
		spec = 0.2;
		sheen = 32.0;
	}
	MaterialData(Color^ color) {
		this->color = color;
		diff = 1.0;
		amb = diff;
		spec = 0.2;
		sheen = 32.0;
	}
};

//texture characteristics
public ref struct TextureData {
	array<System::Byte>^ color_values;
	int Width;
	int Height;
	System::String^ path;
	bool defined;
	TextureData() {defined = false;}
	TextureData(System::String^  tex_path) {
		path = tex_path;
		defined = true;
		//create texture's bitmap
		Bitmap^ tex_bitmap = gcnew Bitmap(Image::FromFile(tex_path));
		Width = tex_bitmap->Width;
		Height = tex_bitmap->Height;
		//lock the bitmaps' bits
		Imaging::BitmapData^ tex_data = tex_bitmap->LockBits(Rectangle(0, 0, Width, Height),
															 Imaging::ImageLockMode::ReadWrite,
															 tex_bitmap->PixelFormat);
		//declare an array to hold the bytes of the bitmap
		int bytes = System::Math::Abs(tex_data->Stride) * Height;
		color_values = gcnew array<System::Byte>(bytes);
		//copy the RGB values into the array
		System::Runtime::InteropServices::Marshal::Copy(tex_data->Scan0, color_values, 0, bytes);
		//unlock the bits
		tex_bitmap->UnlockBits(tex_data);
		delete tex_bitmap;
	}
	~TextureData() { delete[] color_values; }
};
