#include "Vector3.h"

// Constructors
Vector3::Vector3() {
    x = 0; y = 0; z = 0;
}
Vector3::Vector3(float x, float y, float z) {
    this->x = x; this->y = y; this->z = z;
}
Vector3::Vector3(const float xyz[3]) {
    x = xyz[0]; y = xyz[1]; z = xyz[2];
}

// Conversion
void Vector3::into_array(float xyz[3]) const {
    xyz[0] = x; xyz[1] = y; xyz[2] = z;
}


// Math
// Addition
Vector3 Vector3::add(const Vector3& v) const {
    return Vector3(x+v.x, y+v.y, z+v.z);
}
Vector3 Vector3::add(float x, float y, float z) const {
    return Vector3(this->x+x, this->y+y, this->z+z);
}
void Vector3::add_mutate(const Vector3& v) {
    x += v.x; y += v.y; z += v.z;
}
void Vector3::add_mutate(float x, float y, float z) {
    this->x += x; this->x += y; this->x += z;
}

// Subtraction
Vector3 Vector3::subtract(const Vector3& v) const {
    return Vector3(x-v.x, y-v.y, z-v.z);
}
Vector3 Vector3::subtract(float x, float y, float z) const {
    return Vector3(this->x-x, this->y-y, this->z-z);
}
Vector3 Vector3::subtract_inverse(float x, float y, float z) const {
    return Vector3(x-this->x, y-this->y, z-this->z);
}
void Vector3::subtract_mutate(const Vector3& v) {
    x -= v.x; y -= v.y; z -= v.z;
}
void Vector3::subtract_mutate(float x, float y, float z) {
    this->x -= x; this->y -= y; this->z -= z;
}
void Vector3::subtract_inverse_mutate(float x, float y, float z) {
    this->x = x-this->x; this->y = y-this->y; this->z = z-this->z;
}
void Vector3::subtract_inverse_mutate(const Vector3& v) {
    x = v.x-x; y = v.y-y; z = v.z-z;
}

// Multiplication
Vector3 Vector3::multiply(const Vector3& v) const {
    return Vector3(x*v.x, y*v.y, z*v.z);
}
Vector3 Vector3::multiply(float x, float y, float z) const {
    return Vector3(this->x*x, this->y*y, this->z*z);
}
Vector3 Vector3::multiply(float scalar) const {
    return Vector3(x*scalar, y*scalar, z*scalar);
}
void Vector3::multiply_mutate(const Vector3& v) {
    x *= v.x; y *= v.y; z *= v.z;
}
void Vector3::multiply_mutate(float x, float y, float z) {
    this->x *= x; this->y *= y; this->z *= z;
}
void Vector3::multiply_mutate(float scalar) {
    x *= scalar; y *= scalar; z *= scalar;
}

// Division (Order matters)
Vector3 Vector3::divide(const Vector3& v) const {
    return Vector3(x/v.x, y/v.y, z/v.z);
}
Vector3 Vector3::divide(float x, float y, float z) const {
    return Vector3(this->x/x, this->y/y, this->z/z);
}
Vector3 Vector3::divide(float scalar) const {
    return Vector3(x/scalar, y/scalar, z/scalar);
}
Vector3 Vector3::divide_inverse(float x, float y, float z) const {
    return Vector3(x/this->x, y/this->y, z/this->z);
}
Vector3 Vector3::divide_inverse(float scalar) const {
    return Vector3(scalar/x, scalar/y, scalar/z);
}
void Vector3::divide_mutate(const Vector3& v) {
    x /= v.x; y /= v.y; z /= v.z;
}
void Vector3::divide_mutate(float x, float y, float z) {
    this->x /= x; this->y /= y; this->z /= z;
}
void Vector3::divide_mutate(float scalar) {
    x /= scalar; y /= scalar; z /= scalar;
}
void Vector3::divide_inverse_mutate(float x, float y, float z) {
    this->x = x/this->x; this->y = y/this->y; this->z = z/this->z;
}
void Vector3::divide_inverse_mutate(float scalar) {
    x = scalar/x; y = scalar/y; z = scalar/z;
}


// 3D Math
//  1 means same direction, 0 means perpendicular, and -1 means opposite
float Vector3::dot(const Vector3& v) const {
    return x*v.x + y*v.y + z*v.z;
}
float Vector3::dot(float magnitude1, float magnitude2, float angle) {
    return magnitude1 * magnitude2 * std::cos(angle);
}
float Vector3::dot(float x1, float y1, float z1, float x2, float y2, float z2) {
    return x1*x2 + y1*y2 + z1*z2;
}

float Vector3::angle(const Vector3& v) const {
    return std::asin(dot(v) / (magnitude() * v.magnitude()));
}
float Vector3::angle(float x, float y, float z) const {
    return angle(this->x, this->y, this->z, x,y,z);
}
float Vector3::angle(float x1, float y1, float z1, float x2, float y2, float z2) {
    return std::asin(dot(x1,y1,z1, x2,y2,z2) / (magnitude(x1,y1,z1) * magnitude(x2,y2,z2)));
}

