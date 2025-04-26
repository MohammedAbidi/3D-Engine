#include <iostream>
#include <cmath> // sqrt, cmath

constexpr int BASE_HALF_LENGTH = 5;
constexpr int PARTICLES_PER_GRID_LENGTH = 5;
constexpr int GRASS_FACES = 5;

constexpr int TOTAL_PARTICLES = 4*BASE_HALF_LENGTH*BASE_HALF_LENGTH*PARTICLES_PER_GRID_LENGTH*PARTICLES_PER_GRID_LENGTH;


// https://en.wikipedia.org/wiki/Linear_congruential_generator
static constexpr int RAND_SEED_MAX = 0x7FFF;
static int rand_seed = 0;
int rand_lcg() {
	rand_seed = rand_seed * 0x343FD + 0x269EC3; // windows style
	return (rand_seed >> 16) & RAND_SEED_MAX;   // 0x7FFF
}

// https://graphics.cs.niu.edu/csci630/notes/random.pdf
float rand_gaussian() { // around -3 to 3
	const unsigned M = RAND_SEED_MAX; // RAND_MAX = 0x7FFF in windows (0x7FFFFFFF in glibc?)
	const unsigned n = 4;

	float sum = 0.f;
	for (unsigned i = 0; i < n; i++)
		sum += (float)rand_lcg();

	return (1.f/M) * sqrtf(12.f/n) * sum - sqrtf(3.f*n);
}

int main(void) {
	// Set up
	std::cout <<
	"Display \"Static Particle System\" \"Screen\" \"rgbdouble\"\n"
	"\n"
	"CameraFOV 25\n"
	"CameraEye "<<(BASE_HALF_LENGTH*3)<<" "<<(BASE_HALF_LENGTH*3)<<" "<<(BASE_HALF_LENGTH*3)<<"\n"
	"CameraAt  0 0 "<<(-BASE_HALF_LENGTH/10.f)<<"\n"
	"CameraUp  0 0 1\n"
	"Background .4 .6 .8\n"
	"\n"
	"WorldBegin\n"
	"OptionBool \"DoubleSide\" on\n"
	"FarLight -1 -2 -3  1 1 1  1  # Sun\n"
	"Ka .3\n"
	"Kd .7\n"
	"\n"
	"XformPush\n"
	"Color .4 .2 .0 # Brown\n"
	"Scale " << BASE_HALF_LENGTH+.3f << " " << BASE_HALF_LENGTH+.3f << " .25\n"
	"Translate 0 0 -1\n"
	"Cube # Disk 1 -1 360\n"
	"XformPop\n"
	"\n";

	// Loop
	for     (int xi = -BASE_HALF_LENGTH; xi < BASE_HALF_LENGTH; xi++) {
	    for (int yi = -BASE_HALF_LENGTH; yi < BASE_HALF_LENGTH; yi++) {
			// Grid Loop
			for     (float x = (float)xi; x < 1.f + (float)xi; x += (1.f / PARTICLES_PER_GRID_LENGTH)) {
			    for (float y = (float)yi; y < 1.f + (float)yi; y += (1.f / PARTICLES_PER_GRID_LENGTH)) {
					// Init
					float position[3];
					position[0] = x;
					position[1] = y;
					position[2] = 0;

					float velocity[3];
					velocity[0] = .03f*rand_gaussian(); // Spread out
					velocity[1] = .03f*rand_gaussian();
					velocity[2] = .03f*rand_gaussian() + 1.f; // Start Upward

					velocity[0] = std::copysign(.03f + std::abs(velocity[0]), velocity[0]); // Offset
					velocity[1] = std::copysign(.03f + std::abs(velocity[1]), velocity[1]);

					float direction[2]; // Remember original direction
					direction[0] = velocity[0];
					direction[1] = velocity[1];

					float acceleration[3];
					acceleration[0] = .02f*rand_gaussian(); // Stretch out
					acceleration[1] = .02f*rand_gaussian();
					acceleration[2] = -.25f; // Slowly fall

					float color[3]; // From messing around
					color[0] = .5f + .1f*rand_gaussian();
					color[1] = 1;
					color[2] = .5f * ((float) rand_lcg() / RAND_SEED_MAX);

					// Calculate
					constexpr int GRASS_VERTICES = GRASS_FACES*2 + 1;
					float vertices[GRASS_VERTICES][3];
					int faces[GRASS_FACES][4];

					// Init position (Base)
					vertices[GRASS_VERTICES-1][0] = position[0];
					vertices[GRASS_VERTICES-1][1] = position[1];
					vertices[GRASS_VERTICES-1][2] = position[2];
					for (int i = GRASS_VERTICES - 2; i > 0; i -= 2) {
						acceleration[0] += .01f*rand_gaussian();
						acceleration[1] += .01f*rand_gaussian();
						acceleration[2] += .01f*rand_gaussian();

						velocity[0] += acceleration[0];
						velocity[1] += acceleration[1];
						velocity[2] += acceleration[2];

						position[0] += velocity[0];
						position[1] += velocity[1];
						position[2] += velocity[2];

						float* prev = vertices[i+1];
						float* curr = vertices[i];
						float* next = vertices[i-1];

						// Grass thickness
						curr[0] = prev[0] - ((float)i/4.f)*direction[1]; // Grass bends counterclockwise
						curr[1] = prev[1] + ((float)i/4.f)*direction[0]; // perpendicular to direction
						curr[2] = prev[2];

						next[0] = position[0];
						next[1] = position[1];
						next[2] = position[2];
					}

					for (int i = 0; i < GRASS_FACES; i++) {
						int* f = faces[i];
						f[0] = i*2 - 1;
						f[1] = i*2 + 1;
						f[2] = i*2 + 2;
						f[3] = i*2;
					}
					faces[0][0] = 0; // The exception (connect tip corner)

					// Draw
					std::cout << "Color ";
					std::cout << color[0] << " ";
					std::cout << color[1] << " ";
					std::cout << color[2] << "\n";

					std::cout << "PolySet \"P\" "
					          << GRASS_VERTICES << " "
					          << GRASS_FACES    << "\n";

					for (int i = 0; i < GRASS_VERTICES; i++)
						std::cout << vertices[i][0] << " "
						         << vertices[i][1] << " "
						         << vertices[i][2] << "\n";

					for (int i = 0; i < GRASS_FACES; i++)
						std::cout << faces[i][0] << " "
						          << faces[i][1] << " "
						          << faces[i][2] << " "
						          << faces[i][3] << " -1\n";
				}
			}
		}
	}

	std::cout << "\nWorldEnd" << std::endl;
	return 0;
}
