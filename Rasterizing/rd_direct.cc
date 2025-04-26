#include "rd_direct.h"
#include "rd_error.h"
#include "rd_display.h"

#include <cstring> // for memcpy
#include <iostream> // for debugging



// Shaders
// Each shader needs to determine where the surface color and surface normal information are coming from. 
// If the global interpolation flag is on and the appropriate vertex flag is on, then the information can be found in the surface_point_values variable.
// Otherwise, it is found in the separate global variables which are constant for the polygon.
void clamp_down3(float a[3], float n) { // For clamping colors
    a[0] = a[0] > n ? n : a[0];
    a[1] = a[1] > n ? n : a[1];
    a[2] = a[2] > n ? n : a[2];
}

Vector3 get_surface_normal() {
    // Get World Space normal from surface point (if interpolating, otherwise flat shading)
    if (vertex_interpolation_flag && _vertex_normal_flag)
        return Vector3(surface_point_values.coord[ATTR_NX],
                       surface_point_values.coord[ATTR_NY],
                       surface_point_values.coord[ATTR_NZ]);
    else
        return polygon_normal;
}

Vector3 get_surface_position() {
    // Get World Space poition from surface point
    return Vector3(surface_point_values.coord[ATTR_WORLD_X],
                   surface_point_values.coord[ATTR_WORLD_Y],
                   surface_point_values.coord[ATTR_WORLD_Z]);
}

// ambience = ambient_coefficient * ambient_light[3];
void get_ambience(float return_color[3]) {
    return_color[0] = ambient_coefficient * ambient_light[0];
    return_color[1] = ambient_coefficient * ambient_light[1];
    return_color[2] = ambient_coefficient * ambient_light[2];
}

// diffusion = lots of light loops
void get_diffusion(float return_color[3]) {
    // Clear
    return_color[0] = 0;
    return_color[1] = 0;
    return_color[2] = 0;

    // Normal and Surface Position
    const Vector3 normal = get_surface_normal();
    const Vector3 surface_position = get_surface_position();

    // Far Lights
    for (light_data& far_light : far_lights) {
        // Calculate and check angle
        float angle = normal.dot(-far_light.xyz); // Angle (negative so it's backward to do dot product)
        if (angle <= 0) continue; // from 1 to -1, if less than 0 then facing away
    
        // Accumulate
        return_color[0] += angle * far_light.rgb[0]; 
        return_color[1] += angle * far_light.rgb[1];
        return_color[2] += angle * far_light.rgb[2];
    }

    // Point Lights
    for (light_data& point_light : point_lights) {
        // Get Direction
        Vector3 light = point_light.xyz - surface_position; // L = light_pos - surface_pos // Inverse with Light distance and direction

        // Calculate and check angle
        float angle = normal.dot(light.normalize()); // Normalize (inverse direction)
        if (angle <= 0) continue;
        
        // Accumulate with intensity (angle * (1.f / light.magnitude2()))
        float I = angle / light.magnitude2(); // Intensity (1/r^2) because point light strength fades
        return_color[0] += I * point_light.rgb[0];
        return_color[1] += I * point_light.rgb[1];
        return_color[2] += I * point_light.rgb[2];
    }
    
    // Multiply by diffusion coeffiecient
    return_color[0] *= diffuse_coefficient;
    return_color[1] *= diffuse_coefficient;
    return_color[2] *= diffuse_coefficient;
}


// Reflects L on normal N and returns R (BOTH MUST BE NORMAL VECTORS)
Vector3 reflect(Vector3 L, Vector3 N) {
    float angle = N.dot(L);
    if (angle >= 0.f) angle = 1.f; // I dont know why this works, derived from me messing around trying to fix a spaceship and now it's stuck (I'd fix it but it's finals week and I have to write like 10 pages of stuff)
    return (2*angle*N - L).normalize(); // R = 2*(N.dot(L))*N - L; // Reflection
}

#include <math.h> // only for std::pow() in specular shading
void get_specular(float return_color[3]) {
    // Clear
    return_color[0] = 0;
    return_color[1] = 0;
    return_color[2] = 0;
    
    // Normal and Surface Position
    const Vector3 normal = get_surface_normal();
    const Vector3 surface_position = get_surface_position();
    const Vector3 view_direction = (surface_position - camera_eye_position).normalize();

    // Far Lights
    for (light_data& far_light : far_lights) {
        Vector3 reflection_direction = reflect(far_light.xyz, normal);
        float angle = view_direction.dot(reflection_direction);
        if (angle <= 0.f) continue; // Check angle
        
        // Calculate
        float I = pow(angle, specular_exponent); // Intensity
        return_color[0] += I * far_light.rgb[0]; // Accumulate
        return_color[1] += I * far_light.rgb[1];
        return_color[2] += I * far_light.rgb[2];
    }
    
    // Point Lights
    for (light_data& point_light : point_lights) {
        // Calculate Direction
        Vector3 light = (surface_position - point_light.xyz); // Inverse Light distance and direction
        Vector3 light_direction = light.normalize(); // Normalize (direction)

        // Calculate angle
        Vector3 reflection_direction = reflect(light_direction, normal);
        float angle = view_direction.dot(reflection_direction);
        if (angle <= 0.0f) continue;

        // Calculate
        float I = (1.f / light.magnitude2()) * pow(angle, specular_exponent); // Intensity (1/r^2) * (angle^exp)
        return_color[0] += I * point_light.rgb[0]; // Accumulate
        return_color[1] += I * point_light.rgb[1];
        return_color[2] += I * point_light.rgb[2];
    }
    
    // Multiply by specular coefficient
    return_color[0] *= specular_coefficient;
    return_color[1] *= specular_coefficient;
    return_color[2] *= specular_coefficient;
};

void matte(float color[3]) {
    // Ambience and Diffusion (calculations stored in the float[3] arrays)
    float ambience[3]; get_ambience(ambience);
    float diffuse[3];  get_diffusion(diffuse);

    // matte = surface_color * (Ka * ambience  +  Kd * diffuse(normal))
    color[0] = surface_color[0] * (ambience[0] + diffuse[0]); // Ci = Os * Cs * (Ka * ambient() + Kd * diffuse(Nf))
    color[1] = surface_color[1] * (ambience[1] + diffuse[1]); // I think Os is surface opacity, but we're not using it right now
    color[2] = surface_color[2] * (ambience[2] + diffuse[2]); // Note: I = Ka + Kd + Ks // I = ambient_coefficient + diffuse_coefficient + specular_coefficient

    // Clamp and return
    clamp_down3(color, 1.f); // Clamped Down (inf:0 -> 1:0)
}

void metal(float color[3]) { // Same as matte, but with specular instead
    // Ambience and Specular
    float ambience[3]; get_ambience(ambience);
    float specular[3]; get_specular(specular);

    // metal = surface_color * (Ka * ambience  +  Kd * specular(normal))
    color[0] = surface_color[0] * (ambience[0] + specular[0]);
    color[1] = surface_color[1] * (ambience[1] + specular[1]);
    color[2] = surface_color[2] * (ambience[2] + specular[2]);
     
    // Clamp and return
    clamp_down3(color, 1.f);
}

void plastic(float color[3]) {
    // Ambience, Diffusion, and Specular
    float ambience[3]; get_ambience(ambience);
    float diffuse[3];  get_diffusion(diffuse);
    float specular[3]; get_specular(specular);

    // plastic = surface_color  * (Ka * ambience  +  Kd * diffuse(normal))
    //         + specular_color *  Ks * specular(normal)
    color[0] = surface_color[0] * (ambience[0] + diffuse[0]) + specular_color[0] * specular[0];
    color[1] = surface_color[1] * (ambience[1] + diffuse[1]) + specular_color[1] * specular[1];
    color[2] = surface_color[2] * (ambience[2] + diffuse[2]) + specular_color[2] * specular[2];

    clamp_down3(color, 1.f); // Clamp and return
}




  //////////////
 // REDirect //
//////////////
int REDirect::rd_render_init() {
    // Default values are usually set here
    current_color[0] = 1.0f; // Default writing color of white
    current_color[1] = 1.0f;
    current_color[2] = 1.0f;

    camera_fov  = 90.0f;  // Camera field of view in degrees (Default is 90)
    camera_near = 1.0f;   // Camera near clipping depth (Default is 1)
    camera_far  = 1.0e9f; // Camera far clipping depth (Default is 1 billion)

    camera_eye_position     = Vector3(0, 0, 0); // A Vector3 treated like a point for the camera eyepoint (Default is 0,0,0)
    camera_look_at_position = Vector3(0, 0,-1); // A Vector3 treated like a point for the camera eyepoint (Default is 0,0,-1) (Note: 0,0,0 to 0,0,-1 means it's looking down)
    camera_up_direction     = Vector3(0, 1, 0); // A Vector3 for the camera up vector (Default is 0,1,0)

    // An ambient coefficient, a diffuse coefficient, and a specular coefficient. Default values should be 1, 0, and 0 respectively.
    ambient_coefficient  = 1.0f;
    diffuse_coefficient  = 0.0f;
    specular_coefficient = 0.0f;

    // Default colors of white, with no point/directional lights
    ambient_light[0] = 1.0f;
    ambient_light[1] = 1.0f;
    ambient_light[2] = 1.0f;
    surface_color[0] = 1.0f;
    surface_color[1] = 1.0f;
    surface_color[2] = 1.0f;
    specular_color[0] = 1.0f;
    specular_color[1] = 1.0f;
    specular_color[2] = 1.0f;
    specular_exponent = 10.0f; // The default value is 10.0

    _vertex_color_flag = false;
    _vertex_normal_flag = false;
    _vertex_texture_flag = false;
    vertex_interpolation_flag = true;

    viewing_vector = Vector3(0,0,1); // ???
    polygon_normal = Vector3(0,0,1); // ???

    point_lights.clear();
    far_lights.clear();

    surface_shader = &matte; // Set class function pointer default to matte

    std::cout << std::endl;  // I dont know why, but if this is also removed then random data is hit with immutable garbage and nothing works (maybe b/c of std::endl's flushing idk)

    return RD_OK;
}

int REDirect::rd_display(const string& name, const string& type, const string& mode) {
    // rd_set_background(current_color); // If u want to set a default background, earliest might be here
    return RD_OK;
}

int REDirect::rd_format(int xresolution, int yresolution) {
    return RD_OK;
}

