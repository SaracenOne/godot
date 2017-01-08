/*************************************************************************/
/*  vector3.h                                                            */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                    http://www.godotengine.org                         */
/*************************************************************************/
/* Copyright (c) 2007-2016 Juan Linietsky, Ariel Manzur.                 */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/
#ifndef VECTOR3_H
#define VECTOR3_H

#include "typedefs.h"
#include "math_defs.h"
#include "math_funcs.h"
#include "ustring.h"

const SimdFloat4 GD_ALIGN16(sse_vMzeroMask) = { -0.0f, -0.0f, -0.0f, -0.0f };
const SimdFloat4 GD_ALIGN16(sse_v1110) = { 1.0f, 1.0f, 1.0f, 0.0f };
const SimdFloat4 GD_ALIGN16(sse_vHalf) = { 0.5f, 0.5f, 0.5f, 0.5f };
const SimdFloat4 GD_ALIGN16(sse_v1_5) = { 1.5f, 1.5f, 1.5f, 1.5f };

#ifdef _MSC_VER
#pragma warning(disable: 4556) // value of intrinsic immediate argument '4294967239' is out of range '0 - 255'
#endif

GD_ALIGN16(struct) Vector3 {

	enum Axis {
		AXIS_X,
		AXIS_Y,
		AXIS_Z,
	};

	union {

#ifdef USE_QUAD_VECTORS

		GD_ALIGN16(struct) {
			real_t x;
			real_t y;
			real_t z;
			real_t _unused;
		};
		real_t coord[4];
#if defined (USE_SSE) || defined (USE_NEON)
		SimdFloat4 vec128;
#endif
#else

		struct {
			real_t x;
			real_t y;
			real_t z;
		};

		real_t coord[3];
#endif
	};


#if defined (USE_SSE) || defined (USE_NEON)
	_FORCE_INLINE_ Vector3(SimdFloat4 p_v) {

		vec128 = p_v;
	}

	_FORCE_INLINE_ Vector3(const Vector3& p_v)
	{
		vec128 = p_v.vec128;
	}

	_FORCE_INLINE_ Vector3& operator=(const Vector3& p_v) {

		vec128 = p_v.vec128;
		return *this;
	}
#endif


	_FORCE_INLINE_ const real_t& operator[](int p_axis) const {

		return coord[p_axis];
	}

	_FORCE_INLINE_ real_t& operator[](int p_axis) {

		return coord[p_axis];
	}

	void set_axis(int p_axis,real_t p_value);
	real_t get_axis(int p_axis) const;

	int min_axis() const;
	int max_axis() const;

	_FORCE_INLINE_ real_t length() const;
	_FORCE_INLINE_ real_t length_squared() const;

	_FORCE_INLINE_ void fast_normalize();
	_FORCE_INLINE_ void normalize();
	_FORCE_INLINE_ Vector3 normalized() const;
	_FORCE_INLINE_ Vector3 inverse() const;

	_FORCE_INLINE_ void zero();

	void snap(float p_val);
	Vector3 snapped(float p_val) const;

	void rotate(const Vector3& p_axis,float p_phi);
	Vector3 rotated(const Vector3& p_axis,float p_phi) const;

	/* Static Methods between 2 vector3s */

	_FORCE_INLINE_ Vector3 linear_interpolate(const Vector3& p_b,float p_t) const;
	Vector3 cubic_interpolate(const Vector3& p_b,const Vector3& p_pre_a, const Vector3& p_post_b,float p_t) const;
	Vector3 cubic_interpolaten(const Vector3& p_b,const Vector3& p_pre_a, const Vector3& p_post_b,float p_t) const;

	_FORCE_INLINE_ Vector3 cross(const Vector3& p_b) const;
	_FORCE_INLINE_ real_t dot(const Vector3& p_b) const;
	_FORCE_INLINE_ Vector3 dot3(const Vector3 &p_v0, const Vector3 &p_v1, const Vector3 &p_v2) const;

	_FORCE_INLINE_ Vector3 abs() const;
	_FORCE_INLINE_ Vector3 floor() const;
	_FORCE_INLINE_ Vector3 ceil() const;

	_FORCE_INLINE_ real_t distance_to(const Vector3& p_b) const;
	_FORCE_INLINE_ real_t distance_squared_to(const Vector3& p_b) const;

	_FORCE_INLINE_ real_t angle_to(const Vector3& p_b) const;


	_FORCE_INLINE_ Vector3 slide(const Vector3& p_vec) const;
	_FORCE_INLINE_ Vector3 reflect(const Vector3& p_vec) const;


	_FORCE_INLINE_ real_t triple(const Vector3& p_v1, const Vector3& p_v2) const;

	/* Operators */

	_FORCE_INLINE_ Vector3& operator+=(const Vector3& p_v);
	_FORCE_INLINE_ Vector3 operator+(const Vector3& p_v) const;
	_FORCE_INLINE_ Vector3& operator-=(const Vector3& p_v);
	_FORCE_INLINE_ Vector3 operator-(const Vector3& p_v) const;
	_FORCE_INLINE_ Vector3& operator*=(const Vector3& p_v);
	_FORCE_INLINE_ Vector3 operator*(const Vector3& p_v) const;
	_FORCE_INLINE_ Vector3& operator/=(const Vector3& p_v);
	_FORCE_INLINE_ Vector3 operator/(const Vector3& p_v) const;

	/* Operators */

	_FORCE_INLINE_ Vector3& operator*=(real_t p_scalar);
	_FORCE_INLINE_ Vector3 operator*(real_t p_scalar) const;
	_FORCE_INLINE_ Vector3& operator/=(real_t p_scalar);
	_FORCE_INLINE_ Vector3 operator/(real_t p_scalar) const;

	_FORCE_INLINE_ Vector3 operator-() const;

	_FORCE_INLINE_ bool operator==(const Vector3& p_v) const;
	_FORCE_INLINE_ bool operator!=(const Vector3& p_v) const;
	_FORCE_INLINE_ bool operator<(const Vector3& p_v) const;
	_FORCE_INLINE_ bool operator<=(const Vector3& p_v) const;

	operator String() const;

	_FORCE_INLINE_ Vector3() { zero(); }
	_FORCE_INLINE_ Vector3(real_t p_x,real_t p_y,real_t p_z) { set(p_x, p_y, p_z); }
	_FORCE_INLINE_ void set(real_t p_x, real_t p_y, real_t p_z) { x = p_x; y = p_y; z = p_z; _unused = real_t(0.f); }

};

