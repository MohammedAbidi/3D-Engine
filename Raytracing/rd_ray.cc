#include "rd_ray.h" // Brings with it vector and string (from rd_enginebase.h)

#include "rd_display.h" // For global screen size
#include "rd_error.h" // For RD_OK (aka 0)

#include <cmath> // For trig functions
#include <iostream> // For debugging


// Bias Constant: As small as possible without causing visual shading 'dots' (Easy 'hacky' way of dealing with "shadow acne")
static constexpr float RDRAY_BIAS = .005f; 



  /////////////
 // Vector3 //
/////////////
template<class T>
union Vector3_t {
    // Data (Same 3 contiguous pieces of memory)
    T xyz[3]; // To use data like an array (maybe rename to .arr? .v?)
    T rgb[3]; // Alias for Color3

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wpedantic" // Ignore warning about anonymous structs
    struct {T x,y,z;};
    struct {T r,g,b;}; // Alias for Color3
    #pragma GCC diagnostic pop


    // Constructors
    inline Vector3_t() = default; // Compiler default (POD compatible) // Default is 0,0,0 anyway
    inline Vector3_t(const T& X, const T& Y = T(0), const T& Z = T(0)) : x(X), y(Y), z(Z) {} // Copied (other constructors are ok as POD constructor already exists (the 'default' one))
    inline Vector3_t(const T arr[3]) : x(arr[0]), y(arr[1]), z(arr[2]) {} // Passed as an array of size 3


    // Math
    inline constexpr T mag()  const { return std::sqrt(mag2()); }
    inline constexpr T mag2() const { return x*x + y*y + z*z; }
    inline constexpr T dot(const Vector3_t<T>& v) const { return x*v.x + y*v.y + z*v.z; }
    inline Vector3_t<T>  crossed(const Vector3_t<T>& v) const { return Vector3_t<T>(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x); } // Flip y sign into -(x*v.z - z*v.x)

    // Mutates
    inline Vector3_t<T>& clamp_down_mutate(const T& n) { x = std::min(x,n), y = std::min(y,n), z = std::min(z,n); return *this; }

    // Returns a new Vector3
    inline Vector3_t<T> normalized() const {
        const T M2 = mag2();
        if (M2 <= T(0)) return Vector3_t<T>(); // (0,0,0) or error (should never happen)
        const T I = T(1) / std::sqrt(M2);
        return Vector3_t<T>(x*I, y*I, z*I);
    }

    // Same as above but mutates the current Vector3
    inline Vector3_t<T>& normalize() {
        const T M2 = mag2();
        if (M2 <= T(0)) {
            x = T(0), y = T(0), z = T(0); // (0,0,0) or error (should never happen)
        } else {
            const T I = T(1) / std::sqrt(M2); // inverse multiplication is faster than 3 divisions (possible precision loss here?)
            x *= I, y *= I, z *= I;
        }
        return *this;
    }

    // Returns a new reflection (R) of this (L) on normal (N)
    // Should probably flip this (L) before using
    // WARNING: BOTH MUST BE NORMAL VECTORS
    inline Vector3_t<T> reflection(const Vector3_t<T>& N) const {
        return (T(2)*(this->dot(N))*N - *this).normalize(); // R = 2*(N.dot(L))*N - L; // Reflection 
    }


    // Operators (I went overboard for full freedom)
    // Produces a new Vector3 with each component (x,y,z) of this operated with each component of v
    inline Vector3_t<T>  operator +  (const Vector3_t<T>& v) const { return Vector3_t<T>(x + v.x, y + v.y, z + v.z); } // Addition       (v1 + v2)
    inline Vector3_t<T>  operator -  (const Vector3_t<T>& v) const { return Vector3_t<T>(x - v.x, y - v.y, z - v.z); } // Subtraction    (v1 - v2)
    inline Vector3_t<T>  operator *  (const Vector3_t<T>& v) const { return Vector3_t<T>(x * v.x, y * v.y, z * v.z); } // Multiplication (v1 * v2)
    inline Vector3_t<T>  operator /  (const Vector3_t<T>& v) const { return Vector3_t<T>(x / v.x, y / v.y, z / v.z); } // Division       (v1 / v2)
    inline Vector3_t<T>  operator -  ()                      const { return Vector3_t<T>(-x, -y, -z);                } // Negative       (-v1)

    // Mutates this Vector3 with each component (x,y,z) of this operated with each component of v
    inline Vector3_t<T>& operator += (const Vector3_t<T>& v) { x += v.x, y += v.y, z += v.z; return *this; } // Mutating Addition       (v1 += v2)
    inline Vector3_t<T>& operator -= (const Vector3_t<T>& v) { x -= v.x, y -= v.y, z -= v.z; return *this; } // Mutating Subtraction    (v1 -= v2)
    inline Vector3_t<T>& operator *= (const Vector3_t<T>& v) { x *= v.x, y *= v.y, z *= v.z; return *this; } // Mutating Multiplication (v1 *= v2)
    inline Vector3_t<T>& operator /= (const Vector3_t<T>& v) { x /= v.x, y /= v.y, z /= v.z; return *this; } // Mutating Division       (v1 /= v2)

    // Mutates this Vector3 with each component (x,y,z) of v operated by the same scalar s (aka: same number)
    inline Vector3_t<T>& operator += (const T& s) { x += s, y += s, z += s; return *this; } // Mutating Addition       (v += s)
    inline Vector3_t<T>& operator -= (const T& s) { x -= s, y -= s, z -= s; return *this; } // Mutating Subtraction    (v -= s)
    inline Vector3_t<T>& operator *= (const T& s) { x *= s, y *= s, z *= s; return *this; } // Mutating Multiplication (v *= s)
    inline Vector3_t<T>& operator /= (const T& s) { x /= s, y /= s, z /= s; return *this; } // Mutating Division       (v /= s)

    // Produces a new Vector3 with each component (x,y,z) of v operated by the same scalar s (aka: same number)
    inline friend Vector3_t<T> operator + (const Vector3_t<T>& v, const T& s) { return Vector3_t<T>(v.x + s, v.y + s, v.z + s); }; // x+s, y+s, z+s
    inline friend Vector3_t<T> operator - (const Vector3_t<T>& v, const T& s) { return Vector3_t<T>(v.x - s, v.y - s, v.z - s); }; // x-s, y-s, z-s
    inline friend Vector3_t<T> operator * (const Vector3_t<T>& v, const T& s) { return Vector3_t<T>(v.x * s, v.y * s, v.z * s); }; // x*s, y*s, z*s
    inline friend Vector3_t<T> operator / (const Vector3_t<T>& v, const T& s) { return Vector3_t<T>(v.x / s, v.y / s, v.z / s); }; // x/s, y/s, z/s