int REDirect::rd_frame_begin(int frame_no) {
    frame_number = frame_no;
    return RD_OK;
}

//  clip_to_device_transform * (camera_to_clip * (world_to_camera * (object_to_world * point))) // Intended
//  clip_to_device_transform * ((camera_to_clip * world_to_camera) * (object_to_world * point)) // Actual?
int REDirect::rd_world_begin() {
    rd_disp_init_frame(frame_number);

    // The current transform is set to the identity.
    current_transform = Matrix4::identity();
    // std::cout << "object to world:\n" << current_transform << std::endl;

    // The world to camera transformation is be computed using 
    // the camera eyepoint, look at point, and up vector.
    Matrix4 world_to_camera = Matrix4::world_to_camera(camera_eye_position, camera_look_at_position, camera_up_direction);
    // std::cout << "world to camera:\n" << world_to_camera << std::endl;

    // The camera to clipping coordinate transformation matrix can be computed using 
    // the near and far clipping depths and the field of view.
    Matrix4 camera_to_clip = Matrix4::camera_to_clip(camera_fov, camera_near, camera_far, (float)display_xSize/display_ySize);
    // std::cout << "camera to clip:\n" << camera_to_clip << std::endl;

    // These last two transformations are combined and stored as the world to clipping coordinate matrix.
    world_to_clip_transform = camera_to_clip.multiply(world_to_camera);
    // std::cout << "world to clip combo:\n" << world_to_clip_transform << std::endl;

    // The clipping coordinate to device coordinate transform is also computed here.
    clip_to_device_transform = Matrix4::clip_to_device(display_xSize, display_ySize);
    // std::cout << "clip to device:\n" << clip_to_device_transform << std::endl;

    // New empty transformation stack
    stack = std::stack<Matrix4>();

    // Initialize new depth buffer with all values set to 1.0f
    z_buffer = vector<float>(display_xSize*display_ySize, 1.0f);

    // Dynamically initialize edge_table for scan_conversion();
    edge_table = new Edge[display_ySize];

    // Normal transform matrix
    normal_transform = Matrix4::identity();

    return RD_OK;
}

int REDirect::rd_world_end() {
    delete[] edge_table;
    stack = std::stack<Matrix4>(); // stack.clear() via empty initialization
    z_buffer.clear();
    point_lights.clear();
    far_lights.clear();
    return rd_disp_end_frame();
}

int REDirect::rd_frame_end() {
    return RD_OK;
}

int REDirect::rd_render_cleanup() {
    return RD_OK;
}


// Stores the current color values to be used as the drawing color.
int REDirect::rd_color(const float color[]) {
    // Store new color values
    current_color[0] = color[0];
    current_color[1] = color[1];
    current_color[2] = color[2];

    surface_color[0] = color[0];
    surface_color[1] = color[1];
    surface_color[2] = color[2];

    // std::copy(color, color + 3, current_color);
    // std::copy(std::begin(color), std::end(color), std::begin(current_color));
    return RD_OK;
}

// Sets the color values to be used as the screen background color.
int REDirect::rd_background(const float color[]) {
    return rd_set_background(color);
}

// Plots a point using the current_color
// x and y are screen coordinates, z is the screen 'depth' from 0.0 to 1.0, with 0 being the closest
// True if plotted, else false and nothing happened
bool REDirect::plot(int x, int y, float z, const float color[3]) {
    // std::cout << x << ", " << y << ", " << z << std::endl;

    // Check z_buffer to see if this point is behind another and return early if so (0.0 is close, 1.0 is far)
    if (z > z_buffer[y*display_xSize + x])
        return false;

    // This point is closer, save and draw it (or over a 'farther' point)
    z_buffer[y*display_xSize + x] = z;
    rd_write_pixel(x, y, color);
    return true;
}

void REDirect::point_pipeline(float x, float y, float z) {
    Point4 p = Point4(x, y, z); // Point "Object"

    current_transform.multiply_mutate(p); // Object to World

    // Lighting happens here (WIP)

    world_to_clip_transform.multiply_mutate(p); // World to Camera AND Camera to Clip

    // Point clipping
    if (p.x < 0 || 0 > p.w-p.x ||
        p.y < 0 || 0 > p.w-p.y ||
        p.z < 0 || 0 > p.w-p.z)
        return; // Out of bounds
    
    clip_to_device_transform.multiply_mutate(p); // Clip to Device

    plot(p.x/p.w, p.y/p.w, p.z/p.w, current_color); // Draw with z buffer
}

// Takes a point, turns it into a homogeneous point and passes it to the point pipeline.
// clip_to_device_transform * world_to_clip_transform * current_transform * point (from right to left)
int REDirect::rd_point(const float p[3]) {
    point_pipeline(p[0], p[1], p[2]);
    return RD_OK;
    // return rd_write_pixel(p[0], p[1], current_color);
}


// (2D) Helper function that draws a line using the current drawing color.
// Unrolled to 8 specific versions to prioritize speed over initial storage (atleast 8x larger?)
void bresenham_line_algorithm(int x1, int y1, int x2, int y2, const float current_color[3]) {
    int dx = x2 - x1;
    int dy = y2 - y1;

    if (dy > 0) { // Top Quadrant
        if (dx > 0) { // Top-Right Quadrant
            if (dx > dy) { // 0 to 45 degrees (right-up)
                const int B = 2*dy;
                const int A = B - 2*dx;

                int p = B - dx; // Initial decision value (2dy - dx)
                while (x1 <= x2) {
                    rd_write_pixel(x1++, y1, current_color);
                    if (p > 0) { // Arbitrary split decision > or <
                        y1++;
                        p += A; // p += 2dy - 2dx
                    } else
                        p += B; // p += 2dy
                }
            } else { // 45 to 90 degrees (up-right)
                // Swap x/y
                const int B = 2*dx;
                const int A = B - 2*dy;

                int p = B - dy;
                while (y1 <= y2) {
                    rd_write_pixel(x1, y1++, current_color);
                    if (p > 0) {
                        p += A;
                        x1++;
                    } else
                        p += B;
                }
            }
        } else { // Top-Left Quadrant
            dx = -dx;
            if (dy > dx) { // 90 to 135 degrees (up-left)
                // Swap x/y
                const int B = 2*dx;
                const int A = B - 2*dy;

                int p = B - dy;
                while (y1 <= y2) {
                    rd_write_pixel(x1, y1++, current_color);
                    if (p > 0) {
                        x1--; // Decrement
                        p += A;
                    } else
                        p += B;
                }
            } else { // 135 to 180 degrees (left-up)
                const int B = 2*dy;
                const int A = B - 2*dx;

                int p = B - dx;
                while (x1 >= x2) {
                    rd_write_pixel(x1--, y1, current_color); // Decrement
                    if (p > 0) {
                        y1++;
                        p += A;
                    } else
                        p += B;
                }
            }
        }
    } else { // Bottom Quadrant
        dy = -dy;
        if (dx > 0) { // Bottom-Right Quadrant
            if (dx > dy) { // 315 to 360 degrees (right-down)
                const int B = 2*dy;
                const int A = B - 2*dx;

                int p = B - dx;
                while (x1 <= x2) {
                    rd_write_pixel(x1++, y1, current_color);
                    if (p > 0) {
                        y1--; // Decrement
                        p += A;
                    } else
                        p += B;
                }
            } else { // 270 to 315 degrees (down-right)
                // Swap x and y
                const int B = 2*dx;
                const int A = B - 2*dy;

                int p = B - dy;
                while (y1 >= y2) { // Decrement
                    rd_write_pixel(x1, y1--, current_color);
                    if (p > 0) {
                        x1++;
                        p += A;
                    } else
                        p += B;
                }
            }
        } else { // Bottom-Left Quadrant
            dx = -dx;
            if (dx > dy) { // 180 to 225 degrees (left-down)
                const int B = 2*dy;
                const int A = B - 2*dx;

                int p = B - dx;
                while (x1 >= x2) { // Decrement
                    rd_write_pixel(x1--, y1, current_color);
                    if (p > 0) {
                        y1--; // Decrement
                        p += A;
                    } else
                        p += B;
                }
            } else { // 225 to 270 degrees (down-left)
                // Swap x/y
                const int B = 2*dx;
                const int A = B - 2*dy;

                int p = B - dy;
                while (y1 >= y2) { // Decrement
                    rd_write_pixel(x1, y1--, current_color);
                    if (p > 0) {
                        x1--; // Decrement
                        p += A;
                    } else
                        p += B;
                }
            }
        }
    }
}

// (3D) Helper function that draws a line but uses floats
void digital_differential_analyzer(float x1,float y1,float z1, float x2,float y2,float z2, const float color[3], REDirect& _this) {
    // Set Up
    const int STEPS = std::max(std::abs((int)x2 - (int)x1), std::abs((int)y2 - (int)y1)); // Endpoints need to be integers (floored)
    if (STEPS == 0) { // Looking right at it (or duplicate points)
        _this.plot(x1,y1,std::min(z1,z2), color);
        return;
    }

    // Draw
    const float DX = x2 - x1;
    const float DY = y2 - y1;
    const float DZ = z2 - z1;
    for (int i = 0; i <= STEPS; i++) {
        const float t = (float)i/STEPS;
        //const float NEW_COLOR[3] = { t,t,t };
        //rd_write_pixel(x1 + t*DX, y1 + t*DY, NEW_COLOR);
        //for(int x = 0; x < 6e5; x++) std::cout << "";
        //continue;
        // std::cout << x1 + t*DX << ", " << y1 + t * DY << ", " << z1 + t * DZ << std::endl;
        _this.plot(x1 + t*DX, y1 + t*DY, z1 + t*DZ, color); // x/y are floored here when casted to int
    }
}


// Line Pipeline
std::queue<Vector3> lp_points = std::queue<Vector3>();

