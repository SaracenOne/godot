/*************************************************************************/
/*  math_defs.h                                                          */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                    http://www.godotengine.org                         */
/*************************************************************************/
/* Copyright (c) 2007-2015 Juan Linietsky, Ariel Manzur.                 */
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
#ifndef MATH_DEFS_H
#define MATH_DEFS_H



#if 1
	#if 1
		#ifndef USE_SSE
			#define USE_SSE
		#endif
	#elif defined (__ARM_NEON__)
		#ifdef __clang__
			#ifndef USE_NEON
				#define USE_NEON
			#endif
		#endif
	#endif

	#define ATTRIBUTE_ALIGNED16(a) a __attribute__ ((aligned (16)))
#else
	#define ATTRIBUTE_ALIGNED16(a) a
#endif


#if defined (USE_SSE)

	#if 1
		#include <smmintrin.h>
	#elif defined (__SSSE3__)
		#include <tmmintrin.h>
	#elif defined (__SSE3__)
		#include <pmmintrin.h>
	#else
		#include <emmintrin.h>
	#endif

	typedef __m128		SimdFloat4;

#if defined(_MSC_VER)
#    define CastiTo128f(a) (_mm_castsi128_ps(a))
#    define CastfTo128d(a) (_mm_castps_pd(a))
#    define CastdTo128f(a) (_mm_castpd_ps(a))
#else
#    define CastiTo128f(a) ((__m128)(a))
#    define CastfTo128d(a) ((__m128d)(a))
#    define CastdTo128f(a) ((__m128) (a))
#endif

	#define vMzeroMask (_mm_set_ps(-0.0f, -0.0f, -0.0f, -0.0f))
	#define vFFF0Mask (_mm_set_epi32(0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF))
	#define vFFF0fMask CastiTo128f(vFFF0Mask)
	#define vxyzMaskf vFFF0fMask

	#define SHUFFLE(x,y,z,w) ((w)<<6 | (z)<<4 | (y)<<2 | (x))
	#define pshufd_ps( _a, _mask ) _mm_shuffle_ps((_a), (_a), (_mask) )
	#define splat_ps( _a, _i )  pshufd_ps((_a), SHUFFLE(_i,_i,_i,_i) )

#elif defined (USE_NEON)

	#include <arm_neon.h>

	typedef float32x4_t	SimdFloat4;
	const float32x4_t ATTRIBUTE_ALIGNED16(vMzeroMask) = (float32x4_t){-0.0f, -0.0f, -0.0f, -0.0f};
	const int32x4_t ATTRIBUTE_ALIGNED16(vFFF0Mask) = (int32x4_t){static_cast<int32_t>(0xFFFFFFFF),
		static_cast<int32_t>(0xFFFFFFFF), static_cast<int32_t>(0xFFFFFFFF), 0x0};

#endif



#define CMP_EPSILON 0.00001
#define CMP_EPSILON2 (CMP_EPSILON*CMP_EPSILON)
#define CMP_NORMALIZE_TOLERANCE 0.000001
#define CMP_POINT_IN_PLANE_EPSILON 0.00001

/**
  * "Real" is a type that will be translated to either floats or fixed depending
  * on the compilation setting
  */

enum ClockDirection {

        CLOCKWISE,
	COUNTERCLOCKWISE
};


#ifdef REAL_T_IS_DOUBLE

typedef double real_t;

#else

typedef float real_t;

#endif


#endif // MATH_DEFS_H
