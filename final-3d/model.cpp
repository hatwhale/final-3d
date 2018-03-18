#include "model.h"

void Model::ModelInitialize(System::String ^type) {
	this->type = type;
	vertices = gcnew Matrix(DIM + 1);
	project_vertices = gcnew Matrix(DIM + 1);
	center = gcnew Vector(0, 0, 0, 1);
	body_matrix = gcnew Matrix(DIM + 1);
	vertex_norms = gcnew Matrix(DIM + 1);
	project_body_matrix = gcnew Matrix(DIM + 1);
	faces = gcnew List<List<unsigned int>^>();

	tex_vertices = gcnew Matrix(2);
	tex_faces = gcnew List<List<unsigned int>^>();
	tex_data = gcnew TextureData();
}
//body matrix calculation
void Model::BodyMatrixCalculate() {
	center->Clear();
	center = AlgebraTool::Barycenter(vertices);
	for(unsigned int i = 0; i < this->FacesCount(); i++) {
		Vector^ cross_product;
		cross_product = AlgebraTool::Cross(this->Get(i, 0), this->Get(i, 1), this->Get(i, 2));
		cross_product->Add(-AlgebraTool::Dot(cross_product, this->Get(i, 0)));
		if(AlgebraTool::Dot(cross_product, this->center) < 0)
			cross_product *= -1;
		this->body_matrix->Add(cross_product);
	}
}
void Model::BodyMatrixCalculate(System::String^ type) {
	center->Clear();
	center = AlgebraTool::Barycenter(vertices);
	if(this->body_matrix->shape(0) == 0)
		for(unsigned int i = 0; i < this->FacesCount(); i++) {
			Vector^ cross_product;
			cross_product = AlgebraTool::Cross(this->Get(i, 0), this->Get(i, 1), this->Get(i, 2));
			cross_product->Add(-AlgebraTool::Dot(cross_product, this->Get(i, 0)));
			this->body_matrix->Add(cross_product);
		}
	else {
		for(unsigned int i = 0; i < this->FacesCount(); i++) {
			Vector^ cross_product;
			cross_product = AlgebraTool::Cross(this->Get(i, 0), this->Get(i, 1), this->Get(i, 2));
			cross_product->Add(-AlgebraTool::Dot(cross_product, this->Get(i, 0)));
			if(AlgebraTool::Dot(this->GetFaceNorm(i), this->center) < 0)
				cross_product *= -1;
			this->body_matrix->Set(cross_product, i);
			delete cross_product;
		}
	}
}
//vertex normals calculation
void Model::VertexNormsCalculate() {
	List<Vector^>^ norm_vectors = gcnew List<Vector^>();
	for(unsigned int i = 0; i < this->VerticesCount(); i++)
		norm_vectors->Add(gcnew Vector(0, 0, 0, 0));
	for(unsigned int i = 0; i < this->FacesCount(); i++)
		for each(unsigned int vertex in faces[i])
			norm_vectors[vertex] += this->GetFaceNorm(i);
	for(unsigned int i = 0; i < this->VerticesCount(); i++)
		vertex_norms->Add(norm_vectors[i]);
	delete norm_vectors;
}
//load model from the file
void Model::LoadModelConstruct(System::String^  path) {
	this->ModelInitialize(path->Substring(path->LastIndexOf('\\') + 1,
											path->LastIndexOf('.') - path->LastIndexOf('\\') - 1));
	this->mat_data = gcnew MaterialData();

	StreamReader ^stream_reader = gcnew StreamReader(path);
	try {
		array<System::Char> ^separator = gcnew array<System::Char>{' '};
		array<System::String^> ^object_data;
		System::String ^object_string = "", ^type = "";
		while (object_string = stream_reader->ReadLine()) {
			object_data = object_string->Split(separator, System::StringSplitOptions::RemoveEmptyEntries);
			if(type == "texture") {
				if(!System::IO::File::Exists(object_string)) {
					System::String^ exe_path = System::Windows::Forms::Application::ExecutablePath;
					exe_path = exe_path->Substring(0, exe_path->LastIndexOf('\\') + 1);
					object_string = exe_path + object_string;
					delete exe_path;
				}
				this->ApplyTexture(object_string);
				type = "pass";
			}
			else if(object_data->Length == 1)
				type = object_data[0];
			else if (type == "vertices") {
				List<double>^ vertex = gcnew List<double>();
				for each (System::String^ str in object_data)
					vertex->Add(System::Convert::ToDouble(str));
				vertex->Add(1);
				vertices->Add(vertex);
			}
			else if (type == "faces") {
				List<unsigned int>^ face_vertices = gcnew List<unsigned int>();
				for each (System::String^ str in object_data)
					face_vertices->Add(System::Convert::ToUInt32(str) - 1);
				faces->Add(face_vertices);
			}
			else if (type == "tex_vertices") {
				List<double>^ vertex = gcnew List<double>();
				for each (System::String^ str in object_data)
					vertex->Add(System::Convert::ToDouble(str));
				tex_vertices->Add(vertex);
			}
			else if (type == "tex_faces") {
				List<unsigned int>^ face_vertices = gcnew List<unsigned int>();
				for each (System::String^ str in object_data)
					face_vertices->Add(System::Convert::ToUInt32(str) - 1);
				tex_faces->Add(face_vertices);
			}
			else if (type == "color") {
				List<unsigned int>^ color_values = gcnew List<unsigned int>();
				for each (System::String^ str in object_data)
					color_values->Add(System::Convert::ToUInt32(str));
				if(object_data->Length > 0)
					delete mat_data;
				switch(object_data->Length) {
				case 1: this->mat_data = gcnew MaterialData(Color::FromArgb(color_values[0])); break;
				case 3: this->mat_data = gcnew MaterialData(Color::FromArgb(color_values[0],
																			color_values[1],
																			color_values[2])); break;
				case 4: this->mat_data = gcnew MaterialData(Color::FromArgb(color_values[0],
																			color_values[1],
																			color_values[2],
																			color_values[3])); break;
				}
				delete color_values;
			}
		}
		delete separator;
	}
	finally {
		delete stream_reader;
	}

	this->BodyMatrixCalculate("");
	this->VertexNormsCalculate();
}