#ifdef VECTOR3_IMPL_OVERRIDE

#include "vector3_inline.h"

#else

Vector3 Vector3::cross(const Vector3& p_b) const {
#if defined (USE_SSE)
	__m128	T, V;

	T = pshufd_ps(vec128, SHUFFLE(1, 2, 0, 3));	//	(Y Z X 0)
	V = pshufd_ps(p_b.vec128, SHUFFLE(1, 2, 0, 3));	//	(Y Z X 0)

	V = _mm_mul_ps(V, vec128);
	T = _mm_mul_ps(T, p_b.vec128);
	V = _mm_sub_ps(V, T);

	V = pshufd_ps(V, SHUFFLE(1, 2, 0, 3));
	return Vector3(V);
#elif defined(USE_NEON)
	float32x4_t T, V;
	// form (Y, Z, X, _) of vec128 and v.vec128
	float32x2_t Tlow = vget_low_f32(vec128);
	float32x2_t Vlow = vget_low_f32(p_b.vec128);
	T = vcombine_f32(vext_f32(Tlow, vget_high_f32(vec128), 1), Tlow);
	V = vcombine_f32(vext_f32(Vlow, vget_high_f32(p_b.vec128), 1), Vlow);

	V = vmulq_f32(V, vec128);
	T = vmulq_f32(T, p_b.vec128);
	V = vsubq_f32(V, T);
	Vlow = vget_low_f32(V);
	// form (Y, Z, X, _);
	V = vcombine_f32(vext_f32(Vlow, vget_high_f32(V), 1), Vlow);
	V = (float32x4_t)vandq_s32((int32x4_t)V, vFFF0Mask);

	return Vector3(V);
#else
	return Vector3(
		(y * p_b.z) - (z * p_b.y),
		(z * p_b.x) - (x * p_b.z),
		(x * p_b.y) - (y * p_b.x)
		);
#endif
}

