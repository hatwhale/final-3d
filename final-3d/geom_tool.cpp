#include "geom_tool.h"

void GeomTool::AddPixel(Dictionary<int, SortedDictionary<int, bool>^>^ pixels, int x, int y, bool boundary) {
	if(!pixels->ContainsKey(x))
		pixels->Add(x, gcnew SortedDictionary<int, bool>());
	if(!pixels[x]->ContainsKey(y))
		pixels[x]->Add(y, boundary);
}

void GeomTool::DrawLine(Dictionary<int, SortedDictionary<int, bool>^>^ pixels, Point^  p_0, Point^  p_1, RenderData^  params) {
	int x_0 = p_0->X, y_0 = p_0->Y,
		x_1 = p_1->X, y_1 = p_1->Y;
	if(x_0 > x_1 || (x_0 == x_1 && y_0 > y_1)) {
		x_0 = p_1->X; y_0 = p_1->Y;
		x_1 = p_0->X; y_1 = p_0->Y;
	}
	/*
	let's make the local coordinate center equal to (x_0, y_0),
	so we have to draw ((0, 0), (x_1 - x_0, y_1 - y_0))

	octants in local coordinate system (center = (x_0, y_0)):
		\3|2/
		4\|/1
		---+---
		5/|\8
		/6|7\

	sign(x_1 - x_0) and sign(y_1 - y_0) show where the next pixel is
	*/
	int sign_delta_x = System::Math::Sign(x_1 - x_0),
		sign_delta_y = System::Math::Sign(y_1 - y_0);
	int delta_x = System::Math::Abs(x_1 - x_0),
		delta_y = System::Math::Abs(y_1 - y_0);
	/*
	octants:
		\3|2/
		4\|/1
		---+---
		5/|\8
		/6|7\

	gradient stuff:
		k = abs(y_1 - y_0)/abs(x_1 - x_0)
			k < 1, i. e. abs(y_1 - y_0) < abs(x_1 - x_0)
		for 1, 4, 5, 8 octants, and we move along x (and constantly change)
			k > 1, i. e. abs(y_1 - y_0) > abs(x_1 - x_0)
		for 2, 3, 6, 7 octants, and we move along y (and constantly change)
	*/
	bool slope = delta_y <= delta_x;
	int delta = slope ? delta_x : delta_y;
	/*
	for k = delta_y/delta_x < 1:
		e += k each step and if e >= 0, e--
		error = 2 * e * delta_x, so
			error += 2 * delta_y each step
			if error >= 0, error -= 2 * delta_x
	for k = delta_y/delta_x > 1,
	move along y, so k -> 1/k (swap delta_x and delta_y)
		e += 1/k each step and if e >= 0, e--
		error = 2 * e * delta_y, so
			error += 2 * delta_x each step
			if error >= 0, error -= 2 * delta_y
	*/
	int x = x_0, y = y_0;
	int error = slope ? 2 * delta_y - delta_x : 2 * delta_x - delta_y;
	for (int step = 1; step <= delta; step++) {
		//add pixel and "clipping"
		int clip_x = x >= params->box->Left ? x < params->box->Right ? x : params->box->Right : params->box->Left - 1;
		int clip_y = y >= params->box->Top ? y < params->box->Bottom ? y : params->box->Bottom : params->box->Top - 1;
		bool inside_box = x >= params->box->Left && x < params->box->Right && y >= params->box->Top && y < params->box->Bottom;
		this->AddPixel(pixels, clip_x, clip_y, inside_box);

		int delta_error = 0;
		if(error > 0) {
			if(slope) { //move along x, but change y if error > 0
				y += sign_delta_y;
				delta_error += -2 * delta_x;
			} else { //move along y, but change x if error > 0
				x += sign_delta_x;
				delta_error += -2 * delta_y;
			}
		}
		if(slope) { //move along x and change it in step
			x += sign_delta_x;
			delta_error += 2 * delta_y;
		} else { //move along y and change it in step
			y += sign_delta_y;
			delta_error += 2 * delta_x;
		}
		error += delta_error;
	}
}

Dictionary<int, SortedDictionary<int, bool>^>^ GeomTool::DrawPolygon(array<Point>^  points, RenderData^  params) {
	//generate polygon's pixels
	Dictionary<int, SortedDictionary<int, bool>^> ^pixels = gcnew Dictionary<int, SortedDictionary<int, bool>^>();
	for(int i = 0; i < points->Length; i++)
		this->DrawLine(pixels, points[i], points[(i + 1) % points->Length], params);
	//filling y-lines with "clipping"
	if(params->fill || params->depth_test)
		for each(int x in pixels->Keys) {
			int first_y = Enumerable::First(pixels[x]).Key;
			int last_y = Enumerable::Last(pixels[x]).Key;
			for(int i = first_y; i <= last_y; i++) {
				if(!pixels[x]->ContainsKey(i))
					pixels[x]->Add(i, false);
				else if(!pixels[x][i])
					pixels[x]->Remove(i);
			}
		}
	return pixels;
}