// This entire 3view script was originally written by Dr. Duffin at @ NIU
#include <iostream>
#include <fstream>


int usage(const char* argv0) {
	std::cerr << "Usage: " << argv0 << "   polyset_file\n";
	std::cerr << "   results are sent to the standard output and can be\n";
	std::cerr << "   redirected to a file. Using the polyset_file name of '-'\n";
	std::cerr << "   takes input from the standard input.\n";
	return 0;
}

constexpr char const* header[] = {
"Display  \"Object Test\"  \"Screen\" \"rgbdouble\"",
"",
"ObjectBegin \"Object\""
};

constexpr char const* tailer[] = {
"ObjectEnd",
"",
"CameraEye 0 0 100",
"CameraAt  0 0 0",
"CameraUp  0 1 0",
"CameraFOV 20",
"",
"Background 0.5 0.5 0.5",
"",
"WorldBegin",
"",
"FarLight 0 0 -1 1 1 1 0.8",
"FarLight -1 0 0 1 0 0 0.6",
"FarLight 0 -1 0 0 1 0 0.6",
"FarLight 1 0 0 0 1 1 0.6",
"FarLight 0 1 0 1 0 1 0.6",
"Ka 0.3",
"Kd 0.8",
"",
"XformPush",
"Translate -10 10 0",
"Rotate \"Z\" -90",
"Rotate \"Y\" -90",
"ObjectInstance \"Object\"",
"XformPop",
"",
"XformPush",
"Translate -10 -10 0",
"Rotate \"Z\" 180"
"Rotate \"X\" 90",
"ObjectInstance \"Object\"",
"XformPop",
"",
"XformPush",
"Translate 10 -10 0",
"ObjectInstance \"Object\"",
"XformPop",
"",
"XformPush",
"Translate 10 10 0",
"Rotate \"X\" 30",
"Rotate \"Y\" -30",
"Rotate \"Z\" -90",
"Rotate \"Y\" -90",
"ObjectInstance \"Object\"",
"XformPop",
"",
"WorldEnd"
};

int main(int argc, char* argv[]) {
	if (argc < 2)
		return usage(argv[0]);

	// Connect to correct stream
	std::ifstream inp_file;
	std::istream* inp = nullptr;
	if (argv[1][0] == '-' && argv[1][1] == '\0') {
		inp_file.open(argv[1], std::ios::binary);
		if (!inp_file) {
			std::cerr << "Can't open " << argv[1] << std::endl;
			return -1;
		}
		inp = & inp_file;
	} else {
		inp = & std::cin;
	}

	// Print out header
	for (size_t i = 0; i < sizeof(header)/sizeof(char*); i++)
		std::cout << header[i] << '\n';

	// Insert object
	char transfer = (char)inp->get();
	while (!inp->eof()) {
		std::cout.put(transfer);
		transfer = (char)inp->get();
	}

	// Close stream
	if (inp != & std::cin)
		inp_file.close();

	// Output tailer
	for (size_t i = 0; i < sizeof(tailer)/sizeof(char*); i++)
		std::cout << tailer[i] << '\n';
}
