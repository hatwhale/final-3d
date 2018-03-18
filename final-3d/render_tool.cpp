#include "render_tool.h"

//color multiplication: [0..255, 0..255, 0..255] * coef -> [0..255, 0..255, 0..255]
Color^ operator * (double coef, Color^ color) {
	double R = coef * color->R, G = coef * color->G, B = coef * color->B;
	return Color::FromArgb(int(System::Math::Round(R > 0 ? R < 255 ? R : 255 : 0)),
						   int(System::Math::Round(G > 0 ? G < 255 ? G : 255 : 0)),
						   int(System::Math::Round(B > 0 ? B < 255 ? B : 255 : 0)));
}

void RenderTool::RenderMatricesCalculate() {
	delete render_matrix;
	delete body_render_matrix;

	//look-at matrices
	Vector^ camera_pos = gcnew Vector(render_data->camera_pos, DIM);
	Matrix^ look_at = AlgebraTool::Transform(camera_pos, "look_at");
	Matrix^ body_look_at = AlgebraTool::InvTransformT(camera_pos, "look_at");
	delete camera_pos;
	//translation matrices
	Vector^ shift = gcnew Vector(0, 0, AlgebraTool::Length(render_data->camera_pos) + view_z);
	Matrix^ translation = AlgebraTool::Transform(shift, "translate");
	Matrix^ body_translation = AlgebraTool::InvTransformT(shift, "translate");
	delete shift;
	//projection matrices
	double z_coef = 0;
	if(render_data->projection == "perspective") 
		z_coef = -AlgebraTool::Length(render_data->camera_pos);
	Vector^ project_vector = gcnew Vector(z_coef, render_data->near_z, render_data->far_z);
	Matrix^ project_matrix = AlgebraTool::Transform(project_vector, "project");
	Matrix^ body_project_matrix = AlgebraTool::InvTransformT(project_vector, "project");
	delete project_vector;

	render_matrix = (look_at *= translation) *= project_matrix;
	body_render_matrix = (body_look_at *= body_translation) *= body_project_matrix;

	delete project_matrix;
	delete body_project_matrix;
	delete translation;
	delete body_translation;
}

RenderTool::RenderTool(int Width, int Height, RenderData^ render_data, LightData^ light_data) {
	tool = gcnew AlgebraTool();
	geom_tool = gcnew GeomTool();

	this->render_data = render_data;
	this->light_data = light_data;

	render_matrix = gcnew Matrix(0);
	body_render_matrix  = gcnew Matrix(0);

	view_z = render_data->view_z;

	this->Width = Width;
	this->Height = Height;
	origin = gcnew Point(Width/2, Height/2);

	render_bitmap = gcnew Bitmap(Width, Height);

	//lock the bitmaps' bits
	bitmap_data = render_bitmap->LockBits(Rectangle(0, 0, Width, Height),
										  Imaging::ImageLockMode::ReadWrite,
										  render_bitmap->PixelFormat);

	//declare an arrays to hold the bytes of the bitmaps
	int bytes = System::Math::Abs(bitmap_data->Stride) * Height;
	color_values = gcnew array<System::Byte>(bytes);
	z_buffer = gcnew array<System::Byte>(bytes);

	//unlock the bits
	render_bitmap->UnlockBits(bitmap_data);
}

//light coefficient calculation
double RenderTool::LightCoef(Vector^ world_pos, Vector^ norm, MaterialData^ mat_data) {
	//direction to light point calculation
	Vector^ light_dir = AlgebraTool::Normalize(gcnew Vector(light_data->light_pos, DIM) -= world_pos);
	//direction and distance to view point calculation
	Vector^ view_dir = gcnew Vector(render_data->camera_pos, DIM) -= world_pos;
	double distance = AlgebraTool::Length(view_dir);
	view_dir /= distance;
	//reflect direction (of light) calculation
	Vector^ reflect_dir = AlgebraTool::Normalize((2 * AlgebraTool::Dot(light_dir, norm)) * norm -= light_dir);
	//attenuation coefficient calculation
	double attenuation = 1.0/(0.5 + 0.001 * distance);
	//diffuse coefficient calculation
	double diff = System::Math::Max(AlgebraTool::Dot(norm, light_dir), 0.0);
	//specular coefficient calculation
	double spec = System::Math::Pow(System::Math::Max(AlgebraTool::Dot(view_dir, reflect_dir), 0.0), mat_data->sheen);
	//color coefficient calculation
	double light_coef = mat_data->amb * light_data->amb +
						attenuation * (diff * mat_data->diff * light_data->diff + 
									   spec * mat_data->spec * light_data->spec);
	delete norm;
	delete view_dir;
	delete light_dir;
	delete reflect_dir;
	
	return light_coef;
}