// Clips using bitcode
// Returns true if clip is drawable (accepted, else false is rejected)
// "Returns" p0 and p1 clipped points
bool line_bit_clipping(Point4& p0, Point4& p1, Point4& p, uint8_t& last_kode, float last_bc[7]) {
    // Line clipping Setup (another method)
    // But it's excatly the same as last_kode = bit_kode_pack(end, last_bc)
    float bc[7] = {p.x, p.w-p.x, p.y, p.w-p.y, p.z, p.w-p.z, p.w};
    uint8_t kode = ((((((((((
        0b00000000
        | (p.w-p.z < 0)) << 1)
        | (    p.z < 0)) << 1)
        | (p.w-p.y < 0)) << 1)
        | (    p.y < 0)) << 1)
        | (p.w-p.x < 0)) << 1)
        | (    p.x < 0);

    // Clip Check
    if (kode & last_kode) { // Trivial Reject
        last_kode = kode;
        memcpy(last_bc, bc, 28); // last_bc = bc, 28 = 7*4 = float[4] = sizeof bc
        return false;
    }

    // Temporary points for drawing only (in case they get clipped in a trivial accept)
    p0 = Point4(last_bc[0], last_bc[2], last_bc[4], last_bc[6]);
    p1 = Point4(p);

    uint8_t klip = kode | last_kode;
    if (!klip) { // Trivial accept
        last_kode = kode;
        memcpy(last_bc, bc, 28);
        return true;
    }

    // Non-Trivial, Check intersections
    float a0 = 0.0f, a1 = 1.0f; // Start and End alpha
    uint8_t mask = 0b00000001;
    for (int i = 0; i < 6; i++, mask <<= 1) {
        if (!(klip & mask)) continue; // not intersection, ignore

        // Calculate boundary intersection
        float a = last_bc[i] / (last_bc[i] - bc[i]);
        if (last_kode & mask) // outside to inside
            a0 = a>a0 ? a:a0; // a0 = max(a, a0)
        else // inside to outside
            a1 = a<a1 ? a:a1; // a1 = min(a, a1)

        if (a1 < a0) { // Early confirmed Reject
            last_kode = kode;
            memcpy(last_bc, bc, 28);
            return false;
        };
    }

    // Done, shrink point(s) of this line accordingly
    if (kode) { // Right shrink if not in clip
        p1.x = p0.x + a1*(p1.x-p0.x); // x1
        p1.y = p0.y + a1*(p1.y-p0.y); // y1
        p1.z = p0.z + a1*(p1.z-p0.z); // z1
        p1.w = p0.w + a1*(p1.w-p0.w); // w1
    }

    // Using bc[0] because p1 may have changed
    if (last_kode) { // Left shrink if not in clip
        p0.x = p0.x + a0*(bc[0]-p0.x); // x0
        p0.y = p0.y + a0*(bc[2]-p0.y); // y0
        p0.z = p0.z + a0*(bc[4]-p0.z); // z0
        p0.w = p0.w + a0*(bc[6]-p0.w); // w0
    }

    // Save
    last_kode = kode;
    memcpy(last_bc, bc, 28);
    return true;
}

