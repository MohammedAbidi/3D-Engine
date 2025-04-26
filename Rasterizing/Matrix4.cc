#include "Matrix4.h"


// Constants
static const float TO_RADIANS = std::acos(-1) / 180.0f; // Used in rotation
static const float TO_RADIANS_2 = (float)TO_RADIANS / 2.0f; // Used in camera_to_clip


// Constructors
Matrix4::Matrix4() { // same as identity
    xx = 1; xy = 0; xz = 0; xw = 0;
    yx = 0; yy = 1; yz = 0; yw = 0;
    zx = 0; zy = 0; zz = 1; zw = 0;
    wx = 0; wy = 0; wz = 0; ww = 1;
}
Matrix4::Matrix4(float x,  float y,  float z) {
    xx = 1; xy = 0; xz = 0; xw = x;
    yx = 0; yy = 1; yz = 0; yw = y;
    zx = 0; zy = 0; zz = 1; zw = z;
    wx = 0; wy = 0; wz = 0; ww = 1;
}
Matrix4::Matrix4(float x, float y, float z, float w) {
    xx = 1; xy = 0; xz = 0; xw = x;
    yx = 0; yy = 1; yz = 0; yw = y;
    zx = 0; zy = 0; zz = 1; zw = z;
    wx = 0; wy = 0; wz = 0; ww = w;
}
Matrix4::Matrix4(Point4& p) {
    xx = 1; xy = 0; xz = 0; xw = p.x;
    yx = 0; yy = 1; yz = 0; yw = p.y;
    zx = 0; zy = 0; zz = 1; zw = p.z;
    wx = 0; wy = 0; wz = 0; ww = p.w;
}
Matrix4::Matrix4(Vector3& v) {
    xx = 1; xy = 0; xz = 0; xw = v.x;
    yx = 0; yy = 1; yz = 0; yw = v.y;
    zx = 0; zy = 0; zz = 1; zw = v.z;
    wx = 0; wy = 0; wz = 0; ww = 1;
}
Matrix4::Matrix4(Point4& r, Point4& u, Point4& f, Point4& p) {
    xx = r.x; xy = u.x; xz = f.x; xw = p.x;
    yx = r.y; yy = u.y; yz = f.y; yw = p.y;
    zx = r.z; zy = u.z; zz = f.z; zw = p.z;
    wx = r.w; wy = u.w; wz = f.w; ww = p.w;
}
Matrix4::Matrix4(Vector3& r, Vector3& u, Vector3& f, Vector3& p) {
    xx = r.x; xy = u.x; xz = f.x; xw = p.x;
    yx = r.y; yy = u.y; yz = f.y; yw = p.y;
    zx = r.z; zy = u.z; zz = f.z; zw = p.z;
    wx = 0;   wy = 0;   wz = 0;   ww = 1;
}
Matrix4::Matrix4(const float m[16]) {
    xx = m[0];  xy = m[1];  xz = m[2];  xw = m[3];
    yx = m[4];  yy = m[5];  yz = m[6];  yw = m[7];
    zx = m[8];  zy = m[9];  zz = m[10]; zw = m[11];
    wx = m[12]; wy = m[13]; wz = m[14]; ww = m[15];
}
Matrix4::Matrix4(const float m[4][4]) {
    xx = m[0][0]; xy = m[0][1]; xz = m[0][2]; xw = m[0][3];
    yx = m[1][0]; yy = m[1][1]; yz = m[1][2]; yw = m[1][3];
    zx = m[2][0]; zy = m[2][1]; zz = m[2][2]; zw = m[2][3];
    wx = m[3][0]; wy = m[3][1]; wz = m[3][2]; ww = m[3][3];
}
Matrix4::Matrix4(float xx, float xy, float xz, float xw,
                 float yx, float yy, float yz, float yw,
                 float zx, float zy, float zz, float zw,
                 float wx, float wy, float wz, float ww) {
    this->xx = xx; this->xy = xy; this->xz = xz; this->xw = xw;
    this->yx = yx; this->yy = yy; this->yz = yz; this->yw = yw;
    this->zx = zx; this->zy = zy; this->zz = zz; this->zw = zw;
    this->wx = wx; this->wy = wy; this->wz = wz; this->ww = ww;
}



// Matrix Identity
Matrix4 Matrix4::identity() {
    return Matrix4(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);
}
void Matrix4::identity_mutate() {
    xx = 1; xy = 0; xz = 0; xw = 0;
    yx = 0; yy = 1; yz = 0; yw = 0;
    zx = 0; zy = 0; zz = 1; zw = 0;
    wx = 0; wy = 0; wz = 0; ww = 1;
}



