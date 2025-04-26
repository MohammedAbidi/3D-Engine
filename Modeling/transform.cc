#include <fstream>
#include <iostream>
#include <cmath> // NAN


// Globals
#define OK 0
#define ILLEGAL_TOKEN -1
#define UNEXPECTED_EOF -2
#define ILLEGAL_VERTEX_INDEX -3
#define VERTEX_LIST_OVERFLOW -4
#define UNKNOWN_VERTEX_TYPE -5
#define VERTEX_LIST_MAX_SIZE 65536 // Arbitrary (usually minimum = vertices * floats_per_vertex)

// eat_space, get_vertex_size, parse_polyset, and set_attribute_indices
// originally written by Dr. Duffin at @ NIU
int rd_input_line = 0; // Useful for error messages

// Removes ("eats") extraneous white space and comments from input
static void eat_space(std::istream& file) {
	int dummychar;
	int valid = 0;

	// Main loop
	do {
		dummychar = file.get();
		if (file.eof()) return; // Read past end of file (EOF)

		switch(dummychar){
		case ' ':  // Space
		case '\t': // Tab
		case '\r': // Needed for DOS files
			break;

		case '\n': // Line
			rd_input_line++;
			break;

		case '#': // Eat comments
			while (file.get() != '\n') if (file.eof()) return;
			rd_input_line++;
			break;

		default:
			file.putback(char(dummychar));
			valid = 1;
			break;
		}
	} while(!valid);
}

// Given a vertex type string returns the number of array elements
// taken up by a single vertex.
int get_vertex_size(const std::string& vertex_type) {
	const char* attribute = &vertex_type[0];
	int vertex_size = 0;

	for (size_t i = vertex_type.length(); i--; attribute++) {
		switch (*attribute) {
		case 'P':
		case 'N':
		case 'D':
		case 'C':
			vertex_size += 3;
			break;
		case 'T':
			vertex_size += 2;
			break;
		/*
		case '\"': break; // Ignore \"
		default:   break;
		*/
		}
	}

	return vertex_size;
}

// Given an input stream returns
//     the vertex type string (vtype),
//     the number of vertices (nvertex),
//     the number of faces (nface),
//     and the values of the vertex coordinate (coord)
//     and the values of the polygon vertex indices (vertex_list)
// The arrays must already have memory allocated to them.
int parse_polyset(
	std::istream& istr,
	std::string & vtype,
	int& nvertex,
	int& nface,
	float coord[],
	int vertex_list[]
)  {
	eat_space(istr);
	std::string token;
	istr >> token;
	if (token != "PolySet") return ILLEGAL_TOKEN;

	eat_space(istr);
	if (istr.eof()) return UNEXPECTED_EOF;
	istr >> vtype;

	int vsize = get_vertex_size(vtype);
	if (vsize < 0) return vsize; // error code

	eat_space(istr);
	istr >> nvertex;
	eat_space(istr);
	istr >> nface;

	for (int i = 0, size = vsize*nvertex; i < size; i++) {
		eat_space(istr);
		if (istr.eof()) return UNEXPECTED_EOF;
		eat_space(istr);

		float dvalue;
		istr >> dvalue;
		coord[i] = dvalue;
	}

	int index_cnt = 0;
	for (int i = 0; i < nface;) {
		eat_space(istr);
		if (istr.eof()) return UNEXPECTED_EOF;
		eat_space(istr);

		int index;
		istr >> index;
		if (index >= nvertex)
			return ILLEGAL_VERTEX_INDEX;

		vertex_list[index_cnt++] = index;
		if (index_cnt > VERTEX_LIST_MAX_SIZE)
			return VERTEX_LIST_OVERFLOW;

		if (index == -1) i++;  // New face
	}

	return OK;
}