real_t Vector3::dot(const Vector3& p_b) const {
#if defined (USE_SSE)
	__m128 vd = _mm_mul_ps(vec128, p_b.vec128);
	__m128 z = _mm_movehl_ps(vd, vd);
	__m128 y = _mm_shuffle_ps(vd, vd, 0x55);
	vd = _mm_add_ss(vd, y);
	vd = _mm_add_ss(vd, z);
	return _mm_cvtss_f32(vd);
#elif defined(USE_NEON)
	float32x4_t vd = vmulq_f32(vec128, p_b.vec128);
	float32x2_t x = vpadd_f32(vget_low_f32(vd), vget_low_f32(vd));
	x = vadd_f32(x, vget_high_f32(vd));
	return vget_lane_f32(x, 0);
#else
	return x*p_b.x + y*p_b.y + z*p_b.z;
#endif
}
Vector3 Vector3::dot3(const Vector3 &p_v0, const Vector3 &p_v1, const Vector3 &p_v2) const {
#if defined (USE_SSE)

	__m128 a0 = _mm_mul_ps(p_v0.vec128, this->vec128);
	__m128 a1 = _mm_mul_ps(p_v1.vec128, this->vec128);
	__m128 a2 = _mm_mul_ps(p_v2.vec128, this->vec128);
	__m128 b0 = _mm_unpacklo_ps(a0, a1);
	__m128 b1 = _mm_unpackhi_ps(a0, a1);
	__m128 b2 = _mm_unpacklo_ps(a2, _mm_setzero_ps());
	__m128 r = _mm_movelh_ps(b0, b2);
	r = _mm_add_ps(r, _mm_movehl_ps(b2, b0));
	a2 = _mm_and_ps(a2, vxyzMaskf);
	r = _mm_add_ps(r, CastdTo128f(_mm_move_sd(CastfTo128d(a2), CastfTo128d(b1))));
	return Vector3(r);

#elif defined(USE_NEON)
	static const uint32x4_t xyzMask = (const uint32x4_t){ static_cast<uint32_t>(-1), static_cast<uint32_t>(-1), static_cast<uint32_t>(-1), 0 };
	float32x4_t a0 = vmulq_f32(p_v0.vec128, this->vec128);
	float32x4_t a1 = vmulq_f32(p_v1.vec128, this->vec128);
	float32x4_t a2 = vmulq_f32(p_v2.vec128, this->vec128);
	float32x2x2_t zLo = vtrn_f32(vget_high_f32(a0), vget_high_f32(a1));
	a2 = (float32x4_t)vandq_u32((uint32x4_t)a2, xyzMask);
	float32x2_t b0 = vadd_f32(vpadd_f32(vget_low_f32(a0), vget_low_f32(a1)), zLo.val[0]);
	float32x2_t b1 = vpadd_f32(vpadd_f32(vget_low_f32(a2), vget_high_f32(a2)), vdup_n_f32(0.0f));
	return Vector3(vcombine_f32(b0, b1));
#else
	return Vector3(dot(p_v0), dot(p_v1), dot(p_v2));
#endif
}

Vector3 Vector3::abs() const {

	return Vector3( Math::abs(x), Math::abs(y), Math::abs(z) );
}

Vector3 Vector3::floor() const {

	return Vector3( Math::floor(x), Math::floor(y), Math::floor(z) );
}

Vector3 Vector3::ceil() const {

	return Vector3( Math::ceil(x), Math::ceil(y), Math::ceil(z) );
}

Vector3 Vector3::linear_interpolate(const Vector3& p_b, float p_t) const {
#if defined (USE_SSE)
	__m128	vt = _mm_load_ss(&p_t);	//	(t 0 0 0)
	vt = pshufd_ps(vt, 0x80);	//	(rt rt rt 0.0)
	__m128 vl = _mm_sub_ps(p_b.vec128, vec128);
	vl = _mm_mul_ps(vl, vt);
	vl = _mm_add_ps(vl, vec128);

	return Vector3(vl);
#elif defined(USE_NEON)
	float32x4_t vl = vsubq_f32(p_b.vec128, vec128);
	vl = vmulq_n_f32(vl, p_t);
	vl = vaddq_f32(vl, vec128);

	return Vector3(vl);
#else
	return Vector3(
		x+(p_t * (p_b.x-x)),
		y+(p_t * (p_b.y-y)),
		z+(p_t * (p_b.z-z))
	);
#endif
}

real_t Vector3::distance_to(const Vector3& p_b) const {

	return (p_b-*this).length();
}

real_t Vector3::distance_squared_to(const Vector3& p_b) const {

	return (p_b-*this).length_squared();
}

real_t Vector3::angle_to(const Vector3& p_b) const {

	return Math::acos(this->dot(p_b) / Math::sqrt(this->length_squared() * p_b.length_squared()));
}

/* Operators */