// Matrix Multiply
// [xx_xx xy_yx xz_zx xw_wx, xx_xy xy_yy xz_zy xw_wy, xx_xz xy_yz xz_zz xw_wz, xx_xw xy_yw xz_zw xw_ww] = [xx xy xz xw] * [xx xy xz xw]
// [yx_xx yy_yx yz_zx yw_wx, yx_xy yy_yy yz_zy yw_wy, yx_xz yy_yz yz_zz yw_wz, yx_xw yy_yw yz_zw yw_ww] = [yx yy yz yw] * [yx yy yz yw]
// [zx_xx zy_yx zz_zx zw_wx, zx_xy zy_yy zz_zy zw_wy, zx_xz zy_yz zz_zz zw_wz, zx_xw zy_yw zz_zw zw_ww] = [zx zy zz zw] * [zx zy zz zw]
// [wx_xx wy_yx wz_zx ww_wx, wx_xy wy_yy wz_zy ww_wy, wx_xz wy_yz wz_zz ww_wz, wx_xw wy_yw wz_zw ww_ww] = [wx wy wz ww] * [wx wy wz ww]
Matrix4 Matrix4::multiply(const Matrix4& m1, const Matrix4& m2) {
    return Matrix4(
        m1.xx*m2.xx + m1.xy*m2.yx + m1.xz*m2.zx + m1.xw*m2.wx,
        m1.xx*m2.xy + m1.xy*m2.yy + m1.xz*m2.zy + m1.xw*m2.wy,
        m1.xx*m2.xz + m1.xy*m2.yz + m1.xz*m2.zz + m1.xw*m2.wz,
        m1.xx*m2.xw + m1.xy*m2.yw + m1.xz*m2.zw + m1.xw*m2.ww,

        m1.yx*m2.xx + m1.yy*m2.yx + m1.yz*m2.zx + m1.yw*m2.wx,
        m1.yx*m2.xy + m1.yy*m2.yy + m1.yz*m2.zy + m1.yw*m2.wy,
        m1.yx*m2.xz + m1.yy*m2.yz + m1.yz*m2.zz + m1.yw*m2.wz,
        m1.yx*m2.xw + m1.yy*m2.yw + m1.yz*m2.zw + m1.yw*m2.ww,

        m1.zx*m2.xx + m1.zy*m2.yx + m1.zz*m2.zx + m1.zw*m2.wx,
        m1.zx*m2.xy + m1.zy*m2.yy + m1.zz*m2.zy + m1.zw*m2.wy,
        m1.zx*m2.xz + m1.zy*m2.yz + m1.zz*m2.zz + m1.zw*m2.wz,
        m1.zx*m2.xw + m1.zy*m2.yw + m1.zz*m2.zw + m1.zw*m2.ww,

        m1.wx*m2.xx + m1.wy*m2.yx + m1.wz*m2.zx + m1.ww*m2.wx,
        m1.wx*m2.xy + m1.wy*m2.yy + m1.wz*m2.zy + m1.ww*m2.wy,
        m1.wx*m2.xz + m1.wy*m2.yz + m1.wz*m2.zz + m1.ww*m2.wz,
        m1.wx*m2.xw + m1.wy*m2.yw + m1.wz*m2.zw + m1.ww*m2.ww
    );
}
Matrix4 Matrix4::multiply(const Matrix4& m) const {
    return multiply(*this, m);
}
void Matrix4::multiply_mutate(const Matrix4& m1, const Matrix4& m2) {
    const float XX = m1.xx*m2.xx + m1.xy*m2.yx + m1.xz*m2.zx + m1.xw*m2.wx;
    const float XY = m1.xx*m2.xy + m1.xy*m2.yy + m1.xz*m2.zy + m1.xw*m2.wy;
    const float XZ = m1.xx*m2.xz + m1.xy*m2.yz + m1.xz*m2.zz + m1.xw*m2.wz;
    const float XW = m1.xx*m2.xw + m1.xy*m2.yw + m1.xz*m2.zw + m1.xw*m2.ww;

    const float YX = m1.yx*m2.xx + m1.yy*m2.yx + m1.yz*m2.zx + m1.yw*m2.wx;
    const float YY = m1.yx*m2.xy + m1.yy*m2.yy + m1.yz*m2.zy + m1.yw*m2.wy;
    const float YZ = m1.yx*m2.xz + m1.yy*m2.yz + m1.yz*m2.zz + m1.yw*m2.wz;
    const float YW = m1.yx*m2.xw + m1.yy*m2.yw + m1.yz*m2.zw + m1.yw*m2.ww;

    const float ZX = m1.zx*m2.xx + m1.zy*m2.yx + m1.zz*m2.zx + m1.zw*m2.wx;
    const float ZY = m1.zx*m2.xy + m1.zy*m2.yy + m1.zz*m2.zy + m1.zw*m2.wy;
    const float ZZ = m1.zx*m2.xz + m1.zy*m2.yz + m1.zz*m2.zz + m1.zw*m2.wz;
    const float ZW = m1.zx*m2.xw + m1.zy*m2.yw + m1.zz*m2.zw + m1.zw*m2.ww;

    const float WX = m1.wx*m2.xx + m1.wy*m2.yx + m1.wz*m2.zx + m1.ww*m2.wx;
    const float WY = m1.wx*m2.xy + m1.wy*m2.yy + m1.wz*m2.zy + m1.ww*m2.wy;
    const float WZ = m1.wx*m2.xz + m1.wy*m2.yz + m1.wz*m2.zz + m1.ww*m2.wz;
    const float WW = m1.wx*m2.xw + m1.wy*m2.yw + m1.wz*m2.zw + m1.ww*m2.ww;

    xx = XX; xy = XY; xz = XZ; xw = XW;
    yx = YX; yy = YY; yz = YZ; yw = YW;
    zx = ZX; zy = ZY; zz = ZZ; zw = ZW;
    wx = WX; wy = WY; wz = WZ; ww = WW;
}
void Matrix4::multiply_mutate_raw(const Matrix4& m1, const Matrix4& m2) {
    xx = m1.xx*m2.xx + m1.xy*m2.yx + m1.xz*m2.zx + m1.xw*m2.wx;
    xy = m1.xx*m2.xy + m1.xy*m2.yy + m1.xz*m2.zy + m1.xw*m2.wy;
    xz = m1.xx*m2.xz + m1.xy*m2.yz + m1.xz*m2.zz + m1.xw*m2.wz;
    xw = m1.xx*m2.xw + m1.xy*m2.yw + m1.xz*m2.zw + m1.xw*m2.ww;

    yx = m1.yx*m2.xx + m1.yy*m2.yx + m1.yz*m2.zx + m1.yw*m2.wx;
    yy = m1.yx*m2.xy + m1.yy*m2.yy + m1.yz*m2.zy + m1.yw*m2.wy;
    yz = m1.yx*m2.xz + m1.yy*m2.yz + m1.yz*m2.zz + m1.yw*m2.wz;
    yw = m1.yx*m2.xw + m1.yy*m2.yw + m1.yz*m2.zw + m1.yw*m2.ww;

    zx = m1.zx*m2.xx + m1.zy*m2.yx + m1.zz*m2.zx + m1.zw*m2.wx;
    zy = m1.zx*m2.xy + m1.zy*m2.yy + m1.zz*m2.zy + m1.zw*m2.wy;
    zz = m1.zx*m2.xz + m1.zy*m2.yz + m1.zz*m2.zz + m1.zw*m2.wz;
    zw = m1.zx*m2.xw + m1.zy*m2.yw + m1.zz*m2.zw + m1.zw*m2.ww;

    wx = m1.wx*m2.xx + m1.wy*m2.yx + m1.wz*m2.zx + m1.ww*m2.wx;
    wy = m1.wx*m2.xy + m1.wy*m2.yy + m1.wz*m2.zy + m1.ww*m2.wy;
    wz = m1.wx*m2.xz + m1.wy*m2.yz + m1.wz*m2.zz + m1.ww*m2.wz;
    ww = m1.wx*m2.xw + m1.wy*m2.yw + m1.wz*m2.zw + m1.ww*m2.ww;
}
void Matrix4::multiply_mutate(const Matrix4& m) {
    this->multiply_mutate(*this, m);
}
// Multiply Matrix * Point
// [xx_x + xy_y + xz_z + xw_w] = [xx xy xz xw] * [x]
// [yx_x + yy_y + yz_z + yw_w] = [yx yy yz yw] * [y]
// [zx_x + zy_y + zz_z + zw_w] = [zx zy zz zw] * [z]
// [wx_x + wy_y + wz_z + ww_w] = [wx wy wz ww] * [w]
Point4 Matrix4::multiply(const float x, const float y, const float z, const float w) const {
    return Point4(
        xx*x + xy*y + xz*z + xw*w,
        yx*x + yy*y + yz*z + yw*w,
        zx*x + zy*y + zz*z + zw*w,
        wx*x + wy*y + wz*z + ww*w
    );
}
Point4 Matrix4::multiply(const Point4& p) const {
    return multiply(p.x, p.y, p.z, p.w);
}
void Matrix4::multiply_mutate(Point4& p) {
    const float X = xx*p.x + xy*p.y + xz*p.z + xw*p.w;
    const float Y = yx*p.x + yy*p.y + yz*p.z + yw*p.w;
    const float Z = zx*p.x + zy*p.y + zz*p.z + zw*p.w;
    const float W = wx*p.x + wy*p.y + wz*p.z + ww*p.w;
    p.x = X; p.y = Y; p.z = Z; p.w = W;
}
// Multiply Matrix * Vector (With homogenious converion)
// [xx_x + xy_y + xz_z + xw] = [xx xy xz xw] * [x]
// [yx_x + yy_y + yz_z + yw] = [yx yy yz yw] * [y]
// [zx_x + zy_y + zz_z + zw] = [zx zy zz zw] * [z]
// [wx_x + wy_y + wz_z + ww] = [wx wy wz ww] * [1]
Vector3 Matrix4::multiply(float x, float y, float z) const {
    const float W = wx*x + wy*y + wz*z + ww;
    return Vector3(
        (float) (xx*x + xy*y + xz*z + xw) / W,
        (float) (yx*x + yy*y + yz*z + yw) / W,
        (float) (zx*x + zy*y + zz*z + zw) / W
    );
}
Vector3 Matrix4::multiply(const Vector3& p) const {
    return multiply(p.x, p.y, p.z);
}
void Matrix4::multiply_mutate(Vector3& p) {
    const float W =          wx*p.x + wy*p.y + wz*p.z + ww;
    const float X = (float) (xx*p.x + xy*p.y + xz*p.z + xw) / W;
    const float Y = (float) (yx*p.x + yy*p.y + yz*p.z + yw) / W;
    const float Z = (float) (zx*p.x + zy*p.y + zz*p.z + zw) / W;
    p.x = X; p.y = Y; p.z = Z;
}



