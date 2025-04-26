// This entire cview script was originally written by Dr. Duffin at @ NIU
#include <iostream>
#include <fstream>
#include <cmath>
#include <ctime>

// Mathematically built to repeat every 600 frames
constexpr float SPIN_PERIOD     = 120; // Number of frames
constexpr float WOBBLE_PERIOD   = 200; // Number of frames
constexpr int   DELAY_PER_FRAME = 100000000; // nanosec

int usage(const char* argv0) {
	std::cerr << "Usage: " << argv0 << " [-d] n_cols  n_rows  n_planes polyset_file\n";
	std::cerr << "   Output goes to standard out and should be piped to rd_view\n";
	std::cerr << "   -d  turn *off* double-sided polygons" << std::endl;
	return -1;
}

constexpr char const* rd_preface[] = {
	"Display \"Marching Cubes\" \"Screen\" \"rgbdouble\"",
	// "Display \"Marching Cubes\" \"PNM\" \"rgb\"",
};


int main(int argc, char* argv[]) {
	if (argc < 5)
		return usage(argv[0]);

	bool doubleside_flag = true;
	int arg_index = 1;
	if (argv[arg_index] == std::string("-d")) {
		doubleside_flag = false;
		arg_index++;
	}

	const long int ncols    = strtol(argv[arg_index++], 0, 10);
	const long int nrows    = strtol(argv[arg_index++], 0, 10);
	const long int nplanes  = strtol(argv[arg_index++], 0, 10);
	const char* in_filename = argv[arg_index++];

	// Read in the polyset
	std::ifstream in_file;
	in_file.open(in_filename);
	if (!in_file) {
		std::cerr << "Can't open " << in_filename << " for input." << std::endl;
		return -1;
	}

	std::string data = {};
	std::string line = {};
	getline(in_file, line);
	while (!in_file.eof()) {
		data += line + '\n';
		getline(in_file, line);
	}
	in_file.close();

	// Output Cout
	for(size_t i = 0; i < sizeof(rd_preface)/sizeof(char*); i++)
		std::cout << rd_preface[i] << std::endl;

	std::cout << "ObjectBegin \"Data\"\n";
	std::cout << data;
	std::cout << "ObjectEnd\n";
	
	int frame_count = 0;
	while (true) {
		// if (frame_count >= 600) break;

		std::cout << "FrameBegin " << frame_count++ << "\n";
		std::cout << "WorldBegin\n";

		// Translate
		long int max_dim = ncols;
		max_dim = (max_dim > nrows)? max_dim : nrows;
		max_dim = (max_dim > nplanes)? max_dim : nplanes;

		std::cout << "Translate 0 0 " << -max_dim << "\n";

		// Rotate X (wobble)
		std::cout << "Rotate \"X\" " << 60.0 * sin(float(frame_count) / WOBBLE_PERIOD * 2.0*M_PI) << "\n";
		// Rotate Y (spin)
		std::cout << "Rotate \"Y\" " << float(frame_count) / SPIN_PERIOD * 360 << "\n";
		// Rotate (orient)
		std::cout << "Rotate \"X\" -90\n";

		// Translate (center)
		std::cout << "Translate "
			       << -(float)(ncols-1)   / 2.0
			<< " " << -(float)(nrows-1)   / 2.0
			<< " " << -(float)(nplanes-1) / 2.0 << "\n";

		// Frame
		/*
		std::cout << "Color 1.0 0.0 0.0\n";
		std::cout << "Line 0 0 0  " 
		<< (ncols-1) << " 0 0\n";
		std::cout << "Line 0 " << (nrows-1) << " 0  " 
		<< (ncols-1) << " " << (nrows-1) << " 0\n";
		std::cout << "Line 0 0 " << (nplanes-1) << "  "
		<< (ncols-1) << " 0 " << (nplanes-1) << "\n";
		std::cout << "Line 0 " << (nrows-1) << " " << (nplanes-1) << "  "
		<< (ncols-1) << " " << (nrows-1) << " " << (nplanes-1) << "\n";

		std::cout << "Color 0.0 1.0 0.0\n";
		std::cout << "Line 0 0 0  "
		<< "0 " << (nrows-1) << " 0\n";
		std::cout << "Line " << (ncols-1) << " 0 0  "
		<< (ncols-1) << " " << (nrows-1) << " 0\n";
		std::cout << "Line 0 0 " << (nplanes-1) << "  "
		<< "0 " << (nrows-1) << " " << (nplanes-1) << "\n";
		std::cout << "Line " << (ncols-1) << " 0 " << (nplanes-1) << "  "
		<< (ncols-1) << " " << (nrows-1) << " " << (nplanes-1) << "\n";

		std::cout << "Color 0.0 0.0 1.0\n";
		std::cout << "Line 0 0 0  "
		<< "0 0 " << (nplanes-1) << "\n";
		std::cout << "Line " << (ncols-1) << " 0 0  "
		<< (ncols-1) << " 0 " << (nplanes-1) << "\n";
		std::cout << "Line 0 " << (nrows-1) << " 0  "
		<< "0 " << (nrows-1) << " " << (nplanes-1) << "\n";
		std::cout << "Line " << (ncols-1) << " " << (nrows-1) << " 0  "
		<< (ncols-1) << " " << (nrows-1) << " " << (nplanes-1) << "\n";
		*/
		/**/
		std::cout << "LineSet \"PC\"\n";
		std::cout << "24 12\n";
		std::cout << "0 0 0 0.3 0 0\n";
		std::cout << (ncols-1) << " 0 0 1 0 0\n";
		std::cout << "0 " << (nrows-1) << " 0 0.3 0 0\n";
		std::cout << (ncols-1) << " " << (nrows-1) << " 0 1 0 0\n";
		std::cout << "0 0 " << (nplanes-1) << " 0.3 0 0\n";
		std::cout << (ncols-1) << " 0 " << (nplanes-1) << " 1 0 0\n";
		std::cout << "0 " << (nrows-1) << " " << (nplanes-1) << " 0.3 0 0\n";
		std::cout << (ncols-1) << " " << (nrows-1) << " " << (nplanes-1) << " 1 0 0\n";
		std::cout << "0 0 0 0 0.3 0\n";
		std::cout << "0 " << (nrows-1) << " 0 0 1 0\n";
		std::cout << (ncols-1) << " 0 0 0 0.3 0\n";
		std::cout << (ncols-1) << " " << (nrows-1) << " 0 0 1 0\n";
		std::cout << "0 0 " << (nplanes-1) << " 0 0.3 0\n";
		std::cout << "0 " << (nrows-1) << " " << (nplanes-1) << " 0 1 0\n";
		std::cout << (ncols-1) << " 0 " << (nplanes-1) << " 0 0.3 0\n";
		std::cout << (ncols-1) << " " << (nrows-1) << " " << (nplanes-1) << " 0 1 0\n";
		std::cout << "0 0 0 0 0 0.3\n";
		std::cout << "0 0 " << (nplanes-1) << " 0 0 1\n";
		std::cout << (ncols-1) << " 0 0 0 0 0.3\n";
		std::cout << (ncols-1) << " 0 " << (nplanes-1) << " 0 0 1\n";
		std::cout << "0 " << (nrows-1) << " 0 0 0 0.3\n";
		std::cout << "0 " << (nrows-1) << " " << (nplanes-1) << " 0 0 1\n";
		std::cout << (ncols-1) << " " << (nrows-1) << " 0 0 0 0.3\n";
		std::cout << (ncols-1) << " " << (nrows-1) << " " << (nplanes-1) << " 0 0 1\n";
		std::cout << "0 1 -1\n";
		std::cout << "2 3 -1\n";
		std::cout << "4 5 -1\n";
		std::cout << "6 7 -1\n";
		std::cout << "8 9 -1\n";
		std::cout << "10 11 -1\n";
		std::cout << "12 13 -1\n";
		std::cout << "14 15 -1\n";
		std::cout << "16 17 -1\n";
		std::cout << "18 19 -1\n";
		std::cout << "20 21 -1\n";
		std::cout << "22 23 -1\n";
		//*/

		// Lighting 
		if (doubleside_flag)
			std::cout << "OptionBool \"DoubleSide\" on\n";

		//      std::cout << "FarLight 0 0 -1 1 1 1 1\n";
		std::cout << "FarLight 1 -1 -1  0.2 1 0.2 0.8\n";
		std::cout << "FarLight 0  1 -1  1 0.2 0.2 0.8\n";
		std::cout << "FarLight -1 -1 -1 0.2 0.2 1 0.8\n";

		std::cout << "Ka 0.1\n";
		std::cout << "Kd 0.9\n";

		// Data
		std::cout << "Color 1.0 1.0 1.0\n";
		std::cout << "ObjectInstance \"Data\"\n";

		// Restore
		std::cout << "Ka 1.0\n";
		std::cout << "Kd 0.0\n";

		std::cout << "WorldEnd\n";
		std::cout << "FrameEnd" << std::endl;

		//      sleep(1);
		// Need a better resolution delay
		struct timespec delay;
		delay.tv_sec = 0;
		delay.tv_nsec = DELAY_PER_FRAME;

		nanosleep(&delay, 0);
	}

	return 0;
}