void REDirect::line_pipeline(Vector3 v) {
    lp_points.push(v);
}
void REDirect::line_pipeline(Vector3 v, bool finish_with_face) {
    if (lp_points.empty()) return point_pipeline(v.x,v.y,v.z); // Called with no points
    lp_points.push(v);
    if (finish_with_face) lp_points.push(lp_points.front()); // Add beginning vector to end

    // Setup
    Point4 first = Point4(lp_points.front());
    lp_points.pop();
    // Point4 end = Point4(x,y,z); // Also for the optional face drawing at the end
    current_transform.multiply_mutate(first); // Object to World
    world_to_clip_transform.multiply_mutate(first); // World to Camera AND Camera to Clip

    // Same as last_kode = bit_kode_pack(end, last_bc)
    float last_bc[7] = {first.x, first.w-first.x, first.y, first.w-first.y, first.z, first.w-first.z, first.w}; // adding p.w so I can get it later (nothing to do with clipping)
    uint8_t last_kode = 0b00000000;
    last_kode |= (first.w-first.z < 0); last_kode <<= 1;
    last_kode |= (        first.z < 0); last_kode <<= 1;
    last_kode |= (first.w-first.y < 0); last_kode <<= 1;
    last_kode |= (        first.y < 0); last_kode <<= 1;
    last_kode |= (first.w-first.x < 0); last_kode <<= 1;
    last_kode |= (        first.x < 0);

    // Main clip and draw loop
    for (int i = 0, SIZE = lp_points.size(); i < SIZE; i++) {
        Point4 p = Point4(lp_points.front());
        lp_points.pop();

        // Object to Clip
        current_transform.multiply_mutate(p); // Object to World
        world_to_clip_transform.multiply_mutate(p); // World to Camera AND Camera to Clip

        // Line clipping
        Point4 p0, p1;
        if (!line_bit_clipping(p0, p1, p, last_kode, last_bc)) continue;

        // Clip to Device
        clip_to_device_transform.multiply_mutate(p0); 
        clip_to_device_transform.multiply_mutate(p1);
        Vector3 v0 = p0.to_vector3();
        Vector3 v1 = p1.to_vector3();

        // Draw (with z buffer)
        //std::cout << v0 << " to " << v1 << std::endl;
        //const float t = (float)(i + SIZE) / (SIZE*2);
        //const float gx = ((i%3)==0) ? 1.0f : 0.0f;
        //const float gy = ((i%3)==1) ? 1.0f : 0.0f;
        //const float gz = ((i%3)==2) ? 1.0f : 0.0f;
        //const float X[3] = {gx,gy,gz};
        //bresenham_line_algorithm(v0.x,v0.y, v1.x,v1.y, X);
        digital_differential_analyzer(v0.x,v0.y,v0.z, v1.x,v1.y,v1.z, current_color, *this); 
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Polygon Pipeline
std::vector<attr_point> pp_points  = std::vector<attr_point>(); // input polygon vertices
std::vector<attr_point> pp_clipped = std::vector<attr_point>(); // output polygon clipped vertices
attr_point pp_first[6] = {}; // an array of attr_points for the first point seen by each clipping pipeline stage
attr_point pp_last[6]  = {}; // an array of attr_point for the last point seen by each clipping pipeline stage
bool       pp_flag[6]  = {}; // an array of flags to indicate whether the the pipeline stage has been seen, i.e., whether the first point is valid or uninitialized garbage
// Note: The size of each of these last 3 arrays is the number of stages in the clipping pipeline, one stage for each clipping boundary.

// Helper function that takes an attributed point and a boundary. 
// By using the selected boundary, this function computes the boundary coordinate of the point for that boundary
// and returns whether or not the point is inside the boundary. 
static bool polygon_inside(attr_point& p, int boundary_code) {
    switch (boundary_code) {
    case 0: return              p.coord[0] >= 0.0f; // Left   (  x)
    case 1: return p.coord[3] - p.coord[0] >= 0.0f; // Right  (w-x)
    case 2: return              p.coord[1] >= 0.0f; // Bottom (  y)
    case 3: return p.coord[3] - p.coord[1] >= 0.0f; // Top    (w-y)
    case 4: return              p.coord[2] >= 0.0f; // Back   (  z)
    case 5: return p.coord[3] - p.coord[2] >= 0.0f; // Front  (w-z)
    }
    return false; // Should never happen
}

// Helper function that takes two attributed points and a boundary.
// Simply calculates the "inside" values above for the two points 
// and returns true or false depending on whether the values are equal or not. 
static bool polygon_cross(attr_point& p1, attr_point& p2, int boundary_code) {
    return polygon_inside(p1, boundary_code) != polygon_inside(p2, boundary_code);
}

// Helper function that takes two attributed points and a boundary and returns an attributed point. 
// The intersection point can be found using the approach taken for line clipping: 
// Calculate alpha using the appropriate boundary coordinates for the points and boundary in question and interpolate between the points. 
// Depending on the boundary, the formula for alpha does not need all of the boundary coordinates, so don't compute them all. 
// Use only what's needed for the given boundary. When interpolating, interpolate all of the coordinates of the points.
static attr_point polygon_intersect(attr_point& p1, attr_point& p2, int boundary_code) {
    float a1, a2; // Calculate boundary intersection alpha (0 to 1 for 0 to w)
    switch (boundary_code) {
    case 0: a1 =               p1.coord[0]; a2 =               p2.coord[0]; break; // Left   (  x)
    case 1: a1 = p1.coord[3] - p1.coord[0]; a2 = p2.coord[3] - p2.coord[0]; break; // Right  (w-x)
    case 2: a1 =               p1.coord[1]; a2 =               p2.coord[1]; break; // Bottom (  y)
    case 3: a1 = p1.coord[3] - p1.coord[1]; a2 = p2.coord[3] - p2.coord[1]; break; // Top    (w-y)
    case 4: a1 =               p1.coord[2]; a2 =               p2.coord[2]; break; // Back   (  z)
    case 5: a1 = p1.coord[3] - p1.coord[2]; a2 = p2.coord[3] - p2.coord[2]; break; // Front  (w-z)
    }
    float a = a1/(a1-a2);
    attr_point ip; // ip = p1 + a*(p2-p1);
    for (size_t i = 0; i < ATTR_SIZE; i++)
        ip.coord[i] = p1.coord[i] + a*(p2.coord[i] - p1.coord[i]);
    return ip;
}

// This function does most of the work. 
// It takes primarily as input a point p, and a boundary b to clip against. 
// It works with the edge defined by p and the last point seen at this boundary stage of the clip.
// boundries from 0 to 5: left, right, top, bottom, front, back (arbitaritly, can really be in any order)
void polygon_clip_a_point(attr_point p, int b) {
    if (!pp_flag[b]) { // if this is the first time a point has been seen at stage b
        pp_flag[b] = true;
        pp_first[b] = p; // Keep track of it (for polygon_clip_last_point())
    } else { // Previous point exists
        if (polygon_cross(p, pp_last[b], b)) { //if the edge defined by p and last[b] crosses boundary b
            attr_point ipt = polygon_intersect(p, pp_last[b], b);
            if (b < 5) { // send ipt to the next stage of the pipeline or to the output array depending on b
                polygon_clip_a_point(ipt, b+1);
            } else {
                pp_clipped.push_back(ipt);
            }
        }
    }

    pp_last[b] = p; // Save the most recent vertex seen at this stage.

    if (polygon_inside(p, b)) { // send p to the next stage or to the output array, like above
        if (b < 5) {
            polygon_clip_a_point(p, b+1);
        } else {
            pp_clipped.push_back(p);
        }
    }
}

// This function is very similar to the clip a single point routine, 
// but instead of working with the edge defined by the point passed in and the last point seen, 
// it works with the edge defined by the first point seen and the last point seen.
void polygon_clip_last_point() {
    for (int b = 0; b < 6; b++) { // Loop over the boundaries with b
        if (pp_flag[b] && polygon_cross(pp_first[b], pp_last[b], b)) {
            attr_point ipt = polygon_intersect(pp_last[b], pp_first[b], b);
            if (b < 5) { // send ipt to the next stage of the pipeline or to the output array depending on b
                polygon_clip_a_point(ipt, b+1);
            } else {
                pp_clipped.push_back(ipt);
            }
        }
    }
}

// Recursive helper function to clip a polygon (and return the count)
// Clips pp_points into pp_clipped and returns the number of clipped polygon vertices
int polygon_clipping() {
    memset(pp_flag, false, sizeof pp_flag); // Clear flags

    for (const attr_point& p : pp_points) // clip each point independently
        polygon_clip_a_point(p, 0); // starts with first boundary and recurively reaches last

    polygon_clip_last_point(); // Clips first point with last point

    return pp_clipped.size();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Scan conversion
bool buildEdgeList(std::vector<attr_point> &points, int count, Edge* edge_table);
void makeEdgeRec(attr_point& upper, attr_point& lower, Edge* edge_table);
void addActiveList(int scan, Edge* AET, Edge* edge_table);
void insertEdge(Edge* list, Edge* e);
void updateAET(int scanline, Edge* active);
void deleteAfter(Edge* q);
void resortAET(Edge* active);
void fill_between_the_edges(int scanline, Edge* active, REDirect& _this);

// bool buildEdgeList(std::vector<attr_point> &points, int count);

// The Edge Table
// An edge table (an array of Edges) should be dynamically created at the same time the z-buffer is created. 
// The array should have an edge for every scanline of the image. Most of the values in the edges are not used. 
// All that is needed is the pointer to the edges associated with each scan line.
// These pointers should be initialized to null pointer values. 
// If the scan conversion algorithm is implemented correctly, the initialization should only need to be done once. 
// The edge table should be freed up at the same time the z-buffer is returned to the system memory pool.
// Edge* edge_table = new Edge[display_ySize]; in rd_world_begin();


// scan_convert()
// This is the main function for scan converting a polygon. 
// It takes an array of attributed points (and most likely an integer indicating the number of attributed points in the polygon). 
// Pseudo-code for the scan conversion is given:
void scan_conversion(REDirect& _this) {
    // return scan_conversion_test(_this);
    
    // Clear edge table
    for (int i = 0; i < display_ySize; i++)
        _this.edge_table[i].next = nullptr;

    Edge* AET = new Edge; // Local variables: An Edge which represents the head of the active edge table (AET)

    if (!buildEdgeList(pp_clipped, pp_clipped.size(), _this.edge_table))
        return; // No edges cross a scanline

    AET->next = nullptr; // clear the AET

    // Loop an integer, scan, over the scanlines of the display
    for (int scan = 0; scan < display_ySize; scan++) {
        // Take the edges starting on this scanline from the edge table
        // and add them to the active edge table (AET).
        if (_this.edge_table[scan].next != nullptr)
            addActiveList(scan, AET, _this.edge_table);

        if (AET->next != nullptr) { // if AET is not empty
            fill_between_the_edges(scan, AET, _this); // fill between the edge pairs in the AET
            updateAET(scan, AET); // update the AET
            resortAET(AET); // re-sort the AET
        }
    }

    delete AET;
}

// buildEdgeList
// This routine takes an array of attributed points and a count and places the edges created by the points into the edge table ET. It returns a flag value indicating whether or not the polygon crossed a scanline.
// Local variables:   v1 and v2 are indices into the attributed point
// array.  v1 is the trailing vertex of the edge.  v2 is the leading
// vertex of the edge.  A scanline_crossed variable.
bool buildEdgeList(std::vector<attr_point> &points, int count, Edge* edge_table) {
    bool scanline_crossed = false;

    int v1 = count - 1;  // The last vertex in the polygon

    // Loop v2 over the number of vertices in the polygon
    for (int v2 = 0; v2 < count; v2++) {
        if (std::ceil(points[v1].coord[1]) != std::ceil(points[v2].coord[1])) { // if (the y coordinates of v1 and v2 fall on different scanlines)
            scanline_crossed = true;

            if (points[v1].coord[1] < points[v2].coord[1]) {
                makeEdgeRec(points[v2], points[v1], edge_table); // Make an edge record from vertex[v1] to vertex[v2]
            } else {
                makeEdgeRec(points[v1], points[v2], edge_table); // Make an edge record from vertex[v2] to vertex[v1]
            }
        }

        v1 = v2; // Move to next edge
    }

    return scanline_crossed;
}

// makeEdgeRec
// This routine takes two attributed points, upper and lower, and inserts an Edge created from those two points into the edge table (ET).
// Local variables:  two floating point values, dy and factor.  Also a
// pointer e for the new Edge created.
void makeEdgeRec(attr_point& upper, attr_point& lower, Edge* edge_table) {
    float dy = upper.coord[1] - lower.coord[1];

    // Dynamically allocate a new edge.  Pointer to same is stored in e.
    Edge *e = new Edge;
    e->next = nullptr;

    // Calculate the edge value increments between scan lines
    for (int i = 0; i < ATTR_SIZE; i++)
        e->inc.coord[i] = (upper.coord[i] - lower.coord[i]) / dy;

    // Edge starts on scanline ceil(lower.y)
    float factor = std::ceil(lower.coord[1]) - lower.coord[1];  // Gives the fractional position of the first scanline crossing

    // Calculate the starting values for the edge
    // e->p = lower + factor * e->inc
    for (int i = 0; i < ATTR_SIZE; i++)
        e->p.coord[i] = lower.coord[i] + factor * e->inc.coord[i];

    // Find the last scanline for the edge
    e->yLast = std::ceil(upper.coord[1]) - 1;

    // Insert e into the edge table list of edges starting on scanline ceil(lower.y)
    int scanindex = std::ceil(lower.coord[1]);
    if (edge_table[scanindex].next == nullptr) {
        edge_table[scanindex].next = e;
    } else {
        Edge *current = edge_table[scanindex].next;

        // Set last edge to e
        while (current->next != nullptr)
            current = current->next;
        current->next = e;
    }
}

void addActiveList(int scan, Edge* AET, Edge* edge_table) {
    //This routine takes an integer scanline and a pointer to an Edge, the AET.
    Edge *p, *q;

    p = edge_table[scan].next; // Get the edges starting on this scan line

    while(p) {
        q = p->next;  // Hold the rest of the list
        insertEdge(AET, p);
        p = q;
    }

    // Keep edge table clean -- edges have been transfered
    edge_table[scan].next = 0;
}

void insertEdge(Edge* list, Edge* e) {
    // This routine takes two pointers to Edges, a linked list and an Edge e, to be inserted into that list. 
    // The list is to be maintained sorted by increasing x coordinate values of the edges in the list.
    Edge *p, *q = list;

    // p leads
    p = q->next;

    while (p != 0 && (e->p.coord[0] > p->p.coord[0])) {
        // Step to the next edge
        q = p;
        p = p->next;
    }

    // Link the edge into the list after q
    e->next = q->next;
    q->next = e;
}

void updateAET(int scanline, Edge* active) {
    // This function takes an integer scanline and a pointer to an Edge the AET.
    Edge *q = active, *p = active->next;
    // p leads

    while(p) {
        if (scanline == p->yLast) { // This is the last scanline for this edge
            p = p->next;    // Move p along
            deleteAfter(q);  // get rid of old node
        } else {
            // Update the attribute values
            // p->p += p->inc;
            for (int i = 0; i < ATTR_SIZE; i++)
                p->p.coord[i] += p->inc.coord[i];

            q = p;
            p = p->next;
        }
    }
}

void deleteAfter(Edge* q) {
    // This function takes a pointer to an Edge q. The node after this Edge is to be deleted.
    Edge *p = q->next;

    q->next = p->next;
    delete p;
}

void resortAET(Edge* active) {
    // This function takes a pointer to an Edge, the active edge table, and resorts it by increasing x values. It does this by taking all of the edges out of the active edge list and then reinserting them into the active edge table.
    Edge *q, *p = active->next;

    active->next = 0;
    while(p) {
        q = p->next;
        insertEdge(active, p);
        p = q;
    }
}


void fill_between_the_edges(int scanline, Edge* active, REDirect& _this) {
    // This function takes a scan line and a pointer to an edge, the active edge table. It fills in the pixels between edge pairs in the table
    Edge *p1, *p2;

    p1 = active->next;
    while (p1) {
        p2 = p1->next;  // Get the pair of edges from the AET

        if (p1->p.coord[0] != p2->p.coord[0]) {// if(the x values are not equal) {
            // Calculate the attribute increments along the scanline
            float dx = p2->p.coord[0] - p1->p.coord[0];

            // inc = (p2 - p1) / dx
            attr_point inc;
            for (int i = 0; i < ATTR_SIZE; i++)
                inc.coord[i] = (p2->p.coord[i] - p1->p.coord[i]) / dx;


            // Calculate the starting values for the edge
            float factor = std::ceil(p1->p.coord[0]) - p1->p.coord[0]; // Gives the fractional position of the first pixel crossing

            // value = p1 + factor * inc;
            attr_point value;
            for (int i = 0; i < ATTR_SIZE; i++)
                value.coord[i] = p1->p.coord[i] + factor * inc.coord[i];

            float endx = std::ceil(p2->p.coord[0]);

            while (value.coord[0] < endx) {
                // The attributes in the value variable need to be stored in the global surface_point_values variable
                // and divided by the interpolated constant in that value. 
                // This will give the proper interpolated world coordinate values for the attributes. 
                // The attributes in value should not be changed.
                surface_point_values = attr_point(value); // Copy into surface_point_values
                const float CONSTANT = value.coord[ATTR_CONSTANT];
                for (int i = ATTR_R; i < ATTR_SIZE; i++) // Divide values by CONSTANT
                    surface_point_values.coord[i] /= CONSTANT;

                // Calculate the color for the pixel and plot it.
                float color[3]; // return value from shader
                surface_shader(color); // Get color to shade

                // x and z come from the current values, y is the current scanline
                _this.plot(value.coord[0], scanline, value.coord[2], color);

                // Increment the values
                // value += inc;
                for (int i = 0; i < ATTR_SIZE; i++)
                    value.coord[i] += inc.coord[i];
            }
        }

        p1 = p2->next;
    }
}


// Polygon Pipeline
void REDirect::polygon_pipeline(float x, float y, float z) {
    polygon_pipeline(x,y,z, false);
}
void REDirect::polygon_pipeline(float x, float y, float z, bool finish_and_draw) {
    polygon_pipeline(Vector3(x,y,z), finish_and_draw);
}
void REDirect::polygon_pipeline(Vector3 v) {
    polygon_pipeline(v, false);
}
void REDirect::polygon_pipeline(Vector3 v, bool finish_and_draw) {
    // MOVE (add to queue in clip coords)
    Point4 p = Point4(v);
    current_transform.multiply_mutate(p); // Object to World

    // Lighting Start
    attr_point a;
    p.into_array(a.coord); // xyzw

    a.coord[ATTR_CONSTANT] = 1.0f; // Must be set to 1 (before clipping?)

    // Colors
    a.coord[ATTR_R] = surface_color[0];
    a.coord[ATTR_G] = surface_color[1];
    a.coord[ATTR_B] = surface_color[2];

    // Compute and save normal (should already be in World Space)
    if (_vertex_normal_flag) { // If true, these matter (else its garbage idc)
        a.coord[ATTR_NX] = polygon_normal.x;
        a.coord[ATTR_NY] = polygon_normal.y;
        a.coord[ATTR_NZ] = polygon_normal.z;
    }

    // ???
    a.coord[ATTR_S] = 0;
    a.coord[ATTR_T] = 0;

    // Remember world xyz for later
    a.coord[ATTR_WORLD_X] = p.x / p.w; // divide by w optional? (is always 1 right?)
    a.coord[ATTR_WORLD_Y] = p.y / p.w;
    a.coord[ATTR_WORLD_Z] = p.z / p.w;
    // Lighting End


    // World to Camera AND Camera to Clip
    world_to_clip_transform.multiply_mutate(p); 
    p.into_array(a.coord); // update a.xyzw
    pp_points.push_back(a); // Add to list in clip coordinates
    if (!finish_and_draw) return; // Done with using a and p (and its namespace, so no problem if being reused for something else later on);

    // DRAW
    if (polygon_clipping()) { // Check if not nothing clipped is in bounds (in Clip Space), otherwise theres something more to draw
        // Pre process vertex list for conversion
        for (int i = 0, SIZE = pp_clipped.size(); i < SIZE; i++) {
            // Clip to Device
            attr_point a = pp_clipped[i];
            Point4 p = Point4(a.coord);
            clip_to_device_transform.multiply_mutate(p);
            p.into_array(a.coord);

            // Normalize and save (divide by w)
            // a /= w
            const float W = a.coord[3];
            for (size_t i = 0; i < ATTR_SIZE; i++)
                a.coord[i] /= W;

            pp_clipped[i] = a; // Redundant if a is passed from vector by reference
        }

        // Before the scan conversion routine is called, the global polygon normal, which is in object coordinates, needs to be converted to world coordinates. (already happened in all the shapes)
        // normal_transform.multiply_mutate(polygon_normal);
        if (!(vertex_interpolation_flag && _vertex_normal_flag)) { // If false, polygon_normal needs to be the normal of this polygon face
            /**
            // Old simple verion (not reliable)
            Vector3 a = Vector3(pp_clipped[0].coord[ATTR_WORLD_X], pp_clipped[0].coord[ATTR_WORLD_Y], pp_clipped[0].coord[ATTR_WORLD_Z]);
            Vector3 b = Vector3(pp_clipped[1].coord[ATTR_WORLD_X], pp_clipped[1].coord[ATTR_WORLD_Y], pp_clipped[1].coord[ATTR_WORLD_Z]);
            Vector3 c = Vector3(pp_clipped[2].coord[ATTR_WORLD_X], pp_clipped[2].coord[ATTR_WORLD_Y], pp_clipped[2].coord[ATTR_WORLD_Z]);
            polygon_normal = (a-b).cross(b-c).normalize();
            //*/

            /**/
            // Prof. Version (in a triangle, c and d are the same)
            Vector3 a = Vector3(pp_clipped[0].coord[ATTR_WORLD_X], pp_clipped[0].coord[ATTR_WORLD_Y], pp_clipped[0].coord[ATTR_WORLD_Z]) / pp_clipped[0].coord[ATTR_CONSTANT];
            Vector3 b = Vector3(pp_clipped[1].coord[ATTR_WORLD_X], pp_clipped[1].coord[ATTR_WORLD_Y], pp_clipped[1].coord[ATTR_WORLD_Z]) / pp_clipped[1].coord[ATTR_CONSTANT];
            Vector3 c = Vector3(pp_clipped[2].coord[ATTR_WORLD_X], pp_clipped[2].coord[ATTR_WORLD_Y], pp_clipped[2].coord[ATTR_WORLD_Z]) / pp_clipped[2].coord[ATTR_CONSTANT];
            Vector3 d = Vector3(pp_clipped.back().coord[ATTR_WORLD_X], pp_clipped.back().coord[ATTR_WORLD_Y], pp_clipped.back().coord[ATTR_WORLD_Z]) / pp_clipped.back().coord[ATTR_CONSTANT];
            polygon_normal = (a-c).cross(b-d).normalize();

            // Debugging to see normals (comment out both current_tranforms in line_pipeline too)
            /*
            Vector3 center = Vector3(0,0,0);
            for (attr_point p : pp_clipped)
                center = center + Vector3(p.coord[ATTR_WORLD_X], p.coord[ATTR_WORLD_Y], p.coord[ATTR_WORLD_Z]) / p.coord[ATTR_CONSTANT];
            center = center / pp_clipped.size();
            line_pipeline(center); line_pipeline(center + polygon_normal, false);
            */

            /**
            // Semi-Robust and possibly expensive sum of consecutive cross products to guarantee a normal for flat polygons.
            const Vector3 ZERO = Vector3(0,0,0); // Could be anything that is not on an edge of the polygon
            polygon_normal = ZERO;
            a = pp_clipped.back();
            Vector3 p0 = Vector3(a.coord[ATTR_WORLD_X], a.coord[ATTR_WORLD_Y], a.coord[ATTR_WORLD_Z]);
            for (attr_point& a : pp_clipped) {
                Vector3 p1 = Vector3(a.coord[ATTR_WORLD_X], a.coord[ATTR_WORLD_Y], a.coord[ATTR_WORLD_Z]);
                polygon_normal = polygon_normal + (p0-ZERO).cross(p1-ZERO);
                p0 = p1;
            }
            polygon_normal.normalize_mutate();
            //*/
        }

        // Scan convert draw and fill color (in Device Space), then reset for the next polygon
        scan_conversion(*this);
    }

    pp_points.clear();
    pp_clipped.clear();
}


// (3D) Line drawing
// Takes two points, converts them into homogeneous points 
// and passes them to the line pipeline one at a time, 
// moving to the first point and drawing to the second point.
int REDirect::rd_line(const float start[3], const float end[3]) {
    line_pipeline(Vector3(start));
    line_pipeline(Vector3(end), false);
    return RD_OK;
}


// (2D) Draws a circle using the current drawing color.
void draw_circle_pixels(int xC, int yC, int radius, const float current_color[3]) {
    int x = 0;
    int y = radius;
    int p = 1 - radius; // Decision variable

    while (y >= x) { // 0 to 45
        // For all 8 parts of the circle
        const int xPx = xC + x; // xCenter Plus xOffset
        const int xMx = xC - x; // xCenter Minus xOffset
        const int yPy = yC + y; // etc...
        const int yMy = yC - y;
        rd_write_pixel(xPx, yPy, current_color);
        rd_write_pixel(xPx, yMy, current_color);
        rd_write_pixel(xMx, yPy, current_color);
        rd_write_pixel(xMx, yMy, current_color);

        const int xPy = xC + y;
        const int xMy = xC - y;
        const int yPx = yC + x;
        const int yMx = yC - x;
        rd_write_pixel(xPy, yPx, current_color);
        rd_write_pixel(xPy, yMx, current_color);
        rd_write_pixel(xMy, yPx, current_color);
        rd_write_pixel(xMy, yMx, current_color);

        x++;
        if (p >= 0) { // Techinically 1/4
            y--;
            p += 2*x + 1 - 2*y;
        } else {
            p += 2*x + 1;
        }
    }
}

// 2D Circle drawing
int REDirect::rd_circle(const float center[3], float radius) {
    draw_circle_pixels(center[0], center[1], radius, current_color);
    return RD_OK;
}


// Helpful function that returns true if the color read at x and y is equal to rgb
bool read_and_compare_rgb(int x, int y, float rgb[3]) {
    float color[3];
    rd_read_pixel(x, y, color);
    return rgb[0] == color[0] && rgb[1] == color[1] && rgb[2] == color[2];
}

// Stretches the beginning first, then searches for ends and adds them to the stack to lazily simulate recursion
// 2 Assumptions: xS and xE are the same, and xS and xE are in-bounds
void search_for_spans_and_stack(int xS, int xE, int y, float seedColor[3], std::stack<int>& stack) {
    // Check vertical bounds
    if (y < 0 || y >= display_ySize) return;

    // Check first range
    int const END = xE; xE = xS;
    if (read_and_compare_rgb(xS, y, seedColor)) { // Check if this start cell is valid
        while (++xE < display_xSize && read_and_compare_rgb(xE, y, seedColor)); // From start+1 to right, increment and check
        while (--xS >= 0 && read_and_compare_rgb(xS, y, seedColor)); // From start-1 to left, decrement and check

        // Add first range to stack
        stack.push(xS+1); // shift by 1 to be in the starting point
        stack.push(xE);
        stack.push(y);
    }

    // Check right for any other ranges in the current span seperated by "walls"
    while (++xE < END) { // Search range limited to the current span
        xS = xE; // Remember start position
        if (read_and_compare_rgb(xE, y, seedColor)) { // Check if this start cell+1 is valid
            while (++xE < display_xSize && read_and_compare_rgb(xE, y, seedColor)); // From start+1 to right, increment and check

            // Add new range to stack
            stack.push(xS);
            stack.push(xE);
            stack.push(y);
        }
    }
}

// Optimize early return if color is the same?
// (2D) Flood fills the area from the seed point with the current drawing color
int REDirect::rd_fill(const float seed_point[3]) {
    int x = seed_point[0];
    int y = seed_point[1];
    float seedColor[3];
    rd_read_pixel(x, y, seedColor);

    // Same color (needs to be like this because clors are float and there is floating point error ~1/256)
    if ((.039f > std::abs(current_color[0] - seedColor[0]))
     && (.039f > std::abs(current_color[1] - seedColor[1]))
     && (.039f > std::abs(current_color[2] - seedColor[2])))
        return RD_OK;

    // Check current span
    int xS = x, xE = x;
    if (y >= 0 && y < display_ySize && read_and_compare_rgb(x,y,seedColor)) { // Check start
        while (++xE < display_xSize && read_and_compare_rgb(xE,y,seedColor)); // Check start+1 to right
        while (--xS >= 0 && read_and_compare_rgb(xS,y,seedColor)); // Check start-1 to left
    }

    std::stack<int> stack = std::stack<int>(); // Push Order: xS, xE, y
    stack.push(xS+1); // shift by +1 to be in the starting point
    stack.push(xE);
    stack.push(y);

    // Main Loop
    while(!stack.empty()) {
        // Get next "line" in line (haha)
        y  = stack.top(); stack.pop();
        xE = stack.top(); stack.pop();
        xS = stack.top(); stack.pop();

        // Fill the current line then search up and down for more
        for (x = xS; x < xE; x++) rd_write_pixel(x, y, current_color); // Fill Current Line
        search_for_spans_and_stack(xS, xE, y+1, seedColor, stack); // Search Up 1
        search_for_spans_and_stack(xS, xE, y-1, seedColor, stack); // Search Down 1
    }

    return RD_OK;
}



///////////
// Stubs //
///////////
/**********************   Camera  ******************************************/
int REDirect::rd_camera_eye(const float eyepoint[3]) {
    camera_eye_position = Vector3(eyepoint);
    return RD_OK;
}

int REDirect::rd_camera_at(const float atpoint[3]) {
    camera_look_at_position = Vector3(atpoint);
    return RD_OK;
}

int REDirect::rd_camera_up(const float up[3]) {
    camera_up_direction = Vector3(up);
    return RD_OK;
}

int REDirect::rd_camera_fov(float fov) {
    camera_fov = fov;
    return RD_OK;
}

int REDirect::rd_clipping(float znear, float zfar) {
    camera_near = znear;
    camera_far = zfar;
    return RD_OK;
}


/**********************   Transformations **********************************/
// Takes an array of three floats, creates a translation matrix and multiplies it by the current transform, 
// storing the result back in the current transform.
int REDirect::rd_translate(const float offset[3]) {
    current_transform.translate_mutate(offset[0],offset[1],offset[2]);
    // doesnt affect normal direction, so no worries about normal_transform
    return RD_OK;
}

// Takes an array of three floats, the scale factors in x, y, and z, creates a scale matrix and multiplies it by the current transform,
// storing the result back in the current transform.
int REDirect::rd_scale(const float scale_factor[3]) {
    current_transform.scale_mutate(scale_factor[0],scale_factor[1],scale_factor[2]);
    normal_transform.scale_mutate(1.f/scale_factor[0], 1.f/scale_factor[1], 1.f/scale_factor[2]); // Inverse transpose scale
    return RD_OK;
}

// Takes a float which is the angle of rotation in degrees and creates a rotation matrix in the xy plane. 
// The matrix is multiplied by the current transformation matrix and the results stored back in the current transform.
int REDirect::rd_rotate_xy(float angle) {
    current_transform.rotate_xy_mutate(angle);
    normal_transform.rotate_xy_mutate(angle); // Inverse transpose (cancels out for all axis-aligned rotations)
    return RD_OK;
}

// Same but in the X-Axis from y to z
int REDirect::rd_rotate_yz(float angle) {
    current_transform.rotate_yz_mutate(angle);
    normal_transform.rotate_xy_mutate(angle);
    return RD_OK;
}

// Same but in the Y-Axis from z to x
int REDirect::rd_rotate_zx(float angle) {
    current_transform.rotate_zx_mutate(angle);
    normal_transform.rotate_xy_mutate(angle);
    return RD_OK;
}

int REDirect::rd_matrix(const float* mat) {
    return RD_OK;
}

// Push a copy of the current transform onto the transformation stack. 
// The current transformation is left unchanged.
int REDirect::rd_xform_push() {
    //std::cout << "PUSH :\n" << current_transform << '\n';
    stack.push(current_transform); // stack makes a copy automatically so its fine
    stack.push(normal_transform); // also this, then get this back in reverse order
    return RD_OK;
}

// Pop the top of the transformation stack into the current transform.
int REDirect::rd_xform_pop() {
    if (stack.empty()) // Return error if empty
        return RD_INPUT_TRANSFORM_STACK_UNDERFLOW;

    normal_transform = stack.top();
    stack.pop();

    //std::cout << "TOP :\n" << stack.top() << '\n';
    //std::cout << "POP :\n" << current_transform << '\n';
    current_transform = stack.top();
    //std::cout << "NOW :\n" << current_transform << '\n';
    stack.pop();
    return RD_OK;
}


/**********************   Geometric Objects  *******************************/
int REDirect::rd_bezier_curve(const string& vertex_type, int degree, const vector<float>& vertex) {
    return RD_OK;
}

int REDirect::rd_bezier_patch(const string& vertex_type, int u_degree, int v_degree, const vector<float>& vertex) {
    return RD_OK;
}

int REDirect::rd_catmull_clark_sds(const string& vertex_type, const vector<float>& coord, int nvertex, const vector<int>& vertex_list, int nface, const vector<int>& crease_list, int ncrease, const vector<float>& sharpness) {
    return RD_OK;
}

// This routine renders a set of points. 
// It takes a string describing the information associated with each point, 
// an array of numbers and the size of the array. 
// The value of the string determines how many floating point values describe a point. 
// This can be used to assign colors and surface normals to each point. 
// 
// Right now it should be ignored, assuming that there are only three values per point   x, y, and z.
// The array gives the values assigned to the points -- in this case the x, y and z values of point 0, 
// followed by the x, y, and z coordinates of point 1, and so on. 
// The given size of this array (which has already been allocated when this function is called) 
// is the number of points, not the number of values.
// 
// Each point in the set is passed through the point pipeline in turn, clipped, and plotted.
int REDirect::rd_pointset(const string& vertex_type, int nvertex, const vector<float>& vertex) {
    for (int i = 0, LENGTH = nvertex * 3; i < LENGTH; i += 3)
        point_pipeline(vertex[i], vertex[i+1], vertex[i+2]);

    return RD_OK;
}

// This routine renders a set of polygons. 
// It takes a string describing the information associated with each polygon vertex, 
// and two array of numbers and the size of those arrays. 
// The value of the string determines how many floating point values describe a vertex.
// This can be used to assign colors, surface normals, and texture map coordinates to an object on a per vertex basis. 
// Right now it should be ignored, assuming that there are only three values per vertex   x, y, and z.
// 
// The first array gives the values assigned to the vertices -- in this case the x, y and z values of vertex 0, 
// followed by the x, y, and z coordinates of vertex 1, and so on. 
// The given size of this array (which has already been allocated when this function is called) is the number of vertices, not the number of values.
//
// The second array gives a list of vertices that describe the faces of the set. 
// Faces in the array are separated by a value of -1. 
// For example 1, 4, 7, 5, -1, 0, 1, 2, 3, -1 ... 
// means move to vertex 1, draw to vertex 4, draw to vertex 7, draw to vertex 5 and then back to vertex 1. 
// Then move to vertex 0, draw to vertex 1, draw to vertices 2, 3, back to 0 and so on.
// TODO
int REDirect::rd_polyset(const string& vertex_type, int nvertex, const vector<float>& vertex, int nface, const vector<int>& face) {
    // Read
    vector<Vector3> vertices = vector<Vector3>();

    for (int i = 0, LENGTH = nvertex * 3; i < LENGTH; i += 3) {
        vertices.push_back(Vector3(vertex[i], vertex[i+1], vertex[i+2]));
    }

    // Safety check (should probably add one above as well for groups of 3)
    if (nvertex <  1) return RD_OK; // Nothing
    if (nvertex == 1) { // Point
        point_pipeline(vertices[0].x, vertices[0].y, vertices[0].z);
        return RD_OK;
    } else if (nvertex == 2) { // Line
        line_pipeline(vertices[0]);
        line_pipeline(vertices[1], false);
        return RD_OK;
    }


    // Flat shading
    // At the beginning of each object, turn on the vertex normal flag.
    // And turn off the vertex_color_flag and the vertex_texture_flag.
    _vertex_normal_flag = false; // Set flags to be flat
    _vertex_color_flag = false;
    _vertex_texture_flag = false;

    // Your routines should calculate a surface normal for each vertex and place it in the appropriate fields of the attributed point before it is passed to the polygon pipeline. 
    // The surface normal values should be obtained from what you know about the properties of the object being rendered. 
    // For example, the normal vector to a circle or a sphere is the vector obtained from going from the origin through the surface point. 
    // In other words, for a circle or sphere, the normal vector has the same coordinate values as the vertex position.


    // Immediately before sending the last point of a polygon to the polygon pipeline, 
    // a normal vector for that polygon should be calculated and stored in the global poly_normal variable. 
    // This should be done regardless of whether or not vertex normals have been calculated or provided.

    // Draw (if face starts with any amount of -1's this doesnt work)
    for (int i = 0, LENGTH = face.size(); i < LENGTH; i++) {
        int x = face[i];
        if (x >= 0) { // not -1: Save
            if (face[i+1] < 0) continue; // hack for now

            polygon_pipeline(vertices[x]);
            continue;
        }

        // Immediately before sending the last point of a polygon to the polygon pipeline, 
        // a normal vector for that polygon should be calculated and stored in the global poly_normal variable. 
        // This should be done regardless of whether or not vertex normals have been calculated or provided.
        // The polygon normal (which should be calculated and stored in the global surface normal variable [polygon_normal]) can be found by taking the cross product of two of the polygon edges. 
        // Order is important here. If you see the vertices traversed in counter-clockwise order, the normal should be coming toward you.
        /*
        const Vector3 &P0 = vertices[face[i-3]];
        const Vector3 &P1 = vertices[face[i-2]];
        const Vector3 &P2 = vertices[face[i-1]];

        polygon_normal = P1.subtract(P0); // Copy as A(p1 - p0)
        polygon_normal.cross_mutate(P2.subtract(P1)); // C = A x B(p2-p1) to calculate normal
        line_pipeline(Vector3()); line_pipeline(polygon_normal*2, false); // Debug for normals
        normal_transform.multiply_mutate(polygon_normal); // Object to World
        polygon_normal.normalize_mutate(); // Normalize
        */

        // Last point (draws everything)
        polygon_pipeline(vertices[face[i-1]], true);
    }

    std::cout << "\n";
    return RD_OK;
}

// WIP
int REDirect::rd_lineset(const string& vertex_type, int nvertex, const vector<float>& vertex, int nseg, const vector<int>& seg) {
    // Read
    vector<Vector3> vertices = vector<Vector3>();

    for (int i = 0, LENGTH = nvertex * 3; i < LENGTH; i += 3) {
        vertices.push_back(Vector3(vertex[i], vertex[i+1], vertex[i+2]));
    }

    // Draw (if seg starts with any amount of -1's this doesnt work)
    for (int i = 0, LENGTH = seg.size(); i < LENGTH; i++) {
        int x = seg[i];
        if (x < 0) { // -1
            line_pipeline(vertices[seg[i-1]]);
            continue;
        }

        line_pipeline(vertices[x], true);
    }

    return RD_OK;
}

// Run a cone through the pipeline. The cone should have a circular base of a given radius on the xy plane. 
// The cone should have a given height in the positive z direction.
int REDirect::rd_cone(float height, float radius, float thetamax) {
    _vertex_normal_flag = true; // These sides are meant to be percieved as "smooth" (interp at each vertex)
    _vertex_color_flag = false;
    _vertex_texture_flag = false;

    // Draw loop
    int STEPS = 20;
    const float UP = radius / height; // r/h, up direction to make the slope above xz cos/sin perpendicular to the hypotenuse
    const float RATIO = ((float)360.0 / STEPS) * (std::acos(-1)/180);
    for (int i = 0; i < STEPS; i++) {
        const float COS_0 = std::cos( i   *RATIO);
        const float COS_1 = std::cos((i+1)*RATIO);
        const float SIN_0 = std::sin( i   *RATIO);
        const float SIN_1 = std::sin((i+1)*RATIO);

        // Old Normal Calculaton for the right side (cross multiplication)
        // polygon_normal = v1.subtract(v0); // Copy as A(p1 - p0)
        // polygon_normal.cross_mutate(TOP.subtract(v1)); // Use B(p2-p1) to calculate normal as C (C = A x B)
        // polygon_normal.normalize_mutate(); // Normalize

        // Left Side
        polygon_normal = normal_transform.multiply(COS_0, SIN_0, UP); // Normal in world coordinates
        polygon_normal.normalize_mutate(); // Normalize
        polygon_pipeline(0, 0, height); // Draw from top...
        polygon_pipeline(radius*COS_0, radius*SIN_0, 0); // ...to left bottom edge, then from here to...

        // Right Side
        polygon_normal = normal_transform.multiply(COS_1, SIN_1, UP);
        polygon_normal.normalize_mutate(); // Normalize
        polygon_pipeline(radius*COS_1, radius*SIN_1, 0); // ...right bottom edge (and save vertex for next draw), then...
        polygon_pipeline(0, 0, height, true); // ...Draw: Finish triangle back to top tip (duplicate point for 2nd normal)
    }

    return RD_OK;
}

// Run the faces of the cube (+/- 1 in x, y, and z) through the transformation pipeline.
// Each face drawn in a counter clockwise order
int REDirect::rd_cube() {
    _vertex_normal_flag = false; // Flat shading
    _vertex_color_flag = false;
    _vertex_texture_flag = false;

    Vector3 vertices[8] = {
        Vector3(-1,-1,-1),
        Vector3(-1,-1, 1),
        Vector3(-1, 1,-1),
        Vector3(-1, 1, 1),
        Vector3( 1,-1,-1),
        Vector3( 1,-1, 1),
        Vector3( 1, 1,-1),
        Vector3( 1, 1, 1)
    };

    // Counter Clockwise facing me (hard coded)
    // Front
    polygon_normal = normal_transform.multiply(0,0,1); // Hard-coded normal in world space
    polygon_normal.normalize_mutate(); // Normalize normal
    polygon_pipeline(vertices[5]);       //  1,-1, 1
    polygon_pipeline(vertices[7]);       //  1, 1, 1
    polygon_pipeline(vertices[3]);       // -1, 1, 1
    polygon_pipeline(vertices[1], true); // -1,-1, 1

    // Back
    polygon_normal = normal_transform.multiply(0,0,-1);
    polygon_normal.normalize_mutate();
    polygon_pipeline(vertices[6]);       //  1, 1,-1
    polygon_pipeline(vertices[4]);       //  1,-1,-1
    polygon_pipeline(vertices[0]);       // -1,-1,-1
    polygon_pipeline(vertices[2], true); // -1, 1,-1

    // Top
    polygon_normal = normal_transform.multiply(0,1,0);
    polygon_normal.normalize_mutate();
    polygon_pipeline(vertices[7]);       //  1, 1, 1
    polygon_pipeline(vertices[6]);       //  1, 1,-1
    polygon_pipeline(vertices[2]);       // -1, 1,-1
    polygon_pipeline(vertices[3], true); // -1, 1, 1

    // Bottom
    polygon_normal = normal_transform.multiply(0,-1,0);
    polygon_normal.normalize_mutate();
    polygon_pipeline(vertices[5]);       //  1,-1, 1
    polygon_pipeline(vertices[1]);       // -1,-1, 1
    polygon_pipeline(vertices[0]);       // -1,-1,-1
    polygon_pipeline(vertices[4], true); //  1,-1,-1

    // Left (my right)
    polygon_normal = normal_transform.multiply(1,0,0);
    polygon_normal.normalize_mutate();
    polygon_pipeline(vertices[7]);       //  1, 1, 1
    polygon_pipeline(vertices[5]);       //  1,-1, 1
    polygon_pipeline(vertices[4]);       //  1,-1,-1
    polygon_pipeline(vertices[6], true); //  1, 1,-1

    // Right (my left)
    polygon_normal = normal_transform.multiply(-1,0,0);
    polygon_normal.normalize_mutate();
    polygon_pipeline(vertices[2]);       // -1, 1,-1
    polygon_pipeline(vertices[0]);       // -1,-1,-1
    polygon_pipeline(vertices[1]);       // -1,-1, 1
    polygon_pipeline(vertices[3], true); // -1, 1, 1

    return RD_OK;
}

// Run a circular cylinder through the pipeline. 
// The ends should be circles of a given radius parallel to the xy plane. 
// The circles are centered at (0, 0, zmin) and (0, 0, zmax) where zmin and zmax are the z coordinates of the ends of the cylinder. 
// Thus the cylinder extends from -radius to radius in x and y and from zmin to zmax in z.
// Very similar to cone
int REDirect::rd_cylinder(float radius, float zmin, float zmax, float thetamax) {
    // Draw loop
    _vertex_normal_flag = true; // Smooth sides
    _vertex_color_flag = false;
    _vertex_texture_flag = false;

    const int STEPS = 20;
    const double RATIO = ((double)360.0 / STEPS) * (std::acos(-1)/180);
    for (int i = 0; i < STEPS; i++) {
        const float COS_0 = radius * std::cos( i   *RATIO);
        const float COS_1 = radius * std::cos((i+1)*RATIO);
        const float SIN_0 = radius * std::sin( i   *RATIO);
        const float SIN_1 = radius * std::sin((i+1)*RATIO);

        // Left Side (Left normal is stretched by radius, but it'll be normalized at the end so it's fine)
        polygon_normal = normal_transform.multiply(COS_0, SIN_0, 0); // Object to World so it's in World Space
        polygon_normal.normalize_mutate(); // Normalize
        polygon_pipeline(COS_0, SIN_0, zmax); // Draw bottom edge
        polygon_pipeline(COS_0, SIN_0, zmin); // Draw wall edge

        // Right Side
        polygon_normal = normal_transform.multiply(COS_1, SIN_1, 0);
        polygon_normal.normalize_mutate();
        polygon_pipeline(COS_1, SIN_1, zmin); // Draw top edge
        polygon_pipeline(COS_1, SIN_1, zmax, true); // Move "draw pointer" to this bottom point
    }

    return RD_OK;
}

// Run a circular disc through the pipeline. The disk should have a given radius. 
// The disk is parallel to the xy plane and centered about the z axis. 
// A height parameters gives the position of the disk along the z axis.
// Similar to cone
int REDirect::rd_disk(float height, float radius, float theta) {
    _vertex_normal_flag = false; // Flat disk
    _vertex_color_flag = false;
    _vertex_texture_flag = false;

    // Draw loop
    const int STEPS = 20;
    const double RATIO = ((double)360.0 / STEPS) * (std::acos(-1)/180);
    for (int i = 0; i < STEPS; i++) {
        const float ANGLE = i*RATIO;
        polygon_pipeline(radius * std::cos(ANGLE), 
                         radius * std::sin(ANGLE), 
                         height
        );
    }

    // Finish drawing
    polygon_normal = normal_transform.multiply(0,0,1); // Normal pointing up (in Object to World so it's in World Space)
    polygon_normal.normalize_mutate(); // Normalize
    polygon_pipeline(radius, 0, height, true); // Last point

    return RD_OK;
}

int REDirect::rd_hyperboloid(const float start[3], const float end[3], float thetamax) {
    return RD_OK;
}

int REDirect::rd_paraboloid(float rmax, float zmin, float zmax, float thetamax) {
    return RD_OK;
}


/**/
// Run a sphere (or some representation of a sphere) through the pipeline. 
// The sphere should be centered at the origin and have a radius given by a parameter.
int REDirect::rd_sphere(float radius, float zmin, float zmax, float thetamax) {
    // Setup
    _vertex_normal_flag = true; // This while spehere is meant to be percieved as "smooth" (interp at each vertex)
    _vertex_color_flag = false;
    _vertex_texture_flag = false;

    const int STEPS = 20; // Must be a multiple of 4
    const int Y_STEPS = STEPS/4;
    const float RATIO = ((float)360 / STEPS) * (std::acos(-1)/180); // 360/steps * pi/180 // to radians

    // Middle Squares
    for (int level = 1-Y_STEPS; level <= Y_STEPS; level++) {
        float VERTICAL_ANGLE = level*RATIO;
        const float VERTICAL_COS_UP = radius * std::cos(VERTICAL_ANGLE);
        const float VERTICAL_SIN_UP = radius * std::sin(VERTICAL_ANGLE);

        VERTICAL_ANGLE -= RATIO; // (level - 1) * RATIO
        const float VERTICAL_COS_DOWN = radius * std::cos(VERTICAL_ANGLE);
        const float VERTICAL_SIN_DOWN = radius * std::sin(VERTICAL_ANGLE);

        float horizontal_angle = 0; // horizontal_angle_left = rotation*RATIO
        float HORIZONTAL_COS = std::cos(horizontal_angle);
        float HORIZONTAL_SIN = std::sin(horizontal_angle);
        for (int rotation = 0; rotation < STEPS; rotation++) {
            Vector3 p; // Temp value for point position in Object Space

            // Start (top-left)
            p = Vector3(VERTICAL_COS_UP * HORIZONTAL_COS,
                        VERTICAL_COS_UP * HORIZONTAL_SIN,
                        VERTICAL_SIN_UP);
            polygon_normal = normal_transform.multiply(p); // World normal at this point (coincidentally the same object coords as sphere position)
            polygon_normal.normalize_mutate(); // From Object to World, now Normalize
            polygon_pipeline(p); // Save this point
            // line_pipeline(p); line_pipeline(p * 1.5, false); // Delete me


            // Down (bottom-left)
            p = Vector3(VERTICAL_COS_DOWN * HORIZONTAL_COS,
                        VERTICAL_COS_DOWN * HORIZONTAL_SIN,
                        VERTICAL_SIN_DOWN);
            polygon_normal = normal_transform.multiply(p);
            polygon_normal.normalize_mutate();
            polygon_pipeline(p);


            horizontal_angle += RATIO; // horizontal_angle_right = (rotation+1)*RATIO
            HORIZONTAL_COS = std::cos(horizontal_angle);
            HORIZONTAL_SIN = std::sin(horizontal_angle);

            // Right (bottom-right)
            p = Vector3(VERTICAL_COS_DOWN * HORIZONTAL_COS,
                        VERTICAL_COS_DOWN * HORIZONTAL_SIN,
                        VERTICAL_SIN_DOWN);
            polygon_normal = normal_transform.multiply(p);
            polygon_normal.normalize_mutate();
            polygon_pipeline(p);

            // Up and Finish (top-right)
            p = Vector3(VERTICAL_COS_UP * HORIZONTAL_COS,
                        VERTICAL_COS_UP * HORIZONTAL_SIN,
                        VERTICAL_SIN_UP);
            polygon_normal = normal_transform.multiply(p);
            polygon_normal.normalize_mutate();
            polygon_pipeline(p, true); // Draw
        }
    }

    return RD_OK;
}
//*/


/**
int REDirect::rd_sphere(float radius, float zmin, float zmax, float thetamax) {
    // Setup
    const int Y_STEPS = 5;
    const int X_STEPS = Y_STEPS*4; // Must be a multiple of 4
    const float RATIO = ((float)360 / X_STEPS) * (std::acos(-1)/180); // 360/steps * pi/180 // to radians

    // Note: Can be optimized as current angle becomes last angle
    for (int level = -Y_STEPS; level <= Y_STEPS; level++) { // If u wanna ignore the top code (triangled squares)
        const float VERTICAL_ANGLE_UP   = level*RATIO;
        const float VERTICAL_ANGLE_DOWN = (level-1)*RATIO;

        for (int rotation = 0; rotation < X_STEPS; rotation++) {
            const float HORIZONTAL_ANGLE_LEFT  = rotation*RATIO;
            const float HORIZONTAL_ANGLE_RIGHT = (rotation+1)*RATIO;
            float x,y,z;

            // Start (top-left)
            x = radius * std::cos(VERTICAL_ANGLE_UP) * std::cos(HORIZONTAL_ANGLE_LEFT);
            y = radius * std::cos(VERTICAL_ANGLE_UP) * std::sin(HORIZONTAL_ANGLE_LEFT);
            z = radius * std::sin(VERTICAL_ANGLE_UP);
            polygon_pipeline(x,y,z);

            // Down (bottom-left)
            x = radius * std::cos(VERTICAL_ANGLE_DOWN) * std::cos(HORIZONTAL_ANGLE_LEFT);
            y = radius * std::cos(VERTICAL_ANGLE_DOWN) * std::sin(HORIZONTAL_ANGLE_LEFT);
            z = radius * std::sin(VERTICAL_ANGLE_DOWN);
            polygon_pipeline(x,y,z);

            // Right (bottom-right)
            x = radius * std::cos(VERTICAL_ANGLE_DOWN) * std::cos(HORIZONTAL_ANGLE_RIGHT);
            y = radius * std::cos(VERTICAL_ANGLE_DOWN) * std::sin(HORIZONTAL_ANGLE_RIGHT);
            z = radius * std::sin(VERTICAL_ANGLE_DOWN);
            polygon_pipeline(x,y,z);

            // Up and Finish (top-right)
            x = radius * std::cos(VERTICAL_ANGLE_UP) * std::cos(HORIZONTAL_ANGLE_RIGHT);
            y = radius * std::cos(VERTICAL_ANGLE_UP) * std::sin(HORIZONTAL_ANGLE_RIGHT);
            z = radius * std::sin(VERTICAL_ANGLE_UP);
            polygon_pipeline(x,y,z, true);
        }
    }

    return RD_OK;
}
//*/



int REDirect::rd_sqsphere(float radius, float north, float east, float zmin, float zmax, float thetamax) {
    return RD_OK;
}

int REDirect::rd_sqtorus(float radius1, float radius2, float north, float east, float phimin, float phimax, float thetamax) {
    return RD_OK;
}

int REDirect::rd_torus(float radius1, float radius2, float phimin, float phimax, float thetamax) {
    return RD_OK;
}

int REDirect::rd_tube(const float start[3], const float end[3], float radius) {
    return RD_OK;
}


/********************  Lighting & Shading  ***************************/
int REDirect::rd_opacity(float opacity) {
    return RD_OK;
}

int REDirect::rd_emission(const float color[], float intensity) {
    return RD_OK;
}

int REDirect::rd_surface(const string& shader_type) {
    if (shader_type == "matte")
        surface_shader = &matte;
    else if (shader_type == "plastic")
        surface_shader = &plastic;
    else if (shader_type == "metal")
        surface_shader = &metal;

    return RD_OK;
}

int REDirect::rd_cone_light(const float pos[3], const float at[3], float theta_min, float theta_max, const float color[], float intensity) {
    return RD_OK;
}

int REDirect::rd_point_light(const float pos[3], const float color[], float intensity) {
    light_data l; // Intensity parameters for each light should simply be multiplied by the color before being stored in the light color.
    l.rgb[0] = intensity * color[0]; 
    l.rgb[1] = intensity * color[1]; 
    l.rgb[2] = intensity * color[2];
    l.xyz = Vector3(pos);
    point_lights.push_back(l); // Add data
    return RD_OK;
}

int REDirect::rd_far_light(const float dir[3], const float color[], float intensity) {
    light_data l; // Intensity parameters for each light should simply be multiplied by the color before being stored in the light color.
    l.rgb[0] = intensity * color[0]; 
    l.rgb[1] = intensity * color[1]; 
    l.rgb[2] = intensity * color[2];
    l.xyz = Vector3(dir);
    l.xyz.normalize_mutate(); // Normalize just in case;
    far_lights.push_back(l); // Add data
    return RD_OK;
}

int REDirect::rd_ambient_light(const float color[], float intensity) {
    ambient_light[0] = intensity * color[0];
    ambient_light[1] = intensity * color[1];
    ambient_light[2] = intensity * color[2];
    return RD_OK;
}

int REDirect::rd_specular_color(const float color[], int exponent) {
    specular_exponent = exponent;
    specular_color[0] = color[0];
    specular_color[1] = color[1];
    specular_color[2] = color[2];
    return RD_OK;
}

int REDirect::rd_k_ambient(float Ka) {
    ambient_coefficient = Ka;
    return RD_OK;
}

int REDirect::rd_k_diffuse(float Kd) {
    diffuse_coefficient = Kd;
    return RD_OK;
}

int REDirect::rd_k_emission(float Ke) {
    return RD_OK;
}

int REDirect::rd_k_specular(float Ks) {
    specular_coefficient = Ks;
    return RD_OK;
}

int REDirect::rd_attribute_push() {
    return RD_OK;
}

int REDirect::rd_attribute_pop() {
    return RD_OK;
}


/****************************  Mapping  ******************************/
int REDirect::rd_map_border(const string& map_type, const string& horizontal, const string& vertical) {
    return RD_OK;
}

int REDirect::rd_map_bound(const string& map_type, float s_min, float t_min, float s_max, float t_max) {
    return RD_OK;
}

int REDirect::rd_map_load(const string& filename, const string& label) {
    return RD_OK;
}

int REDirect::rd_map_sample(const string& map_type, const string& intra_level, const string& inter_level) {
    return RD_OK;
}

int REDirect::rd_map(const string& map_type, const string& label) {
    return RD_OK;
}


/****************************  Options  **********************************/
int REDirect::rd_option_array(const string& name, int n, const vector<float>& values) {
    return RD_OK;
}

// This method takes a name of an option and its value. 
// The implementation is simply a large if / elseif construct that recognizes the name and does the appropriate thing. 
// For this assignment, the option that you need to recognize is the Interpolate option. 
// It should set a global interpolation flag to the appropriate value. 
// By default the global interpolation flag should be true. 
int REDirect::rd_option_bool(const string& name, bool flag) {
    // use normals if both _vertex_normal_flag and vertex_interpolation_flag is true
    if (name == "Interpolate")
        vertex_interpolation_flag = flag;
    else if (name == "NORMAL?")
        _vertex_normal_flag = flag;

    return RD_OK;
}

int REDirect::rd_option_list(const string& name, int n, const vector<string>& values) {
    return RD_OK;
}

int REDirect::rd_option_real(const string& name, float value) {
    return RD_OK;
}

int REDirect::rd_option_string(const string& name, const string& value) {
    return RD_OK;
}

int REDirect::rd_custom(const string& label) {
    return RD_OK;
}

REDirect::~REDirect() {
}