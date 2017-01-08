/*************************************************************************/
/*  matrix3.h                                                            */
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
#ifndef MATRIX3_H
#define MATRIX3_H

#include "vector3.h"
#include "quat.h"

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/

#if defined(USE_SSE)
#define SIMD_v1000 (_mm_set_ps(0.0f,0.0f,0.0f,1.0f))
#define SIMD_v0100 (_mm_set_ps(0.0f,0.0f,1.0f,0.0f))
#define SIMD_v0010 (_mm_set_ps(0.0f,1.0f,0.0f,0.0f))
#elif defined(USE_NEON)
const SimdFloat4 GD_ALIGN16(SIMD_v1000) = { 1.0f, 0.0f, 0.0f, 0.0f };
const SimdFloat4 GD_ALIGN16(SIMD_v0100) = { 0.0f, 1.0f, 0.0f, 0.0f };
const SimdFloat4 GD_ALIGN16(SIMD_v0010) = { 0.0f, 0.0f, 1.0f, 0.0f };
#endif

GD_ALIGN16(class) Matrix3{
public:

	Vector3 elements[3];

	_FORCE_INLINE_ const Vector3& operator[](int axis) const {

		return elements[axis];
	}
	_FORCE_INLINE_ Vector3& operator[](int axis) {

		return elements[axis];
	}

	void invert();
	void transpose();

	Matrix3 inverse() const;
	Matrix3 transposed() const;

	_FORCE_INLINE_ float determinant() const;

	void from_z(const Vector3& p_z);

	_FORCE_INLINE_ Vector3 get_axis(int p_axis) const {
		// get actual basis axis (elements is transposed for performance)
		return Vector3( elements[0][p_axis], elements[1][p_axis], elements[2][p_axis] );
	}
	_FORCE_INLINE_ void set_axis(int p_axis, const Vector3& p_value) {
		// get actual basis axis (elements is transposed for performance)
		elements[0][p_axis]=p_value.x;
		elements[1][p_axis]=p_value.y;
		elements[2][p_axis]=p_value.z;
	}

	void rotate(const Vector3& p_axis, real_t p_phi);
	Matrix3 rotated(const Vector3& p_axis, real_t p_phi) const;

	void scale( const Vector3& p_scale );
	Matrix3 scaled( const Vector3& p_scale ) const;
	Vector3 get_scale() const;

	Vector3 get_euler() const;
	void set_euler(const Vector3& p_euler);

	// transposed dot products
	_FORCE_INLINE_ real_t tdotx(const Vector3& v) const  {
		return elements[0][0] * v[0] + elements[1][0] * v[1] + elements[2][0] * v[2];
	}
	_FORCE_INLINE_ real_t tdoty(const Vector3& v) const {
		return elements[0][1] * v[0] + elements[1][1] * v[1] + elements[2][1] * v[2];
	}
	_FORCE_INLINE_ real_t tdotz(const Vector3& v) const {
		return elements[0][2] * v[0] + elements[1][2] * v[1] + elements[2][2] * v[2];
	}

	bool operator==(const Matrix3& p_matrix) const;
	bool operator!=(const Matrix3& p_matrix) const;

	_FORCE_INLINE_ Vector3 xform(const Vector3& p_vector) const;
	_FORCE_INLINE_ Vector3 xform_inv(const Vector3& p_vector) const;
	_FORCE_INLINE_ void operator*=(const Matrix3& p_matrix);
	_FORCE_INLINE_ Matrix3 operator*(const Matrix3& p_matrix) const;

	int get_orthogonal_index() const;
	void set_orthogonal_index(int p_index);

	operator String() const;

	void get_axis_and_angle(Vector3 &r_axis,real_t& r_angle) const;

	/* create / set */


	_FORCE_INLINE_ void set(real_t xx, real_t xy, real_t xz, real_t yx, real_t yy, real_t yz, real_t zx, real_t zy, real_t zz) {

		elements[0].set(xx, xy, xz);
		elements[1].set(yx, yy, yz);
		elements[2].set(zx, zy, zz);
	}
	_FORCE_INLINE_ Vector3 get_column(int i) const {

		return Vector3(elements[0][i],elements[1][i],elements[2][i]);
	}

	_FORCE_INLINE_ Vector3 get_row(int i) const {

		return elements[i];
	}
	_FORCE_INLINE_ void set_row(int i, const Vector3& p_row) {
		elements[i] = p_row;
	}

	_FORCE_INLINE_ void set_zero() {
		elements[0].zero();
		elements[1].zero();
		elements[2].zero();
	}

	_FORCE_INLINE_ Matrix3 transpose_xform(const Matrix3& m) const {
#if defined (USE_SSE)
		// zeros w
		//    static const __m128i xyzMask = (const __m128i){ -1ULL, 0xffffffffULL };
		__m128 row = elements[0].vec128;
		__m128 m0 = _mm_and_ps(m[0].vec128, vFFF0fMask);
		__m128 m1 = _mm_and_ps(m[1].vec128, vFFF0fMask);
		__m128 m2 = _mm_and_ps(m[2].vec128, vFFF0fMask);
		__m128 r0 = _mm_mul_ps(m0, _mm_shuffle_ps(row, row, 0));
		__m128 r1 = _mm_mul_ps(m0, _mm_shuffle_ps(row, row, 0x55));
		__m128 r2 = _mm_mul_ps(m0, _mm_shuffle_ps(row, row, 0xaa));
		row = elements[1].vec128;
		r0 = _mm_add_ps(r0, _mm_mul_ps(m1, _mm_shuffle_ps(row, row, 0)));
		r1 = _mm_add_ps(r1, _mm_mul_ps(m1, _mm_shuffle_ps(row, row, 0x55)));
		r2 = _mm_add_ps(r2, _mm_mul_ps(m1, _mm_shuffle_ps(row, row, 0xaa)));
		row = elements[2].vec128;
		r0 = _mm_add_ps(r0, _mm_mul_ps(m2, _mm_shuffle_ps(row, row, 0)));
		r1 = _mm_add_ps(r1, _mm_mul_ps(m2, _mm_shuffle_ps(row, row, 0x55)));
		r2 = _mm_add_ps(r2, _mm_mul_ps(m2, _mm_shuffle_ps(row, row, 0xaa)));
		return Matrix3(r0, r1, r2);

#elif defined(USE_NEON)
		// zeros w
		static const uint32x4_t xyzMask = (const uint32x4_t){ static_cast<uint32_t>(-1), static_cast<uint32_t>(-1), static_cast<uint32_t>(-1), 0 };
		float32x4_t m0 = (float32x4_t)vandq_u32((uint32x4_t)m[0].vec128, xyzMask);
		float32x4_t m1 = (float32x4_t)vandq_u32((uint32x4_t)m[1].vec128, xyzMask);
		float32x4_t m2 = (float32x4_t)vandq_u32((uint32x4_t)m[2].vec128, xyzMask);
		float32x4_t row = elements[0].vec128;
		float32x4_t r0 = vmulq_lane_f32(m0, vget_low_f32(row), 0);
		float32x4_t r1 = vmulq_lane_f32(m0, vget_low_f32(row), 1);
		float32x4_t r2 = vmulq_lane_f32(m0, vget_high_f32(row), 0);
		row = elements[1].vec128;
		r0 = vmlaq_lane_f32(r0, m1, vget_low_f32(row), 0);
		r1 = vmlaq_lane_f32(r1, m1, vget_low_f32(row), 1);
		r2 = vmlaq_lane_f32(r2, m1, vget_high_f32(row), 0);
		row = elements[2].vec128;
		r0 = vmlaq_lane_f32(r0, m2, vget_low_f32(row), 0);
		r1 = vmlaq_lane_f32(r1, m2, vget_low_f32(row), 1);
		r2 = vmlaq_lane_f32(r2, m2, vget_high_f32(row), 0);
		return Matrix3(r0, r1, r2);
#else
		return Matrix3(
			elements[0].x * m[0].x + elements[1].x * m[1].x + elements[2].x * m[2].x,
			elements[0].x * m[0].y + elements[1].x * m[1].y + elements[2].x * m[2].y,
			elements[0].x * m[0].z + elements[1].x * m[1].z + elements[2].x * m[2].z,
			elements[0].y * m[0].x + elements[1].y * m[1].x + elements[2].y * m[2].x,
			elements[0].y * m[0].y + elements[1].y * m[1].y + elements[2].y * m[2].y,
			elements[0].y * m[0].z + elements[1].y * m[1].z + elements[2].y * m[2].z,
			elements[0].z * m[0].x + elements[1].z * m[1].x + elements[2].z * m[2].x,
			elements[0].z * m[0].y + elements[1].z * m[1].y + elements[2].z * m[2].y,
			elements[0].z * m[0].z + elements[1].z * m[1].z + elements[2].z * m[2].z);
#endif
	}
	Matrix3(real_t xx, real_t xy, real_t xz, real_t yx, real_t yy, real_t yz, real_t zx, real_t zy, real_t zz) {

		set(xx, xy, xz, yx, yy, yz, zx, zy, zz);
	}

	void orthonormalize();
	Matrix3 orthonormalized() const;

	operator Quat() const;

	Matrix3(const Quat& p_quat); // euler
	Matrix3(const Vector3& p_euler); // euler
	Matrix3(const Vector3& p_axis, real_t p_phi);

	_FORCE_INLINE_ Matrix3() {

#if defined (USE_SSE) || defined (USE_NEON)
		elements[0] = SIMD_v1000;
		elements[1] = SIMD_v0100;
		elements[2] = SIMD_v0010;
#else
		set(real_t(1.0), real_t(0.0), real_t(0.0),
			real_t(0.0), real_t(1.0), real_t(0.0),
			real_t(0.0), real_t(0.0), real_t(1.0));
#endif
	}

#if defined (USE_SSE) || defined (USE_NEON)
	_FORCE_INLINE_ Matrix3(const SimdFloat4 p_v0, const SimdFloat4 p_v1, const SimdFloat4 p_v2)
	{
		elements[0].vec128 = p_v0;
		elements[1].vec128 = p_v1;
		elements[2].vec128 = p_v2;
	}

	_FORCE_INLINE_ Matrix3(const Vector3& p_v0, const Vector3& p_v1, const Vector3& p_v2)
	{
		elements[0] = p_v0;
		elements[1] = p_v1;
		elements[2] = p_v2;
	}

	_FORCE_INLINE_ Matrix3(const Matrix3& p_matrix)
	{
		elements[0].vec128 = p_matrix.elements[0].vec128;
		elements[1].vec128 = p_matrix.elements[1].vec128;
		elements[2].vec128 = p_matrix.elements[2].vec128;
	}

	_FORCE_INLINE_ Matrix3& operator=(const Matrix3& p_matrix)
	{
		elements[0].vec128 = p_matrix.elements[0].vec128;
		elements[1].vec128 = p_matrix.elements[1].vec128;
		elements[2].vec128 = p_matrix.elements[2].vec128;

		return *this;
	}

#else

	_FORCE_INLINE_ Matrix3(const Matrix3& p_matrix)
	{
		elements[0] = p_matrix.elements[0];
		elements[1] = p_matrix.elements[1];
		elements[2] = p_matrix.elements[2];
	}

	_FORCE_INLINE_ Matrix3& operator=(const Matrix3& p_matrix)
	{
		elements[0] = p_matrix.elements[0];
		elements[1] = p_matrix.elements[1];
		elements[2] = p_matrix.elements[2];
		return *this;
	}

#endif


};

