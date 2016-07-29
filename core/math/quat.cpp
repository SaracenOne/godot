/*************************************************************************/
/*  quat.cpp                                                             */
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
#include "quat.h"
#include "print_string.h"

#if defined(USE_SSE)

#define vQInv (_mm_set_ps(+0.0f, -0.0f, -0.0f, -0.0f))
#define vPPPM (_mm_set_ps(-0.0f, +0.0f, +0.0f, +0.0f))
#define vOnes (_mm_set_ps(1.0f, 1.0f, 1.0f, 1.0f))

#elif defined(USE_NEON)

const SimdFloat4 ATTRIBUTE_ALIGNED16(vQInv) = { -0.0f, -0.0f, -0.0f, +0.0f };
const SimdFloat4 ATTRIBUTE_ALIGNED16(vPPPM) = { +0.0f, +0.0f, +0.0f, -0.0f };

#endif

void Quat::set_euler(const Vector3& p_euler) {
	real_t half_yaw = p_euler.x * 0.5;
	real_t half_pitch = p_euler.y * 0.5;
	real_t half_roll = p_euler.z * 0.5;
	real_t cos_yaw = Math::cos(half_yaw);
	real_t sin_yaw = Math::sin(half_yaw);
	real_t cos_pitch = Math::cos(half_pitch);
	real_t sin_pitch = Math::sin(half_pitch);
	real_t cos_roll = Math::cos(half_roll);
	real_t sin_roll = Math::sin(half_roll);
	set(cos_roll * sin_pitch * cos_yaw+sin_roll * cos_pitch * sin_yaw,
		cos_roll * cos_pitch * sin_yaw - sin_roll * sin_pitch * cos_yaw,
		sin_roll * cos_pitch * cos_yaw - cos_roll * sin_pitch * sin_yaw,
		cos_roll * cos_pitch * cos_yaw+sin_roll * sin_pitch * sin_yaw);
}

void Quat::operator*=(const Quat& q) {

#if defined (USE_SSE)
	__m128 vQ2 = q.vec128;

	__m128 A1 = pshufd_ps(vec128, SHUFFLE(0, 1, 2, 0));
	__m128 B1 = pshufd_ps(vQ2, SHUFFLE(3, 3, 3, 0));

	A1 = _mm_mul_ps(A1, B1);

	__m128 A2 = pshufd_ps(vec128, SHUFFLE(1, 2, 0, 1));
	__m128 B2 = pshufd_ps(vQ2, SHUFFLE(2, 0, 1, 1));

	A2 = _mm_mul_ps(A2, B2);

	B1 = pshufd_ps(vec128, SHUFFLE(2, 0, 1, 2));
	B2 = pshufd_ps(vQ2, SHUFFLE(1, 2, 0, 2));

	B1 = _mm_mul_ps(B1, B2);	//	A3 *= B3

	vec128 = splat_ps(vec128, 3);	//	A0
	vec128 = _mm_mul_ps(vec128, vQ2);	//	A0 * B0

	A1 = _mm_add_ps(A1, A2);	//	AB12
	vec128 = _mm_sub_ps(vec128, B1);	//	AB03 = AB0 - AB3 
	A1 = _mm_xor_ps(A1, vPPPM);	//	change sign of the last element
	vec128 = _mm_add_ps(vec128, A1);	//	AB03 + AB12

#elif defined(USE_NEON)

	float32x4_t vQ1 = vec128;
	float32x4_t vQ2 = q.vec128;
	float32x4_t A0, A1, B1, A2, B2, A3, B3;
	float32x2_t vQ1zx, vQ2wx, vQ1yz, vQ2zx, vQ2yz, vQ2xz;

	{
		float32x2x2_t tmp;
		tmp = vtrn_f32(vget_high_f32(vQ1), vget_low_f32(vQ1));       // {z x}, {w y}
		vQ1zx = tmp.val[0];

		tmp = vtrn_f32(vget_high_f32(vQ2), vget_low_f32(vQ2));       // {z x}, {w y}
		vQ2zx = tmp.val[0];
	}
	vQ2wx = vext_f32(vget_high_f32(vQ2), vget_low_f32(vQ2), 1);

	vQ1yz = vext_f32(vget_low_f32(vQ1), vget_high_f32(vQ1), 1);

	vQ2yz = vext_f32(vget_low_f32(vQ2), vget_high_f32(vQ2), 1);
	vQ2xz = vext_f32(vQ2zx, vQ2zx, 1);

	A1 = vcombine_f32(vget_low_f32(vQ1), vQ1zx);                    // X Y  z x 
	B1 = vcombine_f32(vdup_lane_f32(vget_high_f32(vQ2), 1), vQ2wx); // W W  W X 

	A2 = vcombine_f32(vQ1yz, vget_low_f32(vQ1));
	B2 = vcombine_f32(vQ2zx, vdup_lane_f32(vget_low_f32(vQ2), 1));

	A3 = vcombine_f32(vQ1zx, vQ1yz);        // Z X Y Z
	B3 = vcombine_f32(vQ2yz, vQ2xz);        // Y Z x z

	A1 = vmulq_f32(A1, B1);
	A2 = vmulq_f32(A2, B2);
	A3 = vmulq_f32(A3, B3);	//	A3 *= B3
	A0 = vmulq_lane_f32(vQ2, vget_high_f32(vQ1), 1); //	A0 * B0

	A1 = vaddq_f32(A1, A2);	//	AB12 = AB1 + AB2
	A0 = vsubq_f32(A0, A3);	//	AB03 = AB0 - AB3 

	//	change the sign of the last element
	A1 = (SimdFloat4)veorq_s32((int32x4_t)A1, (int32x4_t)vPPPM);
	A0 = vaddq_f32(A0, A1);	//	AB03 + AB12

	vec128 = A0;
#else
	set(w * q.x + x * q.w + y * q.z - z * q.y,
		w * q.y + y * q.w + z * q.x - x * q.z,
		w * q.z + z * q.w + x * q.y - y * q.x,
		w * q.w - x * q.x - y * q.y - z * q.z);
#endif
}

