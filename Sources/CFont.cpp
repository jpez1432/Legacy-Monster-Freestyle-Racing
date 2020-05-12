
// Include
#include "CFont.h"                 // Font Class Header File

// Constructor
CFont::CFont( ) : ListID(0)
{



}

// Destructor
CFont::~CFont( )
{

    // Delete OpenGL Font Display List
    if ( glIsList( ListID ) ) glDeleteLists( ListID, 1 );

}

// Create Font
bool CFont::CreateFontGL( GL_Window* Window, string Name, int Size, bool Bold, bool Italic, bool Underline, bool Strike )
{

	HFONT hFont;								// Windows Font
	ListID = glGenLists( 96 );		            // 96 Character Display Lists

	// Is Bold Flag
	DWORD BoldVal = 0; if ( Bold == true ) BoldVal = FW_BOLD;

	// Create Windows Font
	hFont = CreateFont( Size, 0, 0, 0, BoldVal, Italic, Underline, Strike,
						ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
						ANTIALIASED_QUALITY, FF_DONTCARE | DEFAULT_PITCH, Name.c_str( ) );

	if( !hFont )		// Creation Failed
		return false;	// Error

	SelectObject( Window->hDC, hFont );						// Select Font
	wglUseFontBitmaps( Window->hDC, 32, 96, ListID );		// Use OpenGL Font

	return true;	// Ok


}

// Render Font Text
void CFont::RenderText( GL_Window* Window, string Str, float R, float G, float B, float A, float PosX, float PosY )
{

	glColor4f( R, G, B, A);							// Text Color
	glRasterPos2f( PosX, PosY );					// Position Text

	glPushAttrib( GL_LIST_BIT );					// Push List Bits

	glListBase( ListID - 32 );			            // Set List Base

	glCallLists( GLsizei(Str.length( )),
				 GL_UNSIGNED_BYTE, Str.c_str( ) );	// Call Font Display List

	glPopAttrib( );									// Pop Attribute Bits

}

