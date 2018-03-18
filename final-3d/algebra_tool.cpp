#include "algebra_tool.h"

//matrix creation

//scalar matrix
Matrix^ AlgebraTool::ScalarMatrix(int n, double k) {
	Matrix^ I = gcnew Matrix(n, n);
	for(int i = 0; i < n; i++)
		I->Set(k, i, i);
	return I;
}

//translation matrix
Matrix^ AlgebraTool::Translate(Vector^ vector) {
	Matrix^ translate_matrix = AlgebraTool::ScalarMatrix(DIM + 1, 1);
	for(int i = 0; i < vector->Count; i++)
		translate_matrix->Set(vector[i], -1, i);
	return translate_matrix;
}
//rotation matrix
Matrix^ AlgebraTool::Rotate(Vector^ angles) {
	Matrix^ rotate_matrix = AlgebraTool::ScalarMatrix(DIM + 1, 1);
	for(int i = 0; i < angles->Count; i++) {
		int k, l;
		switch(i) {
			case 0: k = 1; l = 2; break;
			case 1: k = 0; l = 2; break;
			case 2: k = 0; l = 1; break;
		}
		double coef = System::Math::Pow(-1, k + l);
		Matrix^ transform = AlgebraTool::ScalarMatrix(DIM + 1, 1);
		transform->Set(System::Math::Cos(angles[i]), k, k);
		transform->Set(coef * System::Math::Sin(angles[i]), k, l);
		transform->Set(coef * System::Math::Sin(-angles[i]), l, k);
		transform->Set(System::Math::Cos(angles[i]), l, l);
		rotate_matrix *= transform;
		delete transform;
	}
	return rotate_matrix;
}
//scale matrix
Matrix^ AlgebraTool::Scale(Vector^ vector) {
	Matrix^ scale_matrix = AlgebraTool::ScalarMatrix(DIM + 1, 1);
	for(int i = 0; i < vector->Count; i++)
		scale_matrix->Set(vector[i], i, i);
	return scale_matrix;
}
//projection matrix: vector = (z_c, near_z, far_z)
Matrix^ AlgebraTool::Project(Vector^ c_n_f) {
	Matrix^ project_matrix = AlgebraTool::ScalarMatrix(DIM + 1, 1);
	double r = c_n_f[0] != 0 ? -1.0 / c_n_f[0] : 0.0;
	double a = (1.0 + r * c_n_f[2]) / (c_n_f[2] - c_n_f[1]);
	double b = -c_n_f[1] * a;
	project_matrix->Set(r, -2, -1);
	project_matrix->Set(a, -2, -2);
	project_matrix->Set(b, -1, -2);
	return project_matrix;
}
//projection matrix: vector = (z_c, near_z, far_z)
Matrix^ AlgebraTool::InvProjectT(Vector^ c_n_f) {
	Matrix^ project_matrix = AlgebraTool::ScalarMatrix(DIM + 1, 1);
	double r = c_n_f[0] != 0 ? -1.0 / c_n_f[0] : 0.0;
	double a = (1.0 + r * c_n_f[2]) / (c_n_f[2] - c_n_f[1]);
	double b = -c_n_f[1] * a;
	double det = a - b * r;
	project_matrix->Set(1 / det, -2, -2);
	project_matrix->Set(-r / det, -1, -2);
	project_matrix->Set(-b / det, -2, -1);
	project_matrix->Set(a / det, -1, -1);
	return project_matrix;
}

Matrix^ AlgebraTool::LookAt(Vector^ camera_pos) {
	Vector^ camera_z_dir = AlgebraTool::Normalize(-camera_pos);
	Vector^ y_dir = gcnew Vector(0.0, 1.0, 0.0);
	Vector^ camera_x_dir = AlgebraTool::Normalize(AlgebraTool::Cross(y_dir, camera_z_dir));
	Vector^ camera_y_dir = AlgebraTool::Cross(camera_z_dir, camera_x_dir);
	delete y_dir;
	Matrix^ look_at_matrix = AlgebraTool::ScalarMatrix(DIM + 1, 1.0);
	look_at_matrix->Set(camera_x_dir[0], 0, 0);
	look_at_matrix->Set(camera_x_dir[1], 1, 0);
	look_at_matrix->Set(camera_x_dir[2], 2, 0);
	look_at_matrix->Set(camera_y_dir[0], 0, 1);
	look_at_matrix->Set(camera_y_dir[1], 1, 1);
	look_at_matrix->Set(camera_y_dir[2], 2, 1);
	look_at_matrix->Set(camera_z_dir[0], 0, 2);
	look_at_matrix->Set(camera_z_dir[1], 1, 2);
	look_at_matrix->Set(camera_z_dir[2], 2, 2);
	delete camera_x_dir;
	delete camera_y_dir;
	delete camera_z_dir;

	return look_at_matrix;
}