Quat Quat::operator*(const Quat& q) const {

	Quat r=*this;
	r*=q;
	return r;
}




real_t Quat::length() const {

	return Math::sqrt(length_squared());
}

void Quat::normalize() {
#if defined (USE_SSE)
	__m128	vd;

	vd = _mm_mul_ps(vec128, vec128);

	__m128 t = _mm_movehl_ps(vd, vd);
	vd = _mm_add_ps(vd, t);
	t = _mm_shuffle_ps(vd, vd, 0x55);
	vd = _mm_add_ss(vd, t);

	vd = _mm_sqrt_ss(vd);
	vd = _mm_div_ss(vOnes, vd);
	vd = pshufd_ps(vd, 0); // splat
	vec128 = _mm_mul_ps(vec128, vd);
#else
	*this /= length();
#endif
}


Quat Quat::normalized() const {
	return *this / length();
}

Quat Quat::inverse() const {
#if defined (USE_SSE)
	return Quat(_mm_xor_ps(vec128, vQInv));
#elif defined(USE_NEON)
	return Quat((SimdFloat4)veorq_s32((int32x4_t)vec128, (int32x4_t)vQInv));
#else	
	return Quat(-x, -y, -z, w);
#endif
}


Quat Quat::slerp(const Quat& q, const real_t& t) const {

#if 0


	Quat dst=q;
	Quat src=*this;

	src.normalize();
	dst.normalize();

	real_t cosine = dst.dot(src);

	if (cosine < 0 && true) {
		cosine = -cosine;
		dst = -dst;
	} else {
		dst = dst;
	}

	if (Math::abs(cosine) < 1 - CMP_EPSILON) {
		// Standard case (slerp)
		real_t sine = Math::sqrt(1 - cosine*cosine);
		real_t angle = Math::atan2(sine, cosine);
		real_t inv_sine = 1.0f / sine;
		real_t coeff_0 = Math::sin((1.0f - t) * angle) * inv_sine;
		real_t coeff_1 = Math::sin(t * angle) * inv_sine;
		Quat ret=  src * coeff_0 + dst * coeff_1;

		return ret;
	} else {
		// There are two situations:
		// 1. "rkP" and "q" are very close (cosine ~= +1), so we can do a linear
		//    interpolation safely.
		// 2. "rkP" and "q" are almost invedste of each other (cosine ~= -1), there
		//    are an infinite number of possibilities interpolation. but we haven't
		//    have method to fix this case, so just use linear interpolation here.
		Quat ret =  src * (1.0f - t) + dst *t;
		// taking the complement requires renormalisation
		ret.normalize();
		return ret;
	}
#else

	real_t         to1[4];
	real_t        omega, cosom, sinom, scale0, scale1;


	// calc cosine
	cosom = x * q.x + y * q.y + z * q.z
			+ w * q.w;


	// adjust signs (if necessary)
	if ( cosom <0.0 ) {
		cosom = -cosom; to1[0] = - q.x;
		to1[1] = - q.y;
		to1[2] = - q.z;
		to1[3] = - q.w;
	} else  {
		to1[0] = q.x;
		to1[1] = q.y;
		to1[2] = q.z;
		to1[3] = q.w;
	}


	// calculate coefficients

	if ( (1.0 - cosom) > CMP_EPSILON ) {
		// standard case (slerp)
		omega = Math::acos(cosom);
		sinom = Math::sin(omega);
		scale0 = Math::sin((1.0 - t) * omega) / sinom;
		scale1 = Math::sin(t * omega) / sinom;
	} else {
		// "from" and "to" quaternions are very close
		//  ... so we can do a linear interpolation
		scale0 = 1.0 - t;
		scale1 = t;
	}
	// calculate final values
	return Quat(
		scale0 * x + scale1 * to1[0],
		scale0 * y + scale1 * to1[1],
		scale0 * z + scale1 * to1[2],
		scale0 * w + scale1 * to1[3]
	);
#endif
}

