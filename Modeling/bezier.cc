#include "bezier.h"

#include <iostream>
#include <cmath>

// #define DEBUGGING

// Helper functions
namespace render_direct {
	int set_m_attrs(
		const string& vertex_type,
		bool add_normal_flag = false
	) {
		int err;

		// At the beginning of the function
		data_m_attr.clear();

		err = data_m_attr.set_data_indices(vertex_type);
		if (err) return err;

		err = render_m_attr.set_render_indices(vertex_type);
		if (err) return err;

		if (add_normal_flag) // Hmmm
			render_m_attr.add_normal();

		err = render_m_attr.add_shading_offset();
		if (err) return err;


		// Debugging
		#ifdef DEBUGGING
		std::cout << "data_m_attr.geom_flag    = " << (data_m_attr.geom_flag    ? "true":"false") << '\n';
		std::cout << "data_m_attr.color_flag   = " << (data_m_attr.color_flag   ? "true":"false") << '\n';
		std::cout << "data_m_attr.normal_flag  = " << (data_m_attr.normal_flag  ? "true":"false") << '\n';
		std::cout << "data_m_attr.texture_flag = " << (data_m_attr.texture_flag ? "true":"false") << '\n';
		std::cout << "data_m_attr.weight_flag  = " << (data_m_attr.weight_flag  ? "true":"false") << '\n';
		std::cout << "data_m_attr.opacity_flag = " << (data_m_attr.opacity_flag ? "true":"false") << '\n';

		std::cout << "data_m_attr.size         = " << data_m_attr.size << '\n';

		std::cout << "data_m_attr.geometry     = " << data_m_attr.geometry << '\n';
		std::cout << "data_m_attr.color        = " << data_m_attr.color    << '\n';
		std::cout << "data_m_attr.normal       = " << data_m_attr.normal   << '\n';
		std::cout << "data_m_attr.texture      = " << data_m_attr.texture  << '\n';
		std::cout << "data_m_attr.weight       = " << data_m_attr.weight   << '\n';
		std::cout << "data_m_attr.opacity      = " << data_m_attr.opacity  << '\n';

		
		std::cout << "render_m_attr.geom_flag    = " << (render_m_attr.geom_flag    ? "true":"false") << '\n';
		std::cout << "render_m_attr.color_flag   = " << (render_m_attr.color_flag   ? "true":"false") << '\n';
		std::cout << "render_m_attr.normal_flag  = " << (render_m_attr.normal_flag  ? "true":"false") << '\n';
		std::cout << "render_m_attr.texture_flag = " << (render_m_attr.texture_flag ? "true":"false") << '\n';
		std::cout << "render_m_attr.weight_flag  = " << (render_m_attr.weight_flag  ? "true":"false") << '\n';
		std::cout << "render_m_attr.opacity_flag = " << (render_m_attr.opacity_flag ? "true":"false") << '\n';

		std::cout << "render_m_attr.size         = " << render_m_attr.size << '\n';

		std::cout << "render_m_attr.geometry     = " << render_m_attr.geometry << '\n';
		std::cout << "render_m_attr.color        = " << render_m_attr.color    << '\n';
		std::cout << "render_m_attr.normal       = " << render_m_attr.normal   << '\n';
		std::cout << "render_m_attr.texture      = " << render_m_attr.texture  << '\n';
		std::cout << "render_m_attr.weight       = " << render_m_attr.weight   << '\n';
		std::cout << "render_m_attr.opacity      = " << render_m_attr.opacity  << '\n';
		#endif

		return 0;
	}