// Translation (Multiplication that only translates, which skips lots of multiplications/additions)
// [xx, xy, xz,  xx*Tx + xy*Ty + xz*Tz + xw] = [xx, xy, xz, xw] * [1, 0, 0, Tx]
// [yx, yy, yz,  yx*Tx + yy*Ty + yz*Tz + yw] = [yx, yy, yz, yw] * [0, 1, 0, Ty]
// [zx, zy, zz,  zx*Tx + zy*Ty + zz*Tz + zw] = [zx, zy, zz, zw] * [0, 0, 1, Tz]
// [wx, wy, wz,  wx*Tx + wy*Ty + wz*Tz + ww] = [wx, wy, wz, ww] * [0, 0, 0,  1]
Matrix4 Matrix4::translate(float tx, float ty, float tz) const {
    return Matrix4(
        xx, xy, xz, xw + xx*tx + xy*ty + xz*tz,
        yx, yy, yz, yw + yx*tx + yy*ty + yz*tz,
        zx, zy, zz, zw + zx*tx + zy*ty + zz*tz,
        wx, wy, wz, ww + wx*tx + wy*ty + wz*tz
    );
}
Matrix4 Matrix4::translate(const float xyz[3]) const {
    return translate(xyz[0], xyz[1], xyz[2]);
}
Matrix4 Matrix4::translate(const Vector3& v) const {
    return translate(v.x, v.y, v.z);
}
void Matrix4::translate_mutate(float tx, float ty, float tz) {
    xw += xx*tx + xy*ty + xz*tz;
    yw += yx*tx + yy*ty + yz*tz;
    zw += zx*tx + zy*ty + zz*tz;
    ww += wx*tx + wy*ty + wz*tz;
}
void Matrix4::translate_mutate(const float xyz[3]) {
    translate_mutate(xyz[0], xyz[1], xyz[2]);
}
void Matrix4::translate_mutate(const Vector3& v) {
    translate_mutate(v.x, v.y, v.z);
}
// Translation (Multiplication that only translates with the transformation matrix on the left side, skipping lots of multiplications/additions)
// [xx + wx*Tx, xy + wy*Tx, xz + wz*Tx, xw + ww*Tx] = [1, 0, 0, Tx] * [xx, xy, xz, xw]
// [yx + wx*Ty, yy + wy*Ty, yz + wz*Ty, yw + ww*Ty] = [0, 1, 0, Ty] * [yx, yy, yz, yw]
// [zx + wx*Tz, zy + wy*Tz, zz + wz*Tz, zw + ww*Tz] = [0, 0, 1, Tz] * [zx, zy, zz, zw]
// [     wx   ,      wy   ,      wz   ,      ww   ] = [0, 0, 0,  1] * [wx, wy, wz, ww]
Matrix4 Matrix4::translate_left_of_matrix(float tx, float ty, float tz) const {
    return Matrix4(
        xx + wx*tx, xy + wy*tx, xz + wz*tx, xw + ww*tx,
        yx + wx*ty, yy + wy*ty, yz + wz*ty, yw + ww*ty,
        zx + wx*tz, zy + wy*tz, zz + wz*tz, zw + ww*tz,
        wx, wy, wz, ww
    );
}
Matrix4 Matrix4::translate_left_of_matrix(const float xyz[3]) const {
    return translate_left_of_matrix(xyz[0], xyz[1], xyz[2]);
}
Matrix4 Matrix4::translate_left_of_matrix(const Vector3& v) const {
    return translate_left_of_matrix(v.x, v.y, v.z);
}
void Matrix4::translate_left_of_matrix_mutate(float tx, float ty, float tz) {
    xx += wx*tx; xy += wy*tx; xz += wz*tx; xw += ww*tx;
    yx += wx*ty; yy += wy*ty; yz += wz*ty; yw += ww*ty;
    zx += wx*tz; zy += wy*tz; zz += wz*tz; zw += ww*tz;
}
void Matrix4::translate_left_of_matrix_mutate(const float xyz[3]) {
    translate_left_of_matrix_mutate(xyz[0], xyz[1], xyz[2]);
}
void Matrix4::translate_left_of_matrix_mutate(const Vector3& v) {
    translate_left_of_matrix_mutate(v.x, v.y, v.z);
}
// Translation Matrix
// [1, 0, 0, Tx]
// [0, 1, 0, Ty]
// [0, 0, 1, Tz]
// [0, 0, 0,  1]
Matrix4 Matrix4::translation_matrix(float x, float y, float z) {
    return Matrix4(x, y, z);
}
Matrix4 Matrix4::translation_matrix(const float xyz[3]) {
    return Matrix4(xyz);
}
Matrix4 Matrix4::translation_matrix(const Vector3& v) {
    return Matrix4(v.x, v.y, v.z);
}
void Matrix4::translation_matrix_mutate(float x, float y, float z) {
    xx = 1; xy = 0; xz = 0; xw = x;
    yx = 0; yy = 1; yz = 0; yw = y;
    zx = 0; zy = 0; zz = 1; zw = z;
    wx = 0; wy = 0; wz = 0; ww = 1;
}
void Matrix4::translation_matrix_mutate(const float xyz[3]) {
    xx = 1; xy = 0; xz = 0; xw = xyz[0];
    yx = 0; yy = 1; yz = 0; yw = xyz[1];
    zx = 0; zy = 0; zz = 1; zw = xyz[2];
    wx = 0; wy = 0; wz = 0; ww = 1;
}
void Matrix4::translation_matrix_mutate(const Vector3& v) {
    xx = 1; xy = 0; xz = 0; xw = v.x;
    yx = 0; yy = 1; yz = 0; yw = v.y;
    zx = 0; zy = 0; zz = 1; zw = v.z;
    wx = 0; wy = 0; wz = 0; ww = 1;
}