    // Same as above, but swapped (just in case the operators mean something else for other classes)
    inline friend Vector3_t<T> operator + (const T& s, const Vector3_t<T>& v) { return Vector3_t<T>(s + v.x, s + v.y, s + v.z); }; // s+x, s+y, s+z
    inline friend Vector3_t<T> operator - (const T& s, const Vector3_t<T>& v) { return Vector3_t<T>(s - v.x, s - v.y, s - v.z); }; // s-x, s-y, s-z
    inline friend Vector3_t<T> operator * (const T& s, const Vector3_t<T>& v) { return Vector3_t<T>(s * v.x, s * v.y, s * v.z); }; // s*x, s*y, s*z
    inline friend Vector3_t<T> operator / (const T& s, const Vector3_t<T>& v) { return Vector3_t<T>(s / v.x, s / v.y, s / v.z); }; // s/x, s/y, s/z

    // For Debugging
    inline friend std::ostream& operator << (std::ostream& out, const Vector3_t<T>& v) {
        return out << '(' << v.x << ", " << v.y << ", " << v.z << ')';
    }
};
typedef Vector3_t<float> Vector3, Color3;


  //////////////////
 // Mini Matrix4 //
//////////////////
struct Matrix4 {
    static constexpr float TO_RADIANS = 3.1415926535f / 180.0f; // hard-coded pi/180: Used in rotation

    float xx=1, xy=0, xz=0, xw=0,
          yx=0, yy=1, yz=0, yw=0,
          zx=0, zy=0, zz=1, zw=0,
          wx=0, wy=0, wz=0, ww=1;

    Matrix4() { // Default: Identity Matrix
        xx = 1; xy = 0; xz = 0; xw = 0;
        yx = 0; yy = 1; yz = 0; yw = 0;
        zx = 0; zy = 0; zz = 1; zw = 0;
        wx = 0; wy = 0; wz = 0; ww = 1;
    }

    // Same as multiply_mutate() but takes individual arguments and returns a new Vector3
    Vector3 multiply_new(float x, float y, float z) const {
        const float W = wx*x + wy*y + wz*z + ww;
        return Vector3(
            (xx*x + xy*y + xz*z + xw) / W,
            (yx*x + yy*y + yz*z + yw) / W,
            (zx*x + zy*y + zz*z + zw) / W
        );
    }

    // Note: Vector3 treated like a point, not a direction
    void multiply_mutate(Vector3& v) {
        const float W =          wx*v.x + wy*v.y + wz*v.z + ww;
        const float X = (float) (xx*v.x + xy*v.y + xz*v.z + xw) / W;
        const float Y = (float) (yx*v.x + yy*v.y + yz*v.z + yw) / W;
        const float Z = (float) (zx*v.x + zy*v.y + zz*v.z + zw) / W;
        v.x = X; v.y = Y; v.z = Z;
    }

    // Only used in rd_translate()
    void translate_mutate(const Vector3& v) {
        xw += xx*v.x + xy*v.y + xz*v.z;
        yw += yx*v.x + yy*v.y + yz*v.z;
        zw += zx*v.x + zy*v.y + zz*v.z;
        ww += wx*v.x + wy*v.y + wz*v.z;
    }

    // Only used in rd_scale()
    void scale_mutate(const Vector3& v) {
        xx *= v.x; xy *= v.y; xz *= v.z;
        yx *= v.x; yy *= v.y; yz *= v.z;
        zx *= v.x; zy *= v.y; zz *= v.z;
        wx *= v.x; wy *= v.y; wz *= v.z;
    }

    // Only used in rd_rotate_xy()
    void rotate_xy_mutate(float angle) { // in degrees
        angle *= TO_RADIANS;
        const float sine   = std::sin(angle);
        const float cosine = std::cos(angle);
        float
        Y = xy*cosine - xx*sine;  xx = xx*cosine + xy*sine;  xy = Y; 
        Y = yy*cosine - yx*sine;  yx = yx*cosine + yy*sine;  yy = Y; 
        Y = zy*cosine - zx*sine;  zx = zx*cosine + zy*sine;  zy = Y; 
        Y = wy*cosine - wx*sine;  wx = wx*cosine + wy*sine;  wy = Y;
    }

    // Only used in rd_rotate_yz()
    void rotate_yz_mutate(float angle) { // in degrees
        angle *= TO_RADIANS;
        const float sine   = std::sin(angle);
        const float cosine = std::cos(angle);
        float
        Z = xz*cosine - xy*sine;  xy = xy*cosine + xz*sine;  xz = Z;
        Z = yz*cosine - yy*sine;  yy = yy*cosine + yz*sine;  yz = Z;
        Z = zz*cosine - zy*sine;  zy = zy*cosine + zz*sine;  zz = Z;
        Z = wz*cosine - wy*sine;  wy = wy*cosine + wz*sine;  wz = Z;
    }

    // Only used in rd_rotate_zx()
    void rotate_zx_mutate(float angle) { // in degrees
        angle *= TO_RADIANS;
        const float sine   = std::sin(angle);
        const float cosine = std::cos(angle);
        float
        Z = xz*cosine + xx*sine;  xx = xx*cosine - xz*sine;  xz = Z;
        Z = yz*cosine + yx*sine;  yx = yx*cosine - yz*sine;  yz = Z;
        Z = zz*cosine + zx*sine;  zx = zx*cosine - zz*sine;  zz = Z;
        Z = wz*cosine + wx*sine;  wx = wx*cosine - wz*sine;  wz = Z;
    }
};



  ////////////
 // Lights //
////////////
// Only used as a way to store light data (currently only far lights or point lights)
struct light_data {
    Color3 color; // Color multiplied by Intensity
    Vector3 xyz; // Either position (point light) or direction (far light)
};



  //////////////////////
 // Global Variables //
//////////////////////
// Default colors of white, with no point/directional lights
static Color3 rd_global_current_color  = Color3(1, 1, 1);
static Color3 rd_global_ambient_color  = Color3(1, 1, 1);
static Color3 rd_global_specular_color = Color3(1, 1, 1);
static Color3 rd_global_background_color; // Default is set in rd_display

// An ambient coefficient, a diffuse coefficient, and a specular coefficient. Default values should be 1, 0, and 0 respectively
static float rd_global_ambient_coefficient  = 1.0f;
static float rd_global_diffuse_coefficient  = 0.0f;
static float rd_global_specular_coefficient = 0.0f;
static float rd_global_specular_exponent    = 10.0f; // Default exponent is 10

