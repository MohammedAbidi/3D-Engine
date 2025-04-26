#include <iostream>
#include <cmath>

constexpr float GRID_LENGTH = 100;
constexpr float const COLOR_SPECTRUM[][4] = { // r,g,b,t (t values MUST be in order)
    { .0f, .0f, .1f, .35f}, // Deep Water
    { .0f, .3f, .5f,  .5f}, // Water
    {.76f, .7f, .5f,  .5f}, // Sand
    { .0f, .5f, .0f,  .6f}, // Grass
    { .3f, .3f, .3f, .75f}, // Rock
    { .5f, .5f, .5f, .95f}, // Stone
    { 1.f, 1.f, 1.f,  1.f}, // Snow
};

constexpr void lerp_colors(float t, float out[3]) {
    if (t     <= COLOR_SPECTRUM[0][3]) {
        out[0] = COLOR_SPECTRUM[0][0];
        out[1] = COLOR_SPECTRUM[0][1];
        out[2] = COLOR_SPECTRUM[0][2];
        return;
    }

    constexpr int SIZE = sizeof(COLOR_SPECTRUM)/sizeof(COLOR_SPECTRUM[0]);
    if (t     >= COLOR_SPECTRUM[SIZE-1][3]) {
        out[0] = COLOR_SPECTRUM[SIZE-1][0];
        out[1] = COLOR_SPECTRUM[SIZE-1][1];
        out[2] = COLOR_SPECTRUM[SIZE-1][2];
        return;
    }

    for (int i = 1; i < SIZE; i++) {
        if (t < COLOR_SPECTRUM[i][3]) {
            t = (t - COLOR_SPECTRUM[i-1][3]) / (COLOR_SPECTRUM[i][3] - COLOR_SPECTRUM[i-1][3]);
            out[0] = COLOR_SPECTRUM[i-1][0]  + (COLOR_SPECTRUM[i][0] - COLOR_SPECTRUM[i-1][0]) * t;
            out[1] = COLOR_SPECTRUM[i-1][1]  + (COLOR_SPECTRUM[i][1] - COLOR_SPECTRUM[i-1][1]) * t;
            out[2] = COLOR_SPECTRUM[i-1][2]  + (COLOR_SPECTRUM[i][2] - COLOR_SPECTRUM[i-1][2]) * t;
            return;
        }
    }

    // Safeguard (Should never happen)
    out[0] = COLOR_SPECTRUM[SIZE-1][0];
    out[1] = COLOR_SPECTRUM[SIZE-1][1];
    out[2] = COLOR_SPECTRUM[SIZE-1][2];
}


// Custom rand functions
constexpr int RAND_SEED_MAX = 0x7FFF; // RAND_MAX = 0x7FFF in windows (0x7FFFFFFF in glibc?)
int rand_seed = 0;
int rand_lcg() {
    rand_seed = rand_seed * 0x343FD + 0x269EC3; // windows style
    return (rand_seed >> 16) & RAND_SEED_MAX;   // 0x7FFF
}

constexpr float rand_gaussian() { // around -3 to 3
    constexpr unsigned N = 4;
    const float MUL = (1.f/RAND_SEED_MAX) * sqrtf(12.f/N); // precalculated
    const float SUB = sqrtf(3.f*N);

    float sum = 0.0f;
    for (unsigned i = 0; i < N; i++)
        sum += (float)rand_lcg();

    return sum*MUL - SUB;
//  return (1.f/RAND_SEED_MAX) * sqrtf(12.f/N) * sum - sqrtf(3.f*N);
}


// Slightly modified from code provided by Richard F. Voss
// as found in The Science of Fractal Images
// by Barnsley, Devaney, Mandelbrot, Peitgen, Saupe, and Voss, pages 100–101.
constexpr float f3(float delta, float x0, float x1, float x2) {
    return (x0 + x1 + x2) / 3.f + delta * rand_gaussian();
}

constexpr float f4(float delta, float x0, float x1, float x2, float x3) {
    return (x0 + x1 + x2 + x3) / 4.f + delta * rand_gaussian();
}

