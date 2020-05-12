
// Include
#include "CWindowGL.h"               // WindowGL Class Header File

// Constructor
CWindowGL::CWindowGL( )
{


}

// Destructor
CWindowGL::~CWindowGL( )
{


}

// Register Window Class
bool CWindowGL::RegisterWindowGL( Application* App, WNDPROC WindowProc )
{

	WNDCLASSEX WindowClassEx;								// Window Class Ex Struct
	ZeroMemory( &WindowClassEx, sizeof( WNDCLASSEX ) );		// Zero Memory

	WindowClassEx.cbSize = sizeof( WNDCLASSEX );										// Size Of Struct
	WindowClassEx.style = CS_OWNDC ;						                            // Drawing
	WindowClassEx.lpfnWndProc = ( WNDPROC ) WindowProc;									// Window Proc
	WindowClassEx.hInstance = App->hInstance;											// Application Instance
	WindowClassEx.hIcon = LoadIcon( App->hInstance, MAKEINTRESOURCE( IDI_ICON ) );	    // Large Icon
	WindowClassEx.hIconSm = LoadIcon( App->hInstance, MAKEINTRESOURCE( IDI_ICON ) );	// Small Icon
	WindowClassEx.hbrBackground = ( HBRUSH ) GetStockObject( BLACK_BRUSH );				// Background Color
	WindowClassEx.hCursor = LoadCursor( App->hInstance, IDC_ARROW );					// Mouse Cursor Arrow
	WindowClassEx.lpszClassName = App->ClassName;										// Application Class Name
	WindowClassEx.lpszMenuName = App->ClassName;										// Menu

	// Register Window Class
	if ( !RegisterClassEx( &WindowClassEx ) )
	{
		return false;	// Failed
	}

	return true;	// Ok

}