//platonic solids
/*
cube
tetrahedron
octahedron
icosahedron
dodecahedron
*/
void Model::PlatonicSolidConstruct(System::String^ type, double radius) {
	this->ModelInitialize(type);

	if(type == "cube") {
		for(int i = -1; i <= 1; i += 2)
			for(int j = -1; j <= 1; j += 2)
				for(int k = -1; k <= 1; k += 2) 
					vertices->Add(gcnew Vector(i, j, k, 1));
	}
	else if(type == "tetrahedron") {
		for(int i = -1; i <= 1; i += 2)
			for(int j = -1; j <= 1; j += 2)
				for(int k = -1; k <= 1; k += 2)
					if(i + j + k == 1 || i + j + k == -3)
						vertices->Add(gcnew Vector(i, j, k, 1));
	}
	else if(type == "octahedron") {
		for(int i = -1; i <= 1; i += 2)
			for(int j = 0; j < 3; j++) {
				List<double>^ vertex = gcnew Vector(0, 0, 0, 1);
				vertex[j] = i;
				vertices->Add(vertex);
			}
	}
	else if(type == "icosahedron") {
		double phi = (1 + System::Math::Sqrt(5))/2;
		for(int i = -1; i <= 1; i += 2)
			for(int j = -1; j <= 1; j += 2)
				for(int k = 0; k < 3; k++) 
					switch(k) {
						case 0: vertices->Add(gcnew Vector(0, i, j * phi, 1)); break;
						case 1: vertices->Add(gcnew Vector(i, j * phi, 0, 1)); break;
						case 2: vertices->Add(gcnew Vector(j * phi, 0, i, 1)); break;
					}
	}
	else if(type == "dodecahedron") {
		double phi_1 = (1 + System::Math::Sqrt(5))/2;
		double phi_2 = (-1 + System::Math::Sqrt(5))/2;
		for(int i = -1; i <= 1; i += 2)
			for(int j = -1; j <= 1; j += 2)
				for(int k = 0; k <= 4; k++)
					switch(k) {
						case 0: vertices->Add(gcnew Vector(0, i * phi_2, j * phi_1, 1)); break;
						case 1: vertices->Add(gcnew Vector(i * phi_2, j * phi_1, 0, 1)); break;
						case 2: vertices->Add(gcnew Vector(j * phi_1, 0, i * phi_2, 1)); break;
						case 3: vertices->Add(gcnew Vector(i, j, -1, 1)); break;
						case 4: vertices->Add(gcnew Vector(i, j, 1, 1)); break;
					}
	}

	double min_length = AlgebraTool::Dot(vertices->Get(0), vertices->Get(1), vertices->Get(1));
	for(unsigned int i = 2; i < vertices->shape(0); i++) {
		double length = AlgebraTool::Dot(vertices->Get(0), vertices->Get(i), vertices->Get(i));
		min_length = min_length < length ? min_length : length;
	}

	for(unsigned int vertex = 0; vertex < vertices->shape(0); vertex++) {
		List<unsigned int>^ neighbors = gcnew List<unsigned int>();
		for(unsigned int i = 0; i < vertices->shape(0); i++)
			if(System::Math::Abs(AlgebraTool::Dot(vertices->Get(vertex), vertices->Get(i), vertices->Get(i)) - min_length) < EPS)
				neighbors->Add(i);
		double near_length = AlgebraTool::Dot(vertices->Get(neighbors[0]), vertices->Get(neighbors[1]), vertices->Get(neighbors[1]));
		for each(unsigned int i in neighbors) 
			for each(unsigned int j in neighbors) {
				double length = AlgebraTool::Dot(vertices->Get(i), vertices->Get(j), vertices->Get(j));
				near_length = near_length < length || i == j ? near_length : length;
			}
		for each(unsigned int i in neighbors) 
			for each(unsigned int j in neighbors)
				if(System::Math::Abs(AlgebraTool::Dot(vertices->Get(i), vertices->Get(j), vertices->Get(j)) - near_length) < EPS) {
					List<unsigned int>^ face_vertices = gcnew List<unsigned int>();
					Vector^ cross_product = AlgebraTool::Cross(vertices->Get(vertex), vertices->Get(i), vertices->Get(j));
					cross_product->Add(-AlgebraTool::Dot(cross_product, vertices->Get(vertex)));
					if(AlgebraTool::Dot(cross_product, this->center) >= 0) {
						face_vertices->Add(i);
						face_vertices->Add(vertex);
						face_vertices->Add(j);
					} else {
						face_vertices->Add(j);
						face_vertices->Add(vertex);
						face_vertices->Add(i);
						cross_product *= -1;
					}
					int find_index = body_matrix->Find(cross_product, EPS);
					if(find_index == -1) {
						faces->Add(face_vertices);
						this->body_matrix->Add(cross_product);
					} else {
						if(faces[find_index]->Contains(face_vertices[0]))
							for each(unsigned int vertex in face_vertices)
								if(!faces[find_index]->Contains(vertex))
									faces[find_index]->Add(vertex);
						else if(faces[find_index]->Contains(face_vertices[2])) {
							for each(unsigned int vertex in faces[find_index])
								if(!face_vertices->Contains(vertex))
									face_vertices->Add(vertex);
							faces[find_index] = face_vertices;
						}
					}
				}
	}
	this->VertexNormsCalculate();

	Vector^ radius_vector = (gcnew Vector(this->center) -= vertices->Get(0));
	this->scale(radius/AlgebraTool::Length(radius_vector));
	delete radius_vector;

	if(type == "cube")
		for(int i = 0; i <= 1; i++)
			for(int j = 0; j <= 1; j++)
				tex_vertices->Add(gcnew Vector(i, i == 0 ? j : 1 - j));
	else {
		Vector^ shift = gcnew Vector(1, 1);
		double n = type == "dodecahedron" ? 5 : 3;
		for(int i = 0; i < n; i++) {
			double angle = 2 * System::Math::PI / n;
			tex_vertices->Add((gcnew Vector(System::Math::Sin(i * angle), System::Math::Cos(i * angle)) += shift) /= 2);
		}
		delete shift;
	}
	for(int i = 0; i < faces->Count; i++) {
		List<unsigned int>^ face_vertices = gcnew List<unsigned int>();
		for(int j = 0; j < faces[i]->Count; j++)
			face_vertices->Add(j);
		tex_faces->Add(face_vertices);
	}
}