// Camera
static float   rd_global_camera_fov              = 90.0f; // Camera field of view in degrees (Default is 90)
static Vector3 rd_global_camera_eye_position     = Vector3(0, 0, 0); // A Vector3 treated like a point for the camera eyepoint (Default is 0,0,0)
static Vector3 rd_global_camera_look_at_position = Vector3(0, 0,-1); // A Vector3 treated like a point for the camera eyepoint (Default is 0,0,-1) (Note: 0,0,0 to 0,0,-1 means it's looking down)
static Vector3 rd_global_camera_up_direction     = Vector3(0, 1, 0); // A Vector3 for the camera up vector (Default is 0,1,0)

// The lights and any associated information (number of each type of light).
static std::vector<light_data> rd_far_lights;
static std::vector<light_data> rd_point_lights;
static Matrix4 current_transform = Matrix4(); // Global world transformation



  /////////
 // Ray //
/////////
struct Ray {
    Vector3 origin, direction;
    Ray(Vector3 _origin, Vector3 _direction) : origin(_origin), direction(_direction) {}

    // Methods defined later below
    inline bool hit() const;
    inline float hit_t() const;
    Color3 raycast(unsigned int depth) const;
};



  ///////////////////////
 // Material (Shader) //
///////////////////////
struct ShaderData { // A temporary way for me to pass data for shading each object
    Vector3 hit_position, hit_normal, ray_direction; // Could template as a Vector3_t<Vector3>? (nah, remembering the names will be confusing and I don't need all those functions)
};
struct Material { // A permanent way to store data for shading each object
    // Data (Defaults are global variables defined later below in constructor)
    Color3 current_color;
    Color3 ambient_color;
    Color3 specular_color;

    float ambient_coefficient;
    float diffuse_coefficient;
    float specular_coefficient;
    float specular_exponent;

    // A pointer to a function that takes an attributed point and returns the shaded color using this material. This is the surface_shader. 
    Color3 (Material::*surface_shader)(const ShaderData& sd, unsigned int depth) const;


    // Constructor defined later below
    Material();

    // Helper functions
    // Ambience
    Color3 get_ambience() const {
        return ambient_color * ambient_coefficient;
    };

    // Diffusion, lots of light loops
    Color3 get_diffusion(const ShaderData& sd) const {
        // Set up Normal and Surface Position
        Color3 return_color = Color3(0,0,0);

        // Far Lights
        for (light_data& far_light : rd_far_lights) {
            // Calculate and check angle
            float angle = sd.hit_normal.dot(far_light.xyz); // Angle (far_light.xyz is already negative for this very purpose to do dot product)
            if (angle <= 0) continue; // from 1 to -1, if less than 0 then facing away

            // Shadow (throw a ray and skip if it hits)
            if (Ray(sd.hit_position + sd.hit_normal*RDRAY_BIAS, far_light.xyz).hit()) continue; // Hacky Solution to "Shadow Acne": Multiplying position by a tiny normal

            // Accumulate
            return_color += far_light.color * angle;
        }

        // Point Lights
        for (light_data& point_light : rd_point_lights) {
            // Calculate Direction
            const Vector3 light_vector = point_light.xyz - sd.hit_position; // L = light_pos - surface_pos // Inverse with Light distance and direction
            const Vector3 light_direction = light_vector.normalized();

            // Calculate and check angle
            float angle = sd.hit_normal.dot(light_direction); // Normalize (inverse direction)
            if (angle <= 0) continue;

            // Shadow (throw a ray and skip if it doesnt hit)
            float light_mag2 = light_vector.mag2(); // For comparing with (x*x < mag2) instead of (x < sqrt(light))
            float ray_distance = Ray(sd.hit_position + sd.hit_normal*RDRAY_BIAS, light_direction).hit_t();
            if (ray_distance*ray_distance < light_mag2) continue;

            // Accumulate with intensity (angle * (1.f / light.mag2()))
            return_color += point_light.color * (angle / light_mag2); // Intensity (1/r^2) because point light strength fades)
        }

        // Multiply by diffusion coeffiecient
        return_color *= diffuse_coefficient;
        return return_color;
    }

    Color3 get_specular(const ShaderData& sd, unsigned int depth = 0) const {
        // Set up Normal and Surface Position
        Color3 return_color = Color3(0,0,0);

        // Far Lights
        for (light_data& far_light : rd_far_lights) {
            // Calculate and check angle
            Vector3 view_direction_inverse = (rd_global_camera_eye_position - sd.hit_position).normalize();
            Vector3 reflection_direction = far_light.xyz.reflection(sd.hit_normal); // (far_light.xyz dir already flipped)
            float angle = view_direction_inverse.dot(reflection_direction); 
            if (angle <= 0.0f) continue; // Check angle

            // Shadow check
            if (Ray(sd.hit_position + sd.hit_normal*RDRAY_BIAS, far_light.xyz).hit()) continue;

            // Calculate and Accumulate
            return_color += far_light.color * pow(angle, specular_exponent); // Intensity
        }

        // Point Lights
        for (light_data& point_light : rd_point_lights) {
            // Calculate Direction
            Vector3 light_vector = (sd.hit_position - point_light.xyz); // Inverse Light distance and direction
            Vector3 light_direction = light_vector.normalized(); // Normalize (direction)

            // Calculate and check angle
            Vector3 view_direction = (sd.hit_position - rd_global_camera_eye_position).normalize();
            Vector3 reflection_direction = light_direction.reflection(sd.hit_normal);
            float angle = view_direction.dot(reflection_direction);
            if (angle <= 0.0f) continue;

            // Shadow check
            float light_mag2 = light_vector.mag2();
            float ray_distance = Ray(sd.hit_position + sd.hit_normal*RDRAY_BIAS, -light_direction).hit_t();
            if (ray_distance*ray_distance < light_mag2) continue;

            // Calculate and Accumulate
            return_color += point_light.color * pow(angle, specular_exponent) / light_mag2; // Intensity (1/r^2) * (angle^exp)
        }

        // Reflection (EXPENSIVE)
        if (depth > 0) {
            float angle = sd.hit_normal.dot(-sd.ray_direction);
            if (angle > 0.0f)
                return_color += get_reflection(sd, depth) * angle; // acculumate by reflection_color * angle as
        }

        // Multiply by specular coefficient
        return_color *= specular_coefficient;
        return return_color;
    };

    // New: Ray Reflection (EXPENSIVE w/ recursion)
    Color3 get_reflection(const ShaderData& sd, unsigned int depth) const {
        // Calculate reflection by shooting ray (recursive)
        const Vector3 reflect_direction = (-sd.ray_direction).reflection(sd.hit_normal); // d - 2n*(d.n); // normalized (flipped for reflecting)
        Ray ray = Ray(sd.hit_position + sd.hit_normal*RDRAY_BIAS, reflect_direction); // Attempt to fix shadow acne shenanigans
        return ray.raycast(depth-1); // reflection color
    }


