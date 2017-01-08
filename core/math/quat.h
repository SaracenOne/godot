/*************************************************************************/
/*  quat.h                                                               */
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
#ifndef QUAT_H
#define QUAT_H

#include "math_defs.h"
#include "math_funcs.h"
#include "ustring.h"
#include "vector3.h"

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
GD_ALIGN16(class) Quat{
public:

#if defined (USE_SSE) || defined (USE_NEON)
	union {

		GD_ALIGN16(struct) {
			real_t x;
			real_t y;
			real_t z;
			real_t w;
		};

		SimdFloat4 vec128;

	};
#else
	real_t x, y, z, w;
#endif

	_FORCE_INLINE_ real_t length_squared() const;
	real_t length() const;
	void normalize();
	Quat normalized() const;
	Quat inverse() const;
	_FORCE_INLINE_ real_t dot(const Quat& q) const;
	void set_euler(const Vector3& p_euler);
	Quat slerp(const Quat& q, const real_t& t) const;
	Quat slerpni(const Quat& q, const real_t& t) const;
	Quat cubic_slerp(const Quat& q, const Quat& prep, const Quat& postq,const real_t& t) const;

	_FORCE_INLINE_ void get_axis_and_angle(Vector3& r_axis, real_t &r_angle) const {
		r_angle = 2 * Math::acos(w);
		r_axis.x = -x / Math::sqrt(1-w*w);
		r_axis.y = -y / Math::sqrt(1-w*w);
		r_axis.z = -z / Math::sqrt(1-w*w);
	}

	void operator*=(const Quat& q);
	Quat operator*(const Quat& q) const;



	Quat operator*(const Vector3& v) const
	{
		return Quat( w * v.x + y * v.z - z * v.y,
			w * v.y + z * v.x - x * v.z,
			w * v.z + x * v.y - y * v.x,
			-x * v.x - y * v.y - z * v.z);
	}

	_FORCE_INLINE_ Vector3 xform(const Vector3& v) const {

		Quat q = *this * v;
		q *= this->inverse();
		return Vector3(q.x,q.y,q.z);
	}

	_FORCE_INLINE_ void operator+=(const Quat& q);
	_FORCE_INLINE_ void operator-=(const Quat& q);
	_FORCE_INLINE_ void operator*=(const real_t& s);
	_FORCE_INLINE_ void operator/=(const real_t& s);
	_FORCE_INLINE_ Quat operator+(const Quat& q2) const;
	_FORCE_INLINE_ Quat operator-(const Quat& q2) const;
	_FORCE_INLINE_ Quat operator-() const;
	_FORCE_INLINE_ Quat operator*(const real_t& s) const;
	_FORCE_INLINE_ Quat operator/(const real_t& s) const;


	_FORCE_INLINE_ bool operator==(const Quat& p_quat) const;
	_FORCE_INLINE_ bool operator!=(const Quat& p_quat) const;

	operator String() const;

	_FORCE_INLINE_ void set(real_t p_x, real_t p_y, real_t p_z, real_t p_w) {
		x=p_x; y=p_y; z=p_z; w=p_w;
	}
	inline Quat(real_t p_x, real_t p_y, real_t p_z, real_t p_w) {
		x=p_x; y=p_y; z=p_z; w=p_w;
	}
	Quat(const Vector3& axis, const real_t& angle);

	Quat(const Vector3& v0, const Vector3& v1) // shortest arc
	{
		Vector3 c = v0.cross(v1);
		real_t  d = v0.dot(v1);

		if (d < -1.0 + CMP_EPSILON) {
			x=0;
			y=1;
			z=0;
			w=0;
		} else {

			real_t  s = Math::sqrt((1.0f + d) * 2.0f);
			real_t rs = 1.0f / s;

			x=c.x*rs;
			y=c.y*rs;
			z=c.z*rs;
			w=s * 0.5;
		}
	}

	inline Quat() {x=y=z=0; w=1; }

#if defined(USE_SSE) || defined(USE_NEON)
	// Set Vector 
	_FORCE_INLINE_ Quat(const SimdFloat4 vec)
	{
		vec128 = vec;
	}

	// Copy constructor
	_FORCE_INLINE_ Quat(const Quat& rhs)
	{
		vec128 = rhs.vec128;
	}

	// Assignment Operator
	_FORCE_INLINE_ Quat& operator=(const Quat& v)
	{
		vec128 = v.vec128;

		return *this;
	}

#endif

};