// Scale (Multiplication that only scales, which skips lots of multiplications/additions)
// [xx*Sx, xy*Sy, xz*Sz, xw] = [xx, xy, xz, xw] * [Sx, 0, 0, 0]
// [yx*Sx, yy*Sy, yz*Sz, yw] = [yx, yy, yz, yw] * [0, Sy, 0, 0]
// [zx*Sx, zy*Sy, zz*Sz, zw] = [zx, zy, zz, zw] * [0, 0, Sz, 0]
// [wx*Sx, wy*Sy, wz*Sz, ww] = [wx, wy, wz, ww] * [0, 0, 0,  1]
Matrix4 Matrix4::scale(float sx, float sy, float sz) const {
    return Matrix4(
        xx*sx, xy*sy, xz*sz, xw,
        yx*sx, yy*sy, yz*sz, yw,
        zx*sx, zy*sy, zz*sz, zw,
        wx*sx, wy*sy, wz*sz, ww
    );
}
Matrix4 Matrix4::scale(const float xyz[3]) const {
    return scale(xyz[0], xyz[1], xyz[2]);
}
Matrix4 Matrix4::scale(const Vector3& v) const {
    return scale(v.x, v.y, v.z);
}
void Matrix4::scale_mutate(float sx, float sy, float sz) {
    xx *= sx; xy *= sy; xz *= sz;
    yx *= sx; yy *= sy; yz *= sz;
    zx *= sx; zy *= sy; zz *= sz;
    wx *= sx; wy *= sy; wz *= sz;
}
void Matrix4::scale_mutate(const float xyz[3]) {
    scale_mutate(xyz[0], xyz[1], xyz[2]);
}
void Matrix4::scale_mutate(Vector3& v) {
    scale_mutate(v.x, v.y, v.z);
}
// Left side transformation (WIP)
// [xx*Sx, xy*Sx, xz*Sx, xw*Sx] = [Sx, 0, 0, 0] * [xx, xy, xz, xw]
// [yx*Sy, yy*Sy, yz*Sy, yw*Sy] = [0, Sy, 0, 0] * [yx, yy, yz, yw]
// [zx*Sz, zy*Sz, zz*Sz, zw*Sz] = [0, 0, Sz, 0] * [zx, zy, zz, zw]
// [wx,    wy,    wz,    ww   ] = [0, 0, 0,  1] * [wx, wy, wz, ww]
Matrix4 Matrix4::scale_left_of_matrix(float sx, float sy, float sz) const {
    return Matrix4(
        xx*sx, xy*sx, xz*sx, xw*sx,
        yx*sy, yy*sy, yz*sy, yw*sy,
        zx*sz, zy*sz, zz*sz, zw*sz,
        wx,    wy,    wz,    ww
    );
}
Matrix4 Matrix4::scale_left_of_matrix(const float xyz[3]) const {
    return scale_left_of_matrix(xyz[0], xyz[1], xyz[2]);
}
Matrix4 Matrix4::scale_left_of_matrix(const Vector3& v) const {
    return scale_left_of_matrix(v.x, v.y, v.z);
}
void Matrix4::scale_left_of_matrix_mutate(float sx, float sy, float sz) {
    xx *= sx; xy *= sx; xz *= sx; xw *= sx;
    yx *= sy; yy *= sy; yz *= sy; yw *= sy;
    zx *= sz; zy *= sz; zz *= sz; zw *= sz;
}
void Matrix4::scale_left_of_matrix_mutate(const float xyz[3]) {
    scale_left_of_matrix_mutate(xyz[0], xyz[1], xyz[2]);
}
void Matrix4::scale_left_of_matrix_mutate(Vector3& v) {
    scale_left_of_matrix_mutate(v.x, v.y, v.z);
}
// Scale Matrix
// [Sx, 0, 0, 0]
// [0, Sy, 0, 0]
// [0, 0, Sz, 0]
// [0, 0, 0,  1]
Matrix4 Matrix4::scale_matrix(float x, float y, float z) {
    return Matrix4(x,0,0,0, 0,y,0,0, 0,0,z,0, 0,0,0,1);
}
Matrix4 Matrix4::scale_matrix(const float s[3]) {
    return Matrix4(s[0],0,0,0, 0,s[1],0,0, 0,0,s[2],0, 0,0,0,1);
}
Matrix4 Matrix4::scale_matrix(const Vector3& v) {
    return Matrix4(v.x,0,0,0, 0,v.y,0,0, 0,0,v.z,0, 0,0,0,1);
}
void Matrix4::scale_matrix_mutate(float x, float y, float z) {
    xx = x; xy = 0; xz = 0; xw = 0;
    yx = 0; yy = y; yz = 0; yw = 0;
    zx = 0; zy = 0; zz = z; zw = 0;
    wx = 0; wy = 0; wz = 0; ww = 1;
}
void Matrix4::scale_matrix_mutate(const float s[3]) {
    xx = s[0]; xy = 0;    xz = 0;    xw = 0;
    yx = 0;    yy = s[1]; yz = 0;    yw = 0;
    zx = 0;    zy = 0;    zz = s[2]; zw = 0;
    wx = 0;    wy = 0;    wz = 0;    ww = 1;
}
void Matrix4::scale_matrix_mutate(Vector3& v) {
    xx = v.x; xy = 0;   xz = 0;   xw = 0;
    yx = 0;   yy = v.y; yz = 0;   yw = 0;
    zx = 0;   zy = 0;   zz = v.z; zw = 0;
    wx = 0;   wy = 0;   wz = 0;   ww = 1;
}