    // Shaders
    // Ambience and Diffusion
    Color3 matte(const ShaderData& sd, unsigned int depth) const {(void) depth; // Unused since only specular "reflects"
        Color3 return_color = current_color * (get_ambience() + get_diffusion(sd)); // matte = surface_color * (Ka * ambience  +  Kd * diffuse(normal)) // Ci = Os * Cs * (Ka * ambient() + Kd * diffuse(Nf)). (I think Os is surface opacity, but we're not using it right now) Note: I = Ka + Kd + Ks // I = ambient_coefficient + diffuse_coefficient + specular_coefficient
        return return_color.clamp_down_mutate(1.f); // Clamp down and return (inf:0 -> 1:0)
    };

    // Ambience and Specular (Same as matte, but with specular instead)
    Color3 metal(const ShaderData& sd, unsigned int depth) const {
        Color3 return_color = current_color * (get_ambience() + get_specular(sd, depth));  // metal = surface_color * (Ka * ambience  +  Kd * specular(normal))
        return return_color.clamp_down_mutate(1.f); // Clamp down and return
    };

    // Ambience, Diffusion, and Specular
    Color3 plastic(const ShaderData& sd, unsigned int depth) const {
        Color3 return_color = current_color  * (get_ambience() + get_diffusion(sd)) // plastic = surface_color  * (Ka * ambience  +  Kd * diffuse(normal))
                            + specular_color * get_specular(sd, depth);             //         + specular_color *  Ks * specular(normal)

        return return_color.clamp_down_mutate(1.f);
    };
};

// Example of usage:
// Material m; // Object Creation
// m.surface_shader = &Material::matte; // Set
// (m.*(m.surface_shader))(); // Use object (Used together with an object of its class)
// (m.*rd_global_surface_shader)(); // Use global
static Color3 (Material::*rd_global_surface_shader)(const ShaderData&, unsigned int depth) const = &Material::matte; // Default is matte
Material::Material() { // Default values are global values
    current_color        = rd_global_current_color;
    ambient_color        = rd_global_ambient_color;
    specular_color       = rd_global_specular_color;
    ambient_coefficient  = rd_global_ambient_coefficient;
    diffuse_coefficient  = rd_global_diffuse_coefficient;
    specular_coefficient = rd_global_specular_coefficient;
    specular_exponent    = rd_global_specular_exponent;
    surface_shader       = rd_global_surface_shader;
};



  ///////////////////////////////
 // Axis-Aligned Bounding Box //
///////////////////////////////
struct AABB {
    // Data
    Vector3 min = {}, max = {}; // Boundaries in world coordinates

    // Constructors
    AABB() = default; // Compiler Default
    AABB(const Vector3& MIN, const Vector3& MAX) : min(MIN), max(MAX) {} // Set directly
    AABB(float x0, float y0, float z0, float x1, float y1, float z1) : min({x0,y0,z0}), max({x1,y1,z1}) {} // From values

    // Methods (Theres probably a better way to do this)
    AABB expanded(const Vector3& point) const {
        return AABB( // return {box.min.min(point), box.max.max(point)}; // WIP
            std::min(min.x, point.x), std::min(min.y, point.y), std::min(min.z, point.z),
            std::max(max.x, point.x), std::max(max.y, point.y), std::max(max.z, point.z)
        );
    }

    AABB expanded(const AABB& box) const {
        return AABB( // return {box.min.min(min), box.max.max(max)}; // WIP
            std::min(min.x, box.min.x), std::min(min.y, box.min.y), std::min(min.z, box.min.z),
            std::max(max.x, box.max.x), std::max(max.y, box.max.y), std::max(max.z, box.max.z)
        );
    }

    float get_surface_area() const {
        const Vector3 SIZE = max - min;
        return 2.0f * (SIZE.x*SIZE.y + SIZE.y*SIZE.z + SIZE.z*SIZE.x);
    }

    bool is_touching(const AABB& box) {
        return box.min.x <= max.x  &&  min.x <= box.max.x
            && box.min.y <= max.y  &&  min.y <= box.max.y
            && box.min.z <= max.z  &&  min.z <= box.max.z;
    }

    bool is_inside(const AABB& big) {
        return big.min.x <= min.x  &&  max.x <= big.max.x
            && big.min.y <= min.y  &&  max.y <= big.max.y
            && big.min.z <= min.z  &&  max.z <= big.max.z;
    }

    // I'm sure there's an optimized version out there without so many if-branches hahaha
    float intersect(const Ray& ray) const {
        // Calculate t's
        const Vector3 inverse_direction = 1.0f / ray.direction; // Because multiplication is faster than division and no issues with comparing IEEE754 float -0 and 0
        float tminX = (min.x - ray.origin.x) * inverse_direction.x;
        float tmaxX = (max.x - ray.origin.x) * inverse_direction.x;
        float tminY = (min.y - ray.origin.y) * inverse_direction.y;
        float tmaxY = (max.y - ray.origin.y) * inverse_direction.y;

        // "Sort" t's in order
        if (tminX > tmaxX) std::swap(tminX, tmaxX);
        if (tminY > tmaxY) std::swap(tminY, tmaxY);
        if ((tminX > tmaxY) || (tminY > tmaxX)) return -1; // Quick check (no x/y intersection)

        // Z for start and end alpha (may even be -infinity to infinity)
        float tminZ = (min.z - ray.origin.z) * inverse_direction.z;
        float tmaxZ = (max.z - ray.origin.z) * inverse_direction.z;
        if (tminZ > tmaxZ) std::swap(tminZ, tmaxZ);

        // using std::min/max cuz its faster (https://stackoverflow.com/a/1826175, or same? https://stackoverflow.com/questions/15540250/in-c-is-it-better-to-cap-a-value-using-stdmin-or-an-if-branch)
        float t0 = std::max(tminX, tminY); // x/y clamp left
        float t1 = std::min(tmaxX, tmaxY); // x/y clamp right
        if ((t0 > tmaxZ) || (tminZ > t1)) return -1; // Check (no Z intersection)

        // Final "shortening" of the line
        t0 = std::max(t0, tminZ); // x/y OutIn z
        t1 = std::min(t1, tmaxZ); // x/y InOut z

        // Return closest t
        if (t0 >= 0) return t0;
        if (t1 >= 0) return t1; // If here then the ray might be coming from the inside, return -1 here for 1-sided (currently 2-sided)
        return -1; // Or somehow miss?
    }