//cylinder creation for solids of revolution
void Model::CylinderConstruct(double N, double M, System::String^  type) {
	bool bond = type != "spiral" && type != "spring" && type != "conical_spring";
	double tex_M = (bond ? M + 1 : M);
	for(double i = -N; i <= N; i += 2) {
		for(double j = 0; j < M; j++) {
			double phi = 2 * System::Math::PI * j / (bond ? M : M - 1);
			vertices->Add(gcnew Vector(System::Math::Cos(phi), System::Math::Sin(phi), i/N, 1));
			tex_vertices->Add(gcnew Vector(j / (bond ? M : M - 1), (i + N) / (2 * N)));
			List<unsigned int>^ face_vertices;
			if(i < N && j < (bond ? M : M - 1)) {
				face_vertices = gcnew List<unsigned int>();
				face_vertices->Add(unsigned int(M*(i + N)/2 + j));
				face_vertices->Add(unsigned int(M*(i + N)/2 + M + j));
				face_vertices->Add(unsigned int(M*(i + N)/2 + (j + 1 == M ? 0 : j + 1)));
				faces->Add(face_vertices);
				face_vertices = gcnew List<unsigned int>();
				face_vertices->Add(unsigned int((M*(i + N)/2 + M + (j + 1 == M ? 0 : j + 1))));
				face_vertices->Add(unsigned int((M*(i + N)/2 + M + j)));
				face_vertices->Add(unsigned int((M*(i + N)/2 + (j + 1 == M ? 0 : j + 1))));
				faces->Add(face_vertices);

				face_vertices = gcnew List<unsigned int>();
				face_vertices->Add(unsigned int(tex_M*(i + N)/2 + j));
				face_vertices->Add(unsigned int(tex_M*(i + N)/2 + tex_M + j));
				face_vertices->Add(unsigned int(tex_M*(i + N)/2 + j + 1));
				tex_faces->Add(face_vertices);
				face_vertices = gcnew List<unsigned int>();
				face_vertices->Add(unsigned int((tex_M*(i + N)/2 + tex_M + j + 1)));
				face_vertices->Add(unsigned int((tex_M*(i + N)/2 + tex_M + j)));
				face_vertices->Add(unsigned int((tex_M*(i + N)/2 + j + 1)));
				tex_faces->Add(face_vertices);
			}
		}
		if(bond)
			tex_vertices->Add(gcnew Vector(1, (i + N) / (2 * N)));
	}
	this->BodyMatrixCalculate(type);
}