// Create OpenGL Window
bool CWindowGL::CreateWindowGL( GL_Window* Window )
{

	// Window Style
	DWORD WindowStyle = WS_OVERLAPPED | WS_SYSMENU;;	// Define Window Style
	DWORD WindowExtendedStyle = WS_EX_APPWINDOW;		// Define Window Extended Style

	// Pixel Format Descriptor
	PIXELFORMATDESCRIPTOR pfd =											// PFD Structure
	{
		sizeof (PIXELFORMATDESCRIPTOR),									// Size Of Pixel Format Descriptor
		1,																// Version Number
		PFD_DRAW_TO_WINDOW |											// Format Support Window
		PFD_SUPPORT_OPENGL |											// Format Support OpenGL
		PFD_DOUBLEBUFFER,												// Support Double Buffering
		PFD_TYPE_RGBA,													// Request RGBA Format
		Window->Init.BitsPerPixel,										// Select Color Depth
		0, 0, 0, 0, 0, 0,												// Color Bits Ignored
		0,																// No Alpha Buffer
		0,																// Shift Bit Ignored
		0,																// No Accumulation Buffer
		0, 0, 0, 0,														// Accumulation Bits Ignored
		16,																// 16Bit Z-Buffer (Depth Buffer)
		0,																// Stencil Buffer
		0,																// No Auxiliary Buffer
		PFD_MAIN_PLANE,													// Main Drawing Layer
		0,																// Reserved
		0, 0, 0															// Layer Masks Ignored
	};

	// Define Window Coordinates
	RECT WindowRect = { 0, 0, Window->Init.Width, Window->Init.Height };

	// Fullscreen Mode
	if ( Window->Init.Fullscreen == true )
	{
		// Change Display Resolution
		if ( ChangeDisplayResolution( Window->Init.Width, Window->Init.Height,
									  Window->Init.BitsPerPixel, Window->Init.Vsync ) == false )
		{
			// Error
			MessageBox( HWND_DESKTOP, "Error Changing Display Resolution.", "Error", MB_OK | MB_ICONEXCLAMATION );
			Window->Init.Fullscreen = false;	// Set Window Mode
		}
		else
		{
			ShowCursor( false );					// Hide Mouse Cursor
			WindowStyle = WS_POPUP;					// Popup Window Style
			WindowExtendedStyle |= WS_EX_TOPMOST;	// Window Ontop
		}
	}
	else
	{
		// Adjust Window Rect, Borders
		AdjustWindowRectEx( &WindowRect, WindowStyle, 0, WindowExtendedStyle );
	}

	// Calculate Screen Center
	int CenterX = ( GetSystemMetrics( SM_CXSCREEN ) / 2 ) - ( Window->Init.Width / 2 );
	int CenterY = ( GetSystemMetrics( SM_CYSCREEN ) / 2 ) - ( Window->Init.Height / 2 );

	// Create Window
	Window->hWnd = CreateWindowEx( WindowExtendedStyle,					// Extended Window Style
								   Window->Init.App->ClassName,			// Application Class Name
								   Window->Init.Title,					// Window Title
								   WindowStyle,							// Window Style
								   CenterX, CenterY,					// Window X, Y Position
								   WindowRect.right - WindowRect.left,	// Window Width
								   WindowRect.bottom - WindowRect.top,	// Window Height
								   HWND_DESKTOP,						// Parent HWND
								   0,									// NULL Menu
								   Window->Init.App->hInstance,			// Window Instance
								   Window );

	// Validate Window Creation
	if ( Window->hWnd == 0 )
	{
		// Error
		return false;	// Failed
	}

	// Get Window Device Context
	Window->hDC = GetDC( Window->hWnd );

	// Validate Device Context
	if ( Window->hDC == 0 )
	{
		// Error
		DestroyWindow( Window->hWnd );				// Destroy Window
		Window->hWnd = 0;							// Zero Window Handle
		return false;	// Failed
	}

	// Match Compatiable Pixel Format
	GLuint PixelFormat = ChoosePixelFormat( Window->hDC, &pfd );

	// Validate Pixel Format
	if ( PixelFormat == 0 )
	{
		// Error
		ReleaseDC( Window->hWnd, Window->hDC );		// Release Device Context
		Window->hDC = 0;							// Zero Device Context
		DestroyWindow( Window->hWnd );				// Destroy Window
		Window->hWnd = 0;							// Zero Window Handle
		return false;	// Failed
	}

	// Set Pixel Format
	if ( !SetPixelFormat( Window->hDC, PixelFormat, &pfd ) )
	{
		// Error
		ReleaseDC( Window->hWnd, Window->hDC );		// Release Device Context
		Window->hDC = 0;							// Zero Device Context
		DestroyWindow( Window->hWnd );				// Destroy Window
		Window->hWnd = 0;							// Zero Window Handle
		return false;	// Failed
	}

	// Get Rendering Context
	Window->hRC = wglCreateContext( Window->hDC );

	// Validate Rendering Context
	if ( Window->hRC == 0 )
	{
		// Error
		ReleaseDC( Window->hWnd, Window->hDC );		// Release Device Context
		Window->hDC = 0;							// Zero Device Context
		DestroyWindow( Window->hWnd );				// Destroy Window
		Window->hWnd = 0;							// Zero Window Handle
		return false;	// Failed
	}

	// Current Context
	if ( !wglMakeCurrent( Window->hDC, Window->hRC ) )
	{
		// Error
		wglDeleteContext( Window->hRC );			// Delete Rendering Context
		Window->hRC = 0;							// Zero Rendering Context
		ReleaseDC( Window->hWnd, Window->hDC );		// Release Device Context
		Window->hDC = 0;							// Zero Device Context
		DestroyWindow( Window->hWnd );				// Destroy Window
		Window->hWnd = 0;							// Zero Window Handle
		return false;	// Failed
	}

	// Display OpenGL Window
	ShowWindow( Window->hWnd, SW_NORMAL );

	// Set Visible Flag
	Window->Visible = true;

    // Set Vertical Sync
    EnableVerticalSync( Window->Init.Vsync );

    // Initialize OpenGL Parameters
    Initialize( );

	return true;	// Ok

}

// Destroy OpenGL Window
bool CWindowGL::DestroyWindowGL( GL_Window* Window )
{

	if ( Window->hWnd != 0 )						// Window Handle
	{
		if ( Window->hDC != 0 )						// Window Device Context
		{

			wglMakeCurrent( Window->hDC, 0 );		// Active Zero Rendering Context

			if ( Window->hRC != 0 )					// Rendering Context
			{
				wglDeleteContext( Window->hRC );	// Release Rendering Context
				Window->hRC = 0;					// Zero Rendering Context
			}

			ReleaseDC( Window->hWnd, Window->hDC );	// Release Device Context
			Window->hDC = 0;						// Zero Device Context

		}

		DestroyWindow( Window->hWnd );				// Destroy Window
		Window->hWnd = 0;							// Zero Window Handle
	}

	// Fullscreen Flag
	if ( Window->Init.Fullscreen == true )
	{
		ChangeDisplaySettings( NULL, 0 );			// Switch Display Resolution
		ShowCursor( true );							// Display Mouse Cursor
	}

	return true;	// Ok

}