    // Calculate Normal
    float intersect(const Ray& ray, Vector3& return_normal) const {
        const float t = intersect(ray);
        if (t >= 0) {
            // Calculate normal of bounding box
            const Vector3 v = ((ray.origin + ray.direction*t) - .5*(this->max + this->min)) / (this->max - this->min); // Relative Position ("Normalized"); normalize(hitpos - center)
            return_normal = Vector3(0,0,0); // Set others to 0
            if (v.x > v.y) // value of +-1 in the correct direction
                if (v.x > v.z)
                    return_normal.x = float((v.x>=0) - (v.x<0)); // x>=0?1:-1; // sign(x), 1 or -1 (changed x>0 to x>=0, to avoid 0), then in float format
                else // z > x > y
                    return_normal.z = float((v.z>=0) - (v.z<0));
            else
                if (v.y > v.z)
                    return_normal.y = float((v.y>=0) - (v.y<0));
                else // z > y > x
                    return_normal.z = float((v.z>=0) - (v.z<0));
        }
        return t;
    }
};



  ////////////
 // Object //
////////////
struct Object {
    Material material;

    Object() = default; // default constructor (requested from the compiler)
    virtual ~Object() = default; // virtual destructor

    virtual float intersect(const Ray& ray) const = 0; // Virtual method that every 'Object' has
    virtual float intersect(const Ray& ray, Vector3& return_normal) const = 0; // Same but returns with normal

    virtual AABB getAABB() const = 0; // New (returns an axis-aligned bounding box of this object)
};



  ////////////////////////////
 // Acceleration Structure //
////////////////////////////
class AccelerationTree {
private: // Internally, its all nodes
    struct Node { // static class only for this AccelerationTree class
        // Data
        AABB box; // Parent (this)
        std::vector<Node> children; // Mutually exclusive with object
        Object* object = nullptr; // Cannot exist if children.size() > 0;

        // Methods
        Node() = default; // Empty

        void clear() {
            if (children.size() == 0) {
                if (object != nullptr) {
                    delete object;
                    object = nullptr;
                }
            } else {
                for (Node& child : children) {
                    child.clear();
                }
                children.clear();
            }
        }

        // We know the size (new_box), but not where to put it yet (pointer new_object)
        void add(Object* new_object, const AABB& new_box) {
            if (children.size() == 0) {
                if (object == nullptr) { // Empty (Move)
                    this->object = new_object;
                    this->box = new_box;

                } else { // Single Object Node (object != nullptr)
                    // Move this node to children[0]
                    this->children.push_back(Node(*this)); // Copy
                    this->object = nullptr; // Pointer moved

                    // Add new_object to children[1]
                    Node new_node;
                    new_node.object = new_object;
                    new_node.box = new_box;
                    this->children.push_back(new_node); // Move

                    // Expand this box to fit both boxes above
                    this->box = this->box.expanded(new_node.box);
                }
            } else { // Has children (children.size() > 0)
                // Add according to least combined surface area
                float minDiff = this->box.expanded(new_box).get_surface_area();
                float parentDiff = minDiff - this->box.get_surface_area(); // Initial with parent (usually the biggest)

                // Search for smallest
                Node* minNode = this;
                for (Node& child : children) {
                    float childDiff = parentDiff + child.box.expanded(new_box).get_surface_area();
                    if (child.children.size() > 0) // If this is not an object (i.e. has children)
                        childDiff -= child.box.get_surface_area();

                    if (childDiff < minDiff) { // Update if smaller
                        minNode = &child;
                        minDiff = childDiff;
                    }
                }

                // If parent ended up with the least difference, add as a new child to children[]
                this->box = this->box.expanded(new_box); // Expand parent (efficient if it's this, otherwise not so much)
                if (minNode == this) {
                    Node new_node;
                    new_node.object = new_object;
                    new_node.box = new_box;
                    this->children.push_back(new_node);
                } else { // Else, continue recurse in the smallest child to find the best spot
                    minNode->add(new_object, new_box);
                }
            }
        }

        Object* raycast(const Ray& ray, Vector3& hit_position, Vector3& hit_normal, float& t) const {
            // Check if this even hits
            if (box.intersect(ray) <= 0)
                return nullptr; // No Hit

            // If object node, then finally check if it actually hits the object
            if (object != nullptr) {
                t = object->intersect(ray, hit_normal);
                if (0 <= t && t < 1e300) { // Hit
                    hit_position = ray.origin + ray.direction*t;
                    return object;
                } else {
                    return nullptr; // Miss (after all that...)
                }
            }

            // Else, check all the children and return the closest child
            float closest = 1e300; // Infinity (huge double casted down to float) // (unsigned) ~0); doesnt work, infinity (bitwise not 0x00..00 != 0x7F800000, which is infinity in IEEE 547)
            Vector3 position, normal; // Other temp return variables
            Object* hit_object = nullptr; // For closest
            for (const Node& child : children) {
                Object* obj = child.raycast(ray, position, normal, t);
                if (obj == nullptr || t < 0 || t >= closest) continue; // No intersection (yet) or it's farther

                // Hit (and/or closer), save and continue searching
                hit_normal = normal; // Normal should've been calculated by the object's intesect()
                hit_position = position;
                hit_object = obj;
                closest = t;
            }

            // Done, return closest data (if any)
            t = closest;
            return hit_object;
        }

        // A simpler faster version just to check if we will hit anything (only care where)
        // Returns infinity if no hit
        float hit_t(const Ray& ray) const {
            if (box.intersect(ray) <= 0) return 1e300; // Check if this even hits

            // If object node, then check if it actually hits the object
            if (object != nullptr) {
                float t = object->intersect(ray);
                if (0 <= t) // Hit
                    return t;
                else
                    return 1e300; // Miss (after all that...)
            }

            float tX = 1e300; // Closest data
            for (const Node& child : children) {
                float t = child.hit_t(ray);
                if (t >= 0 && t < tX) // If not out of bounds or farther that any previous
                    tX = t; // Save and continue searching
            }

            return tX; // No hit if tX >= 1e300 (infinity)
        }

        // The simplest, fastest version that just checks if anything is hit (dont care where)
        bool hit(const Ray& ray) const {
            if (box.intersect(ray) <= 0) return false; // Check if this even hits

            if (object != nullptr) return object->intersect(ray) >= 0; // If object node, then check if it actually hits the object

            for (const Node& child : children)
                if (child.hit(ray))
                    return true; // Intersection (somewhere)

            return false; // No hits
        }
    };

    // Data
    Node root;

public:
    inline void add(Object* obj) {
        root.add(obj, obj->getAABB());
    }

    // Returns hit_object, hit_position, and hit_normal (const because this is READ ONLY. No Touchy!)
    inline const Object* raycast(const Ray& ray, Vector3& return_hit_position, Vector3& return_hit_normal) const {
        float _;
        return root.raycast(ray, return_hit_position, return_hit_normal, _);
    }

    // A simpler faster version just to check if we will hit anything (only care where)
    inline float hit_t(const Ray& ray) const {
        return root.hit_t(ray); // Returns infinity if no hit
    }

