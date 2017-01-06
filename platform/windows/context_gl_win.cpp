/*************************************************************************/
/*  context_gl_win.cpp                                                   */
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
#if defined(OPENGL_ENABLED) || defined(LEGACYGL_ENABLED) || defined(GLES2_ENABLED)

//
// C++ Implementation: context_gl_x11
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#define WINVER 0x0500
#include "context_gl_win.h"

//#include "drivers/opengl/glwrapper.h"
//#include "ctxgl_procaddr.h"

#define WGL_CONTEXT_MAJOR_VERSION_ARB  0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB  0x2092
#define WGL_CONTEXT_FLAGS_ARB          0x2094
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x00000002

#define WGL_DRAW_TO_WINDOW_ARB             0x2001
#define WGL_ACCELERATION_ARB               0x2003
#define WGL_SUPPORT_OPENGL_ARB             0x2010
#define WGL_DOUBLE_BUFFER_ARB              0x2011
#define WGL_COLOR_BITS_ARB                 0x2014
#define WGL_ALPHA_BITS_ARB                 0x201B
#define WGL_DEPTH_BITS_ARB                 0x2022
#define WGL_STENCIL_BITS_ARB               0x2023
#define WGL_FULL_ACCELERATION_ARB          0x2027
#define WGL_SAMPLE_BUFFERS_ARB             0x2041
#define WGL_SAMPLES_ARB                    0x2042

typedef HGLRC (APIENTRY* PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC, HGLRC, const int*);
typedef BOOL(WINAPI * PFNWGLCHOOSEPIXELFORMATARBPROC) (HDC, const int*, const FLOAT *, UINT, int *, UINT *);

void ContextGL_Win::release_current() {

	wglMakeCurrent(hDC,NULL);
}

void ContextGL_Win::make_current() {

	wglMakeCurrent(hDC,hRC);
}

int ContextGL_Win::get_window_width() {

	return OS::get_singleton()->get_video_mode().width;
}

int ContextGL_Win::get_window_height() {

	return OS::get_singleton()->get_video_mode().height;
}

void ContextGL_Win::swap_buffers() {

	SwapBuffers(hDC);
}

/*
static GLWrapperFuncPtr wrapper_get_proc_address(const char* p_function) {

	print_line(String()+"getting proc of: "+p_function);
	GLWrapperFuncPtr func=(GLWrapperFuncPtr)get_gl_proc_address(p_function);
	if (!func) {
		print_line("Couldn't find function: "+String(p_function));
		print_line("error: "+itos(GetLastError()));
	}
	return func;

}
*/

void ContextGL_Win::set_use_vsync(bool p_use) {

	if (wglSwapIntervalEXT) {
		wglSwapIntervalEXT(p_use?1:0);
	}
	use_vsync=p_use;

}

bool ContextGL_Win::is_using_vsync() const {

	return use_vsync;
}


Error ContextGL_Win::initialize() {

	static	PIXELFORMATDESCRIPTOR pfd= {
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,	
		24,
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,// No Alpha Buffer
		0,// Shift Bit Ignored
		0,// No Accumulation Buffer
		0, 0, 0, 0,// Accumulation Bits Ignored
		24,// 24Bit Z-Buffer (Depth Buffer)  
		8,// No Stencil Buffer
		0,// No Auxiliary Buffer
		PFD_MAIN_PLANE, // Main Drawing Layer
		0,// Reserved
		0, 0, 0	// Layer Masks Ignored
	};
	
	if (!(hDC = GetDC(hWnd))) {
		MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return ERR_CANT_CREATE;								// Return FALSE
	}

	if (multisample_supported == true)
	{
		pixel_format = multisample_format;
	}
	else
	{
		if (!(pixel_format = ChoosePixelFormat(hDC, &pfd)))	// Did Windows Find A Matching Pixel Format?
		{
			MessageBox(NULL, "Can't Find A Suitable pixel_format.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
			return ERR_CANT_CREATE;								// Return FALSE
		}
	}

	if(!SetPixelFormat(hDC, pixel_format, &pfd))		// Are We Able To Set The Pixel Format?
	{
		MessageBox(NULL,"Can't Set The pixel_format.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return ERR_CANT_CREATE;								// Return FALSE
	}

	if (!(hRC=wglCreateContext(hDC)))				// Are We Able To Get A Rendering Context?
	{
		MessageBox(NULL,"Can't Create A Temporary GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return ERR_CANT_CREATE;								// Return FALSE
	}

	wglMakeCurrent(hDC,hRC);

	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)  wglGetProcAddress ("wglSwapIntervalEXT");
//	glWrapperInit(wrapper_get_proc_address);


	return OK;
}

void ContextGL_Win::shutdown() {
	if (hDC != 0)
	{
		release_current();
		if (hRC != 0)
		{
			wglDeleteContext(hRC);
			hRC = 0;

		}
		ReleaseDC(hWnd, hDC);
		hDC = 0;
	}
}

int ContextGL_Win::test_multisample_support(int p_samples) {
	// TODO: test extensions

	PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = NULL;
	wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");

	if (wglChoosePixelFormatARB != NULL)
	{
		int pixel_format;
		bool valid;
		UINT num_formats;
		float fAttributes[] = { 0, 0 };

		int iAttributes[] = {
			WGL_DRAW_TO_WINDOW_ARB, 1,
			WGL_SUPPORT_OPENGL_ARB, 1,
			WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
			WGL_COLOR_BITS_ARB, 24,
			WGL_ALPHA_BITS_ARB, 8,
			WGL_DEPTH_BITS_ARB, 24,
			WGL_STENCIL_BITS_ARB, 8,
			WGL_DOUBLE_BUFFER_ARB, 1,
			WGL_SAMPLE_BUFFERS_ARB, 1,
			WGL_SAMPLES_ARB, p_samples,
			0, 0 };

		valid = wglChoosePixelFormatARB(hDC, iAttributes, fAttributes, 1, &pixel_format, &num_formats);

		if (valid && num_formats >= 1)
		{
			multisample_supported = true;
			multisample_format = pixel_format;

			return p_samples;
		}
	}

	return 0;
}

void ContextGL_Win::set_active_hwnd(HWND hwnd) {
	hWnd = hwnd;
}


ContextGL_Win::ContextGL_Win() {

	multisample_supported = false;
	multisample_format = 0;
	use_vsync=false;
}

ContextGL_Win::~ContextGL_Win() {


}


#endif