void midPointFM2D(float* X, int maxlevel, float delta, float H, int seed) {
    rand_seed = seed;
    int N = 1 << maxlevel; // 2^maxlevel
    const int I = N+1; // for indexing

    /* set the initial random corners */
    X[0      ] = delta * rand_gaussian();
    X[      N] = delta * rand_gaussian();
    X[I*N    ] = delta * rand_gaussian();
    X[I*N + N] = delta * rand_gaussian();

    int D = N;
    int d = N/2;

    for (int stage = 0; stage < maxlevel; stage++) {
        /* going from grid type I to type II */
        delta *= powf(0.5f, 0.5f*H);

        /* interpolate and offset mid points */
        for     (int x = d; x <= N-d; x += D) {
            for (int y = d; y <= N-d; y += D) {
                X[I*x + y] = f4(delta, X[I*(x+d) + (y+d)], X[I*(x+d) + (y-d)],
                                       X[I*(x-d) + (y+d)], X[I*(x-d) + (y-d)]);
            }
        }

        /* displace existing points */
        for     (int x = 0; x <= N; x += D) {
            for (int y = 0; y <= N; y += D) {
                X[I*x + y] += delta * rand_gaussian();
            }
        }
        

        /* going from grid type II to type I */
        delta *= powf(0.5f, 0.5f*H);

        /* interpolate and offset mid points at boundary */
        for (int x = d; x <= N-d; x += D) {
            X[I*x    ] = f3(delta, X[I*(x+d)    ], X[I*(x-d)    ], X[I*x + (d)]);
            X[I*x + N] = f3(delta, X[I*(x+d) + N], X[I*(x-d) + N], X[I*x + (N-d)]);
            X[      x] = f3(delta, X[      (x+d)], X[      (x-d)], X[I*d + x]);
            X[I*N + x] = f3(delta, X[I*N + (x+d)], X[I*N + (x-d)], X[I*(N-d) + x]);
        }

        /* interpolate and offset mid points in interior */
        for     (int x = d; x <= N-d; x += D) {
            for (int y = D; y <= N-d; y += D) {
                X[I*x + y] = f4(delta, X[I*x + (y+d)], X[I*x + (y-d)],
                                       X[I*(x+d) + y], X[I*(x-d) + y]);
            }
        }

        for     (int x = D; x <= N-d; x += D) {
            for (int y = d; y <= N-d; y += D) {
                X[I*x + y] = f4(delta, X[I*x + (y+d)], X[I*x + (y-d)],
                                       X[I*(x+d) + y], X[I*(x-d) + y]);
            }
        }

        /* displace existing points */
        for     (int x = 0; x <= N; x += D) {
            for (int y = 0; y <= N; y += D) {
                X[I*x + y] += delta * rand_gaussian();
            }
        }

        for     (int x = d; x <= N-d; x += D) {
            for (int y = d; y <= N-d; y += D) {
                X[I*x + y] += delta * rand_gaussian();
            }
        }

        /* prepare for next level */
        D = D/2;
        d = d/2;
    }
}