//sphere and its derivatives
/*
kinder
surprise
pear
almost_thorus
moon
drop
fruit
garlic
*/
void Model::SphereDConstruct(System::String^ type, double radius, double N) {
	this->ModelInitialize(type);
	this->CylinderConstruct(N, 2 * N, type);

	for(unsigned int i = 0; i < vertices->shape(0); i++) {
		double theta = vertices->Get(i, 2) * System::Math::PI / 2;
		if(type != "cylinder") { //unit sphere
			double r = System::Math::Cos(theta);
			vertices->Set(r * vertices->Get(i, 0), i, 0);
			vertices->Set(r * vertices->Get(i, 1), i, 1);
			vertices->Set(System::Math::Sin(theta), i, 2);
		}

		double x_c = vertices->Get(i, 0),
				y_c = vertices->Get(i, 1),
				z_c = vertices->Get(i, 2);

		//vertices->Set(z_c + 4 * phi / (2 * System::Math::PI), i, 2);
		if(type == "kinder") { //kinder-surprise
			if(z_c > 0)
				vertices->Set(1 + z_c, i, 2);
		} else if(type == "surprise") {
			if(z_c > 0)
				vertices->Set(1 - 0.5 * z_c, i, 2);
		} else if(type == "pear") { //pear
			if(z_c > 0.5)
				vertices->Set(z_c + 2.5 * System::Math::Pow(z_c - 0.5, 2), i, 2);
		} else if(type == "almost_thorus") { //almost thorus
			vertices->Set(z_c - System::Math::Pow(2 * theta / System::Math::PI, 3), i, 2);
		} else if(type == "moon") { //moon
			vertices->Set(0.5 * (x_c + System::Math::Pow(4 * theta/System::Math::PI, 2)), i, 0);
			vertices->Set(0.5 * y_c, i, 1);
		} else if(type == "drop") { //drop
			if(z_c > 0)
				vertices->Set(z_c + System::Math::Pow(2 * theta / System::Math::PI, 4), i, 2);
		} else if(type == "fruit" || type == "garlic") { //fruit or garlic
			double coef = 1 + System::Math::Abs(2 * x_c * y_c)/2;
			vertices->Set(coef * x_c, i, 0);
			vertices->Set(coef * y_c, i, 1);
			if(type == "garlic" && z_c > 0)
				vertices->Set(z_c + System::Math::Pow(2 * theta / System::Math::PI, 5), i, 2);
		}
	}
	if(type != "cylinder")
		for(unsigned int i = 0; i < this->FacesCount(); i++)
			for(int j = 0; j < faces[i]->Count; j++) {
				if(faces[i][j] < 2 * N)
					faces[i][j] = 0;
				else if(faces[i][j] > 2 * N * N)
					faces[i][j] = type != "almost_thorus" ? unsigned int(2 * N * N) : 0;
			}
	this->BodyMatrixCalculate(type);
	this->VertexNormsCalculate();

	this->scale(radius);
}