//set pixel, considering the depth test

void RenderTool::RenderPixel(int x, int y, Color^ color, int new_depth, unsigned int model_num) {
	z_buffer[4 * (y * Width + x) + 3] = 254 - model_num;
	z_buffer[4 * (y * Width + x) + 2] = 255 - new_depth;
	z_buffer[4 * (y * Width + x) + 1] = 255 - new_depth;
	z_buffer[4 * (y * Width + x)] = 255 - new_depth;

	color_values[4 * (y * Width + x) + 3] = color->A;
	color_values[4 * (y * Width + x) + 2] = color->R;
	color_values[4 * (y * Width + x) + 1] = color->G;
	color_values[4 * (y * Width + x)] = color->B;
}

void RenderTool::RenderPixel(int x, int y, Color^ color, Matrix^ render_face, Vector^ coord, unsigned int model_num) {
	if(!render_data->depth_test)
		this->RenderPixel(x, y, color, 255, model_num);
	else {
		int depth = 255 - z_buffer[4 * (y * Width + x)];
		int new_depth = int(System::Math::Round(255 * AlgebraTool::Dot(coord, render_face, 2)));
		if(!(coord == 0.0) && new_depth >= 0 && new_depth < depth)
			this->RenderPixel(x, y, color, new_depth, model_num);
	}
}

void RenderTool::RenderPixel(int x, int y, Color^ color, Matrix^ render_face, unsigned int model_num) {
	if(!render_data->depth_test)
		this->RenderPixel(x, y, color, 255, model_num);
	else {
		Vector^ coord = AlgebraTool::RenderCoord(render_face, x - origin->X, y - origin->Y);
		this->RenderPixel(x, y, color, render_face, coord, model_num);
		delete coord;
	}
}

bool RenderTool::Shaded(Matrix^ world_face, Vector^ coord, unsigned int model_num, System::String^ type) {
	if(render_data->shadow_test) {
		Vector^ vector = gcnew Vector(coord);
		vector *= world_face;
		vector *= shadow_tool->render_matrix;
		vector /= vector[3];
		int v_x = int(System::Math::Round(vector[0])) + origin->X;
		int v_y = int(System::Math::Round(vector[1])) + origin->Y;
		int v_z = int(System::Math::Round(255 * vector[2]));
		delete vector;
		Rectangle^ box = shadow_tool->render_data->box;
		v_x = v_x >= box->Left ? v_x < box->Right ? v_x : box->Right - 1 : box->Left;
		v_y = v_y >= box->Top ? v_y < box->Bottom ? v_y : box->Bottom - 1 : box->Top;
		int light_depth = 255 - shadow_tool->z_buffer[4 * (v_y * Width + v_x)];
		int light_model_num = shadow_tool->GetModelNum(v_x, v_y);
		bool shaded = model_num != light_model_num || type == "thorus";
		if(type == "thorus")
			light_depth += 10;
		return v_z > light_depth && light_model_num != -1 && shaded;
	}
	return false;
}

//get model number from pixel
int RenderTool::GetModelNum(int X, int Y) {
	return 254 - z_buffer[4 * (Y * Width + X) + 3];
}