_FORCE_INLINE_ void Matrix3::operator*=(const Matrix3& p_matrix) {
#if defined (USE_SSE)
	__m128 rv00, rv01, rv02;
	__m128 rv10, rv11, rv12;
	__m128 rv20, rv21, rv22;
	__m128 mv0, mv1, mv2;

	rv02 = elements[0].vec128;
	rv12 = elements[1].vec128;
	rv22 = elements[2].vec128;

	mv0 = _mm_and_ps(p_matrix[0].vec128, vFFF0fMask);
	mv1 = _mm_and_ps(p_matrix[1].vec128, vFFF0fMask);
	mv2 = _mm_and_ps(p_matrix[2].vec128, vFFF0fMask);

	// rv0
	rv00 = splat_ps(rv02, 0);
	rv01 = splat_ps(rv02, 1);
	rv02 = splat_ps(rv02, 2);

	rv00 = _mm_mul_ps(rv00, mv0);
	rv01 = _mm_mul_ps(rv01, mv1);
	rv02 = _mm_mul_ps(rv02, mv2);

	// rv1
	rv10 = splat_ps(rv12, 0);
	rv11 = splat_ps(rv12, 1);
	rv12 = splat_ps(rv12, 2);

	rv10 = _mm_mul_ps(rv10, mv0);
	rv11 = _mm_mul_ps(rv11, mv1);
	rv12 = _mm_mul_ps(rv12, mv2);

	// rv2
	rv20 = splat_ps(rv22, 0);
	rv21 = splat_ps(rv22, 1);
	rv22 = splat_ps(rv22, 2);

	rv20 = _mm_mul_ps(rv20, mv0);
	rv21 = _mm_mul_ps(rv21, mv1);
	rv22 = _mm_mul_ps(rv22, mv2);

	rv00 = _mm_add_ps(rv00, rv01);
	rv10 = _mm_add_ps(rv10, rv11);
	rv20 = _mm_add_ps(rv20, rv21);

	elements[0].vec128 = _mm_add_ps(rv00, rv02);
	elements[1].vec128 = _mm_add_ps(rv10, rv12);
	elements[2].vec128 = _mm_add_ps(rv20, rv22);

#elif defined(USE_NEON)

	float32x4_t rv0, rv1, rv2;
	float32x4_t v0, v1, v2;
	float32x4_t mv0, mv1, mv2;

	v0 = elements[0].vec128;
	v1 = elements[1].vec128;
	v2 = elements[2].vec128;

	mv0 = (float32x4_t)vandq_s32((int32x4_t)p_matrix[0].vec128, vFFF0Mask);
	mv1 = (float32x4_t)vandq_s32((int32x4_t)p_matrix[1].vec128, vFFF0Mask);
	mv2 = (float32x4_t)vandq_s32((int32x4_t)p_matrix[2].vec128, vFFF0Mask);

	rv0 = vmulq_lane_f32(mv0, vget_low_f32(v0), 0);
	rv1 = vmulq_lane_f32(mv0, vget_low_f32(v1), 0);
	rv2 = vmulq_lane_f32(mv0, vget_low_f32(v2), 0);

	rv0 = vmlaq_lane_f32(rv0, mv1, vget_low_f32(v0), 1);
	rv1 = vmlaq_lane_f32(rv1, mv1, vget_low_f32(v1), 1);
	rv2 = vmlaq_lane_f32(rv2, mv1, vget_low_f32(v2), 1);

	rv0 = vmlaq_lane_f32(rv0, mv2, vget_high_f32(v0), 0);
	rv1 = vmlaq_lane_f32(rv1, mv2, vget_high_f32(v1), 0);
	rv2 = vmlaq_lane_f32(rv2, mv2, vget_high_f32(v2), 0);

	elements[0].vec128 = rv0;
	elements[1].vec128 = rv1;
	elements[2].vec128 = rv2;
#else
	set(
		p_matrix.tdotx(elements[0]), p_matrix.tdoty(elements[0]), p_matrix.tdotz(elements[0]),
		p_matrix.tdotx(elements[1]), p_matrix.tdoty(elements[1]), p_matrix.tdotz(elements[1]),
		p_matrix.tdotx(elements[2]), p_matrix.tdoty(elements[2]), p_matrix.tdotz(elements[2]));
#endif
}