Vector3& Vector3::operator+=(const Vector3& p_v) {
#if defined (USE_SSE)
	vec128 = _mm_add_ps(vec128, p_v.vec128);
#elif defined(USE_NEON)
	vec128 = vaddq_f32(vec128, p_v.vec128);
#else
	x += p_v.x;
	y += p_v.y;
	z += p_v.z;
#endif
	return *this;
}

Vector3 Vector3::operator+(const Vector3& p_v) const {
#if defined (USE_SSE)
	return Vector3(_mm_add_ps(vec128, p_v.vec128));
#elif defined (USE_NEON)
	return Vector3(vaddq_f32(vec128, p_v.vec128));
#else
	return Vector3(x + p_v.x, y + p_v.y, z + p_v.z);
#endif
}

Vector3& Vector3::operator-=(const Vector3& p_v) {
#if defined(USE_SSE)
	vec128 = _mm_sub_ps(vec128, p_v.vec128);
#elif defined(USE_NEON)
	vec128 = vsubq_f32(vec128, p_v.vec128);
#else
	x -= p_v.x;
	y -= p_v.y;
	z -= p_v.z;
#endif
	return *this;
}
Vector3 Vector3::operator-(const Vector3& p_v) const {
#if defined(USE_SSE)

	//	without _mm_and_ps this code causes slowdown in Concave moving
	__m128 r = _mm_sub_ps(vec128, p_v.vec128);
	return Vector3(_mm_and_ps(r, vFFF0fMask));
#elif defined(USE_NEON)
	float32x4_t r = vsubq_f32(vec128, p_v.vec128);
	return Vector3((float32x4_t)vandq_s32((int32x4_t)r, vFFF0Mask));
#else
	return Vector3(x - p_v.x, y - p_v.y, z - p_v.z);
#endif
}

Vector3& Vector3::operator*=(const Vector3& p_v) {
#if defined (USE_SSE)
	vec128 = _mm_mul_ps(vec128, p_v.vec128);
#elif defined(USE_NEON)
	vec128 = vmulq_f32(vec128, p_v.vec128);
#else
	x *= p_v.x;
	y *= p_v.y;
	z *= p_v.z;
#endif
	return *this;
}
Vector3 Vector3::operator*(const Vector3& p_v) const {
#if defined (USE_SSE)
	return Vector3(_mm_mul_ps(vec128, p_v.vec128));
#elif defined(USE_NEON)
	return Vector3(vmulq_f32(vec128, p_v.vec128));
#else
	return Vector3(x*p_v.x, y*p_v.y, z* p_v.z);
#endif
}

Vector3& Vector3::operator/=(const Vector3& p_v) {
#if defined (USE_SSE)
	__m128 vec = _mm_div_ps(vec128, p_v.vec128);
	vec128 = _mm_and_ps(vec, vFFF0fMask);
#elif defined(USE_NEON)
	float32x4_t x, y, v, m;

	x = vec128;
	y = p_v.vec128;

	v = vrecpeq_f32(y);			// v ~ 1/y
	m = vrecpsq_f32(y, v);		// m = (2-v*y)
	v = vmulq_f32(v, m);		// vv = v*m ~~ 1/y
	m = vrecpsq_f32(y, v);		// mm = (2-vv*y)
	v = vmulq_f32(v, x);		// x*vv
	vec128 = vmulq_f32(v, m);		// (x*vv)*(2-vv*y) = x*(vv(2-vv*y)) ~~~ x/y
#else
	x /= p_v.x;
	y /= p_v.y;
	z /= p_v.z;
#endif
	return *this;
}

Vector3 Vector3::operator/(const Vector3& p_v) const {
#if defined (USE_SSE)
	__m128 vec = _mm_div_ps(vec128, p_v.vec128);
	vec = _mm_and_ps(vec, vFFF0fMask);
	return Vector3(vec);
#elif defined(USE_NEON)
	float32x4_t x, y, v, m;

	x = vec128;
	y = p_v.vec128;

	v = vrecpeq_f32(y);			// v ~ 1/y
	m = vrecpsq_f32(y, v);		// m = (2-v*y)
	v = vmulq_f32(v, m);		// vv = v*m ~~ 1/y
	m = vrecpsq_f32(y, v);		// mm = (2-vv*y)
	v = vmulq_f32(v, x);		// x*vv
	v = vmulq_f32(v, m);		// (x*vv)*(2-vv*y) = x*(vv(2-vv*y)) ~~~ x/y

	return Vector3(v);
#else
	return Vector3(x / p_v.x, y / p_v.y, z / p_v.z);
#endif
}