//types of light: active or not
//types of filling: fill polygon or draw lines
//types of shading: flat, gourand, fong
void RenderTool::RenderFace(Model^ model, unsigned int model_num, unsigned int face_num) {
	//polygon points calculation
	Matrix^ render_face = model->GetProject(face_num);
	array<Point>^ points = gcnew array<Point>(render_face->shape(0));
	for(unsigned int i = 0; i < render_face->shape(0); i++) 
		points[i] = Point(int(System::Math::Round(render_face->Get(i, 0))) + origin->X,
						  int(System::Math::Round(render_face->Get(i, 1))) + origin->Y);
	//polygon lines creation
	Dictionary<int, SortedDictionary<int, bool>^>^ pixels = geom_tool->DrawPolygon(points, render_data);
	delete points;

	//active lighting, filling polygons
	if(render_data->fill && light_data->active && !render_data->depth_render_mode) {
		Matrix ^world_face = model->Get(face_num);
		Matrix ^tex_face = model->GetTexFace(face_num);
		Matrix ^vertex_norms = model->GetVertexNorms(face_num);

		//turn faces towards camera
		Matrix ^neg_matrix = AlgebraTool::ScalarMatrix(vertex_norms->shape(1), -1);
		neg_matrix->Set(1, -1, -1);
		if(model->GetProjectNorm(face_num)[2] <= 0)
			vertex_norms *= neg_matrix;

		//flat shading
		if(render_data->shading == "flat") {
			Vector^ world_pos = gcnew Vector(AlgebraTool::Barycenter(world_face), DIM);
			Vector^ norm = AlgebraTool::Normalize(gcnew Vector(AlgebraTool::Barycenter(vertex_norms), DIM) *= -1);
			double color_coef = LightCoef(world_pos, norm, model->mat_data);
			if(!model->Texturized() && !render_data->shadow_test) {
				Color^ color = color_coef * model->GetColor();
				for each(int x in pixels->Keys)
					for each(int y in pixels[x]->Keys)
						this->RenderPixel(x, y, color, render_face, model_num);
				delete color;
			}
			else
				for each(int x in pixels->Keys)
					for each(int y in pixels[x]->Keys) {
						Vector^ render_coord = AlgebraTool::RenderCoord(render_face, x - origin->X, y - origin->Y);
						if(!(render_coord == 0.0)) {
							Vector^ world_coord = AlgebraTool::WorldCoord(render_face, render_coord);
							double shadow_coef = color_coef;
							if(this->Shaded(world_face, world_coord, model_num, model->GetType()))
								shadow_coef = model->mat_data->amb * light_data->amb;
							Color^ color = shadow_coef * model->GetColor(world_coord * tex_face);
							this->RenderPixel(x, y, color, render_face, render_coord, model_num);
							delete world_coord;
						}
						delete render_coord;
					}
			delete norm;
			delete world_pos;
		}
		//gourand shading
		else if(render_data->shading == "gourand") {
			Vector^ color_coefs = gcnew Vector();
			for(unsigned int i = 0; i < world_face->shape(0); i++) {
				Vector^ world_pos = gcnew Vector(world_face->Get(i), DIM);
				Vector^ norm = AlgebraTool::Normalize(gcnew Vector(vertex_norms->Get(i), DIM) *= -1);
				color_coefs->Add(LightCoef(world_pos, norm, model->mat_data));
				delete norm;
				delete world_pos;
			}
			for each(int x in pixels->Keys)
				for each(int y in pixels[x]->Keys) {
					Vector^ render_coord = AlgebraTool::RenderCoord(render_face, x - origin->X, y - origin->Y);
					if(!(render_coord == 0.0)) {
						Vector^ world_coord = AlgebraTool::WorldCoord(render_face, render_coord);
						double color_coef = AlgebraTool::Dot(world_coord, color_coefs);
						if(this->Shaded(world_face, world_coord, model_num, model->GetType()))
							color_coef = model->mat_data->amb * light_data->amb;
						Color^ color = color_coef * model->GetColor(world_coord * tex_face);
						this->RenderPixel(x, y, color, render_face, render_coord, model_num);
						delete world_coord;
					}
					delete render_coord;
				}
			delete color_coefs;
		}
		//fong shading
		else if(render_data->shading == "fong") {
			for each(int x in pixels->Keys)
				for each(int y in pixels[x]->Keys) {
					Vector^ render_coord = AlgebraTool::RenderCoord(render_face, x - origin->X, y - origin->Y);
					if(!(render_coord == 0.0)) {
						Vector^ world_coord = AlgebraTool::WorldCoord(render_face, render_coord);
						Vector^ world_pos = gcnew Vector(world_coord * world_face, DIM);
						Vector^ norm = AlgebraTool::Normalize(gcnew Vector(world_coord * vertex_norms, DIM) *= -1);
						double color_coef = LightCoef(world_pos, norm, model->mat_data);
						if(this->Shaded(world_face, world_coord, model_num, model->GetType()))
							color_coef = model->mat_data->amb * light_data->amb;
						Color^ color =  color_coef * model->GetColor(world_coord * tex_face);
						this->RenderPixel(x, y, color, render_face, render_coord, model_num);
						delete norm;
						delete world_pos;
						delete world_coord;
					}
					delete render_coord;
				}
		}

		//turn faces back
		if(model->GetProjectNorm(face_num)[2] <= 0)
			vertex_norms *= neg_matrix;
		delete neg_matrix;

		for each(int x in pixels->Keys)
			pixels[x]->Clear();
		delete[] pixels;
	}
	//inactive lighting, filling polygons
	else if(render_data->fill) {
		if(!model->Texturized())
			for each(int x in pixels->Keys)
				for each(int y in pixels[x]->Keys)
					this->RenderPixel(x, y, model->GetColor(), render_face, model_num);
		else {
			Matrix ^tex_face = model->GetTexFace(face_num);
			for each(int x in pixels->Keys)
				for each(int y in pixels[x]->Keys) {
					Vector^ render_coord = AlgebraTool::RenderCoord(render_face, x - origin->X, y - origin->Y);
					if(!(render_coord == 0.0)) {
						Vector^ world_coord = AlgebraTool::WorldCoord(render_face, render_coord);
						Color^ color = model->GetColor(world_coord * tex_face);
						this->RenderPixel(x, y, color, render_face, model_num);
						delete world_coord;
					}
					delete render_coord;
				}
		}
	}
	//inactive lighting, drawing lines
	else if(render_data->depth_test) {
		for each(int x in pixels->Keys)
			for each(int y in pixels[x]->Keys)
				this->RenderPixel(x, y, pixels[x][y] ? model->mat_data->color : Color::White, render_face, model_num);
	}
	else
		for each(int x in pixels->Keys)
			for each(int y in pixels[x]->Keys)
				if(pixels[x][y])
					this->RenderPixel(x, y, model->mat_data->color, render_face, model_num);

	delete [] pixels;
}