//thorus and its derivatives
/*
thorus
spiral
spring
conical_spring
*/
void Model::ThorusDConstruct(System::String^ type, double radius, double coef, double N) {
	this->ModelInitialize(type);
	double spiral_k = type == "thorus" ? 1 : 2;
	this->CylinderConstruct(N, int(spiral_k * N / coef - 0.5), type);

	for(unsigned int i = 0; i < vertices->shape(0); i++) {
		double theta = vertices->Get(i, 2) * System::Math::PI / 2;
		double phi = System::Math::Acos(vertices->Get(i, 0));
		if(vertices->Get(i, 1) < 0)
			phi = 2 * System::Math::PI - phi;
		double r = 1 + coef * System::Math::Cos(2 * theta); 
		if(type == "spiral" || type == "conical_spring")
			r += spiral_k * phi / (2 * System::Math::PI);
		vertices->Set(r * System::Math::Cos(spiral_k * phi), i, 0);
		vertices->Set(r * System::Math::Sin(spiral_k * phi), i, 1);
		double spring_coef = 0;
		if(type == "spring" || type == "conical_spring")
			spring_coef = spiral_k * phi / (2 * System::Math::PI) - 0.5 * spiral_k;
		vertices->Set(spring_coef + coef * System::Math::Sin(2 * theta), i, 2);
	}
	for(unsigned int i = 0; i < this->FacesCount(); i++)
		for(int j = 0; j < faces[i]->Count; j++)
			faces[i][j] %= unsigned int(N * int(spiral_k * N / coef - 0.5));
	this->BodyMatrixCalculate(type);
	this->VertexNormsCalculate();

	this->scale(radius * (type != "spring" ? (1 + coef) / (spiral_k + coef) : 1));
}


