#ifndef RD_ENGINE_DIRECT_H
#define RD_ENGINE_DIRECT_H

#include "rd_enginebase.h"

#include "Matrix4.h" // Brings Vector3 and Point4 with it as well

#include <stack> // Only for the tranformation stack
#include <vector> // For the z_buffer
#include <queue> // For the line_pipeline
#include <string>


// Edge (only used in scan_conversion)
// An edge structure is needed to hold the all the data associated with an edge that is scan converted. It should contain:
struct Edge {
    int yLast;      // Final scan line of edge
    attr_point p;   // The values of the edge on this scan line
    attr_point inc; // The incremental changes for the values from scanline to scanline

    Edge* next;     // A pointer to link the edges together in edge tables.
};

struct light_data { // Only used as a way to store light data (currently only far light and point lights
    float rgb[3]; // Color multiplied by Intensity
    Vector3 xyz; // Either position (point light) or direction (far light)
};


// These are global because having them as members is a pain rn
attr_point surface_point_values; // An attributed point, surface_point_values which is set with interpolated values during polygon scan conversion. These values are used during lighting calculations.

// An ambient coefficient, a diffuse coefficient, and a specular coefficient. Default values should be 1, 0, and 0 respectively.
float ambient_coefficient, diffuse_coefficient, specular_coefficient; // Global lighting

// Four flags. Three of these indicate the existence of information associated with each attributed point vertex of a polygon
bool _vertex_color_flag, _vertex_normal_flag, _vertex_texture_flag, vertex_interpolation_flag; // The fourth flag is an interpolation flag that determines whether or not interpolated values are used in lighting calculations.

Vector3 viewing_vector; // This is the direction from a surface point, to the eye and is calculated for each surface point processed, as needed.
Vector3 polygon_normal; // This is the surface normal for a polygon. It may or may not be used depending on whether a constant polygon normal value is used for lighting or an interpolated normal value is used.
// Note: Couldn't polygon_normal just be in polygon_pipeline(..., true)?

// The lights and any associated information (number of each type of light).
float ambient_light[3];
std::vector<light_data> far_lights;
std::vector<light_data> point_lights;
float surface_color[3];  // A surface color. This will take over the role of the current drawing color. It should have a default value of white.
float specular_color[3]; // A specular color. This is the color that will be used in specular lighting calculations. It has a default value of white.
float specular_exponent; // A specular exponent. The default value is 10.0.

// A pointer to a function that takes a reference to a color and returns void. This is the surface_shader. 
// You should have three functions, matte, metal, and plastic each of which takes a reference to a color and returns void. 
// The function pointer surface_shader should have a default value of the matte function.
void (*surface_shader)(float color[3]); // Default is matte
void matte(float color[3]);
void metal(float color[3]);
void plastic(float color[3]);

// Only public global because its used in specular calculation
Vector3 camera_eye_position;     // A Vector3 treated like a point for the camera eyepoint (Default is 0,0,0)
Vector3 camera_look_at_position; // A Vector3 treated like a point for the camera eyepoint (Default is 0,0,-1) (Note: 0,0,0 to 0,0,-1 means it's looking down)
Vector3 camera_up_direction;     // A Vector3 for the camera up vector (Default is 0,1,0)


// This is a rendering engine that renders directly to the image buffer as
// primitives come in.  A depth buffer is obviously needed. Transparency is
// not properly handled.
class REDirect: public RenderEngine
{
private:
    // Misc
    int frame_number;

    // Global Transformation Variables
    Matrix4 GARBAGE; // if this is removed then current_transform is hit with garbage and nothing works and I dont know why (this appears to 'tank' the garbage)
    Matrix4 current_transform;        // Transformation matrix for the object to world transformation
    Matrix4 world_to_clip_transform;  // Transformation matrix for the world to clipping coordinates transformation
    Matrix4 clip_to_device_transform; // Transformation matrix for the clipping to device coordinates transformation.
    Matrix4 normal_transform;         // All polygon normals must be transformed by a normal transformation matrix. This is similar to the current transformation matrix.