	attr_point new_m_attr_point_from_vertex_i(const float* vertex, int i=0) {
		attr_point a = {};

		if (data_m_attr.geom_flag) {
			int r = render_m_attr.geometry;
			int d = i+data_m_attr.geometry;

			a.coord[0] = vertex[d];
			a.coord[1] = vertex[d+1];
			a.coord[2] = vertex[d+2];
			a.coord[3] = 1.f;
			a.coord[4] = 1.f;

			a.coord[r] = vertex[d]; r++; d++;
			a.coord[r] = vertex[d]; r++; d++;
			a.coord[r] = vertex[d];
		}

		if (data_m_attr.color_flag) {
			int r = render_m_attr.color;
			int d = i+data_m_attr.color;
			a.coord[r] = vertex[d]; r++; d++;
			a.coord[r] = vertex[d]; r++; d++;
			a.coord[r] = vertex[d];
		}

		if (data_m_attr.normal_flag) {
			int r = render_m_attr.normal;
			int d = i+data_m_attr.normal;
			a.coord[r] = vertex[d]; r++; d++;
			a.coord[r] = vertex[d]; r++; d++;
			a.coord[r] = vertex[d];
		} else if (render_m_attr.normal_flag) { // Optional?
			int r = render_m_attr.normal;
			a.coord[r  ] = vertex[0];
			a.coord[r+1] = vertex[1];
			a.coord[r+2] = vertex[2];
		}

		if (data_m_attr.texture_flag) {
			int r = render_m_attr.texture;
			int d = i+data_m_attr.texture;
			a.coord[r] = vertex[d]; r++; d++;
			a.coord[r] = vertex[d];
		}

		if (data_m_attr.weight_flag) {
			int r = render_m_attr.weight;
			int d = i+data_m_attr.weight;
			a.coord[r] = vertex[d];
			// ???
		}

		if (data_m_attr.opacity_flag) {
			int r = render_m_attr.opacity;
			int d = i+data_m_attr.opacity;
			a.coord[r] = vertex[d];
			// ???
		}

		#ifdef DEBUGGING
		std::cout << i << ": (";
		for (int x = 0; x < a.MAX; x++)
			if (a.coord[x])
				std::cout << a.coord[x] << ", ";
			else
				std::cout << '*';
		std::cout << ")\n";
		#endif

		return a;
	}

	// Linear Interpolation (out, p1, and p2 can be mixed no problem)
	void lerp(float t, int size, const float* p1, const float* p2, float* out) {
		for (int i = 0; i < size; i++) {
			float a = p1[i];
			float b = p2[i];
			out[i] = a + t*(b-a);
		}
	}

	void decasteljau(float t, int degree, const float* in_vertices, int vertices_size, float* out_vertex, int vertex_size) {
		// Copy vertices into temp
		float* temp = new float[vertices_size];
		for (int i = 0; i < vertices_size; i++)
			temp[i] = in_vertices[i];

		// Run algorithm
		// const int degree = (vertices_size/vertex_size) - 1; // Can degree be inferred?
		for (int i = 0; i <= degree; i++) {
			for (int a = 0, b = vertex_size; b < vertices_size; a = b, b += vertex_size) {
				float* p1 = &temp[a];
				float* p2 = &temp[b];
				lerp(t, vertex_size, p1, p2, p1);
			}
			vertices_size -= vertex_size; // Optional (reduce calculations by half)
		}

		// Answer in first, copy and return
		for (int i = 0; i < vertex_size; i++)
			out_vertex[i] = temp[i];

		delete[] temp;
	}

	void decasteljau2D(
		float u,
		float v,

		int vertex_size,
		int vertices_inner_size,
		int vertices_size,

		const float* in_vertices,
		float* out_vertex
	) {
		// Infer degrees
		const int u_degree = (vertices_inner_size / vertex_size) - 1;
		const int v_degree = (vertices_size / vertices_inner_size) - 1;

		// Prepare temp
		const int temp_size = (v_degree+1) * vertex_size;
		float* temp = new float[temp_size];

		// Calculate and save interpolated u-coords
		float* p = temp;
		for (int i = 0; i <= v_degree; i++) {
			decasteljau(
				u,
				u_degree,
				in_vertices + i*vertices_inner_size,
				vertices_inner_size,
				p,
				vertex_size
			);
			p += vertex_size;
		}

		// Calculate Final v-coord and return
		decasteljau(v, v_degree, temp, temp_size, out_vertex, vertex_size);
		delete[] temp;
	}
}

// Bezier Curve
int render_direct::render_bezier_curve(const string& vertex_type, int degree, const vector<float>& vertex) {
	#ifdef DEBUGGING
	std::cout << n_divisions   << '\n';
	std::cout << (obj_normal_flag ? "true":"false") << '\n';
	std::cout << "("  << poly_normal[0]
			  << ", " << poly_normal[1]
			  << ", " << poly_normal[2] << ")\n";

	std::cout << vertex_type   << '\n';
	std::cout << degree        << '\n';
	std::cout << vertex.size() << '\n';
	std::cout << "(";
	for (size_t x = 0; x < vertex.size(); x++)
		std::cout << vertex[x] << ", ";
	std::cout << ")\n";
	#endif


	// Set up
	int err = set_m_attrs(vertex_type, false);
	if (err) return err;

	// First point
	attr_point p = new_m_attr_point_from_vertex_i(vertex.data());
	line_pipeline(p, MOVE);

	// Interpolate and Draw Inner points (using De Casteljau's Algorithm)
	float* temp = new float[data_m_attr.size];
	for (int i = 1; i < n_divisions; i++) {
		decasteljau(
			(float)i / (float)n_divisions, // t
			degree,
			vertex.data(),      // in data
			(int)vertex.size(), // in size
			temp,            // out data
			data_m_attr.size // out size
		);

		p = new_m_attr_point_from_vertex_i(temp);
		line_pipeline(p, DRAW);
	}
	delete[] temp; // Clean up

	// Last point (line)
	p = new_m_attr_point_from_vertex_i(vertex.data(), (int)vertex.size()-data_m_attr.size);
	line_pipeline(p, DRAW);
	return 0;
}

