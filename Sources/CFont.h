
// Define
#ifndef CFONT_H
#define CFONT_H

// Include
#include "Shared.h"             // Shared Header File
#include "CWindowGL.h"          // OpenGL Window Class Header File


// Font Class
class CFont
{

    private:

		GLuint ListID;

    public:

        CFont( );                   // Constructor
        ~CFont( );                  // Destructor

		// Create Font
		bool CreateFontGL( GL_Window* Window, string Name, int Size, bool Bold = false,
							bool Italic = false, bool Underline = false, bool Strike = false );

		// Render Font Text
		void RenderText( GL_Window* Window, string Str, float R, float G, float B, float A, float PosX, float PosY );


};

#endif  // Endif