    // The simplest, fastest version that just checks if anything is hit (dont care where)
    inline bool hit(const Ray& ray) const {
        return root.hit(ray); // All work done in acceleration_tree.hit();
    }

    inline void clear() {
        root.clear();
    }
};



  ////////////
 // Sphere //
////////////
struct Sphere : Object {
    Vector3 position; // In world coordinates
    float radius;

    Sphere(Vector3 _position, float _radius) : position(_position), radius(_radius) {}

    // Geometric Method (less operations)
    float intersect(const Ray& ray) const override { // override keyword is optional, but helps generate errors when this is used incorrectly
        Vector3 dir = this->position - ray.origin;
        float tCA = dir.dot(ray.direction); // tCA is closest approach (assuming ray.direciton is normalized)
        // if (tCA < 0) return -1; // Maybe?
        float m2 = dir.mag2() - tCA*tCA;
        float r2 = radius*radius;
        if (m2 > r2) return -1; // Out of bounds

        float tHC = std::sqrt(r2 - m2); // tHC is the half coord
        float t = tCA - tHC;
        if (t < 0) {
            t = tCA + tHC;
            if (t < 0) return -1; // Both t0 and t1 are negative.
        }
        return t;
    }

    // Analytical Method (Quadratic)
    float intersect2(const Ray& ray) const {
        // Quadratic Formula Setup: x = (-b +-sqrt(b^2 - 4ac)) / 2a
        Vector3 u = ray.origin - position;
        float a = ray.direction.mag2();
        float b = 2 * ray.direction.dot(u);
        float c = u.mag2() - radius*radius;
        float d = b*b - 4*a*c; // Discriminant
        if (d < 0) return -1; // Imaginary Solution

        // Calculate Quadratic Formula
        float t0, t1;
        if (d == 0) { // Quick answer (also because the "Citardauq" method breaks when this is 0)
            t0 = t1 = -0.5f * b/a; // t0 == t1
        } else {
            d = std::sqrt(d);
            d = -0.5f * (b>0 ? b+d : b-d);
            t0 = d/a;
            t1 = c/d; // Genius: https://en.wikipedia.org/wiki/Quadratic_formula#Square_root_in_the_denominator
        }

        // Check Interpolation of closest t
        if (t0 < t1) { // otherwise this is negative, it's backward, continue to try the other one (if you want the back part to be rendered)
            if (t0 >= 0) // if t0 is negative then so is t1
                return t0;
        } else if (t1 >= 0) // Sanity check if both are negative (behind ray)
            return t1; // only t1 is positive
        return -1; // Both negative
    }

    // Same but returns with normal
    float intersect(const Ray& ray, Vector3& return_normal) const override {
        const float t = intersect(ray);
        if (t >= 0) // origin + direction*t // xyz set to hit position (then calculate normal and save)
            return_normal = ((ray.origin + ray.direction*t) - this->position).normalized();
        return t;
    }

    // Sphere  bounds
    AABB getAABB() const override {
        const Vector3 HALF(radius, radius, radius);
        return AABB(position - HALF, position + HALF);
    }
};



  /////////////
 // Polygon //
/////////////
struct Polygon : Object {
    // Usually 3, but can be more as long as they are FLAT.
    std::vector<Vector3> vertices; // Using std::vector cuz its easier to work with.

    float intersect(const Ray& ray, Vector3& return_normal) const override {
        // Calculate normal of polygon (hacky-ish method)
        const Vector3& LAST = vertices.back(); // In a triangle this will be [2], else [3]
        return_normal = ((vertices[0]-vertices[2]).crossed(vertices[1]-LAST)).normalize();

        // Get position using t from plane equation: Ax + By + Cz + D = 0 and t = (o-p).n / n.d
        float t = -(ray.origin - LAST).dot(return_normal) / ray.direction.dot(return_normal);
        if (t < 0) return -1; // Quick check if polygon is behind ray

        // Flatten 3D xyz to a 2D dimension uv (whichever is smallest for accuracy)
        int ui, vi; // 0(x), 1(y), or 2(z)
        float X = std::abs(return_normal.x);
        float Y = std::abs(return_normal.y);
        float Z = std::abs(return_normal.z);
        if (X > Y)
            if (X > Z) // X is largest, flatten to YZ
                ui = 1, vi = 2; // Y, Z (u=Y, v=Z)
            else // Z (z > x > y)
                ui = 0, vi = 1; // X, Y
        else
            if (Y > Z) // Y (y >= xz)
                ui = 0, vi = 2; // X, Z
            else // Z again (z > y > x)
                ui = 0, vi = 1; // X, Y (again)

        // Main loop Setup (for Last-First)
        float u0, v0, u1, v1; // to check if crossed negative
        const Vector3 HIT_POSITION = ray.origin + (t * ray.direction);
        Vector3 p = LAST - HIT_POSITION; // Back vertex LAST = this->vertices.back();
        u1 = p.xyz[ui]; // u0
        v1 = p.xyz[vi]; // v0

        // Main Inside/Outside Loop Check
        bool inside = false; // false (even) means not in polygon, true (odd) means is in polygon
        for (const Vector3& vertex : vertices) {
            u0 = u1, v0 = v1; // Continued from the previous loop
            p = vertex - HIT_POSITION; // shift so that HIT_POSITION is the origin (and one of these wont be used...)
            u1 = p.xyz[ui];
            v1 = p.xyz[vi];

            // Quick checks if line definitely WONT cross
            if ((u0 <  0 && u1 <  0) || // both u's are either Q2 or Q3
                (v0 <  0 && v1 <  0) || // Line doesn't even cross the u boundary (both under)
                (v0 >= 0 && v1 >= 0))   // ... (both over)
                continue;

            // Final check, as one of them is behind and another in front
            // -vB * (uA - uB) / (vA - vB) + uB = uE (if positive, crossed boundary)
            if (v1 * (u0 - u1) / (v0 - v1) <= u1) // Check if crossed a line in front of us
                inside = !inside; // Toggle (crossed)
        }

        // After all that, if inside polygon, return t
        if (inside)
            return t; // normal is valid here
        else
            return -1;
    }

    // Same, but normal is unused
    float intersect(const Ray& ray) const override { Vector3 _; return intersect(ray, _); };

    // Polygon bounds
    AABB getAABB() const override {
        // Start from the first 2 points
        AABB aabb(vertices[0], vertices[1]);

        // Expand from adding the others and return
        for (size_t i = 2, I = vertices.size(); i < I; i++)
            aabb = aabb.expanded(vertices[i]);

        return aabb;
    }
};



  /////////////
 // PolySet //
/////////////
struct PolySet : Object {
    std::vector<Polygon> faces; // Doing it like this because it's easier to work with. Side Effect: Duplicate materials and vertices :(