// Bezier Patch
int render_direct::render_bezier_patch(const string& vertex_type, int u_degree, int v_degree, const vector<float>& vertex) {
	#ifdef DEBUGGING
	std::cout << "n_divisions = " << n_divisions   << '\n';
	std::cout << "obj_normal_flag = " << (obj_normal_flag ? "true":"false") << '\n';
	std::cout << "poly_normal = [" 
					  << poly_normal[0]
			  << ", " << poly_normal[1]
			  << ", " << poly_normal[2] << "]\n";

	std::cout << "vertex_type   = " << vertex_type   << '\n';
	std::cout << "u_degree      = " << u_degree      << '\n';
	std::cout << "v_degree      = " << v_degree      << '\n';
	std::cout << "vertex.size() = " << vertex.size() << '\n';
	std::cout << "vertex = [";
	for (size_t x = 0; x < vertex.size(); x++)
		std::cout << vertex[x] << ", ";
	std::cout << "]\n";
	#endif


	// Set up
	int err = set_m_attrs(vertex_type, true);
	if (err) return err;


	// For convenience
	const int u_size = u_degree*n_divisions + 1;
	const int v_size = v_degree*n_divisions + 1;
	const int vertex_inner_size = data_m_attr.size * (u_degree+1);

	// Allocate space to hold all the interpolated points
	const int bezier_points_inner_size = u_size * data_m_attr.size;
	const int bezier_points_size       = v_size * bezier_points_inner_size;
	float* bezier_points = new float[bezier_points_size]; // arr[v][u][?]

	// Calculate and store point data
	for (int v = 0; v < v_size; v++) {
		const int   vi = v * bezier_points_inner_size;
		const float vt = (float)v / (float)(v_size-1);

		for (int u = 0; u < u_size; u++) {
			const int   ui = u * data_m_attr.size;
			const float ut = (float)u / (float)(u_size-1);

			decasteljau2D(
				ut, vt,

				data_m_attr.size,
				vertex_inner_size,
				(int)vertex.size(), // Same: data_m_attr.size * (u_degree+1) * (v_degree+1)

				vertex.data(),
				bezier_points + vi + ui
			);
		}
	}


	// Calculate Vertex Derivatives (Normals Only)
	constexpr int BASE_SIZE = 3;

	const int bezierU_inner_size = (u_degree  ) * BASE_SIZE; // u-1
	const int bezierU_size       = (v_degree+1) * bezierU_inner_size;
	float* bezierU = new float[bezierU_size];

	for (int v = 0; v < v_degree+1; v++) {
		const int vo = v * bezierU_inner_size;
		const int vi = v * vertex_inner_size;

		for (int u = 0; u < u_degree; u++) {
			const int uo = u * BASE_SIZE;
			const int u1 = u * data_m_attr.size + data_m_attr.geometry; // u
			const int u2 = u1+ data_m_attr.size;                        // u+1

			float* out = bezierU             + vo + uo;
			const float* p1  = vertex.data() + vi + u1;
			const float* p2  = vertex.data() + vi + u2;

			// Optimized Derivative: shift to the left and double (2dx)
			*out = 2 * (*p2 - *p1); out++; p2++; p1++;
			*out = 2 * (*p2 - *p1); out++; p2++; p1++;
			*out = 2 * (*p2 - *p1);
		}
	}

	const int bezierV_inner_size = (u_degree+1) * BASE_SIZE;
	const int bezierV_size       = (v_degree  ) * bezierV_inner_size; // v-1
	float* bezierV = new float[bezierV_size];

	for (int v = 0; v < v_degree; v++) {
		const int vo = v * bezierV_inner_size;
		const int v1 = v * vertex_inner_size; // v
		const int v2 = v1+ vertex_inner_size; // v+1

		for (int u = 0; u < u_degree+1; u++) {
			const int uo = u * BASE_SIZE;
			const int ui = u * data_m_attr.size + data_m_attr.geometry;

			float* out = bezierV             + vo + uo;
			const float* p1  = vertex.data() + v1 + ui;
			const float* p2  = vertex.data() + v2 + ui;

			// Optimized Derivative: shift to the top and double (2dy)
			*out = 2 * (*p2 - *p1); out++; p2++; p1++;
			*out = 2 * (*p2 - *p1); out++; p2++; p1++;
			*out = 2 * (*p2 - *p1);
		}
	}

	// Finally, use them to generate the mathematically accurate normals and save them
	const int bezier_normals_inner_size = u_size * BASE_SIZE; // u-1
	const int bezier_normals_size       = v_size * bezier_normals_inner_size;
	float* bezier_normals = new float[bezier_normals_size];

	for (int v = 0; v < v_size; v++) {
	//  const int   vi = v * bezier_points_inner_size;
		const int   vn = v * bezier_normals_inner_size;
		const float vt = (float)v / (float)(v_size-1);

		for (int u = 0; u < u_size; u++) {
		//  const int   ui = u * data_m_attr.size;
			const int   un = u * BASE_SIZE;
			const float ut = (float)u / (float)(u_size-1);

			// U derivative
			float u_dir[3];
			decasteljau2D(
				ut, vt,
				BASE_SIZE,
				bezierU_inner_size,
				bezierU_size,
				bezierU,
				u_dir
			);

			// V derivative
			float v_dir[3];
			decasteljau2D(
				ut, vt,
				BASE_SIZE,
				bezierV_inner_size,
				bezierV_size,
				bezierV,
				v_dir
			);

			// Calculate cross product and finally there's your normal!
			float* n = bezier_normals + vn + un; // Save in normal position
			n[0] = u_dir[1]*v_dir[2] - u_dir[2]*v_dir[1]; // y1*z2 - z1*y2
			n[1] = u_dir[2]*v_dir[0] - u_dir[0]*v_dir[2]; // z1*x2 - x1*z2 // flipped Y sign
			n[2] = u_dir[0]*v_dir[1] - u_dir[1]*v_dir[0]; // x1*y2 - y1*x2
		}
	}

	delete[] bezierU;
	delete[] bezierV;

	/*
	Example:

	2x3 Patch
	(-3, 4, 0)      ( 1, 6, 5)      ( 2, 3, 1)
	*---------------*---------------*
	|               |               |
	| (-1, 0, 1)    | ( 2, 0,-2)    | ( 1, 1, 5)
	*---------------*---------------*
	|               |               |
	| ( 2, 4, 8)    | ( 1, 1, 1)    | ( 3, 3, 5)
	*---------------*---------------*
	|               |               |
	| (-4, 0, 2)    | ( 6, 7, 5)    | ( 0, 0, 1)
	*---------------*---------------*

	1x3 Patch
	  (8, 4,10)       (2,-6, 8)
	*---------------*
	|               |
	| (6, 0,-6)     | (-2, 2,14)
	*---------------*
	|               |
	| (-2,-6,-14)   | ( 4, 4, 8)
	*---------------*
	|               |
	| (20,14, 6)    | (-12,-14,-8)
	*---------------*

	i.e. ( 6, 7, 5) - (-4, 0, 2) = (10, 7, 3) * 2 = (20, 14, 6)
	*/


	// Draw polygons
	for (int v = 0; v < v_size-1; v++) {
		const int vi = v * bezier_points_inner_size;
		const int vn = v * bezier_normals_inner_size;

		for (int u = 0; u < u_size-1; u++) {
			const int u1 = u*BASE_SIZE;

			float* a = bezier_points + vi + u*data_m_attr.size;
			float* b = a + data_m_attr.size;
			float* c = b + bezier_points_inner_size;
			float* d = c - data_m_attr.size;

			float* n;
			attr_point p;

			n = bezier_normals + vn + u1;
			poly_normal[0] = n[0];
			poly_normal[1] = n[1];
			poly_normal[2] = n[2];
			p = new_m_attr_point_from_vertex_i(a);
			poly_pipeline(p, MOVE);

			n += BASE_SIZE;
			poly_normal[0] = n[0];
			poly_normal[1] = n[1];
			poly_normal[2] = n[2];
			p = new_m_attr_point_from_vertex_i(b);
			poly_pipeline(p, MOVE);

			n += bezier_normals_inner_size;
			poly_normal[0] = n[0];
			poly_normal[1] = n[1];
			poly_normal[2] = n[2];
			p = new_m_attr_point_from_vertex_i(c);
			poly_pipeline(p, MOVE);

			n -= BASE_SIZE;
			poly_normal[0] = n[0];
			poly_normal[1] = n[1];
			poly_normal[2] = n[2];
			p = new_m_attr_point_from_vertex_i(d);
			poly_pipeline(p, DRAW);
		}
	}

	// Clean up
	delete[] bezier_points;
	delete[] bezier_normals;
	return 0;
}