//render list of models
void RenderTool::Render(List<Model^>^ models) {
	//lock the bitmap's bits
	bitmap_data = render_bitmap->LockBits(Rectangle(0, 0, Width, Height),
										  Imaging::ImageLockMode::ReadWrite,
										  render_bitmap->PixelFormat);
	//clear bitmaps
	for(int x = 0; x < Width; x++)
		for(int y = 0; y < Height; y++)
			for(int i = 0; i < 4; i++)
				z_buffer[4 * (y * Width + x) + i] = (i + 1)%4 == 0 ? 255 : 0;
	for(int i = 0; i < color_values->Length; i++)
		color_values[i] = 255;

	RenderData^ shadow_data = gcnew RenderData(render_data->box, light_data->light_pos, true);
	shadow_tool = gcnew RenderTool(Width, Height, shadow_data, light_data);
	if(render_data->shadow_test)
		shadow_tool->Render(models);

	this->RenderMatricesCalculate();
	for(int i = 0; i < models->Count; i++) {
		models[i]->VerticesProjectCalculate(render_matrix);
		models[i]->BodyMatrixProjectCalculate(body_render_matrix);

		for(unsigned int j = 0; j < models[i]->FacesCount(); j++) 
			if(!render_data->early_depth_test || models[i]->GetProjectNorm(j)[2] > 0)
				this->RenderFace(models[i], i, j);
	}

	//copy the ARGB values back to the bitmap
	if(light_data->move && render_data->shadow_test) {
		if(!render_data->depth_render_mode)
			System::Runtime::InteropServices::Marshal::Copy(shadow_tool->color_values, 0, bitmap_data->Scan0, z_buffer->Length);
		else
			System::Runtime::InteropServices::Marshal::Copy(shadow_tool->z_buffer, 0, bitmap_data->Scan0, z_buffer->Length);
	}
	else {
		if(!render_data->depth_render_mode)
			System::Runtime::InteropServices::Marshal::Copy(color_values, 0, bitmap_data->Scan0, color_values->Length);
		else
			System::Runtime::InteropServices::Marshal::Copy(z_buffer, 0, bitmap_data->Scan0, z_buffer->Length);
	}

	//unlock the bits
	render_bitmap->UnlockBits(bitmap_data);

	delete shadow_data;
	delete shadow_tool;
}