// Rotation about Z-axis in degrees (X towards Y)
// [x*cos(a) - y*sin(a)] = [cos(a), -sin(a), 0, 0] * [x] // Negative sine if x going toward y is positive (right hand rule)
// [x*sin(a) + y*cos(a)] = [sin(a),  cos(a), 0, 0] * [y]
// [         z         ] = [   0  ,    0   , 1, 0] * [z]
// [         w         ] = [   0  ,    0   , 0, 1] * [w]
Vector3 Matrix4::rotate_xy(const Vector3& v, float angle) {
    angle *= TO_RADIANS;
    const float sine   = std::sin(angle);
    const float cosine = std::cos(angle);
    return Vector3(v.x*cosine - v.y*sine, v.x*sine + v.y*cosine, v.z);
}
void Matrix4::rotate_xy_mutate(Vector3& v, float angle) {
    angle *= TO_RADIANS;
    const float sine   = std::sin(angle);
    const float cosine = std::cos(angle);
    const float Y = v.x*sine + v.y*cosine;
    v.x = v.x*cosine - v.y*sine; v.y = Y;
}
Point4 Matrix4::rotate_xy(const Point4& p, float angle) {
    angle *= TO_RADIANS;
    const float sine   = std::sin(angle);
    const float cosine = std::cos(angle);
    return Point4(p.x*cosine - p.y*sine, p.x*sine + p.y*cosine, p.z, p.w);
}
void Matrix4::rotate_xy_mutate(Point4& p, float angle) {
    angle *= TO_RADIANS;
    const float sine   = std::sin(angle);
    const float cosine = std::cos(angle);
    const float Y = p.x*sine + p.y*cosine;
    p.x = p.x*cosine - p.y*sine; p.y = Y;
}
// Matrix Rotate with the Rotation Matrix on the right side
// [xx*cos(a) + xy*sin(a), xy*cos(a) - xx*sin(a), xz, xw] = [xx, xy, xz, xw] * [cos(a), -sin(a), 0, 0]
// [yx*cos(a) + yy*sin(a), yy*cos(a) - yx*sin(a), yz, yw] = [yx, yy, yz, yw] * [sin(a),  cos(a), 0, 0]
// [zx*cos(a) + zy*sin(a), zy*cos(a) - zx*sin(a), zz, zw] = [zx, zy, zz, zw] * [   0  ,    0   , 1, 0]
// [wx*cos(a) + wy*sin(a), wy*cos(a) - wx*sin(a), wz, ww] = [wx, wy, wz, ww] * [   0  ,    0   , 0, 1]
Matrix4 Matrix4::rotate_xy(float angle) const {
    angle *= TO_RADIANS;
    const float sine   = std::sin(angle);
    const float cosine = std::cos(angle);
    return Matrix4(
        xx*cosine + xy*sine, xy*cosine - xx*sine, xz, xw,
        yx*cosine + yy*sine, yy*cosine - yx*sine, yz, yw,
        zx*cosine + zy*sine, zy*cosine - zx*sine, zz, zw,
        wx*cosine + wy*sine, wy*cosine - wx*sine, wz, ww
    );
}
void Matrix4::rotate_xy_mutate(float angle) {
    angle *= TO_RADIANS;
    const float sine   = std::sin(angle);
    const float cosine = std::cos(angle);

    float
    Y  = xy*cosine - xx*sine; 
    xx = xx*cosine + xy*sine; 
    xy = Y; 

    Y  = yy*cosine - yx*sine;
    yx = yx*cosine + yy*sine;
    yy = Y; 

    Y  = zy*cosine - zx*sine;
    zx = zx*cosine + zy*sine;
    zy = Y; 

    Y  = wy*cosine - wx*sine;
    wx = wx*cosine + wy*sine;
    wy = Y;
}
// Matrix Rotate with the Rotation Matrix on the left side
// [xx*cos(a) - yx*sin(a), xy*cos(a) - yy*sin(a), xz*cos(a) - yz*sin(a), xw*cos(a) - yw*sin(a)] = [cos(a), -sin(a), 0, 0] * [xx, xy, xz, xw]
// [xx*sin(a) + yx*cos(a), xy*sin(a) + yy*cos(a), xz*sin(a) + yz*cos(a), xw*sin(a) + yw*cos(a)] = [sin(a),  cos(a), 0, 0] * [yx, yy, yz, yw]
// [          zx         ,           zy         ,           zz         ,           zw         ] = [   0  ,    0   , 1, 0] * [zx, zy, zz, zw]
// [          wx         ,           wy         ,           wz         ,           ww         ] = [   0  ,    0   , 0, 1] * [wx, wy, wz, ww]
Matrix4 Matrix4::rotate_xy_left_of_matrix(float angle) const {
    angle *= TO_RADIANS;
    const float sine   = std::sin(angle);
    const float cosine = std::cos(angle);
    return Matrix4(
        xx*cosine - yx*sine,   xy*cosine - yy*sine,   xz*cosine - yz*sine,   xw*cosine - yw*sine,
        xx*sine   + yx*cosine, xy*sine   + yy*cosine, xz*sine   + yz*cosine, xw*sine   + yw*cosine,
        zx, zy, zz, zw,
        wx, wy, wz, ww
    );
}
void Matrix4::rotate_xy_left_of_matrix_mutate(float angle) {
    angle *= TO_RADIANS;
    const float sine   = std::sin(angle);
    const float cosine = std::cos(angle);

    const float XX = xx*cosine - yx*sine;
    const float XY = xy*cosine - yy*sine;
    const float XZ = xz*cosine - yz*sine;
    const float XW = xw*cosine - yw*sine;

    const float YX = xx*sine + yx*cosine;
    const float YY = xy*sine + yy*cosine; 
    const float YZ = xz*sine + yz*cosine;
    const float YW = xw*sine + yw*cosine;

    xx = XX; xy = XY; xz = XZ; xw = XW;
    yx = YX; yy = YY; yz = YZ; yw = YW;
}
// Rotation Matrix on Z-axis (X to Y)
// [cos(a), -sin(a), 0, 0]
// [sin(a),  cos(a), 0, 0]
// [   0  ,    0   , 1, 0]
// [   0  ,    0   , 0, 1]
Matrix4 Matrix4::rotation_xy_matrix(float angle) {
    angle *= TO_RADIANS;
    const float sine   = std::sin(angle);
    const float cosine = std::cos(angle);
    return Matrix4(
        cosine, -sine , 0, 0,
         sine , cosine, 0, 0,
           0  ,   0   , 1, 0,
           0  ,   0   , 0, 1
    );
}
void Matrix4::rotation_xy_matrix_mutate(float angle) {
    angle *= TO_RADIANS;
    const float sine   = std::sin(angle);
    const float cosine = std::cos(angle);
    xx = cosine; xy = -sine;  xz = 0; xw = 0;
    yx = sine;   yy = cosine; yz = 0; yw = 0;
    zx = 0;      zy = 0;      zz = 1; zw = 0;
    wx = 0;      wy = 0;      wz = 0; ww = 1;
}