_FORCE_INLINE_ Matrix3 Matrix3::operator*(const Matrix3& p_matrix) const {
#if defined (USE_SSE)

	__m128 m10 = elements[0].vec128;
	__m128 m11 = elements[1].vec128;
	__m128 m12 = elements[2].vec128;

	__m128 m2v = _mm_and_ps(p_matrix[0].vec128, vFFF0fMask);

	__m128 c0 = splat_ps(m10, 0);
	__m128 c1 = splat_ps(m11, 0);
	__m128 c2 = splat_ps(m12, 0);

	c0 = _mm_mul_ps(c0, m2v);
	c1 = _mm_mul_ps(c1, m2v);
	c2 = _mm_mul_ps(c2, m2v);

	m2v = _mm_and_ps(p_matrix[1].vec128, vFFF0fMask);

	__m128 c0_1 = splat_ps(m10, 1);
	__m128 c1_1 = splat_ps(m11, 1);
	__m128 c2_1 = splat_ps(m12, 1);

	c0_1 = _mm_mul_ps(c0_1, m2v);
	c1_1 = _mm_mul_ps(c1_1, m2v);
	c2_1 = _mm_mul_ps(c2_1, m2v);

	m2v = _mm_and_ps(p_matrix[2].vec128, vFFF0fMask);

	c0 = _mm_add_ps(c0, c0_1);
	c1 = _mm_add_ps(c1, c1_1);
	c2 = _mm_add_ps(c2, c2_1);

	m10 = splat_ps(m10, 2);
	m11 = splat_ps(m11, 2);
	m12 = splat_ps(m12, 2);

	m10 = _mm_mul_ps(m10, m2v);
	m11 = _mm_mul_ps(m11, m2v);
	m12 = _mm_mul_ps(m12, m2v);

	c0 = _mm_add_ps(c0, m10);
	c1 = _mm_add_ps(c1, m11);
	c2 = _mm_add_ps(c2, m12);

	return Matrix3(c0, c1, c2);

#elif defined(USE_NEON)

	float32x4_t rv0, rv1, rv2;
	float32x4_t v0, v1, v2;
	float32x4_t mv0, mv1, mv2;

	v0 = elements[0].vec128;
	v1 = elements[1].vec128;
	v2 = elements[2].vec128;

	mv0 = (float32x4_t)vandq_s32((int32x4_t)p_matrix[0].vec128, vFFF0Mask);
	mv1 = (float32x4_t)vandq_s32((int32x4_t)p_matrix[1].vec128, vFFF0Mask);
	mv2 = (float32x4_t)vandq_s32((int32x4_t)p_matrix[2].vec128, vFFF0Mask);

	rv0 = vmulq_lane_f32(mv0, vget_low_f32(v0), 0);
	rv1 = vmulq_lane_f32(mv0, vget_low_f32(v1), 0);
	rv2 = vmulq_lane_f32(mv0, vget_low_f32(v2), 0);

	rv0 = vmlaq_lane_f32(rv0, mv1, vget_low_f32(v0), 1);
	rv1 = vmlaq_lane_f32(rv1, mv1, vget_low_f32(v1), 1);
	rv2 = vmlaq_lane_f32(rv2, mv1, vget_low_f32(v2), 1);

	rv0 = vmlaq_lane_f32(rv0, mv2, vget_high_f32(v0), 0);
	rv1 = vmlaq_lane_f32(rv1, mv2, vget_high_f32(v1), 0);
	rv2 = vmlaq_lane_f32(rv2, mv2, vget_high_f32(v2), 0);

	return Matrix3(rv0, rv1, rv2);

#else
	return Matrix3(
		p_matrix.tdotx(elements[0]), p_matrix.tdoty(elements[0]), p_matrix.tdotz(elements[0]),
		p_matrix.tdotx(elements[1]), p_matrix.tdoty(elements[1]), p_matrix.tdotz(elements[1]),
		p_matrix.tdotx(elements[2]), p_matrix.tdoty(elements[2]), p_matrix.tdotz(elements[2]) );
#endif
}