    std::stack<Matrix4> stack; // The stack of transformation matrices that hold the values of the current modeling transformation as it changes.

    // Global Camera Variables
    float camera_fov;  // Camera field of view in degrees (Default is 90)
    float camera_near; // Camera near clipping depth (Default is 1)
    float camera_far;  // Camera far clipping depth (Default is 1 billion)

    std::vector<float> z_buffer; // Depth buffer respesenting a 2d screen X*Y, accessed with (y*X + x)

public:

    Edge* edge_table; // Only used in scan_conversion()

    float current_color[3];  // Current RD color
    
    /**********************    Helper functions  *******************************/

    // Plots a point using the depth buffer
    // x and y are screen coordinates, z is the screen 'depth' from 0.0 to 1.0, with 0 being the closest
    // True if plotted, else false and nothing happened
    bool plot(int x, int y, float z, const float color[3]);

    // The point pipeline should take a homogeneous point 
    // and transform it by the current transform and the world to clipping transform. 
    // At this stage the resulting point (in normalized clipping coordinates) 
    // should be tested to see if it lies in the canonical view volume (from 0 to 1 in x, y, and z). 
    // If it passes this test, the point is converted to device coordinates and plotted.
    void point_pipeline(float x, float y, float z);

    // This draws a line passing the points
    // Note: works with 3 or more consecutive lines that end at start
    //       else if drawing with no end (or like 1 or 2 lines), 
    //       use line_pipeline() then just line_pipeline(false) without "finishing")
    // Same as above, but with start flag
    // 
    // if null: will save this point 
    // if false: will finish drawing (draws lines backward)
    // if true: will draw back to start, from last p to p, then p to start (draws twice!)
    //
    // Draws a line from last xy used to (x,y) on device coordinates
    void line_pipeline(Vector3 v); // Plot
    void line_pipeline(Vector3 v, bool finish_with_face); // End and draw. If true, also draws from end to start
    void line_pipeline_GOTO(float x, float y, float z, bool finish_with_face);

    void polygon_pipeline(float x, float y, float z);
    void polygon_pipeline(float x, float y, float z, bool finish_and_draw);
    void polygon_pipeline(Vector3 v); // Save point (identical to calling it with false)
    void polygon_pipeline(Vector3 v, bool finish_and_draw); // Save point or End and draw.


    /**********************   General functions  *******************************/

    virtual int rd_display(const string& name, const string& type, 
        const string& mode);

    virtual int rd_format(int xresolution, int yresolution);

    virtual int rd_world_begin(void);
    virtual int rd_world_end(void);

    virtual int rd_frame_begin(int frame_no);
    virtual int rd_frame_end(void);

    virtual int rd_render_init(void);  /* Initialize renderer */
    virtual int rd_render_cleanup(void);

    /**********************   Camera  ******************************************/

    virtual int rd_camera_eye(const float eyepoint[3]);
    virtual int rd_camera_at(const float atpoint[3]);
    virtual int rd_camera_up(const float up[3]);
    virtual int rd_camera_fov(float fov);
    virtual int rd_clipping(float znear, float zfar);

    /**********************   Transformations **********************************/

    virtual int rd_translate(const float offset[3]);
    virtual int rd_scale(const float scale_factor[3]);
    virtual int rd_rotate_xy(float angle);
    virtual int rd_rotate_yz(float angle);
    virtual int rd_rotate_zx(float angle);
    virtual int rd_matrix(const float * mat);

    virtual int rd_xform_push(void);
    virtual int rd_xform_pop(void);

    /**********************   Geometric Objects  *******************************/

    virtual int rd_bezier_curve(const string& vertex_type,
        int degree, const vector<float>& vertex);

    virtual int rd_bezier_patch(const string& vertex_type,
        int u_degree, int v_degree, 
        const vector<float>& vertex);