Model::Model(System::String^  path) {
	this->LoadModelConstruct(path);
}
Model::Model(System::String^  path, Color color) {
	this->LoadModelConstruct(path);
	delete this->mat_data;
	this->mat_data = gcnew MaterialData(color);
}
Model::Model(System::String^ type, double radius) {
	this->PlatonicSolidConstruct(type, radius);
	Color default_color;
	if(type == "cube")
		default_color = Color::Green;
	else if(type == "tetrahedron")
		default_color = Color::Red;
	else if(type == "octahedron")
		default_color = Color::LightGray;
	else if(type == "icosahedron")
		default_color = Color::Blue;
	else if(type == "dodecahedron")
		default_color = Color::Purple;
	this->mat_data = gcnew MaterialData(default_color);
}
Model::Model(System::String^ type, double radius, Color color) {
	this->PlatonicSolidConstruct(type, radius);
	this->mat_data = gcnew MaterialData(color);
}
Model::Model(System::String^ type, double radius, double N) {
	this->SphereDConstruct(type, radius, N);
	Color default_color;
	if(type == "cylinder")
		default_color = Color::Indigo;
	else if(type == "sphere")
		default_color = Color::ForestGreen;
	else if(type == "kinder")
		default_color = Color::Gold;
	else if(type == "surprise")
		default_color = Color::Goldenrod;
	else if(type == "pear")
		default_color = Color::DarkOrange;
	else if(type == "almost_thorus")
		default_color = Color::LightBlue;
	else if(type == "moon")
		default_color = Color::DarkGoldenrod;
	else if(type == "drop")
		default_color = Color::Blue;
	else if(type == "fruit")
		default_color = Color::Crimson;
	else if(type == "garlic")
		default_color = Color::Silver;
	this->mat_data = gcnew MaterialData(default_color);
}
Model::Model(System::String^ type, double radius, double N, Color color) {
	this->SphereDConstruct(type, radius, N);
	this->mat_data = gcnew MaterialData(color);
}
Model::Model(System::String^ type, double radius, double coef, double N) {
	this->ThorusDConstruct(type, radius, coef, N);
	Color default_color;
	
	if(type == "thorus")
		default_color = Color::CornflowerBlue;
	else if(type == "spiral")
		default_color = Color::BlueViolet;
	else if(type == "spring")
		default_color = Color::Red;
	else if(type == "conical_spring")
		default_color = Color::Firebrick;
	this->mat_data = gcnew MaterialData(default_color);
}
Model::Model(System::String^ type, double radius, double coef, double N, Color color) {
	this->ThorusDConstruct(type, radius, coef, N);
	this->mat_data = gcnew MaterialData(color);
}

Model::~Model() {
	delete vertices;
	delete faces;
	delete center;
	delete body_matrix;
	delete vertex_norms;
	delete project_vertices;
	delete project_body_matrix;

	delete tex_vertices;
	delete tex_faces;
	delete tex_data;
}

//load and apply a texture from the file
void Model::ApplyTexture(System::String^  tex_path) {
	delete tex_data;
	tex_data = gcnew TextureData(tex_path); 
}

//color from texture
Color^ Model::GetColor(List<double>^ pixel) {
	if(this->Texturized()) {
		int x = int(pixel[0] * (tex_data->Width - 1));
		int y = (tex_data->Height - 1) - int(pixel[1] * (tex_data->Height - 1));

		x = x > 0 ? x < tex_data->Width - 1 ? x : tex_data->Width - 1 : 0;
		y = y > 0 ? y < tex_data->Height - 1 ? y : tex_data->Height - 1 : 0;
		int A = tex_data->color_values[4 * (y * tex_data->Width + x) + 3];
		int R = tex_data->color_values[4 * (y * tex_data->Width + x) + 2];
		int G = tex_data->color_values[4 * (y * tex_data->Width + x) + 1];
		int B = tex_data->color_values[4 * (y * tex_data->Width + x)];
		return Color::FromArgb(A, R, G, B);
	}
	else
		return GetColor();
}

//get face matrix (with row-vertices)
Matrix^ Model::Get(int face_num) {
	Matrix^ face_matrix = gcnew Matrix(DIM + 1);
	for each(unsigned int vertex_num in faces[face_num])
		face_matrix->Add(vertices->Get(vertex_num));
	return face_matrix;
}
//get face matrix from texture's faces
Matrix^ Model::GetTexFace(int face_num) {
	Matrix^ face_matrix = gcnew Matrix(DIM - 1);
	if(this->Texturized()) {
		for each(unsigned int vertex_num in tex_faces[face_num])
			face_matrix->Add(tex_vertices->Get(vertex_num));
	}
	return face_matrix;
}
//get vertex normals' matrix of face
Matrix^ Model::GetVertexNorms(int face_num) {
	Matrix^ normal_matrix = gcnew Matrix(DIM + 1);
	for each(unsigned int vertex_num in faces[face_num])
		normal_matrix->Add(this->GetVertexNorm(vertex_num));
	return normal_matrix;
}
//get project face matrix (with row-vertices)
Matrix^ Model::GetProject(int face_num){
	Matrix^ face_matrix = gcnew Matrix(DIM + 1);
	for each(unsigned int vertex_num in faces[face_num])
		face_matrix->Add(project_vertices->Get(vertex_num));
	return face_matrix;
}

