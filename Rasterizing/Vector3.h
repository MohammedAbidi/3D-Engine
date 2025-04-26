#ifndef MY_VECTOR_3_H
#define MY_VECTOR_3_H

#include <cmath> // For trig functions
#include <sstream> // Only for printing and debugging


struct Vector3 {
    float x=0, y=0, z=0;

    // Constructors
    Vector3(); // Default 0,0,0
    Vector3(float x, float y, float z);
    Vector3(const float xyz[3]); // Passed as an array of size 3
    void into_array(float xyz[3]) const; // Conversion, xyz is mutated

    // Math
    Vector3 add(const Vector3& v) const;          // Adds v to this Vector3 and returns a new result.
    Vector3 add(float x, float y, float z) const; // Same but no object parameters
    void add_mutate(const Vector3& v);            // Adds v to this Vector3 and mutates the result.
    void add_mutate(float x, float y, float z);   // Same but no object parameters

    Vector3 subtract(const Vector3& v) const;                  // Subtracts v from this Vector3 and returns a new result.
    Vector3 subtract(float x, float y, float z) const;         // Same but no object parameters
    Vector3 subtract_inverse(float x, float y, float z) const; // (x,y,z) - Vector3
    void subtract_mutate(const Vector3& v);                    // Subtracts v from this Vector3 and mutates the result.
    void subtract_mutate(float x, float y, float z);           // Same but no object parameters
    void subtract_inverse_mutate(float x, float y, float z);   // = (x,y,z) - Vector3
    void subtract_inverse_mutate(const Vector3& v);   // = v - Vector3

    Vector3 multiply(const Vector3& v) const;          // Multiplies each component of this Vector3 with each corrosponding component of v
    Vector3 multiply(float x, float y, float z) const; // New Vector3(Vector3 * (x,y,z))
    Vector3 multiply(float scalar) const;              // New Vector3(Vector3 * scalar)
    void multiply_mutate(const Vector3& v);            // Same, but mutates This Vector3 with v
    void multiply_mutate(float x, float y, float z);   // Sets Vector3 = Vector3 * x,y,z
    void multiply_mutate(float scalar);                // Sets Vector3 = (Vector3 * scalar)

    Vector3 divide(const Vector3& v) const;                  // Divides each component of this Vector3 with each corrosponding component of v
    Vector3 divide(float x, float y, float z) const;         // Same but no object parameters
    Vector3 divide(float scalar) const;                      // New Vector3(Vector3 / scalar)
    Vector3 divide_inverse(float x, float y, float z) const; // (x,y,z) / Vector3
    Vector3 divide_inverse(float scalar) const;              // New Vector3(scalar / Vector3)
    void divide_mutate(const Vector3& v);                    // Same, but mutates This Vector3 with v
    void divide_mutate(float x, float y, float z);           // = Vector3 / (x,y,z)
    void divide_mutate(float scalar);                        // Sets Vector3 = Vector3 / scalar
    void divide_inverse_mutate(float x, float y, float z);   // = (x,y,z) / Vector3
    void divide_inverse_mutate(float scalar);                // Sets Vector3 = scalar / Vector3


    // 3D Math
    float dot(const Vector3& v) const; // dot product
    static float dot(float magnitude1, float magnitude2, float angle); // Not expected to actually be used, but it does exist
    static float dot(float x1, float y1, float z1, // Equivalent to x1*x2 + y1*y2 + z1*z2
                     float x2, float y2, float z2);

    float angle(const Vector3& v) const; // Computes the dot product of this and v and returns the scalar value.
    float angle(float x, float y, float z) const; // Same but with float parameters
    static float angle(float x1, float y1, float z1, // Static version
                       float x2, float y2, float z2);

    float angle_normal(const Vector3& v) const; // Only works if both this and v are normalized (skips divide and float magnitude() calcs)
    float angle_normal(float x, float y, float z) const; // Same but with float parameters
    static float angle_normal(float x1, float y1, float z1, // Static version
                              float x2, float y2, float z2);