// Rotate about X-axis
// [         x         ] = [1,   0   ,    0   , 0] * [x]
// [y*cos(a) - z*sin(a)] = [0, cos(a), -sin(a), 0] * [y]
// [y*sin(a) + z*cos(a)] = [0, sin(a),  cos(a), 0] * [z]
// [         w         ] = [0,   0   ,    0   , 1] * [w]
Vector3 Matrix4::rotate_yz(const Vector3& v, float angle) {
    angle *= TO_RADIANS;
    const float sine   = std::sin(angle);
    const float cosine = std::cos(angle);
    return Vector3(v.x, v.y*cosine - v.z*sine, v.y*sine + v.z*cosine);
}
void Matrix4::rotate_yz_mutate(Vector3& v, float angle) {
    angle *= TO_RADIANS;
    const float sine   = std::sin(angle);
    const float cosine = std::cos(angle);
    const float Z = v.y*sine + v.z*cosine;
    v.y = v.y*cosine - v.z*sine; v.z = Z;
}
Point4 Matrix4::rotate_yz(const Point4& p, float angle) {
    angle *= TO_RADIANS;
    const float sine   = std::sin(angle);
    const float cosine = std::cos(angle);
    return Point4(p.x, p.y*cosine - p.z*sine, p.y*sine + p.z*cosine, p.w);
}
void Matrix4::rotate_yz_mutate(Point4& p, float angle) {
    angle *= TO_RADIANS;
    const float sine   = std::sin(angle);
    const float cosine = std::cos(angle);
    const float Z = p.y*sine + p.z*cosine;
    p.y = p.y*cosine - p.z*sine; p.z = Z;
}
// Matrix Rotate with the Rotation Matrix on the right side
// [xx, xy*cos(a) + xz*sin(a), xz*cos(a) - xy*sin(a), xw] = [xx, xy, xz, xw] * [1,   0   ,    0   , 0]
// [yx, yy*cos(a) + yz*sin(a), yz*cos(a) - yy*sin(a), yw] = [yx, yy, yz, yw] * [0, cos(a), -sin(a), 0]
// [zx, zy*cos(a) + zz*sin(a), zz*cos(a) - zy*sin(a), zw] = [zx, zy, zz, zw] * [0, sin(a),  cos(a), 0]
// [wx, wy*cos(a) + wz*sin(a), wz*cos(a) - wy*sin(a), ww] = [wx, wy, wz, ww] * [0,   0   ,    0   , 1]
Matrix4 Matrix4::rotate_yz(float angle) const {
    angle *= TO_RADIANS;
    const float sine   = std::sin(angle);
    const float cosine = std::cos(angle);
    return Matrix4(
        xx, xy*cosine + xz*sine, xz*cosine - xy*sine, xw,
        yx, yy*cosine + yz*sine, yz*cosine - yy*sine, yw,
        zx, zy*cosine + zz*sine, zz*cosine - zy*sine, zw,
        wx, wy*cosine + wz*sine, wz*cosine - wy*sine, ww
    );
}
void Matrix4::rotate_yz_mutate(float angle) {
    angle *= TO_RADIANS;
    const float sine   = std::sin(angle);
    const float cosine = std::cos(angle);

    float 
    Z  = xz*cosine - xy*sine;
    xy = xy*cosine + xz*sine;
    xz = Z;

    Z  = yz*cosine - yy*sine;
    yy = yy*cosine + yz*sine;
    yz = Z;

    Z  = zz*cosine - zy*sine;
    zy = zy*cosine + zz*sine;
    zz = Z;

    Z  = wz*cosine - wy*sine;
    wy = wy*cosine + wz*sine;
    wz = Z;
}
// Matrix Rotate
// [          xx         ,           xy         ,           xz         ,          x         ] = [1,   0   ,    0   , 0] * [xx, xy, xz, x]
// [yx*cos(a) - zx*sin(a), yy*cos(a) - zy*sin(a), yz*cos(a) - zz*sin(a), y*cos(a) - z*sin(a)] = [0, cos(a), -sin(a), 0] * [yx, yy, yz, y]
// [yx*sin(a) + zx*cos(a), yy*sin(a) + zy*cos(a), yz*sin(a) + zz*cos(a), y*sin(a) + z*cos(a)] = [0, sin(a),  cos(a), 0] * [zx, zy, zz, z]
// [          wx         ,           wy         ,           wz         ,          w         ] = [0,   0   ,    0   , 1] * [wx, wy, wz, w]
Matrix4 Matrix4::rotate_yz_left_of_matrix(float angle) const {
    angle *= TO_RADIANS;
    const float sine   = std::sin(angle);
    const float cosine = std::cos(angle);
    return Matrix4(
        xx, xy, xz, xw,
        yx*cosine - zx*sine,   yy*cosine - zy*sine,   yz*cosine - zz*sine,   yw*cosine - zw*sine,
        yx*sine   + zx*cosine, yy*sine   + zy*cosine, yz*sine   + zz*cosine, yw*sine   + zw*cosine,
        wx, wy, wz, ww
    );
}
void Matrix4::rotate_yz_left_of_matrix_mutate(float angle) {
    angle *= TO_RADIANS;
    const float sine   = std::sin(angle);
    const float cosine = std::cos(angle);

    const float YX = yx*cosine - zx*sine;
    const float YY = yy*cosine - zy*sine;
    const float YZ = yz*cosine - zz*sine;
    const float YW = yw*cosine - zw*sine;

    const float ZX = yx*sine + zx*cosine;
    const float ZY = yy*sine + zy*cosine; 
    const float ZZ = yz*sine + zz*cosine;
    const float ZW = yw*sine + zw*cosine;

    yx = YX; yy = YY; yz = YZ; yw = YW;
    zx = ZX; zy = ZY; zz = ZZ; zw = ZW;
}
// Rotation Matrix on X-axis (Y to Z)
// [1,   0   ,    0   , 0]
// [0, cos(a), -sin(a), 0]
// [0, sin(a),  cos(a), 0]
// [0,   0   ,    0   , 1]
Matrix4 Matrix4::rotation_yz_matrix(float angle) {
    angle *= TO_RADIANS;
    const float sine   = std::sin(angle);
    const float cosine = std::cos(angle);
    return Matrix4(
        1,   0   ,   0   , 0,
        0, cosine, -sine , 0,
        0,  sine , cosine, 0,
        0,   0   ,   0   , 1
    );
}
void Matrix4::rotation_yz_matrix_mutate(float angle) {
    angle *= TO_RADIANS;
    const float sine   = std::sin(angle);
    const float cosine = std::cos(angle);
    xx = 1; xy = 0;      xz = 0;      xw = 0;
    yx = 0; yy = cosine; yz = -sine;  yw = 0;
    zx = 0; zy = sine;   zz = cosine; zw = 0;
    wx = 0; wy = 0;      wz = 0;      ww = 1;
}