    float intersect(const Ray& ray, Vector3& return_normal) const override {
        float tX = 1e300; // Closest t

        // Get closest intersected point on polygon
        for (const Polygon& face : faces) {
            Vector3 normal; // Return value for intersect_face()
            const float t = face.intersect(ray, normal); // Most of the work is done here
            if (t >= 0 && t < tX) {
                return_normal = Vector3(normal); // Copy normal at that closest t and continue
                tX = t;
            }
        }

        if (tX >= 1e300) // If no hit (infinity)
            return -1; // return_normal is invalid here
        else
            return tX;
    };

    // Same, but normal is unused
    float intersect(const Ray& ray) const override { Vector3 _; return intersect(ray, _); };

    // PolySet bounds
    AABB getAABB() const override {
        // Start from the first face
        AABB aabb = faces[0].getAABB();

        // Expand from adding the others and return
        for (size_t i = 1, I = faces.size(); i < I; i++)
            aabb = aabb.expanded(faces[i].getAABB());

        return aabb;
    }
};



  //////////
 // Cube //
//////////
struct Cube : PolySet {
    // Basically a polyset of 6 hardcoded faces
    Cube(const Matrix4& transformation) {
        // 8 Vertices
        const Vector3 A = transformation.multiply_new(-1,-1,-1);
        const Vector3 B = transformation.multiply_new(-1,-1, 1);
        const Vector3 C = transformation.multiply_new(-1, 1,-1);
        const Vector3 D = transformation.multiply_new(-1, 1, 1);
        const Vector3 E = transformation.multiply_new( 1,-1,-1);
        const Vector3 F = transformation.multiply_new( 1,-1, 1);
        const Vector3 G = transformation.multiply_new( 1, 1,-1);
        const Vector3 H = transformation.multiply_new( 1, 1, 1);

        // 6 Faces initialized Counter Clockwise (hard coded)
        faces.resize(6);               // Face   ( Normal )
        faces[0].vertices = {F,H,D,B}; // Top    ( 0, 0, 1) +Z
        faces[1].vertices = {G,E,A,C}; // Bottom ( 0, 0,-1) -Z
        faces[2].vertices = {H,G,C,D}; // Left   ( 0, 1, 0) +Y
        faces[3].vertices = {F,B,A,E}; // Right  ( 0,-1, 0) -Y
        faces[4].vertices = {H,F,E,G}; // Front  ( 1, 0, 0) +X
        faces[5].vertices = {C,A,B,D}; // Back   (-1, 0, 0) -X
    }
};



  /////////
 // Ray //
/////////
static AccelerationTree acceleration_tree; // Initialized below in rd_render_init()

// The simplest, fastest version that just checks if anything is hit (dont care where)
inline bool Ray::hit() const {
    return acceleration_tree.hit(*this); // All work done in acceleration_tree.hit();
}

// A simpler faster version just to check if we will hit anything (only care where)
// Returns infinity if no hit
inline float Ray::hit_t() const {
    return acceleration_tree.hit_t(*this); // All work done in acceleration_tree.hit_t();
}

Color3 Ray::raycast(unsigned int depth = 0) const {
    // Shoot ray using acceleration tree (holding all our objects in boxes)
    Vector3 hit_position, hit_normal; // return variables
    const Object* hit_object = acceleration_tree.raycast(*this, hit_position, hit_normal);
    if (hit_object == nullptr)
        return rd_global_background_color; // Plot background color if the ray doesnt hit anything

    // Hit! pack hit info for the shader
    ShaderData sX;
    sX.hit_normal = hit_normal;
    sX.hit_position = hit_position;
    sX.ray_direction = this->direction; // (direction for reflection in shader)

    // Call shader to calculate and return the color
    Material mX = hit_object->material; // Material of the object hit (for shading)
    return (mX.*(mX.surface_shader))(sX, depth); // return (mX.*rd_global_surface_shader)(sX, reflections);
}



  ///////////////////////////
 // RD Ray Tracing Engine //
///////////////////////////
int RERay::rd_display(const string& name, const string& type, const string& mode) { (void)name; (void)type; (void)mode; // Unused Variables
    rd_global_background_color = Color3(0,0,0); // If u want to set a default background, do it here
    return RD_OK;
}

int RERay::rd_format(int xresolution, int yresolution) { (void)xresolution; (void)yresolution; // Unused Variables
    return RD_OK; // Ignore for now
}

static int rd_frame_number = 0;
int RERay::rd_world_begin() {
    return rd_disp_init_frame(rd_frame_number); // frame number used here?
}

unsigned int rd_global_depth = 0; // Default of 0
int RERay::rd_world_end() {
    // Calculate camera_to_world
    Vector3 F = rd_global_camera_look_at_position - rd_global_camera_eye_position; // 1. Direction the camera is looking (at - eye)
    F.normalize(); // 2. Normalize it
    Vector3 R = F.crossed(rd_global_camera_up_direction); // 3. Direction of the "right" side of the camera
    R.normalize(); // 4. Normalize it
    Vector3 U = R.crossed(F); // 5. Direction of the "top" head of the camera
    U.normalize(); // 6. Normalize it (Optional, since cross of normals is also normal, but squashes risk of floating errors)

    // Calculate device_to_camera (Pretty sure these only need to be computed once)
    const float FOV = std::tan(rd_global_camera_fov * (std::acos(-1)/360.0f));
    F = R*(float(-display_xSize/2)) // Left to middle
      + U*(float( display_ySize/2)) // Top  to middle
      + F*(float( display_ySize/2)/FOV); // fov/2 to radians (depth away from center of "camera")

    // Raycasting from each pixel of the buffer (Device Coordinates)
    for (    int y = 0; y < display_ySize; y++) {
        for (int x = 0; x < display_xSize; x++) {

            // Camera To World: Generate ray from device coordinates to world coordinates (as camera)
            const Vector3 dir = (x*R - y*U + F).normalize();
            const Ray ray = Ray(rd_global_camera_eye_position, dir);
            const Color3 color = ray.raycast(rd_global_depth); // Cast
            rd_write_pixel(x, y, color.rgb);
        }
    }

    return RD_OK;
}

int RERay::rd_frame_begin(int frame_no) {
    rd_frame_number = frame_no;
    return RD_OK;
}

int RERay::rd_frame_end() {
    return RD_OK;
}

int RERay::rd_render_init() {
    acceleration_tree = {}; // Empty init
    return RD_OK;
}

int RERay::rd_render_cleanup() {
    // Clean up: Important! Prevent memory leaks (might happen anyway when out of this scope, but still...)
    acceleration_tree.clear();
    return RD_OK;
}



  ////////////
 // Camera //
////////////
int RERay::rd_camera_eye(const float eyepoint[3]) {
    rd_global_camera_eye_position = Vector3(eyepoint);
    return RD_OK;
}