// Given a vertex type string returns
//    the number of elements taken by a vertex (vertex_size),
//    the index positions of the start of various attributes within a vertex.
//    Current attributes include geometry, color, normal, and texture.
//    The sizes of each attribute are as follows:
//    geometry - 3 values
//    color - 3 values
//    normal - 3 values
//    texture - 2 values
int set_attribute_indices(
	const std::string& vertex_type,
	int& vertex_size,
	int& geometry,
	int& color,
	int& normal,
	int& texture
)  {
	geometry = color = normal = texture = -1;  // Not used
	vertex_size = 0;

	const char* attribute = &vertex_type[0];

	for (size_t i = vertex_type.length(); i; attribute++, i--) {
		switch(*attribute) {
		case '\"': // Ignore \"
			break;

		case 'P':
			geometry = vertex_size;
			vertex_size += 3;
			break;

		case 'N': case 'D':
			normal = vertex_size;
			vertex_size += 3;
			break;

		case 'C':
			color = vertex_size;
			vertex_size += 3;
			break;

		case 'T':
			texture = vertex_size;
			vertex_size += 2;
			break;

		default:
			return UNKNOWN_VERTEX_TYPE;
		}
	}

	return OK;
}


int main(int argc, const char* argv[]) {
	// Standard Setup
	if (argc <= 3) {
		std::cout <<
			"Usage: " << argv[0] << " polyset_file  \"option\"  args... \n"
			"polyset_file - relative path filename (polyset files usually end with .dat)\n"
			"    Note: Using '-' as the name of the source_file will take input form the standard input (then signal end of file using ctrl+D)\n"
			"\n"
			"option - string name of preset options with variable arguments (see below)\n"
			"       - \"skew\"    x  y\n"
			"       - \"twist\"   rotation_degrees\n"
			"       - \"extrude\" extrusion_length \n"
			"       - \"sweep\"   steps  angle_degrees\n"
		<< std::endl;
		return 0;
	}

	// Lazy check
	const std::string option = argv[2];
	if (option != "skew"    && option != "twist"
	 && option != "extrude" && option != "sweep") {
		std::cerr << "Invalid option " << option << std::endl;
		return 2;
	}

	// Prepare Input (read from stdin if file name is "-" (until ctrl + D))
	std::ifstream input_file; // Input file
	std::istream* input_stream = nullptr; // Input stream
	if (argv[1][0] == '-' && argv[1][1] == '\0') {
		input_stream = &std::cin; // Connect to input stream
	} else {
		input_file.open(argv[1], std::ios::in); // File checking
		if (!input_file.is_open()) {
			std::cerr << "Error opening input file: " << argv[1] << std::endl;
			return 1;
		}
		input_stream = &input_file; // File is valid, connect to file stream
	}

	// Parse polyset from input (with error handling)
	std::string vtype; // the vertex type string
	int nvertex;       // the number of vertices
	int nface;         // the number of faces
	float     coord[VERTEX_LIST_MAX_SIZE]; // Values of the vertex coordinate (coord)
	int vertex_list[VERTEX_LIST_MAX_SIZE]; // Values of the polygon vertex indices (vertex_list)
	int err = parse_polyset(*input_stream, vtype, nvertex, nface, coord, vertex_list);

	// Get vertex attributes (if previous was a success)
	int vertex_size; // Total data size per vertex (at most = 3 + 3 + 3 + 2)
	int geometry, color, normal, texture; // the index positions of the start of various attributes within a vertex (-1 if unused)
	if (err == OK)
		err = set_attribute_indices(vtype, vertex_size, geometry, color, normal, texture);

	// Clean up input
	if (input_stream != &std::cin)
		input_file.close();

	// Error checking
	if (err != OK || geometry == -1) { // or no geometrical data
		switch (err) {
		case ILLEGAL_TOKEN:        std::cerr << "Illegal token"          << std::endl; return err;
		case UNEXPECTED_EOF:       std::cerr << "Unexpected end of file" << std::endl; return err;
		case ILLEGAL_VERTEX_INDEX: std::cerr << "Illegal vertex index"   << std::endl; return err;
		case VERTEX_LIST_OVERFLOW: std::cerr << "Vertex list overflow"   << std::endl; return err;
		case UNKNOWN_VERTEX_TYPE:  std::cerr << "Unknown vertex type"    << std::endl; return err;
		default:                   std::cerr << "Unknown error"          << std::endl; return err;
		}
	}


	if (option == "skew")
	{
		// Parse x and y
		const float X_SKEW = strtof(argv[3], nullptr);
		if (X_SKEW == NAN) {
			std::cerr << "Invalid x: " << argv[3] << std::endl;
			return 2;
		}
		const float Y_SKEW = strtof(argv[4], nullptr);
		if (Y_SKEW == NAN) {
			std::cerr << "Invalid y: " << argv[4] << std::endl;
			return 3;
		}

		// First Loop, collect data (bounding box)
		float minZ =  INFINITY; // float( 1e300); // positive and negative infinity
		float maxZ = -INFINITY; // float(-1e300); // (double casted down to float)
		for (int v = geometry, i = 0; i < nvertex; v += vertex_size, i++) {
			const float Z = coord[v+2];
			minZ = std::min(minZ, Z);
			maxZ = std::max(maxZ, Z);
		}
		const float LENGTH_Z =  maxZ - minZ;

		// Quick check if polygon a actually a completely-flat polygon on the XY-plane (just translate it and avoid divison by 0)
		if (LENGTH_Z == 0.0f) {
			for (int i = geometry, _ = 0; _ < nvertex; i += vertex_size, _++) {
				coord[i]   += X_SKEW;
				coord[i+1] += Y_SKEW;
			}
		} else {
			// Main Loop
			for (int i = geometry, _ = 0; _ < nvertex; i += vertex_size, _++) {
				const float INTERPOLATION = (coord[i+2] - minZ) / LENGTH_Z; // Determine the "strength" of the skew based on Z height
				coord[i]   += X_SKEW*INTERPOLATION;
				coord[i+1] += Y_SKEW*INTERPOLATION;
			}
		}
	}
	else if (option == "twist")
	{
		// First Loop, collect data (bounding box)
		float minX =  INFINITY; // float( 1e300); // positive and negative infinity
		float maxX = -INFINITY; // float(-1e300); // (double casted down to float)
		float minY =  INFINITY;
		float maxY = -INFINITY;
		float minZ =  INFINITY;
		float maxZ = -INFINITY;
		for (int v = geometry, i = 0; i < nvertex; v += vertex_size, i++) {
			const float X = coord[v];   minX = std::min(minX, X); maxX = std::max(maxX, X);
			const float Y = coord[v+1]; minY = std::min(minY, Y); maxY = std::max(maxY, Y);
			const float Z = coord[v+2]; minZ = std::min(minZ, Z); maxZ = std::max(maxZ, Z);
		}
		const float CENTER_X = (maxX + minX) / 2.0f;
		const float CENTER_Y = (maxY + minY) / 2.0f;
		const float LENGTH_Z =  maxZ - minZ;
		const float ROTATION = strtof(argv[3],nullptr) * (M_PIf/180.0f); // cstring-degrees to float-radians (this actually could've been calculated at any time)

		// Quick check if polygon a actually a completely-flat polygon on the XY-plane (just rotate it and avoid divison by 0)
		if (LENGTH_Z == 0.0f) {
			const float SINE   = std::sin(ROTATION);
			const float COSINE = std::cos(ROTATION);
			for (int i = geometry, _ = 0; _ < nvertex; i += vertex_size, _++) {
				const float X = coord[i]   - CENTER_X;
				const float Y = coord[i+1] - CENTER_Y;
				coord[i]   = (X*COSINE - Y*SINE)   + CENTER_X;
				coord[i+1] = (X*SINE   + Y*COSINE) + CENTER_Y;
			}
		} else {
			// Main Loop:
			//   Twist around the Z-axis on XY-plane counter-clockwise,
			//   with more "twist" the higher the vertex is (relative to itself)
			for (int i = geometry, _ = 0; _ < nvertex; i += vertex_size, _++) {
				const float INTERPOLATION = ROTATION * (coord[i+2] - minZ) / LENGTH_Z; // Determine the "strength" of the twist based on Z height
				const float SINE   = std::sin(INTERPOLATION);
				const float COSINE = std::cos(INTERPOLATION);

				// Rotate with respect to "origin" (z remains unchanged)
				const float X = coord[i]   - CENTER_X;
				const float Y = coord[i+1] - CENTER_Y;
				coord[i]   = (X*COSINE - Y*SINE)   + CENTER_X;
				coord[i+1] = (X*SINE   + Y*COSINE) + CENTER_Y;
			}
		}
	}
	else if (option == "extrude")
	{
		// Parse extrusion length argument
		const float EXTRUSION_LENGTH = strtof(argv[3], nullptr);
		if (EXTRUSION_LENGTH == NAN) {
			std::cerr << "Invalid extrusion length: " << argv[3] << std::endl;
			return 2;
		}

		// Setup: Count elements in vertex_list
		int vertex_list_size = 0;
		for (int _=0; _ < nface; _++)
			while (vertex_list[vertex_list_size++] != -1);

		// First:
		// Duplicate vertices
		const int COORD_SIZE = nvertex*vertex_size;
		for (int i = 0; i < COORD_SIZE; i++)
			coord[COORD_SIZE+i] = coord[i];

		// Move them upwards (set Z-coordinates)
		const int NEW_COORD_SIZE = COORD_SIZE*2; // NEW_NVERTEX*vertex_size;
		for (int i = COORD_SIZE+geometry+2; i < NEW_COORD_SIZE; i += vertex_size)
			coord[i] += EXTRUSION_LENGTH;


		// Second: duplicate faces
		for (int i=0,j=vertex_list_size,f=0; f < nface; i++,j++) {
			const int v = vertex_list[i];
			if (v == -1) {
				vertex_list[j] = -1; // End
				f++; // Next face
			} else {
				vertex_list[j] = v + nvertex; // Copy
			}
		}

		// Third: build "walls"
		int j = vertex_list_size*2; // Starting point for faces
		for (int _=0,i=0; _ < nface; _++,i++) {
			const int init = vertex_list[i]; // The start value to "loop" back to
			int next = init;
			do {
				i++; // Next
				const int curr = next;
				next = vertex_list[i];
				if (next == -1)
					next = init;

				vertex_list[j++] = curr;
				vertex_list[j++] = next;
				vertex_list[j++] = next + nvertex;
				vertex_list[j++] = curr + nvertex;
				vertex_list[j++] = -1;
			} while (next != init);
		}

		// Finally: flip original (counter-clockwise to clockwise)
		for (int _=0,i=0; _ < nface; _++) {
			// Get eof position (-1)
			int eof = i;
			while (vertex_list[++eof] != -1); // Assumes no consecutive -1's

			// Flip
			for (int a = i, b = eof-1; a < b; a++, b--) {
				const int temp = vertex_list[a];
				vertex_list[a] = vertex_list[b];
				vertex_list[b] = temp;
			}
			i = eof+1; // Next face
		}

		// Done (set new vertices/faces)
		nvertex = nvertex*2;
		nface   = nface*2 + vertex_list_size - nface; // nface*2 + nedges
	}
	else if (option == "sweep")
	{
		// Parse extrusion length argument
		int STEPS;
		try {
			STEPS = std::stoi(argv[3]);
		} catch (const std::invalid_argument& e) {
			std::cerr << "Steps " << argv[3] << " ran into a conversion error: " << e.what() << std::endl;
			return 2;
		} catch (const std::out_of_range& e) {
			 std::cerr << "Error: Steps " << argv[3] << " out of range. " << e.what() << std::endl;
			return 2;
		}

		float ANGLE = strtof(argv[4], nullptr);
		if (ANGLE == NAN) {
			std::cerr << "Invalid angle in degrees: " << argv[4] << std::endl;
			return 3;
		}
		ANGLE *= M_PIf/180.0f; // Convert to radians

		// Setup: Count elements in vertex_list
		const int COORD_SIZE = nvertex*vertex_size; // Original size of coord
		int VERTEX_LIST_SIZE = 0; // Original size of vertex_list
		for (int _=0; _ < nface; _++)
			while (vertex_list[VERTEX_LIST_SIZE++] != -1);
		const int N_EDGES = VERTEX_LIST_SIZE-nface; // Original number of edges

		// Main Loop
		for (int step = 0; step < STEPS; step++) {
			// First:
			// Duplicate vertices
			const int NEW_COORD_POS = COORD_SIZE*(step+1);
			for (int i = 0; i < COORD_SIZE; i++)
				coord[NEW_COORD_POS+i] = coord[i];

			// Rotate them on the XZ-plane
			const int NEW_COORD_SIZE = NEW_COORD_POS + COORD_SIZE;
			const float ROTATION = (float(step+1) / float(STEPS)) * ANGLE;
			const float SINE   = std::sin(ROTATION);
			const float COSINE = std::cos(ROTATION);
			for (int i = NEW_COORD_POS+geometry; i < NEW_COORD_SIZE; i += vertex_size) {
				const float X = coord[i];
				const float Z = coord[i+2];
				coord[i]   = X*COSINE + Z*SINE;
				coord[i+2] = X*SINE   - Z*COSINE;
			}

			// Second: build "walls"
			const int VERTEX_OFFSET = nvertex*step;
			int j = VERTEX_LIST_SIZE + step*N_EDGES*5; // Starting point for faces (5 because thats the size of the data for the "walls")
			for (int _=0,i=0; _ < nface; _++,i++) {
				const int init = vertex_list[i] + VERTEX_OFFSET; // The start value to "loop" back to
				int next = init;
				do {
					i++; // Next
					const int curr = next;
					next = vertex_list[i];

					// Check eof
					if (next == -1)
						next = init;
					else
						next += VERTEX_OFFSET;

					// The 4 "walls" (plus a -1 for eof)
					vertex_list[j++] = curr;
					vertex_list[j++] = next;
					vertex_list[j++] = next + nvertex;
					vertex_list[j++] = curr + nvertex;
					vertex_list[j++] = -1;
				} while (next != init);
			}
		}

		// Finally:
		// Cap new face(s)
		// j = new vertex list position
		const int LAST_OFFSET = nvertex*STEPS;
		for (int i=0,j=VERTEX_LIST_SIZE + STEPS*N_EDGES*5,f=0; f < nface; i++,j++) {
			const int v = vertex_list[i];
			if (v == -1) {
				vertex_list[j] = -1; // End
				f++; // Next face
			} else {
				vertex_list[j] = v + LAST_OFFSET; // Copy
			}
		}

		// Flip original (counter-clockwise to clockwise)
		for (int _=0,i=0; _ < nface; _++) {
			// Get eof position (-1)
			int eof = i;
			while (vertex_list[++eof] != -1); // Assumes no consecutive -1's

			// Flip
			for (int a = i, b = eof-1; a < b; a++, b--) {
				const int temp = vertex_list[a];
				vertex_list[a] = vertex_list[b];
				vertex_list[b] = temp;
			}
			i = eof+1; // Next face
		}

		// Done
		nvertex = nvertex * (STEPS+1);
		nface   = nface*2 +  STEPS*N_EDGES;
	}


	// Output to stdout
	std::cout << "PolySet " << vtype << '\n'
			  << nvertex << " # Vertices\n" // optional
			  << nface   << " # Faces\n\n"; // comments

	// Vertices
	for (int i=0,y=0; y < nvertex; y++) {
		for (int x=0; x < vertex_size; x++,i++) {
			std::cout << coord[i] << ' ';
		}
		std::cout << '\n';
	}

	// Faces
	for (int _=0,i=0,v; _ < nface; _++) {
		std::cout << '\n';
		do {
			v = vertex_list[i++];
			std::cout << v << ' ';
		} while (v != -1);
	}
	std::cout << '\n';


	return 0;
}