int main(int argc, const char * argv[]) {
    // Standard Setup
    if (argc <= 1) {
        std::cerr <<
            "Usage: " << argv[0] << " [-p] n D s sigma\n"
            "  [-p] flag: only output the polyset, without a scene around it\n"
            "    n  which determines the size of the grid, 2n+1 values on a side. (default: 6)\n"
            "    D  the fractal dimension of the terrain which should be between 2.0 and 3.0 inclusive. (default: 2.2, optional)\n"
            "    s  a random number seed. (default: 68, optional)\n"
            "sigma  the initial standard deviation. (default: 10.0, optional)\n"
        << std::endl;
        return 0;
    }

    // Variable Arguments
    bool  only_output_polyset = false;
    int   n = 6;
    float D = 2.2f;
    int   s = 68;
    float sigma = 10.0f;

    int argi = 1;
    if (argv[1][0] == '-' && argv[1][1] == 'p' && argv[1][2] == '\0') {
        only_output_polyset = true;
        argi++;
    }

    char* endptr;
    n = (int) strtol(argv[argi], &endptr, 10);
    if (*endptr != '\0' || n < 0) {
        std::cerr << "Invalid n (First argument must be positive): " << argv[argi] << std::endl;
        return 1;
    }

    if (argc > 2) {
        argi++;
        D = strtof(argv[argi], &endptr);
        if (*endptr != '\0') {
            std::cerr << "Invalid D (fractal dimension): " << argv[argi] << std::endl;
            return 1;
        }
    }

    if (argc > 3) {
        argi++;
        s = (int) strtol(argv[argi], &endptr, 10);
        if (*endptr != '\0') {
            std::cerr << "Invalid s (seed): " << argv[argi] << std::endl;
            return 1;
        }
    }

    if (argc > 4) {
        argi++;
        sigma = strtof(argv[argi], &endptr);
        if (*endptr != '\0') {
            std::cerr << "Invalid sigma: " << argv[argi] << std::endl;
            return 1;
        }
    }

    // Set Up
    const int N = (1 << n); // 2^n  (N = 2^maxlevel)
    const int vertices_count = (N+1)*(N+1);
    const int faces_count    = N*N*2;

    // Generate Terrain Map
    float* grid = new float[vertices_count];
    midPointFM2D(grid, n, sigma, 3.f-D, s);

    // Print
    if (!only_output_polyset) {
        std::cout <<
            "Display \"Fractal Terrain\" \"Screen\" \"rgbdouble\"\n"
            // "Display \"Fractal Terrain\" \"PNM\" \"rgb\"\n"
            // "Format 1080 720\n"
            // "Format 4096 2160\n" // HD
            "\n"
            "CameraFOV 6\n"
            "CameraEye "<<(-GRID_LENGTH*5)<<" "<<(-GRID_LENGTH*5)<<" "<<(GRID_LENGTH*2)<<"\n"
            "CameraAt  0 0 0\n"
            "CameraUp  0 0 1\n"
            "Background .4 .6 .8\n"
            "\n"
            "WorldBegin\n"
            // "OptionBool \"DoubleSide\" on\n"
            // "FarLight -1 -2 -3  1 1 1  1  # Sun\n"
            "FarLight -1 1 1  1 1 1  1\n" // looks better when coming from below?
            // "Surface \"plastic\""
            "Ka .6\n"
            "Kd .4\n"
            // "Ks .4\n"
            // "\n"
            // "XformPush\n"
            // "Scale 50 50 1\n"
            // "Cube\n"
            // "XformPop\n"
            "\n";
    }

    std::cout << "PolySet \"PC\"\n"
              << vertices_count << " # Vertices\n" // optional
              << faces_count    << " # Faces\n";   // comments

    // Vertices
    if (sigma == 0) sigma = 1; // for prevent division by 0 for colors (this will make t = .5 throughout)
    for(int i=0, xi = 0; xi <= N; xi++) {
        for (int yi = 0; yi <= N; yi++, i++) {
            // Interpolate xy to GRID_LENGTH
            static constexpr float a = GRID_LENGTH/2;
            static constexpr float b = -a;
            const float x = (b + (a-b) * ((float)xi/(float)(N+1)));
            const float y = (b + (a-b) * ((float)yi/(float)(N+1)));
            const float z = grid[i]; // Precalculated

            // Spaced out (personal preference)
            // std::cout << '\n'
            //     << std::setw(11) << std::left << x << ' '
            //     << std::setw(11) << std::left << y << ' '
            //     << std::setw(11) << std::left << z << ' ';
            std::cout << '\n' << x << ' ' << y << ' ' << z << ' ';

            // Color (Interpolated Spectrum)
            float t = (z + sigma*2) / (sigma*4);
            float rgb[3];
            lerp_colors(t, rgb);
            std::cout << rgb[0] << ' ' << rgb[1] << ' ' << rgb[2];
        }
    }
    delete[] grid; // Clean up

    // Faces
    std::cout << "\n\n";
    for(int i=0, x = 0; x < N; x++, i++) {
        for (int y = 0; y < N; y++, i++) {
            std::cout << i
               << ' ' << i + 1
               << ' ' << i + N+1
           << " -1\n" << i + 1
               << ' ' << i + N+1 + 1
               << ' ' << i + N+1
           << " -1\n";
        }
    }

    if (!only_output_polyset) {
        std::cout << "\nWorldEnd" << std::endl;
    }

    // std::cerr << "n     = " << n << '\n';
    // std::cerr << "D     = " << D << '\n';
    // std::cerr << "s     = " << s << '\n';
    // std::cerr << "sigma = " << sigma << '\n';

    return 0;
}