int RERay::rd_camera_at(const float atpoint[3]) {
    rd_global_camera_look_at_position = Vector3(atpoint);
    return RD_OK;
}

int RERay::rd_camera_up(const float up[3]) {
    rd_global_camera_up_direction = Vector3(up);
    return RD_OK;
}

int RERay::rd_camera_fov(float fov) {
    rd_global_camera_fov = fov;
    return RD_OK;
}



  /////////////////////
 // Transformations //
/////////////////////
int RERay::rd_translate(const float offset[3]) {
    current_transform.translate_mutate(offset);
    return RD_OK;
}

int RERay::rd_scale(const float scale_factor[3]) {
    current_transform.scale_mutate(scale_factor);
    return RD_OK;
}

int RERay::rd_rotate_xy(float angle) { // in degrees
    current_transform.rotate_xy_mutate(angle);
    return RD_OK;
}

int RERay::rd_rotate_yz(float angle) {
    current_transform.rotate_yz_mutate(angle);
    return RD_OK;
}

int RERay::rd_rotate_zx(float angle) {
    current_transform.rotate_zx_mutate(angle);
    return RD_OK;
}

// Stack
std::vector<Matrix4> transformation_stack = {};
int RERay::rd_xform_push() {
    transformation_stack.push_back(current_transform); // stack will make a copy automatically so it should be fine
    return RD_OK;
}

int RERay::rd_xform_pop() {
    if (transformation_stack.empty()) // Return error if empty
        return RD_INPUT_TRANSFORM_STACK_UNDERFLOW;

    current_transform = transformation_stack.back();
    transformation_stack.pop_back();
    return RD_OK;
}


  ///////////////////////
 // Geometric Objects //
///////////////////////
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
int RERay::rd_polyset(const string& vertex_type, int nvertex, const vector<float>& vertex, int nface, const vector<int>& face) { (void)vertex_type; (void)nface; // Unused Variables
    // Safety checks (should probably uncomment them if theres a way to draw points/lines with raytracing. Currently only expecting groups of 3)
    // if (nvertex <  1) return RD_OK; // Nothing
    // if (nvertex == 1) return Point(vertex[0], vertex[1], vertex[2]);
    // if (nvertex == 2) return Line(vertex[0],vertex[1],vertex[2],  vertex[3],vertex[4],vertex[5],);
    if (nvertex < 3) return RD_OK; // Atleast 3 vertices (btw this is def NOT RD_OK hahaha)

    // Read and transform vertices
    std::vector<Vector3> VERTICES(nvertex); // We know the size, allocate that temporarily on the stack right away and exactly
    for (size_t i = 0, n = 0, N = nvertex*3; n < N; i++, n += 3) // Skip by 3's for accessing
        VERTICES[i] = current_transform.multiply_new(vertex[n], vertex[n+1], vertex[n+2]);

    // Create Polyset (if face starts with any amount of -1's this doesnt work)
    PolySet* polyset = new PolySet(); // Allocate new memory for PolySet (for polymorphism)
    Polygon temp_face = {}; // Accumulate faces to this, then face at -1
    for (int vertex_id : face)
        if (vertex_id < 0) { // -1: Save
            temp_face.vertices.shrink_to_fit(); // Done resizing;
            polyset->faces.push_back(Polygon(temp_face)); // Save Copy
            temp_face = {}; // Clear for next
        } else
            temp_face.vertices.push_back(VERTICES[vertex_id]); // Add vertex from id and continue

    // Save to global list and return
    polyset->faces.shrink_to_fit();
    acceleration_tree.add(polyset);
    return RD_OK;
}


// Basically a PolySet of 6 faces
int RERay::rd_cube() {
    acceleration_tree.add(new Cube(current_transform)); // Let the constructor deal with it
    return RD_OK;
}

int RERay::rd_sphere(float radius, float zmin, float zmax, float thetamax) { (void)zmin; (void)zmax; (void)thetamax; // Unused Variables
    const Vector3 world_center(current_transform.xw, current_transform.yw, current_transform.zw); // Center and Radius (object to world)
    const float world_radius = (current_transform.multiply_new(radius,0,0) - world_center).mag(); // Get magnitude from the World Vector of any point on the sphere (world_vector = world_random_point - world_center)
    acceleration_tree.add(new Sphere(world_center, world_radius)); // Add position (xyz) and radius (r) to global list of objects // (hacky?)
    return RD_OK;
}



  //////////////////////////
 // Lighting and Shading //
//////////////////////////
// Stores the current color values to be used as the drawing color.
int RERay::rd_color(const float color[]) {
    rd_global_current_color = Color3(color); // Store new color values
    return RD_OK;
}

// Sets the color values to be used as the screen background color.
int RERay::rd_background(const float color[]) {
    rd_global_background_color = Color3(color);
    return rd_set_background(color);
}

int RERay::rd_surface(const string& shader_type) {
    if (shader_type == "matte")
        rd_global_surface_shader = &Material::matte;
    else if (shader_type == "plastic")
        rd_global_surface_shader = &Material::plastic;
    else if (shader_type == "metal")
        rd_global_surface_shader = &Material::metal;

    return RD_OK;
}

int RERay::rd_point_light(const float pos[3], const float color[], float intensity) {
    light_data l; // Intensity parameters for each light should simply be multiplied by the color before being stored in the light color.
    l.color = Color3(color) * intensity;
    l.xyz = Vector3(pos);
    rd_point_lights.push_back(l); // Add data
    return RD_OK;
}

int RERay::rd_far_light(const float dir[3], const float color[], float intensity) {
    light_data l; // Intensity parameters for each light should simply be multiplied by the color before being stored in the light color.
    l.color = Color3(color) * intensity;
    l.xyz = -Vector3(dir).normalize(); // Normalize just in case (and negative for dot_product when looping)
    rd_far_lights.push_back(l); // Add data
    return RD_OK;
}

int RERay::rd_ambient_light(const float color[], float intensity) {
    rd_global_ambient_color = Color3(color) * intensity;
    return RD_OK;
}

int RERay::rd_specular_color(const float color[], int exponent) {
    rd_global_specular_exponent = float(exponent);
    rd_global_specular_color = Color3(color);
    return RD_OK;
}

int RERay::rd_k_ambient(float Ka) {
    rd_global_ambient_coefficient = Ka;
    return RD_OK;
}

int RERay::rd_k_diffuse(float Kd) {
    rd_global_diffuse_coefficient = Kd;
    return RD_OK;
}

int RERay::rd_k_specular(float Ks) {
    rd_global_specular_coefficient = Ks;
    return RD_OK;
}

int RERay::rd_option_real(const string& name, float value) {
    if (name == "Levels")
        rd_global_depth = value;

    return RD_OK;
}