Vector3& Vector3::operator*=(real_t p_scalar) {
#if defined (USE_SSE)
	__m128	vs = _mm_load_ss(&p_scalar);	//	(S 0 0 0)
	vs = pshufd_ps(vs, 0x80);	//	(S S S 0.0)
	vec128 = _mm_mul_ps(vec128, vs);
#elif defined(USE_NEON)
	vec128 = vmulq_n_f32(vec128, p_scalar);
#else
	x *= p_scalar;
	y *= p_scalar;
	z *= p_scalar;
#endif
	return *this;
}

_FORCE_INLINE_ Vector3 operator*(real_t p_scalar, const Vector3& p_vec) {

	return p_vec * p_scalar;
}

Vector3 Vector3::operator*(real_t p_scalar) const {
#if defined (USE_SSE)
	__m128	vs = _mm_load_ss(&p_scalar);	//	(S 0 0 0)
	vs = pshufd_ps(vs, 0x80);	//	(S S S 0.0)
	return Vector3(_mm_mul_ps(vec128, vs));
#elif defined(USE_NEON)
	float32x4_t r = vmulq_n_f32(vec128, p_scalar);
	return Vector3((float32x4_t)vandq_s32((int32x4_t)r, vFFF0Mask));
#else
	return Vector3(x*p_scalar, y*p_scalar, z*p_scalar);
#endif
}

Vector3& Vector3::operator/=(real_t p_scalar) {
#if 0 //defined(USE_SSE)
	// this code is not faster !
	__m128 vs = _mm_load_ss(&p_scalar);
	vs = _mm_div_ss(v1110, vs);
	vs = pshufd_ps(vs, 0x00);	//	(S S S S)

	vec128 = _mm_mul_ps(vec128, vs);

	return *this;
#else
	return *this *= real_t(1.0) / p_scalar;
#endif

}

Vector3 Vector3::operator/(real_t p_scalar) const {

#if 0 //defined(USE_SSE)
	// this code is not faster !
	__m128 vs = _mm_load_ss(&p_scalar);
	vs = _mm_div_ss(v1110, vs);
	vs = pshufd_ps(vs, 0x00);	//	(S S S S)

	return Vector3(_mm_mul_ps(vec128, vs));
#else
	return *this * (real_t(1.0) / p_scalar);
#endif
}

Vector3 Vector3::operator-() const {
#if defined (USE_SSE)
	__m128 r = _mm_xor_ps(vec128, vMzeroMask);
	return Vector3(_mm_and_ps(r, vFFF0fMask));
#elif defined(USE_NEON)
	return Vector3((SimdFloat4)veorq_s32((int32x4_t)vec128, (int32x4_t)vMzeroMask));
#else
	return Vector3(-x, -y, -z);
#endif
}

bool Vector3::operator==(const Vector3& p_v) const {
#if defined (USE_SSE)
	return (0xf == _mm_movemask_ps((__m128)_mm_cmpeq_ps(vec128, p_v.vec128)));
#else
	return (x == p_v.x && y == p_v.y && z == p_v.z);
#endif
}

bool Vector3::operator!=(const Vector3& p_v) const {

	return !(*this == p_v);
}

bool Vector3::operator<(const Vector3& p_v) const {

	if (x==p_v.x) {
		if (y==p_v.y)
			return z<p_v.z;
		else
			return y<p_v.y;
	} else {
		return x<p_v.x;
	}
}

bool Vector3::operator<=(const Vector3& p_v) const {

	if (x==p_v.x) {
		if (y==p_v.y)
			return z<=p_v.z;
		else
			return y<p_v.y;
	} else {
		return x<p_v.x;
	}
}

_FORCE_INLINE_ Vector3 vec3_cross(const Vector3& p_a, const Vector3& p_b) {

	return p_a.cross(p_b);
}

_FORCE_INLINE_ real_t vec3_dot(const Vector3& p_a, const Vector3& p_b) {

	return p_a.dot(p_b);
}

_FORCE_INLINE_ real_t vec3_triple(const Vector3& p_v1, const Vector3& p_v2, const Vector3& p_v3) {
	return p_v1.triple(p_v2, p_v3);
}

real_t Vector3::length() const {

	return Math::sqrt(length_squared());
}

