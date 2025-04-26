#ifndef MY_MATRIX_4_H
#define MY_MATRIX_4_H

#include "Point4.h"

#include <cmath> // For trig functions
#include <sstream> // Only for printing and debugging
    
// (xx,yx,zx), (yx,yy,yz), (zx,zy,zz) are like the object's right, up, forward vectors
// while (wx,wy,wz) is like the position (and wx, wy, wz, or ww is more of a scale)
// Default is set to a identity matrix like so
struct Matrix4 {
    float xx=1, xy=0, xz=0, xw=0,
          yx=0, yy=1, yz=0, yw=0,
          zx=0, zy=0, zz=1, zw=0,
          wx=0, wy=0, wz=0, ww=1;

    // Constructors (Default is set to a identity matrix like so)
    // [xx xy xz xw] = [1 0 0 0]
    // [yx yy yz yw] = [0 1 0 0]
    // [zx zy zz zw] = [0 0 1 0]
    // [wx wy wz ww] = [0 0 0 1]
    Matrix4(); // Defaults to an identity matrix
    Matrix4(float x, float y, float z, float w); // Position implemented as identity with (xw,yw,zw,ww) set to (x,y,z,w)
    Matrix4(float x, float y, float z); // Same but w is 1
    Matrix4(Point4&  position); // Same with Point4
    Matrix4(Vector3& position); // Same but w is 1
    Matrix4(Point4&  right, Point4&  up, Point4&  forward, Point4&  position); // This feels like a good idea idk
    Matrix4(Vector3& right, Vector3& up, Vector3& forward, Vector3& position); // Same but last row of w's default to [0,0,0,1]
    Matrix4(const float matrix[16]); // Passed as an array of size 16 (right to left, top to bottom) [xx,xy,xz,wx,yx,yy,yz,yw,zx,zy,...,ww]
    Matrix4(const float matrix[4][4]); // Passed as double array of sizes 4,4 [[xx,xy,xz,wx],[yx,yy,yz,yw],[zx,zy,...],[...,ww]]
    Matrix4(float xx, float xy, float xz, float xw, // Organized like a matrix from right to left, up to down
            float yx, float yy, float yz, float yw,
            float zx, float zy, float zz, float zw,
            float wx, float wy, float wz, float ww);

    // Identity
    static Matrix4 identity(); // Returns a new identity matrix
    void identity_mutate();    // Turns this matrix into an identity matrix


    // Multiply Matrix * Matrix
    // = [xx xy xz xw] * [xx xy xz xw]
    // = [xx xy xz xw] * [xx xy xz xw]
    // = [zx zy zz zw] * [zx zy zz zw]
    // = [wx wy wz ww] * [wx wy wz ww]
    Matrix4 multiply(const Matrix4& m) const;                       // Multiplies this by m on the right and returns a new resulting transformation.
    static Matrix4 multiply(const Matrix4& m1, const Matrix4& m2);  // Multiplies m1 by m2 (m2 on the right) and returns a new resulting transformation.
    void multiply_mutate(const Matrix4& m);                         // Sets itself as this multiplied by m on the right
    void multiply_mutate(const Matrix4& m1, const Matrix4& m2);     // Sets itself as m1 multiplied by m2 (m2 on the right)
    void multiply_mutate_raw(const Matrix4& m1, const Matrix4& m2); // Fastest version, but all 3 matricies MUST be difference references

    // Matrix * Point                                          // [X] = [xx xy xz xw] * [x]
    Point4 multiply(float x, float y, float z, float w) const; // [Y] = [yx yy yz yw] * [y]
    Point4 multiply(const Point4& p) const;                    // [Z] = [zx zy zz zw] * [z]
    void multiply_mutate(Point4& p);                           // [W] = [wx wy wz ww] * [w]

    // Matrix * Vector                                 // [X/W] = [xx xy xz xw] * [x]
    Vector3 multiply(float x, float y, float z) const; // [Y/W] = [yx yy yz yw] * [y]
    Vector3 multiply(const Vector3& v) const;          // [Z/W] = [zx zy zz zw] * [z]
    void multiply_mutate(Vector3& v);                  // [ 1 ] = [wx wy wz ww] * [1]
    // Note: This takes a bit longer as the returning Vector3 needs to be converted to homogenous coordinates