// Reshape 3d Perspective Projection
void CWindowGL::ReshapePerspective( int Width, int Height, float FOV, float ZNear, float ZFar )
{

	glViewport( 0, 0, (GLsizei)(Width), (GLsizei)(Height) );	// Resize Viewport
	glMatrixMode( GL_PROJECTION );								// Select Projection Matrix
	glLoadIdentity( );											// Reset Projection Matrix

	gluPerspective( FOV,										// Field Of Vision
					(GLfloat)(Width)/(GLfloat)(Height),			// Aspect Ratio
					ZNear, ZFar );								// Calculate Perspective 3d Projection

	glMatrixMode( GL_MODELVIEW );								// Select ModelView Matrix
	glLoadIdentity( );											// Reset ModelViewMatrix

}

// Reshape 2d Ortho Projection
void CWindowGL::ReshapeOrtho( int Width, int Height )
{

	glViewport( 0, 0, (GLsizei)(Width), (GLsizei)(Height) );	// Resize Viewport
	glMatrixMode( GL_PROJECTION );								// Select Projection Matrix
	glLoadIdentity( );											// Reset Projection Matrix

	glOrtho( 0, Width, 0, Height, -1, 1 );					    // Set Orthographical 2d Projection

	glMatrixMode( GL_MODELVIEW );								// Select ModelView Matrix
	glLoadIdentity( );											// Reset ModelViewMatrix

}


// Initialize OpenGL
void CWindowGL::Initialize( void )
{

	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );							// Clear Color

	glDepthFunc( GL_LESS );									        // Depth Test
	glClearDepth( 1.0f );											// Clear Depth
	glEnable( GL_DEPTH_TEST );										// Enable Detph Testing

	glShadeModel( GL_SMOOTH );										// Smooth Shading
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );					// Filled Mode
	glEnable( GL_TEXTURE_2D );										// Enable Texturing

	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );			// Blending Mode
	glEnable( GL_BLEND );											// Enable Blending

	glCullFace( GL_FRONT );											// Cull Back Faces
	glFrontFace( GL_CCW);											// Clock Wise

	glAlphaFunc( GL_GREATER, 0.8f );                                // Alpha Function

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);            // S Generate Texture Coordinates
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);            // T Generate Texture Coordinates


    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );			// Perspective Calculation Hint

}

// Enable Verticle Sync
void CWindowGL::EnableVerticalSync( bool EnableVerticalSync )
{

    typedef BOOL (WINAPI * PFNWGLSWAPINTERVALEXTPROC)(GLint);

    static PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT =
        reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC>(
        wglGetProcAddress("wglSwapIntervalEXT"));

    if (wglSwapIntervalEXT)
    {
        wglSwapIntervalEXT( EnableVerticalSync ? 1 : 0 );
    }

}

// Change Screen Resolutions
bool CWindowGL::ChangeDisplayResolution( int Width, int Height, int BitsPerPixel, bool VSync )
{

	DEVMODE dmScreenSettings;									// Device Mode
	ZeroMemory( &dmScreenSettings, sizeof( DEVMODE ) );			// Zero Screen Settings

	dmScreenSettings.dmSize = sizeof( DEVMODE );									// Structure Size
	dmScreenSettings.dmPelsWidth = Width;											// Screen Width
	dmScreenSettings.dmPelsHeight = Height;											// Screen Height
	dmScreenSettings.dmBitsPerPel = BitsPerPixel;									// Screen Bits Per Pixel
	dmScreenSettings.dmFields = ( DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT );

	// Change Screen Display Mode
	if ( ChangeDisplaySettings( &dmScreenSettings, CDS_FULLSCREEN ) != DISP_CHANGE_SUCCESSFUL )
	{
		// Error
		return false;	// Failed
	}

	// Enable Verticle Sync Flag
	EnableVerticalSync( VSync );

	return true;		// Ok

}