// Rotate about Y-axis (with sign flip)
// [z*sin(a) + x*cos(a)] = [ cos(a), 0, sin(a), 0] * [x]
// [         y         ] = [   0   , 1,   0   , 0] * [y]
// [z*cos(a) - x*sin(a)] = [-sin(a), 0, cos(a), 0] * [z]
// [         w         ] = [   0   , 0,   0   , 1] * [w]
Vector3 Matrix4::rotate_zx(const Vector3& v, float angle) {
    angle *= TO_RADIANS;
    const float sine   = std::sin(angle);
    const float cosine = std::cos(angle);
    return Vector3(v.z*sine + v.x*cosine, v.y, v.z*cosine - v.x*sine);
}
void Matrix4::rotate_zx_mutate(Vector3& v, float angle) {
    angle *= TO_RADIANS;
    const float sine   = std::sin(angle);
    const float cosine = std::cos(angle);
    const float Z = v.z*cosine - v.x*sine;
    v.x = v.z*sine + v.x*cosine; v.z = Z;
}
Point4 Matrix4::rotate_zx(const Point4& p, float angle) {
    angle *= TO_RADIANS;
    const float sine   = std::sin(angle);
    const float cosine = std::cos(angle);
    return Point4(p.z*sine + p.x*cosine, p.y, p.z*cosine - p.x*sine, p.w);
}
void Matrix4::rotate_zx_mutate(Point4& p, float angle) {
    angle *= TO_RADIANS;
    const float sine   = std::sin(angle);
    const float cosine = std::cos(angle);
    const float Z = p.z*cosine - p.x*sine;
    p.x = p.z*sine + p.x*cosine; p.z = Z;
}
// Matrix Rotate with the Rotation Matrix on the right side
// [xx*cos(a) - xz*sin(a), xy, xz*cos(a) + xx*sin(a), xw] = [xx, xy, xz, xw] * [ cos(a), 0, sin(a), 0]
// [yx*cos(a) - yz*sin(a), yy, yz*cos(a) + yx*sin(a), yw] = [yx, yy, yz, yw] * [   0   , 1,   0   , 0]
// [zx*cos(a) - zz*sin(a), zy, zz*cos(a) + zx*sin(a), zw] = [zx, zy, zz, zw] * [-sin(a), 0, cos(a), 0]
// [wx*cos(a) - wz*sin(a), wy, wz*cos(a) + wx*sin(a), ww] = [wx, wy, wz, ww] * [   0   , 0,   0   , 1]
Matrix4 Matrix4::rotate_zx(float angle) const {
    angle *= TO_RADIANS;
    const float sine   = std::sin(angle);
    const float cosine = std::cos(angle);
    return Matrix4(
        xx*cosine - xz*sine, xy, xz*cosine + xx*sine, xw,
        yx*cosine - yz*sine, yy, yz*cosine + yx*sine, yw,
        zx*cosine - zz*sine, zy, zz*cosine + zx*sine, zw,
        wx*cosine - wz*sine, wy, wz*cosine + wx*sine, ww
    );
}
void Matrix4::rotate_zx_mutate(float angle) {
    angle *= TO_RADIANS;
    const float sine   = std::sin(angle);
    const float cosine = std::cos(angle);

    float 
    Z  = xz*cosine + xx*sine;
    xx = xx*cosine - xz*sine;
    xz = Z;

    Z  = yz*cosine + yx*sine;
    yx = yx*cosine - yz*sine;
    yz = Z;

    Z  = zz*cosine + zx*sine;
    zx = zx*cosine - zz*sine;
    zz = Z;

    Z  = wz*cosine + wx*sine;
    wx = wx*cosine - wz*sine;
    wz = Z;
}
// Matrix Rotate with the Rotation Matrix on the left side
// [zx*sin(a) + xx*cos(a), zy*sin(a) + xy*cos(a), zz*sin(a) + xz*cos(a), zw*sin(a) + xw*cos(a)] = [ cos(a), 0, sin(a), 0] * [xx, xy, xz, xw]
// [          yx         ,           yy         ,           yz         ,           y          ] = [   0   , 1,   0   , 0] * [yx, yy, yz, yw]
// [zx*cos(a) - xx*sin(a), zy*cos(a) - xy*sin(a), zz*cos(a) - xz*sin(a), zw*cos(a) - xw*sin(a)] = [-sin(a), 0, cos(a), 0] * [zx, zy, zz, zw]
// [          wx         ,           wy         ,           wz         ,           w          ] = [   0   , 0,   0   , 1] * [wx, wy, wz, ww]
Matrix4 Matrix4::rotate_zx_left_of_matrix(float angle) const {
    angle *= TO_RADIANS;
    const float sine   = std::sin(angle);
    const float cosine = std::cos(angle);
    return Matrix4(
        zx*sine   + xx*cosine, zy*sine   + xy*cosine, zz*sine   + xz*cosine, zw*sine   + xw*cosine,
        yx, yy, yz, yw,
        zx*cosine - xx*sine,   zy*cosine - xy*sine,   zz*cosine - xz*sine,   zw*cosine - xw*sine,
        wx, wy, wz, ww
    );
}
void Matrix4::rotate_zx_left_of_matrix_mutate(float angle) {
    angle *= TO_RADIANS;
    const float sine   = std::sin(angle);
    const float cosine = std::cos(angle);

    const float XX = zx*sine + xx*cosine;
    const float XY = zy*sine + xy*cosine;
    const float XZ = zz*sine + xz*cosine;
    const float XW = zw*sine + xw*cosine;

    const float ZX = zx*cosine - xx*sine;
    const float ZY = zy*cosine - xy*sine;
    const float ZZ = zz*cosine - xz*sine;
    const float ZW = zw*cosine - xw*sine;

    xx = XX; xy = XY; xz = XZ; xw = XW;
    zx = ZX; zy = ZY; zz = ZZ; zw = ZW;
}
// Rotation Matrix on Y-axis (Z to X)
// [ cos(a), 0, sin(a), 0]
// [   0   , 1,   0   , 0]
// [-sin(a), 0, cos(a), 0]
// [   0   , 0,   0   , 1]
Matrix4 Matrix4::rotation_zx_matrix(float angle) {
    angle *= TO_RADIANS;
    const float sine   = std::sin(angle);
    const float cosine = std::cos(angle);
    return Matrix4(
        cosine, 0,  sine , 0,
           0  , 1,   0   , 0,
        -sine , 0, cosine, 0,
           0  , 0,   0   , 1
    );
}
void Matrix4::rotation_zx_matrix_mutate(float angle) {
    angle *= TO_RADIANS;
    const float sine   = std::sin(angle);
    const float cosine = std::cos(angle);
    xx = cosine; xy = 0; xz = sine;   xw = 0;
    yx = 0;      yy = 1; yz = 0;      yw = 0;
    zx = -sine;  zy = 0; zz = cosine; zw = 0;
    wx = 0;      wy = 0; wz = 0;      ww = 1;
}