//transformation matrix
Matrix^ AlgebraTool::Transform(Vector^ transform_vector, System::String^ transform_type) {
	Matrix^ transform;
	if(transform_type == "translate")
		transform = AlgebraTool::Translate(transform_vector);
	else if(transform_type == "scale")
		transform = AlgebraTool::Scale(transform_vector);
	else if(transform_type == "rotate")
		transform = AlgebraTool::Rotate(transform_vector);
	else if(transform_type == "project")
		transform = AlgebraTool::Project(transform_vector);
	else if(transform_type == "look_at")
		transform = AlgebraTool::LookAt(transform_vector);
	return transform;
}
//inverse transponse matrix
Matrix^ AlgebraTool::InvTransformT(Vector^ transform_vector, System::String^ transform_type) {
	Matrix ^transform;
	Vector^ inv_vector;
	if(transform_type == "translate") {
		inv_vector = -transform_vector;
		transform = AlgebraTool::Translate(inv_vector)->T();
		delete inv_vector;
	}
	else if(transform_type == "scale") {
		inv_vector = 1.0 / transform_vector;
		transform = AlgebraTool::Scale(inv_vector);
		delete inv_vector;
	}
	else if(transform_type == "rotate")
		transform = AlgebraTool::Rotate(transform_vector);
	else if(transform_type == "project")
		transform = AlgebraTool::InvProjectT(transform_vector);
	else if(transform_type == "look_at")
		transform = AlgebraTool::LookAt(transform_vector);
	return transform;
}

//matrix operations

//mean rows calculation
Vector^ AlgebraTool::Barycenter(Matrix^ const M) {
	Vector^ div_n = gcnew Vector();
	for(unsigned int i = 0; i < M->shape(0); i++)
		div_n->Add(1.0/M->shape(0));
	return div_n *= M;
}

//matrix-vector operations

//left-Dot multiplication: v * M
Vector^ operator * (List<double>^ u, Matrix^ const M) {
	List<double>^ list = M->left_dot(u);
	Vector^ product = gcnew Vector(list);
	delete list;
	return product;
}

//scalar product of vector & matrix column
double AlgebraTool::Dot(List<double>^ u, Matrix^ const M, int column) {
	double Dot_product = 0;
	for(int i = 0; i < u->Count; i++)
		Dot_product += u[i] * M->Get(i, column);
	return Dot_product;
}

//vector operations

//Dot product
double AlgebraTool::Dot(List<double>^ u, List<double>^ v) {
	double Dot_product = 0;
	for(int i = 0; i < u->Count; i++)
		Dot_product += u[i] * v[i];
	return Dot_product;
}
//vector product
Vector^ AlgebraTool::Cross(List<double>^ u, List<double>^ v) {
	return gcnew Vector(u[1] * v[2] - u[2] * v[1],
						u[2] * v[0] - u[0] * v[2],
						u[0] * v[1] - u[1] * v[0]);
}

//scalar product of ca & cb
double AlgebraTool::Dot(List<double>^ c, List<double>^ a, List<double>^ b) {
	Vector^ ac = (gcnew Vector(c) -= a);
	Vector^ bc = (gcnew Vector(c) -= b);
	double result = Dot(ac, bc);
	delete ac;
	delete bc;
	return result;
}
//vector product of ca & cb
Vector^ AlgebraTool::Cross(List<double>^ c, List<double>^ a, List<double>^ b) {
	Vector^ ac = (gcnew Vector(c) -= a);
	Vector^ bc = (gcnew Vector(c) -= b);
	Vector^ result = Cross(ac, bc);
	delete ac;
	delete bc;
	return result;
}

//vector Length
double AlgebraTool::Length(List<double>^ vector) {
	double Dot_product = AlgebraTool::Dot(vector, vector);
	Dot_product -= vector->Count > DIM && vector[DIM] != 0 ? 1 : 0;
	return System::Math::Sqrt(Dot_product);
}
//vector normalization
Vector^ AlgebraTool::Normalize(Vector^ vector) {
	vector /= AlgebraTool::Length(vector);
	return vector;
}

//barycentric coordinates in project space
Vector^ AlgebraTool::RenderCoord(Matrix^ render_face, int x, int y) {
	Vector^ coord = gcnew Vector();
	for(unsigned int i = 0; i < render_face->shape(0); i++)
		coord->Add(0);
	double check_min = System::Double::NegativeInfinity;
	for(unsigned int i = 1; i < render_face->shape(0) - 1; i++) {
		List<double>^ a = render_face->Get(0);
		List<double>^ b = render_face->Get(i);
		List<double>^ c = render_face->Get(i + 1);
		Vector^ v_x = gcnew Vector(a[0] - x, b[0] - a[0], c[0] - a[0]);
		Vector^ v_y = gcnew Vector(a[1] - y, b[1] - a[1], c[1] - a[1]);
		Vector^ e_uv = AlgebraTool::Cross(v_x, v_y);
		e_uv /= e_uv[0];
		double min = System::Math::Min(System::Math::Min(e_uv[0] - e_uv[1] - e_uv[2], e_uv[1]), e_uv[2]);
		if(check_min < min) {
			for(unsigned int j = 1; j < render_face->shape(0); j++)
				coord[j] = 0;
			coord[0] = e_uv[0] - e_uv[1] - e_uv[2];
			coord[i] = e_uv[1];
			coord[i + 1] = e_uv[2];
			check_min = min;
		}
		delete v_x; delete v_y; delete e_uv;
	}
	return coord;
}

//barycentric coordinates in world space:
//correction of interpolation distortions
Vector^ AlgebraTool::WorldCoord(Matrix^ render_face, Vector^ RenderCoord) {
	Vector^ coord = gcnew Vector(RenderCoord);
	double sum_coord = 0;
	for(unsigned int i = 0; i < render_face->shape(0); i++) {
		coord[i] /= render_face->Get(i, -1);
		sum_coord += coord[i];
	}
	return coord /= sum_coord;
}