    virtual int rd_catmull_clark_sds(const string& vertex_type,
        const vector<float>& coord, int nvertex,
        const vector<int>& vertex_list, int nface,
        const vector<int>& crease_list, int ncrease,
        const vector<float>& sharpness);

    virtual int rd_circle(const float center[3], float radius);

    virtual int rd_line(const float start[3], const float end[3]);

    virtual int rd_lineset(const string& vertex_type,
        int nvertex, const vector<float>& vertex,
        int nseg, const vector<int>& seg);

    virtual int rd_point(const float p[3]);

    virtual int rd_pointset(const string& vertex_type,
        int nvertex, const vector<float>& vertex);
    virtual int rd_polyset(const string& vertex_type, 
        int nvertex, const vector<float>& vertex,
        int nface,   const vector<int>& face);

    virtual int rd_cone(float height, float radius, float thetamax);
    virtual int rd_cube(void);
    virtual int rd_cylinder(float radius, float zmin, 
        float zmax, float thetamax);
    virtual int rd_disk(float height, float radius, float theta);

    virtual int rd_hyperboloid(const float start[3], const float end[3], 
        float thetamax); 

    virtual int rd_paraboloid(float rmax, float zmin, 
        float zmax, float thetamax);
    virtual int rd_sphere(float radius, float zmin, float zmax, float thetamax);
    virtual int rd_sqsphere(float radius, float north, float east, 
        float zmin, float zmax, float thetamax); 
    virtual int rd_sqtorus(float radius1, float radius2, 
        float north, float east, float phimin, float phimax, 
        float thetamax);
    virtual int rd_torus(float radius1, float radius2, 
        float phimin, float phimax, float thetamax);
    virtual int rd_tube(const float start[3], const float end[3], float radius);



    /********************  Lighting & Shading  ***************************/

    virtual int rd_background(const float color[]);
    // red, green, blue by default

    virtual int rd_color(const float color[]);

    virtual int rd_opacity(float opacity);

    virtual int rd_emission(const float color[], float intensity);

    virtual int rd_fill(const float seed_point[3]);

    virtual int rd_surface(const string& shader_type);

    virtual int rd_cone_light(const float pos[3], const float at[3], 
        float theta_min, float theta_max,
        const float color[], float intensity);

    virtual int rd_point_light(const float pos[3], 
        const float color[], float intensity);

    virtual int rd_far_light  (const float dir[3], 
        const float color[], float intensity);

    virtual int rd_ambient_light(const float color[], float intensity);


    virtual int rd_specular_color(const float color[], int exponent);

    virtual int rd_k_ambient(float Ka);
    virtual int rd_k_diffuse(float Kd);
    virtual int rd_k_emission(float Ke);
    virtual int rd_k_specular(float Ks);

    virtual int rd_attribute_push(void);
    virtual int rd_attribute_pop(void);

    /****************************  Mapping  ******************************/

    virtual int rd_map_border(const string& map_type,
        const string& horizontal, 
        const string& vertical);
    virtual int rd_map_bound(const string& map_type,
        float s_min, float t_min, 
        float s_max, float t_max);
    virtual int rd_map_load(const string& filename, 
        const string& label);
    virtual int rd_map_sample(const string& map_type,
        const string& intra_level, 
        const string& inter_level);
    virtual int rd_map(const string& map_type, const string& label);


    /****************************  Options  **********************************/

    virtual int rd_option_array(const string& name, int n,
        const vector<float>& values);

    virtual int rd_option_bool(const string& name, bool flag);

    virtual int rd_option_list(const string& name, int n,
        const vector<string>& values);

    virtual int rd_option_real(const string& name, float value);

    virtual int rd_option_string(const string& name, const string& value);

    virtual int rd_custom(const string& label);

    ~REDirect();
};

#endif /* RD_ENGINE_DIRECT_H */