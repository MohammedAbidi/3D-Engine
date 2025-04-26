#include <iostream>
#include <cmath>   // sqrt
#include <ctime> // for looping

constexpr int TOTAL_PARTICLES = 400;
constexpr int SPAWN_RATE = 10; // per frame


// https://en.wikipedia.org/wiki/Linear_congruential_generator
static constexpr int RAND_SEED_MAX = 0x7FFF;
static int rand_seed = 0;
int rand_lcg() {
	// unsigned int a = 1664525;
	// unsigned int c = 1013904223;
	// unsigned int m = 4294967296; // 2^32
	// rand_seed = (a * rand_seed + c) % m;
	// return rand_seed / m;

	// rand_seed = (rand_seed << 13) ^ rand_seed;
	// return ((rand_seed * (rand_seed*rand_seed*15731 + 789221) + 1376312589) & 0x7FFFFFFF);

//  rand_seed = rand_seed * 1103515245 + 12345; // glibc style?
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


struct Particle {
	float position[3];
	float velocity[3];
	float color[3];    // for fun
	float prev_pos[3]; // for line drawing
};

constexpr float GRAVITY = -9.8f; // downward acceleration of 9.8 m/s^2
constexpr float UPPER_BOUNDS[3] = { 1000, 1000, 1000};
constexpr float LOWER_BOUNDS[3] = {-1000,-1000,-1000};
Particle particle_create() {
	Particle p = {}; // everything else 0

	p.velocity[0] = rand_gaussian() * 10;
	p.velocity[1] = rand_gaussian() * 10;
	p.velocity[2] = rand_gaussian() * 10 + 100; // Shoot up

	p.color[0] = 0; //(float) rand_lcg() / RAND_SEED_MAX;
	p.color[1] = (float) rand_lcg() / RAND_SEED_MAX;
	p.color[2] = 1; //(float) rand_lcg() / RAND_SEED_MAX;

	return p;
}

bool particle_update(Particle& p) {
	p.velocity[2] += GRAVITY;

	p.prev_pos[0] = p.position[0];
	p.prev_pos[1] = p.position[1];
	p.prev_pos[2] = p.position[2];

	p.position[0] += p.velocity[0];
	p.position[1] += p.velocity[1];
	p.position[2] += p.velocity[2];

	return LOWER_BOUNDS[0] < p.prev_pos[0]&&p.prev_pos[0] < UPPER_BOUNDS[0]
		&& LOWER_BOUNDS[1] < p.prev_pos[1]&&p.prev_pos[1] < UPPER_BOUNDS[1]
		&& LOWER_BOUNDS[2] < p.prev_pos[2]&&p.prev_pos[2] < UPPER_BOUNDS[2];
}


int main(void) {
	// Init
	bool doubleside_flag = true;
	unsigned frame_count = 0;
	std::cout << "Display \"Dynamic Particle System\" \"Screen\" \"rgbdouble\"\n";
	// std::cout << "Display \"Dynamic Particle System\" \"PNM\" \"rgb\"\n";
	std::cout << "CameraFOV 50\n";
	std::cout << "CameraEye 1000 0 400\n";
	std::cout << "CameraAt  0 0 0\n";
	std::cout << "CameraUp  0 0 1\n";
	std::cout << "Background .4 .5 .6\n";
	// std::cout << "Background .4 .6 .8\n";

	int particles_count = 0;
	Particle particles[TOTAL_PARTICLES] = {};

	// Loop
	while (true) {
		// Update Particles
		for (int i = 0; i < particles_count; i++) {
			Particle& p = particles[i];
			bool is_in_bounds = particle_update(p);

			// backwards remove
			if (!is_in_bounds) {
				particles_count--;
				particles[i] = particles[particles_count];
				i--; // to check the back
			}
		}

		for (size_t i = 0; i < SPAWN_RATE; i++)
		if (particles_count < TOTAL_PARTICLES)
			particles[particles_count++] = particle_create();


		// Draw
		std::cout << "FrameBegin " << frame_count++ << '\n';
		std::cout << "WorldBegin\n";

		if(doubleside_flag)
			std::cout << "OptionBool \"DoubleSide\" on\n";

		std::cout << "Rotate \"Z\" " << float(frame_count) * 5 << "\n";
		std::cout << "Translate 0 -30 -100 \n";
		std::cout << "Rotate \"X\" 3\n";

		std::cout << "FarLight  -1 -1 -1  1 1 1  1\n";
		std::cout << "Ka 0.7\n";
		std::cout << "Kd 0.3\n";
		std::cout << "XformPush\n";
		std::cout << "Color 0 .5 0\n";
		std::cout << "Translate 0 0 -900 \n";
		std::cout << "Scale 30 30 1000 \n";
		std::cout << "Cylinder 1 -1 1 360 \n";
		std::cout << "XformPop\n";
		std::cout << "Ka 1\n";
		std::cout << "Kd 0\n";

		for (int i = 0; i < particles_count; i++) {
			Particle& p = particles[i];
			std::cout << "Color ";
			std::cout << p.color[0] << " ";
			std::cout << p.color[1] << " ";
			std::cout << p.color[2] << "\n";

			std::cout << "Line ";
			std::cout << p.prev_pos[0] << " ";
			std::cout << p.prev_pos[1] << " ";
			std::cout << p.prev_pos[2] << " ";
			std::cout << p.position[0] << " ";
			std::cout << p.position[1] << " ";
			std::cout << p.position[2] << "\n";
		}

		std::cout << "WorldEnd\n";
		std::cout << "FrameEnd" << std::endl;

		// Better sleep (idea from Dr. Duffin @ NIU)
		struct timespec delay;
		delay.tv_sec = 0;
		delay.tv_nsec = 20000000; // nanosec
		nanosleep(&delay, 0);
	}

	return 0;
}