Quat Quat::slerpni(const Quat& q, const real_t& t) const {

	const Quat &from = *this;

	float dot = from.dot(q);

	if (Math::absf(dot) > 0.9999f) return from;

	float	theta		= Math::acos(dot),
		sinT		= 1.0f / Math::sin(theta),
		newFactor	= Math::sin(t * theta) * sinT,
		invFactor	= Math::sin((1.0f - t) * theta) * sinT;

	return Quat( invFactor * from.x + newFactor * q.x,
			   invFactor * from.y + newFactor * q.y,
			   invFactor * from.z + newFactor * q.z,
			   invFactor * from.w + newFactor * q.w );

#if 0
	real_t         to1[4];
	real_t        omega, cosom, sinom, scale0, scale1;


	// calc cosine
	cosom = x * q.x + y * q.y + z * q.z
			+ w * q.w;


	// adjust signs (if necessary)
	if ( cosom <0.0 && false) {
		cosom = -cosom; to1[0] = - q.x;
		to1[1] = - q.y;
		to1[2] = - q.z;
		to1[3] = - q.w;
	} else  {
		to1[0] = q.x;
		to1[1] = q.y;
		to1[2] = q.z;
		to1[3] = q.w;
	}


	// calculate coefficients

	if ( (1.0 - cosom) > CMP_EPSILON ) {
		// standard case (slerp)
		omega = Math::acos(cosom);
		sinom = Math::sin(omega);
		scale0 = Math::sin((1.0 - t) * omega) / sinom;
		scale1 = Math::sin(t * omega) / sinom;
	} else {
		// "from" and "to" quaternions are very close
		//  ... so we can do a linear interpolation
		scale0 = 1.0 - t;
		scale1 = t;
	}
	// calculate final values
	return Quat(
		scale0 * x + scale1 * to1[0],
		scale0 * y + scale1 * to1[1],
		scale0 * z + scale1 * to1[2],
		scale0 * w + scale1 * to1[3]
	);
#endif
}

Quat Quat::cubic_slerp(const Quat& q, const Quat& prep, const Quat& postq,const real_t& t) const {

	//the only way to do slerp :|
	float t2 = (1.0-t)*t*2;
	Quat sp = this->slerp(q,t);
	Quat sq = prep.slerpni(postq,t);
	return sp.slerpni(sq,t2);

}


Quat::operator String() const {

	return String::num(x)+", "+String::num(y)+", "+ String::num(z)+", "+ String::num(w);
}

Quat::Quat(const Vector3& axis, const real_t& angle) {
	real_t d = axis.length();
	if (d==0)
		set(0,0,0,0);
	else {
		real_t s = Math::sin(-angle * 0.5) / d;
		set(axis.x * s, axis.y * s, axis.z * s,
			Math::cos(-angle * 0.5));
	}
}