    // Translation
    Matrix4 translate(float x, float y, float z) const; // Translates this matrix by (x y z), with the transformation matrix on the right side like so
    Matrix4 translate(const float xyz[3]) const;        // = [xx xy xz xw] * [1 0 0 x]
    Matrix4 translate(const Vector3& v) const;          // = [yx yy yz yw] * [0 1 0 y]
    void translate_mutate(float x, float y, float z);   // = [zx zy zz zw] * [0 0 1 z]
    void translate_mutate(const float xyz[3]);          // = [wx wy wz ww] * [0 0 0 1]
    void translate_mutate(const Vector3& v);
    Matrix4 translate_left_of_matrix(float x, float y, float z) const; // Translates this matrix by (x y z), with the transformation matrix on the left side (Not really expecting this to be used)
    Matrix4 translate_left_of_matrix(const float xyz[3]) const;        // = [1 0 0 x] * [xx xy xz xw]
    Matrix4 translate_left_of_matrix(const Vector3& v) const;          // = [0 1 0 y] * [yx yy yz yw]
    void translate_left_of_matrix_mutate(float x, float y, float z);   // = [0 0 1 z] * [zx zy zz zw]
    void translate_left_of_matrix_mutate(const float xyz[3]);          // = [0 0 0 1] * [wx wy wz ww]
    void translate_left_of_matrix_mutate(const Vector3& v);
    static Matrix4 translation_matrix(float x, float y, float z); // Returns a new translation matrix / Turns this matrix into the translation matrix
    static Matrix4 translation_matrix(const float xyz[3]);        // = [1 0 0 x]
    static Matrix4 translation_matrix(const Vector3& v);          // = [0 1 0 y]
    void translation_matrix_mutate(float x, float y, float z);    // = [0 0 1 z]
    void translation_matrix_mutate(const float xyz[3]);           // = [0 0 0 1]
    void translation_matrix_mutate(const Vector3& v);             // Note: Matrix4::translation(xyz) is equivalent to Matrix4(xyz)
    

    // Scale
    Matrix4 scale(float x, float y, float z) const; // Scales this matrix by (x,y,z), with the scale matrix on the right side like so
    Matrix4 scale(const float xyz[3]) const;        // = [xx xy xz xw] * [x 0 0 0]
    Matrix4 scale(const Vector3& v) const;          // = [yx yy yz yw] * [0 y 0 0]
    void scale_mutate(float x, float y, float z);   // = [zx zy zz zw] * [0 0 z 0]
    void scale_mutate(const float xyz[3]);          // = [wx wy wz ww] * [0 0 0 1]
    void scale_mutate(Vector3& v);
    Matrix4 scale_left_of_matrix(float x, float y, float z) const; // Scales this matrix by (x y z), with the scale matrix on the left side (Not really expecting this to be used)
    Matrix4 scale_left_of_matrix(const float xyz[3]) const;        // = [x 0 0 0] * [xx xy xz xw]
    Matrix4 scale_left_of_matrix(const Vector3& v) const;          // = [0 y 0 0] * [yx yy yz yw]
    void scale_left_of_matrix_mutate(float x, float y, float z);   // = [0 0 z 0] * [zx zy zz zw]
    void scale_left_of_matrix_mutate(const float xyz[3]);          // = [0 0 0 1] * [wx wy wz ww]
    void scale_left_of_matrix_mutate(Vector3& v);
    static Matrix4 scale_matrix(float x, float y, float z); // Returns a new scale matrix / Turns this matrix into the scale matrix
    static Matrix4 scale_matrix(const float xyz[3]);        // = [x 0 0 0]
    static Matrix4 scale_matrix(const Vector3& v);          // = [0 y 0 0]
    void scale_matrix_mutate(float x, float y, float z);    // = [0 0 z 0]
    void scale_matrix_mutate(const float xyz[3]);           // = [0 0 0 1]
    void scale_matrix_mutate(Vector3& v);
    

    // Rotation about Z-axis in degrees (X towards Y)
    static Vector3 rotate_xy(const Vector3& v, float angle_in_degrees); // = [cos(a) -sin(a) 0 0] * [x]
    static void rotate_xy_mutate(Vector3& v, float angle_in_degrees);   // = [sin(a)  cos(a) 0 0] * [y]
    static Point4 rotate_xy(const Point4& p, float angle_in_degrees);   // = [   0      0    1 0] * [z]
    static void rotate_xy_mutate(Point4& p, float angle_in_degrees);    // = [   0      0    0 1] * [w] (w is 1 with Vector3)
    Matrix4 rotate_xy(float angle_in_degrees) const; // Rotate matrix with the rotation matrix on the right side
    void rotate_xy_mutate(float angle_in_degrees); // Same but mutates this matrix
    Matrix4 rotate_xy_left_of_matrix(float angle_in_degrees) const; // Rotate matrix with the rotation matrix on the left side
    void rotate_xy_left_of_matrix_mutate(float angle_in_degrees); // Same but mutates this matrix
    static Matrix4 rotation_xy_matrix(float angle_in_degrees); // Returns a new rotation matrix
    void rotation_xy_matrix_mutate(float angle_in_degrees);  // Turns this matrix into the rotation matrix