float Vector3::angle_normal(const Vector3& v) const {
    return std::asin(dot(v));
}
float Vector3::angle_normal(float x, float y, float z) const {
    return std::asin(dot(x,y,z));
}
float Vector3::angle_normal(float x1, float y1, float z1, float x2, float y2, float z2) {
    return std::asin(dot(x1,y1,z1, x2,y2,z2));
}

Vector3 Vector3::cross(const Vector3& v) const {
    return Vector3(
        y*v.z - z*v.y,
        z*v.x - x*v.z, // Flip Y sign into -(x*v.z - z*v.x)
        x*v.y - y*v.x
    );
}
void Vector3::cross_mutate(const Vector3& v) {
    // cross(x,y,z, x,y,z, v.x,v.y,v.z);
    const float X = y*v.z - z*v.y;
    const float Y = z*v.x - x*v.z;
    const float Z = x*v.y - y*v.x;
    x = X; y = Y; z = Z;
}
void Vector3::cross(float& x, float& y, float& z, float x1, float y1, float z1, float x2, float y2, float z2) {
    x = y1*z2 - z1*y2;
    y = z1*x2 - x1*z2;
    z = x1*y2 - y1*x2;
}

float Vector3::magnitude() const {
    return std::sqrt(x*x + y*y + z*z);
}
float Vector3::magnitude(const Vector3& v) const {
    const float X = x - v.x;
    const float Y = y - v.y;
    const float Z = z - v.z;
    return std::sqrt(X*X + Y*Y + Z*Z);
}
float Vector3::magnitude(float x, float y, float z) {
    return std::sqrt(x*x + y*y + z*z);
}
float Vector3::magnitude(float x1, float y1, float z1, float x2, float y2, float z2) {
    const float X = x1 - x2;
    const float Y = y1 - y2;
    const float Z = z1 - z2;
    return std::sqrt(X*X + Y*Y + Z*Z);
}

float Vector3::magnitude2() const {
    return x*x + y*y + z*z;
}
float Vector3::magnitude2(const Vector3& v) const {
    const float X = x - v.x;
    const float Y = y - v.y;
    const float Z = z - v.z;
    return X*X + Y*Y + Z*Z;
}
float Vector3::magnitude2(float x, float y, float z) {
    return x*x + y*y + z*z;
}
float Vector3::magnitude2(float x1, float y1, float z1, float x2, float y2, float z2) {
    const float X = x1 - x2;
    const float Y = y1 - y2;
    const float Z = z1 - z2;
    return X*X + Y*Y + Z*Z;
}

Vector3 Vector3::normalize() const {
    // Unneccesary zero check?
    //if (x == 0.0f && y == 0.0f && z == 0.0f)
    //	return Vector3(); // Resulting magnitude will be 0 (Can't divide by zero)
    return divide(magnitude());
}
void Vector3::normalize_mutate() {
    // Unneccesary zero check?
    //if (x == 0.0f && y == 0.0f && z == 0.0f)
    //	return *this; // Resulting magnitude will be 0 (Can't divide by zero)
    divide_mutate(magnitude());
}
void Vector3::normalize(float& x, float& y, float& z) {
    const float MAGNITUDE = std::sqrt(x*x + y*y + z*z);
    x /= MAGNITUDE;
    y /= MAGNITUDE;
    z /= MAGNITUDE;
}
void Vector3::normalize(float& x, float& y, float& z, float X, float Y, float Z) {
    const float MAGNITUDE = std::sqrt(X*X + Y*Y + Z*Z);
    x = X / MAGNITUDE;
    y = Y / MAGNITUDE;
    z = Z / MAGNITUDE;
}

Vector3 Vector3::inverse() const {
    return Vector3(-x, -y, -z);
}
void Vector3::inverse_mutate() {
    x = -x; y = -y; z = -z;
}


// Extras
Vector3 Vector3::abs() const {
    return Vector3(std::fabs(x), std::fabs(y), std::fabs(z));
}
void Vector3::abs_mutate() {
    x = std::fabs(x); y = std::fabs(y); z = std::fabs(z);
}

Vector3 Vector3::floor() const {
    return Vector3(std::floor(x), std::floor(y), std::floor(z)); // or return Vector3((long)x,(long)y,(long)z); // same speed either way when benchmarked
}
void Vector3::floor_mutate() {
    x = std::floor(x); y = std::floor(y); z = std::floor(z);
}

Vector3 Vector3::ceil() const {
    return Vector3(std::ceil(x), std::ceil(y), std::ceil(z));
}
void Vector3::ceil_mutate() {
    x = std::ceil(x); y = std::ceil(y); z = std::ceil(z);
}

Vector3 Vector3::lerp(const Vector3& goal, float a) const {
    return Vector3( // A + (B-A)*a
        x + (goal.x - x)*a,
        y + (goal.y - y)*a,
        z + (goal.z - z)*a
    );
}
void Vector3::lerp_mutate(const Vector3& goal, float a) {
    x += (goal.x - x)*a;
    y += (goal.y - y)*a;
    z += (goal.z - z)*a;
}