
// Define
#ifndef CINPUT_H
#define CINPUT_H

// Include
#include "Shared.h"             // Common Header File
#include "CWindowGL.h"          // Window OpenGL Class Header File

#include "dinput.h"				// DirectX Input

// DirectX Input Class
class CInput
{

    private:

    protected:

    public:

        CInput( );              // Constructor
        CInput( HWND hwnd );    // Overload Constructor
        ~CInput( );             // Destructor

        // Initialize DirectX Input
		bool InitializeDirectInput( HWND hWnd );

		//. Deinitialize DirectX Input
		void DeinitializeDirectInput( );

		// Update Input State
		HRESULT UpdateInputState( );


};

#endif