    // Rotation about X-axis in degrees (Y towards Z)
    static Vector3 rotate_yz(const Vector3& v, float angle_in_degrees); // = [1     0      0    0] * [x]
    static void rotate_yz_mutate(Vector3& v, float angle_in_degrees);   // = [0  cos(a) -sin(a) 0] * [y]
    static Point4 rotate_yz(const Point4& p, float angle_in_degrees);   // = [0  sin(a)  cos(a) 0] * [z]
    static void rotate_yz_mutate(Point4& p, float angle_in_degrees);    // = [0     0      0    1] * [w] (w is 1 with Vector3)
    Matrix4 rotate_yz(float angle_in_degrees) const; // Rotate matrix with the rotation matrix on the right side
    void rotate_yz_mutate(float angle_in_degrees); // Same but mutates this matrix
    Matrix4 rotate_yz_left_of_matrix(float angle_in_degrees) const; // Rotate matrix with the rotation matrix on the left side
    void rotate_yz_left_of_matrix_mutate(float angle_in_degrees); // Same but mutates this matrix
    static Matrix4 rotation_yz_matrix(float angle_in_degrees); // Returns a new rotation matrix
    void rotation_yz_matrix_mutate(float angle_in_degrees);  // Turns this matrix into the rotation matrix


    // Rotation about Y-axis in degrees (Z towards X)
    static Vector3 rotate_zx(const Vector3& v, float angle_in_degrees); // = [ cos(a) 0 sin(a) 0] * [x]
    static void rotate_zx_mutate(Vector3& v, float angle_in_degrees);   // = [   0    1    0   0] * [y]
    static Point4 rotate_zx(const Point4& p, float angle_in_degrees);   // = [-sin(a) 0 cos(a) 0] * [z]
    static void rotate_zx_mutate(Point4& p, float angle_in_degrees);    // = [   0    0    0   1] * [w] (w is 1 with Vector3)
    Matrix4 rotate_zx(float angle_in_degrees) const; // Rotate matrix with the rotation matrix on the right side
    void rotate_zx_mutate(float angle_in_degrees); // Same but mutates this matrix
    Matrix4 rotate_zx_left_of_matrix(float angle_in_degrees) const; // Rotate matrix with the rotation matrix on the left side
    void rotate_zx_left_of_matrix_mutate(float angle_in_degrees); // Same but mutates this matrix
    static Matrix4 rotation_zx_matrix(float angle_in_degrees); // Returns a new rotation matrix
    void rotation_zx_matrix_mutate(float angle_in_degrees);  // Turns this matrix into the rotation matrix


    // Graphics
    // Using the given eye point, look at point and up vector, all expressed in world coordinates, 
    // returns the transformation from world to camera coordinates.
    static Matrix4 world_to_camera(Point4& eye, Point4& at, Vector3& up); // Slowest version of this method as Point4's are converted to cartesian first
    static Matrix4 world_to_camera(Vector3& eye_position, Vector3& at_position, Vector3& up_direction); // First 2 Vector3's treated as points
    static Matrix4 world_to_camera(const float eyeX, const float eyeY, const float eyeZ,
                                   const float atX,  const float atY,  const float atZ,
                                   const float upX,  const float upY,  const float upZ);

    // Returns a transformation to go from camera coordinates to normalized clipping coordinates (x, y, z all between 0 and 1). 
    // The vertical field of view of the camera is given by fov in degrees. 
    // Near and far are z distances of the near and far clipping planes respectively. 
    // Aspect is the ratio of horizontal to vertical size of the display device. 
    // This routine assumes that the viewing window is centered about the optical axis of the camera.
    static Matrix4 camera_to_clip(float fov, float near, float far, float aspect);

    // Returns a transformation to go from normalized clipping coordinates to device coordinates.
    // Width and height are the size (in pixels) of the final viewing device.
    static Matrix4 clip_to_device(int width, int height);


    // Operators
    friend Vector3 operator * (const Matrix4& m,  const Vector3& v)  { return m.multiply(v);               }; // Vector3 = Matrix * Vector3
    friend Point4  operator * (const Matrix4& m,  const Point4&  p)  { return m.multiply(p);               }; // Point4  = Matrix * Point4
    friend Matrix4 operator * (const Matrix4& m1, const Matrix4& m2) { return m1.multiply(m2);             }; // Matrix  = Matrix * Matrix
    friend Matrix4 operator + (const Matrix4& m,  const Vector3& v)  { return m.translate(v);              }; // Matrix  = Matrix + Vector3 (Translation)
    friend Matrix4 operator - (const Matrix4& m,  const Vector3& v)  { return m.translate(-v.x,-v.y,-v.z); }; // Matrix  = Matrix - Vector3 (Translation)

    // String Stream (Optional)
    friend std::ostream& operator << (std::ostream& out, const Matrix4& m) {
        return out << '['
            << m.xx << ", " << m.xy << ", " << m.xz << ", " << m.xw << "]\n["
            << m.yx << ", " << m.yy << ", " << m.yz << ", " << m.yw << "]\n["
            << m.zx << ", " << m.zy << ", " << m.zz << ", " << m.zw << "]\n["
            << m.wx << ", " << m.wy << ", " << m.wz << ", " << m.ww << "]\n";
    };
};

#endif /* MY_MATRIX_4_H */