    Vector3 cross(const Vector3& v) const; // Computes the cross product of this and v and returns the Vector3 value.
    void cross_mutate(const Vector3& v); // Computes the cross product of this and v and returns the Vector3 value.
    static void cross(float& x, float& y, float& z, // Computes cross of (x1,y1,z1) X (x2,y2,z2) and stores the result in (x,y,z)
                      float x1, float y1, float z1,
                      float x2, float y2, float z2);

    float magnitude() const; // Computes the magnitude of this Vector3
    float magnitude(const Vector3& v) const; // Returns the magnitude between this Vector3 and the given Vector3 (like the distance between them as points, completing the "triangle")
    static float magnitude(float x, float y, float z); // Static versions
    static float magnitude(float x1, float y1, float z1,
                           float x2, float y2, float z2);

    float magnitude2() const; // Computes only the magnitude squared of Vector3 v (this avoids the sqrt() used in magnitude() and is often useful in its own right.
    float magnitude2(const Vector3& v) const; // Returns the magnitude squared between this Vector3 and the given Vector3
    static float magnitude2(float x, float y, float z); // Static versions
    static float magnitude2(float x1, float y1, float z1,
                            float x2, float y2, float z2);

    Vector3 normalize() const; // Returns a new normalized Vector3 of this Vector3 (same direction with a magnitude of 1)
    void normalize_mutate(); // Normalizes this Vector3
    static void normalize(float& x, float& y, float& z); // Normalizes (x,y,z)
    static void normalize(float& x, float& y, float& z, // Normalizes (X,Y,Z) and stores the result in (x,y,z)
                          float  X, float  Y, float  Z);


    // Extras
    Vector3 inverse() const; // Returns a new Vector3 in the opposite direction
    void inverse_mutate(); // Same but mutates this

    Vector3 abs() const; // Returns a new Vector3 from the absolute values of the original's components
    void abs_mutate();

    Vector3 floor() const; // Returns a new Vector3 from the floor of the original's components.
    void floor_mutate();

    Vector3 ceil() const; // Returns a new Vector3 from the ceiling of the original's components.
    void ceil_mutate();

    Vector3 lerp(const Vector3& goal, float a) const; // Returns a Vector3 linearly interpolated between this Vector3 and the given goal at the given alpha a (0.0 to 1.0).
    void lerp_mutate(const Vector3& goal, float a);


    // Operators
    // Produces a Vector3 with each component (x,y,z) of v1 operated with each component of v2
    friend Vector3 operator + (const Vector3& v1, const Vector3& v2) { return v1.add(v2);      }; // Addition       (v1 + v2)
    friend Vector3 operator - (const Vector3& v1, const Vector3& v2) { return v1.subtract(v2); }; // Subtraction    (v1 - v2)
    friend Vector3 operator * (const Vector3& v1, const Vector3& v2) { return v1.multiply(v2); }; // Multiplication (v1 * v2)
    friend Vector3 operator / (const Vector3& v1, const Vector3& v2) { return v1.divide(v2);   }; // Division       (v1 / v2)
    friend Vector3 operator - (const Vector3& v)                     { return v.inverse();     }; // Negative       (-v)

    // Produces a Vector3 with each component (x,y,z) of v operated by the same number (aka: scalar)
    friend Vector3 operator * (const Vector3& v, float n) { return v.multiply(n);       }; // Multiplies a Vector3 by a scalar and returns a Vector3.
    friend Vector3 operator * (float n, const Vector3& v) { return v.multiply(n);       }; // Multiplies a scalar by a Vector3 and returns a Vector3.
    friend Vector3 operator / (const Vector3& v, float n) { return v.divide(n);         }; // Divides a Vector3 by a scalar and returns a Vector3.
    friend Vector3 operator / (float n, const Vector3& v) { return v.divide_inverse(n); }; // Divides a scalar by a Vector3 and returns a Vector3.


    // String Stream (Optional)
    friend std::ostream& operator << (std::ostream& out, const Vector3& v) {
        return out << '(' << v.x << ", " << v.y << ", " << v.z << ')';
    };
};

#endif /* MY_VECTOR_3_H */