real_t Quat::dot(const Quat& q) const {
#if defined (USE_SSE)
	__m128	vd;

	vd = _mm_mul_ps(vec128, q.vec128);

	__m128 t = _mm_movehl_ps(vd, vd);
	vd = _mm_add_ps(vd, t);
	t = _mm_shuffle_ps(vd, vd, 0x55);
	vd = _mm_add_ss(vd, t);

	return _mm_cvtss_f32(vd);
#elif defined(USE_NEON)
	float32x4_t vd = vmulq_f32(vec128, q.vec128);
	float32x2_t x = vpadd_f32(vget_low_f32(vd), vget_high_f32(vd));
	x = vpadd_f32(x, x);
	return vget_lane_f32(x, 0);
#else
	return x * q.x + y * q.y + z * q.z + w * q.w;
#endif
}

real_t Quat::length_squared() const {
	return dot(*this);
}

void Quat::operator+=(const Quat& q) {
#if defined (USE_SSE)
	vec128 = _mm_add_ps(vec128, q.vec128);
#elif defined(USE_NEON)
	vec128 = vaddq_f32(vec128, q.vec128);
#else	
	x += q.x; y += q.y; z += q.z; w += q.w;
#endif
}

void Quat::operator-=(const Quat& q) {
#if defined (USE_SSE)
	vec128 = _mm_sub_ps(vec128, q.vec128);
#elif defined(USE_NEON)
	vec128 = vsubq_f32(vec128, q.vec128);
#else	
	x -= q.x; y -= q.y; z -= q.z; w -= q.w;
#endif
}

void Quat::operator*=(const real_t& s) {
#if defined (USE_SSE)
	__m128	vs = _mm_load_ss(&s);	//	(S 0 0 0)
	vs = pshufd_ps(vs, 0);	//	(S S S S)
	vec128 = _mm_mul_ps(vec128, vs);
#elif defined(USE_NEON)
	vec128 = vmulq_n_f32(vec128, s);
#else
	x *= s; y *= s; z *= s; w *= s;
#endif
}


void Quat::operator/=(const real_t& s) {

	*this *= 1.0 / s;
}

Quat Quat::operator+(const Quat& q2) const {
#if defined (USE_SSE)
	return Quat(_mm_add_ps(vec128, q2.vec128));
#elif defined(USE_NEON)
	return Quat(vaddq_f32(vec128, q2.vec128));
#else	
	const Quat& q1 = *this;
	return Quat( q1.x+q2.x, q1.y+q2.y, q1.z+q2.z, q1.w+q2.w );
#endif
}

Quat Quat::operator-(const Quat& q2) const {
#if defined (USE_SSE)
	return Quat(_mm_sub_ps(vec128, q2.vec128));
#elif defined(USE_NEON)
	return Quat(vsubq_f32(vec128, q2.vec128));
#else	
	const Quat& q1 = *this;
	return Quat( q1.x-q2.x, q1.y-q2.y, q1.z-q2.z, q1.w-q2.w);
#endif
}

Quat Quat::operator-() const {
#if defined (USE_SSE)
	return Quat(_mm_xor_ps(vec128, vMzeroMask));
#elif defined(USE_NEON)
	return Quat((SimdFloat4)veorq_s32((int32x4_t)vec128, (int32x4_t)vMzeroMask));
#else	
	const Quat& q2 = *this;
	return Quat( -q2.x, -q2.y,  -q2.z,  -q2.w);
#endif
}

Quat Quat::operator*(const real_t& s) const {
#if defined (USE_SSE)
	__m128	vs = _mm_load_ss(&s);	//	(S 0 0 0)
	vs = pshufd_ps(vs, 0x00);	//	(S S S S)

	return Quat(_mm_mul_ps(vec128, vs));
#elif defined(USE_NEON)
	return Quat(vmulq_n_f32(vec128, s));
#else
	return Quat(x * s, y * s, z * s, w * s);
#endif
}

Quat Quat::operator/(const real_t& s) const {
	return *this * (1.0 / s);
}


bool Quat::operator==(const Quat& p_quat) const {
#if defined (USE_SSE)
	return (0xf == _mm_movemask_ps((__m128)_mm_cmpeq_ps(vec128, p_quat.vec128)));
#else
	return (x == p_quat.x && y == p_quat.y && z == p_quat.z && w == p_quat.w);
#endif
}

bool Quat::operator!=(const Quat& p_quat) const {

	return !(*this == p_quat);
}


#endif
