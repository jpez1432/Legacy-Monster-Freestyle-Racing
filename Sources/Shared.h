
// Define
#ifndef SHARED_H
#define SHARED_H

#define _WIN32_WINNT 0x0400				// Win32
#define VC_EXTRALEAN					// Exlucde Extra Lean
#define WIN32_LEAN_AND_MEAN			// Exclude Lean And Mean

// Include
#include <windows.h>                    // Win32 Api Header File
#include <commctrl.h>					// Common Control Header
#include <string>                       // String Header File
#include <vector>                       // Vector Header File
#include <list>                         // List Header File
#include <fstream>                      // Fstream Header File
#include <iostream>                     // IOstream Header File

#include <gl/gl.h>						// OpenGL Header File
#include <gl/glu.h>						// Glu Header File

#include "ode/ode.h"                    // ODE Header File

#include "CVector4.h"                   // Vector4 Class Header File

using namespace std;                    // Use Standard Namespace

// Safe Delete Macros
#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }		// Safe Delete
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }		// Safe Release

// Define ODE
#define PI 3.1415926535897932
#define RAD2DEG(a) a*(180.0/PI)			                            // Converts Radians To Degrees
#define DEG2RAD(a) a*(PI/180.0)			                            // Converts Degrees To Radians

// Define OpenGL
#define GL_BGR 0x80E0           // Define BGR
#define GL_BGRA 0x80E1          // Define BGRA

// String To Upper Case
string StringToUpper( string str );

// Instr Function
int Instr(int startPOS, string strSearch, string strFind);

// String Replace Function
string Replace(string Base, string Old, string New);

// Convert ODE Matrix To OpenGL Matrix
void ODEtoOGL( float* M, const dReal* p, const dReal* R );

// Convert OpenGL Matrix To ODE Matrix
void OGLtoODE( const float* M, const dReal* P, dReal* R );

#endif  // Endif
