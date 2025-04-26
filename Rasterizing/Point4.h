#ifndef MY_POINT_4_H
#define MY_POINT_4_H

#include "Vector3.h"

#include <limits> // Only used to set to infinity when dividing by 0
#include <sstream> // Only for printing and debugging


// Attributed point (WIP)
#define ATTR_CONSTANT  4
#define ATTR_R         5
#define ATTR_G         6
#define ATTR_B         7
#define ATTR_NX        8
#define ATTR_NY        9
#define ATTR_NZ       10
#define ATTR_S        11
#define ATTR_T        12
#define ATTR_WORLD_X  13
#define ATTR_WORLD_Y  14
#define ATTR_WORLD_Z  15

#define ATTR_SIZE     16

typedef struct {
    float coord[ATTR_SIZE];
} attr_point;


struct Point4 {
    float x=0, y=0, z=0, w=1;

    // Constructors
    Point4(); // Defaults to 0,0,0,1
    Point4(float x, float y, float z); // w defaults to 1
    Point4(float x, float y, float z, float w);
    Point4(const float xyzw[4]); // Sets x,y,z,w from array
    Point4(const float xyz[3], float w); // sets w while setting xyz from array
    Point4(const Vector3& v); // w defaults to 1
    Point4(const Vector3& v, float w); // sets w while setting from Vector3


    // Conversion
    Vector3 to_vector3() const; // Converts this homogeneous Point4 into a cartesian Vector3. (if w is 0, then this is inf)
    void to_vector3_mutate(Vector3& v) const; // Mutates v
    void into_array(float xyzw[4]) const; // Mutates array xyzw


    // Translation
    Point4 translate(const Vector3& v) const; // Translates this Point4 by v and returns a new Point4 result
    Point4 translate(float x, float y, float z) const; // Same but with variables
    void translate_mutate(const Vector3& v); // Same but this Point4 is set to the result
    void translate_mutate(float x, float y, float z); // Same but with variables


    // Operators
    friend Point4 operator + (const Point4&  p, const Vector3& v) { return p.translate(v);                }; // Translation (p + v)
    friend Point4 operator - (const Point4&  p, const Vector3& v) { return p.translate(-v.x, -v.y, -v.z); }; // Translation (p - v)

    // String Stream (Optional)
    friend std::ostream& operator << (std::ostream& out, const Point4& p) {
        return out << '(' << p.x << ", " << p.y << ", " << p.z << ", " << p.w << ')';
    };
};

#endif /* MY_POINT_4_H */