Vector3 Matrix3::xform(const Vector3& p_vector) const {

#if defined (USE_SSE) || defined (USE_NEON)
	return p_vector.dot3(elements[0], elements[1], elements[2]);
#else
	return Vector3(
		elements[0].dot(p_vector),
		elements[1].dot(p_vector),
		elements[2].dot(p_vector)
	);
#endif
}

Vector3 Matrix3::xform_inv(const Vector3& p_vector) const {
#if defined (USE_SSE)

	const __m128 vv = p_vector.vec128;

	__m128 c0 = splat_ps(vv, 0);
	__m128 c1 = splat_ps(vv, 1);
	__m128 c2 = splat_ps(vv, 2);

	c0 = _mm_mul_ps(c0, _mm_and_ps(elements[0].vec128, vFFF0fMask));
	c1 = _mm_mul_ps(c1, _mm_and_ps(elements[1].vec128, vFFF0fMask));
	c0 = _mm_add_ps(c0, c1);
	c2 = _mm_mul_ps(c2, _mm_and_ps(elements[2].vec128, vFFF0fMask));

	return Vector3(_mm_add_ps(c0, c2));
#elif defined(USE_NEON)
	const float32x4_t vv = p_vector.vec128;
	const float32x2_t vlo = vget_low_f32(vv);
	const float32x2_t vhi = vget_high_f32(vv);

	float32x4_t c0, c1, c2;

	c0 = (float32x4_t)vandq_s32((int32x4_t)elements[0].vec128, vFFF0Mask);
	c1 = (float32x4_t)vandq_s32((int32x4_t)elements[1].vec128, vFFF0Mask);
	c2 = (float32x4_t)vandq_s32((int32x4_t)elements[2].vec128, vFFF0Mask);

	c0 = vmulq_lane_f32(c0, vlo, 0);
	c1 = vmulq_lane_f32(c1, vlo, 1);
	c2 = vmulq_lane_f32(c2, vhi, 0);
	c0 = vaddq_f32(c0, c1);
	c0 = vaddq_f32(c0, c2);

	return Vector3(c0);
#else
	return Vector3(tdotx(p_vector), tdoty(p_vector), tdotz(p_vector));
#endif
}

float Matrix3::determinant() const {

	return vec3_triple(elements[0], elements[1], elements[2]);
}
#endif