// Because pow(negative, non-integer) == NaN
float ipow(float base, float exp) {
	// if (base < 0)
	//     return -std::pow(-base, exp);
	// else
	//     return std::pow(base, exp);
	return std::copysign(std::pow(std::abs(base), exp), base);
}

// Superquadric Sphere
int REDirect::rd_sqsphere(
	float radius, float north, float east,
	float zmin, float zmax, float thetamax
) { (void)zmin; (void)zmax; (void)thetamax;
	// To initialize the rendering pipeline with the following instructions:
	render_direct::obj_normal_flag = true;
	render_m_attr.clear();
	render_m_attr.add_geometry();
	render_m_attr.add_normal();
	render_m_attr.add_shading_offset();

	// data_m_attr.clear();
	// data_m_attr.set_data_indices("P");

	// These should come before any vertices are sent to the polygon pipeline,
	// but they only need to be done once per superquadric.
	// Space is being reserved for per-vertex normals,
	// but the use of this information is controlled in the scene file.

	#ifdef DEBUGGING
	std::cout << "radius   = " << radius   << '\n';
	std::cout << "north    = " << north    << '\n';
	std::cout << "east     = " << east     << '\n';
	std::cout << "zmin     = " << zmin     << '\n';
	std::cout << "zmax     = " << zmax     << '\n';
	std::cout << "thetamax = " << thetamax << '\n';
	std::cout << '\n';
	#endif


	const int   steps = render_direct::n_divisions;
	const float ratio = 2*M_PIf / float(steps);

	for (int v = -steps/4; v < steps/4; v++) { // Y = -90 to 90
		const float v0 = float(v  ) * ratio;
		const float v1 = float(v+1) * ratio;

		for (int u = 0; u < steps; u++) { // X = 0 to 360
			const float u0 = float(u  ) * ratio;
			const float u1 = float(u+1) * ratio;

			// Start (bottom-left)
			float a[3];
			a[0] = radius * ipow(std::cos(v0), north) * ipow(std::cos(u0), east);
			a[1] = radius * ipow(std::cos(v0), north) * ipow(std::sin(u0), east);
			a[2] = radius * ipow(std::sin(v0), north);

			// Right (bottom-right)
			float b[3];
			b[0] = radius * ipow(std::cos(v0), north) * ipow(std::cos(u1), east);
			b[1] = radius * ipow(std::cos(v0), north) * ipow(std::sin(u1), east);
			b[2] = radius * ipow(std::sin(v0), north);

			// Up (top-right)
			float c[3];
			c[0] = radius * ipow(std::cos(v1), north) * ipow(std::cos(u1), east);
			c[1] = radius * ipow(std::cos(v1), north) * ipow(std::sin(u1), east);
			c[2] = radius * ipow(std::sin(v1), north);

			// Left and Finish (top-left)
			float d[3];
			d[0] = radius * ipow(std::cos(v1), north) * ipow(std::cos(u0), east);
			d[1] = radius * ipow(std::cos(v1), north) * ipow(std::sin(u0), east);
			d[2] = radius * ipow(std::sin(v1), north);


			float e1[3]; // edge
			e1[0] = b[0] - a[0];
			e1[1] = b[1] - a[1];
			e1[2] = b[2] - a[2];

			float e2[3];
			e2[0] = d[0] - a[0];
			e2[1] = d[1] - a[1];
			e2[2] = d[2] - a[2];

			render_direct::poly_normal[0] = e1[1]*e2[2] - e1[2]*e2[1]; // y1*z2 - z1*y2
			render_direct::poly_normal[1] = e1[2]*e2[0] - e1[0]*e2[2]; // z1*x2 - x1*z2 // flipped Y sign
			render_direct::poly_normal[2] = e1[0]*e2[1] - e1[1]*e2[0]; // x1*y2 - y1*x2


			// Draw
			attr_point p;
			p.coord[0] = a[0]; p.coord[1] = a[1]; p.coord[2] = a[2]; p.coord[3] = 1;
			render_direct::poly_pipeline(p, MOVE);
			p.coord[0] = b[0]; p.coord[1] = b[1]; p.coord[2] = b[2]; p.coord[3] = 1;
			render_direct::poly_pipeline(p, MOVE);
			p.coord[0] = c[0]; p.coord[1] = c[1]; p.coord[2] = c[2]; p.coord[3] = 1;
			render_direct::poly_pipeline(p, MOVE);
			p.coord[0] = d[0]; p.coord[1] = d[1]; p.coord[2] = d[2]; p.coord[3] = 1;
			render_direct::poly_pipeline(p, DRAW);
		}
	}

	return 0;
}