// Graphics Pipeline
// Note:
// camera to world (T x R)
// [Wx] = [1, 0, 0, Ex] * [xx, xy, xz, 0]
// [Wy] = [0, 1, 0, Ey] * [yx, yy, yz, 0]
// [Wz] = [0, 0, 1, Ez] * [zx, xy, zz, 0]
// [ 0] = [0, 0, 0,  1] * [ 0,  0,  0, 1]
// 
// Pcamera = R^-1 * T^-1 Pworld
// world to camera (R.inverse() x T.inverse())
// Px[xx, yx, zx, -xx*Ex -yx*Ey -zx*Ez] = [xx, yx, zx, 0] * [1, 0, 0, -Ex]
// Py[xy, yy, zy, -xy*Ex -yy*Ey -zy*Ez] = [xy, yy, zy, 0] * [0, 1, 0, -Ey]
// Pz[xz, yz, zz, -xz*Ex -yz*Ey -zz*Ez] = [xz, yz, zz, 0] * [0, 0, 1, -Ez]
// Pw[ 0,  0,  0,           1         ] = [ 0,  0,  0, 1] * [0, 0, 0,   1]
Matrix4 Matrix4::world_to_camera(Point4& eye, Point4& at, Vector3& up) { // Convert to cartesian
    Vector3 e = eye.to_vector3();
    Vector3 a = at.to_vector3();
    return world_to_camera(e, a, up);
}
Matrix4 Matrix4::world_to_camera(Vector3& eye, Vector3& at, Vector3& up) {
    // Calculate camera properties
    Vector3 F = at.subtract(eye); // 1. Direction the camera is looking (at - eye)
    F = F.normalize();            // 2. Normalize it
    Vector3 R = F.cross(up);      // 3. Direction of the "right" side of the camera
    R = R.normalize();            // 4. Normalize it
    Vector3 U = R.cross(F);       // 5. Direction of the "top" head of the camera
    U = U.normalize();            // 6. Normalize it (Optional, since cross of normals is also normal, but squashes risk of floating errors)

    // Use calculations to return the matrix
    return Matrix4( 
        R.x, R.y, R.z, -R.dot(eye),
        U.x, U.y, U.z, -U.dot(eye),
        F.x, F.y, F.z, -F.dot(eye),
        0,0,0,1
    );
}
// Raw version using variables
Matrix4 Matrix4::world_to_camera(float eyeX, float eyeY, float eyeZ, float atX, float atY, float atZ, float upX, float upY, float upZ) {
    // 1. Direction the camera is looking
    float xz = atX - eyeX;
    float yz = atY - eyeY;
    float zz = atZ - eyeZ;

    // 2. Normalize it
    Vector3::normalize(xz, yz, zz);

    // 3. Direction of the "right" side of the camera
    float xx, yx, zx;
    Vector3::cross(xx,yx,zx, xz,yz,zz, upX,upY,upZ);

    // 4. Normalize it
    Vector3::normalize(xx, yx, zx);

    // 5. Direction of the "top" head of the camera
    float xy, yy, zy;
    Vector3::cross(xy,yy,zy, xx,yx,zx, xz,yz,zz);

    // Finally, calculate and return the matrix
    return Matrix4(
        xx, xy, xz, -xx*eyeX -xy*eyeY -xz*eyeZ,
        yx, yy, yz, -yx*eyeX -yy*eyeY -yz*eyeZ,
        zx, zy, zz, -zx*eyeX -zy*eyeY -zz*eyeZ,
        0,0,0,1
    );
}


// 3D "clip" of space where objects farther objects are "smaller"
// Objects should eventually "block" each other on FOV
// fov is given in degrees
// 
// aspect = width/height
// a(aspect), o(fov), f(far), n(near)
//                                                        Scale/Translate clip from (-1,-1,-1)(1,1,1) to (0,0,-1)(1,1,1)
// [1/(2a*tan(o/2),       0      ,   1/2  ,     0    ] = [1/2,  0 , 0, 1/2] * [1/(a*tan(o/2)),     0     ,    0   ,     0    ]
// [       0      , 1/(2tan(o/2)),   1/2  ,     0    ] = [ 0 , 1/2, 0, 1/2] * [       0      , 1/tan(o/2),    0   ,     0    ]
// [       0      ,       0      , f/(f-n), -fn/(f-n)] = [ 0 ,  0 , 1,  0 ] * [       0      ,     0     , f/(f-n), -fn/(f-n)]
// [       0      ,       0      ,    1   ,     0    ] = [ 0 ,  0 , 0,  1 ] * [       0      ,     0     ,    1   ,     0    ]
Matrix4 Matrix4::camera_to_clip(float fov, float near, float far, float aspect) {
    const float TAN2 = std::tan(fov * TO_RADIANS_2) * 2;
    const float F_FN = far / (far - near);
    return Matrix4(
        (float)1.0f/(aspect*TAN2),        0        , 0.5f,     0     ,
                    0            , (float)1.0f/TAN2, 0.5f,     0     ,
                    0            ,        0        , F_FN, -near*F_FN,
                    0            ,        0        ,  1  ,     0
    );
}

// Transforms clip coordinates from -1 to 1 into screen coordinates from 0 to size
// This is because the screen starts at 0,0 from the top-left, 
// while the clip starts at 0,0 from bottom-left.
// 
// (bottom-left) to (top-right) --> (top-left) to (bottom-right)
// (0, 0) to (1, 1)  -->  (0, -height) to (width, 0)
// 
// width - e & height - e: small_epsilon so drawing and clipping is not off by 1 (when floored)
// 
// So scale x,y to screen size, then translate it to screen position
//                                 T(0, height, 0) * S(width, -height, 1)
// [width,    0   , 0,   0   ] = [1, 0, 0,   0   ] * [width,   0, 0, 0]
// [  0  , -height, 0, height] = [0, 1, 0, height] * [0, -height, 0, 0]
// [  0  ,    0   , 1,   0   ] = [0, 0, 1,   0   ] * [0,       0, 1, 0]
// [  0  ,    0   , 0,   1   ] = [0, 0, 0,   1   ] * [0,       0, 0, 1]
Matrix4 Matrix4::clip_to_device(int width, int height) {
    float w = width  - 0.00390625f; // minus epsilon for probably for line clipping or z_buffer (forgot)
    float h = height - 0.00390625f; // arbitrary epsilon of 1/256
    return Matrix4(
        w,  0, 0, 0-.5f, // -.5 for polygon clipping (and especially scan conversion)
        0, -h, 0, h-.5f, // -.5 ...lets me do ceil(x) instead of std::ceil(x - .5)
        0,  0, 1, 0,
        0,  0, 0, 1  
    );
    //).translate(.25,.25,0).scale(.5,.5,1); // Shortens the viewport to help debug clipping
    //).translate(.125,.125,0).scale(.75,.75,1); // 3/4
}