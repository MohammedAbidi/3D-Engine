#include "Point4.h"


Point4::Point4() {
    x=0; y=0; z=0; w=1;
}
Point4::Point4(float x, float y, float z) {
    this->x=x; this->y=y; this->z=z; w=1;
}
Point4::Point4(float x, float y, float z, float w) {
    this->x=x; this->y=y; this->z=z; this->w=w;
}
Point4::Point4(const float xyzw[4]) {
    x=xyzw[0]; y=xyzw[1]; z=xyzw[2]; w=xyzw[3];
}
Point4::Point4(const float xyz[3], float w) {
    x=xyz[0]; y=xyz[1]; z=xyz[2]; this->w=1;
}
Point4::Point4(const Vector3& v) {
    this->x=v.x; this->y=v.y; this->z=v.z; w=1;
}
Point4::Point4(const Vector3& v, float w) {
    this->x=v.x; this->y=v.y; this->z=v.z; this->w=w;
}

static const float INF = std::numeric_limits<float>::infinity();
Vector3 Point4::to_vector3() const {
    if (w == 0) // Check and handle infinities
        return Vector3(x*INF, y*INF, z*INF);
    else
        return Vector3(x/w, y/w, z/w);
}
void Point4::to_vector3_mutate(Vector3& v) const {
    if (w == 0) // Check and handle infinities
        v.x = x*INF, v.y = y*INF, v.z = z*INF;
    else
        v.x = x/w, v.y = y/w, v.z = z/w;
}

void Point4::into_array(float xyzw[4]) const {
    xyzw[0] = x; xyzw[1] = y; xyzw[2] = z; xyzw[3] = w;
}

Point4 Point4::translate(const Vector3& v) const {
    return Point4(x+v.x, y+v.y, z+v.z, w);
}
Point4 Point4::translate(float x, float y, float z) const {
    return Point4(this->x+x, this->y+y, this->z+z, w);
}
void Point4::translate_mutate(const Vector3& v) {
    x += v.x; y += v.y; z += v.z;
}
void Point4::translate_mutate(float x, float y, float z) {
    this->x += x; this->y += y; this->z += z;
}