// Superquadric Torus
int REDirect::rd_sqtorus(
	float radius1, float radius2, float north, float east,
	float phimin, float phimax, float thetamax
) { (void)phimin; (void)phimax; (void)thetamax;
	// To initialize the rendering pipeline with the following instructions:
	render_direct::obj_normal_flag = true;
	render_m_attr.clear();
	render_m_attr.add_geometry();
	render_m_attr.add_normal();
	render_m_attr.add_shading_offset();

	data_m_attr.clear();
	data_m_attr.set_data_indices("P");


	const int   steps = render_direct::n_divisions;
	const float ratio = 2*M_PIf / float(steps);

	for (int v = -steps/2; v < steps/2; v++) { // Y = -180 to 180
		const float v0 = float(v  ) * ratio;
		const float v1 = float(v+1) * ratio;

		for (int u = 0; u < steps; u++) { // X = 0 to 360
			const float u0 = float(u  ) * ratio;
			const float u1 = float(u+1) * ratio;

			// Start (top-left)
			float a[3];
			a[0] = (radius1 + radius2*ipow(std::cos(v0), north)) * ipow(std::cos(u0), east);
			a[1] = (radius1 + radius2*ipow(std::cos(v0), north)) * ipow(std::sin(u0), east);
			a[2] =            radius2*ipow(std::sin(v0), north);

			// Right (top-right)
			float b[3];
			b[0] = (radius1 + radius2*ipow(std::cos(v0), north)) * ipow(std::cos(u1), east);
			b[1] = (radius1 + radius2*ipow(std::cos(v0), north)) * ipow(std::sin(u1), east);
			b[2] =            radius2*ipow(std::sin(v0), north);

			// Down (bottom-right)
			float c[3];
			c[0] = (radius1 + radius2*ipow(std::cos(v1), north)) * ipow(std::cos(u1), east);
			c[1] = (radius1 + radius2*ipow(std::cos(v1), north)) * ipow(std::sin(u1), east);
			c[2] =            radius2*ipow(std::sin(v1), north);

			// Left and Finish (bottom-left)
			float d[3];
			d[0] = (radius1 + radius2*ipow(std::cos(v1), north)) * ipow(std::cos(u0), east);
			d[1] = (radius1 + radius2*ipow(std::cos(v1), north)) * ipow(std::sin(u0), east);
			d[2] =            radius2*ipow(std::sin(v1), north);


			// Normal = (a-c) X (b-d)
			float e1[3]; // edge
			e1[0] = a[0] - c[0];
			e1[1] = a[1] - c[1];
			e1[2] = a[2] - c[2];

			float e2[3];
			e2[0] = b[0] - d[0];
			e2[1] = b[1] - d[1];
			e2[2] = b[2] - d[2];

			render_direct::poly_normal[0] = e1[1]*e2[2] - e1[2]*e2[1]; // y1*z2 - z1*y2
			render_direct::poly_normal[1] = e1[2]*e2[0] - e1[0]*e2[2]; // z1*x2 - x1*z2 // flipped Y sign
			render_direct::poly_normal[2] = e1[0]*e2[1] - e1[1]*e2[0]; // x1*y2 - y1*x2


			// Draw
			attr_point p;
			p = render_direct::new_m_attr_point_from_vertex_i(a);
			render_direct::poly_pipeline(p, MOVE);
			p = render_direct::new_m_attr_point_from_vertex_i(b);
			render_direct::poly_pipeline(p, MOVE);
			p = render_direct::new_m_attr_point_from_vertex_i(c);
			render_direct::poly_pipeline(p, MOVE);
			p = render_direct::new_m_attr_point_from_vertex_i(d);
			render_direct::poly_pipeline(p, DRAW);
		}
	}
	return 0;
}