//save model to the file
void Model::ToFile(System::String ^path) {
	StreamWriter^ stream_writer = gcnew StreamWriter(path, false);
	stream_writer->Write("vertices");
	stream_writer->WriteLine();
	vertices->ToFile(stream_writer, DIM);
	stream_writer->Write("faces");
	stream_writer->WriteLine();
	List<bool>^ dir_check = gcnew List<bool>();
	for(unsigned int i = 0; i < this->FacesCount(); i++) {
		Vector^ cross_product;
		cross_product = AlgebraTool::Cross(this->Get(i, 0), this->Get(i, 1), this->Get(i, 2));
		cross_product->Add(-AlgebraTool::Dot(cross_product, this->Get(i, 0)));
		dir_check->Add((AlgebraTool::Dot(cross_product, this->center) < 0) == (AlgebraTool::Dot(this->GetFaceNorm(i), this->center) < 0));
		delete cross_product;
	}
	for(unsigned int i = 0; i < this->FacesCount(); i++) {
		if(!dir_check[i])
			for(int j = faces[i]->Count - 1; j >= 0; j--)
				stream_writer->Write((faces[i][j] + 1).ToString() + " ");
		else
			for(int j = 0; j < faces[i]->Count; j++)
				stream_writer->Write((faces[i][j] + 1).ToString() + " ");
		stream_writer->WriteLine();
	}
	if(this->Texturized()) {
		stream_writer->Write("tex_vertices");
		stream_writer->WriteLine();
		tex_vertices->ToFile(stream_writer, DIM - 1);
		stream_writer->Write("tex_faces");
		stream_writer->WriteLine();
		for(unsigned int i = 0; i < this->FacesCount(); i++) {
			if(!dir_check[i])
				for(int j = faces[i]->Count - 1; j >= 0; j--)
					stream_writer->Write((tex_faces[i][j] + 1).ToString() + " ");
			else
				for(int j = 0; j < faces[i]->Count; j++)
					stream_writer->Write((tex_faces[i][j] + 1).ToString() + " ");
			stream_writer->WriteLine();
		}
		stream_writer->Write("texture");
		stream_writer->WriteLine();
		stream_writer->Write(tex_data->path);
		stream_writer->WriteLine();
	}
	delete dir_check;
	stream_writer->Write("color");
	stream_writer->WriteLine();
	stream_writer->Write((this->GetColor()->A).ToString() + " ");
	stream_writer->Write((this->GetColor()->R).ToString() + " ");
	stream_writer->Write((this->GetColor()->G).ToString() + " ");
	stream_writer->Write((this->GetColor()->B).ToString() + " ");
	stream_writer->Close();
}

//verticies & body_matrix transformations
void Model::Transform(Vector^ transform_vector, System::String^ transform_type) {
	Matrix ^transform = AlgebraTool::Transform(transform_vector, transform_type);
	Matrix ^body_transform = AlgebraTool::InvTransformT(transform_vector, transform_type);
	center *= transform;
	vertices *= transform;
	body_matrix *= body_transform;
	vertex_norms *= body_transform;
	delete transform;
	delete body_transform;
}
void Model::scale(double radius) {
	Vector^ scale_vector = gcnew Vector(radius, radius, radius);
	this->Transform(scale_vector, "scale");
	delete scale_vector;
}

//projection of verticies calculate
Matrix^ Model::VerticesProjectCalculate(Matrix^ render_matrix) {
	delete project_vertices;
	project_vertices = gcnew Matrix(this->vertices);
	project_vertices *= render_matrix;
	for(unsigned int i = 0; i < project_vertices->shape(0); i++)
		for(unsigned int j = 0; j < project_vertices->shape(1) - 1; j++)
			project_vertices->Set(project_vertices->Get(i, j)/project_vertices->Get(i, -1), i, j);
	return project_vertices;
}
//"projection" of normals calculate
Matrix^ Model::BodyMatrixProjectCalculate(Matrix^ body_render_matrix) {
	delete project_body_matrix;
	project_body_matrix = gcnew Matrix(this->body_matrix);
	project_body_matrix *= body_render_matrix;
	return project_body_matrix;
}