real_t Vector3::length_squared() const {

	return dot(*this);

}
void Vector3::fast_normalize() {

#if defined (USE_SSE)
	__m128 vd = _mm_mul_ps(vec128, vec128);
	__m128 z = _mm_movehl_ps(vd, vd);
	__m128 y = _mm_shuffle_ps(vd, vd, 0x55);
	vd = _mm_add_ss(vd, y);
	vd = _mm_add_ss(vd, z);

	y = _mm_rsqrt_ss(vd); // estimate 

	//  one step NR 
	z = sse_v1_5;
	vd = _mm_mul_ss(vd, sse_vHalf); // vd * 0.5	
	//x2 = vd;
	vd = _mm_mul_ss(vd, y); // vd * 0.5 * y0
	vd = _mm_mul_ss(vd, y); // vd * 0.5 * y0 * y0
	z = _mm_sub_ss(z, vd);  // 1.5 - vd * 0.5 * y0 * y0 

	y = _mm_mul_ss(y, z);   // y0 * (1.5 - vd * 0.5 * y0 * y0)

	y = splat_ps(y, 0x80);
	vec128 = _mm_mul_ps(vec128, y);
#else	
	*this /= length_squared;
#endif
}

void Vector3::normalize() {

	real_t l=length();
	if (l==0) {
		x=y=z=0;
	} else {
		*this /= Math::sqrt(l);
	}
}

Vector3 Vector3::normalized() const {

	Vector3 v=*this;
	v.normalize();
	return v;
}

Vector3 Vector3::inverse() const {

	return Vector3( 1.0/x, 1.0/y, 1.0/z );
}

void Vector3::zero() {
#if defined (USE_SSE)
	vec128 = (__m128)_mm_xor_ps(vec128, vec128);
#elif defined(USE_NEON)
	int32x4_t vi = vdupq_n_s32(0);
	vec128 = vreinterpretq_f32_s32(vi);
#else
	x = y = z = _unused = 0;
#endif
}

Vector3 Vector3::slide(const Vector3& p_vec) const {

	return p_vec - *this * this->dot(p_vec);
}

Vector3 Vector3::reflect(const Vector3& p_vec) const {

	return p_vec - *this * this->dot(p_vec) * 2.0;
}

real_t Vector3::triple(const Vector3& p_v1, const Vector3& p_v2) const {
#if defined (USE_SSE)
	// cross:
	__m128 T = _mm_shuffle_ps(p_v1.vec128, p_v1.vec128, SHUFFLE(1, 2, 0, 3));	//	(Y Z X 0)
	__m128 V = _mm_shuffle_ps(p_v2.vec128, p_v2.vec128, SHUFFLE(1, 2, 0, 3));	//	(Y Z X 0)

	V = _mm_mul_ps(V, p_v1.vec128);
	T = _mm_mul_ps(T, p_v2.vec128);
	V = _mm_sub_ps(V, T);

	V = _mm_shuffle_ps(V, V, SHUFFLE(1, 2, 0, 3));

	// dot:
	V = _mm_mul_ps(V, vec128);
	__m128 z = _mm_movehl_ps(V, V);
	__m128 y = _mm_shuffle_ps(V, V, 0x55);
	V = _mm_add_ss(V, y);
	V = _mm_add_ss(V, z);
	return _mm_cvtss_f32(V);

#elif defined(USE_NEON)
	// cross:
	float32x4_t T, V;
	// form (Y, Z, X, _) of vec128 and v.vec128
	float32x2_t Tlow = vget_low_f32(p_v1.vec128);
	float32x2_t Vlow = vget_low_f32(p_v2.vec128);
	T = vcombine_f32(vext_f32(Tlow, vget_high_f32(p_v1.vec128), 1), Tlow);
	V = vcombine_f32(vext_f32(Vlow, vget_high_f32(p_v2.vec128), 1), Vlow);

	V = vmulq_f32(V, p_v1.vec128);
	T = vmulq_f32(T, p_v2.vec128);
	V = vsubq_f32(V, T);
	Vlow = vget_low_f32(V);
	// form (Y, Z, X, _);
	V = vcombine_f32(vext_f32(Vlow, vget_high_f32(V), 1), Vlow);

	// dot:
	V = vmulq_f32(vec128, V);
	float32x2_t x = vpadd_f32(vget_low_f32(V), vget_low_f32(V));
	x = vadd_f32(x, vget_high_f32(V));
	return vget_lane_f32(x, 0);
#else
	return
		x * (p_v1.y * p_v2.z - p_v1.z * p_v2.y) +
		y * (p_v1.z * p_v2.x - p_v1.x * p_v2.z) +
		z * (p_v1.x * p_v2.y - p_v1.y * p_v2.x);
#endif
}

#endif

#endif // VECTOR3_H
