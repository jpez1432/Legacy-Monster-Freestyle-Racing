
// Define
#ifndef CWINDOWGL_H
#define CWINDOWGL_H

// Include
#include "Shared.h"                 // Shared Header File
#include "Resources.h"		        // Resources Header File

// KeyEvents
typedef struct {					// Keyboard Events
	bool Keys[256];					// Key Flags
} KeyEvents;

// Application
typedef struct {					// Application Info
	HINSTANCE hInstance;			// Application Instance
	const char* ClassName;			// Application Class Name
} Application;

// GL_WindowInit
typedef struct {					// OpenGL Window Init
	Application* App;				// Application Struct
	const char* Title;			    // Window Title
	int Width;						// Window Width
	int Height;						// Window Height
	int BitsPerPixel;				// Window Bits Per Pixel
	bool Vsync;						// Vertical Sync Flag
	bool Fullscreen;				// Fullscreen Flag
} GL_WindowInit;

// GL_Window
typedef struct {					// OpenGL Window
	KeyEvents* Keys;				// Keyboard Events
	GL_WindowInit Init;				// Init Window
	HWND hWnd;						// Window Handle
	HDC hDC;						// Window Device Context
	HGLRC hRC;						// Window Rendering Context
	bool Visible;					// Window Visible Flag
} GL_Window;

// OpenGL Window Class
class CWindowGL
{

    private:

		// Initialize OpenGL
		void Initialize( void );

        // Enable Vertical Sync
        void EnableVerticalSync( bool EnableVerticalSync );

    public:

        CWindowGL( );                   // Constructor
        ~CWindowGL( );                  // Destructor

		// Register Window Class
		bool RegisterWindowGL( Application* App, WNDPROC WindowProc );

		// Create OpenGL Window
		bool CreateWindowGL( GL_Window* Window );

		// Destroy OpenGL Window
		bool DestroyWindowGL( GL_Window* Window );

		// Reshape 3d Perspective Projection
		void ReshapePerspective( int Width, int Height, float FOV, float ZNear, float ZFar );

		// Reshape 2d Ortho Projection
		void ReshapeOrtho( int Width, int Height );

        // Change Display Resolution
        bool ChangeDisplayResolution( int Width, int Height, int BitsPerPixel, bool VSync );

};

#endif  // Endif


