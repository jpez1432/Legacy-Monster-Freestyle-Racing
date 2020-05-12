
// Define
#ifndef CRENDER_H
#define CRENDER_H

// Include
#include "Shared.h"                     // Shared Header File
#include "CWindowGL.h"                  // OpenGL Window Class Header File
#include "CConfig.h"                    // Config Class Header File
#include "CTimer.h"                     // Timer Class Header File
#include "CFont.h"                      // Font Class Header File
#include "CFrustum.h"                   // Frustum Class Header File
#include "CQuadTree.h"                  // QuadTree Class Header File
#include "CCamera.h"                    // Camera Class Header File
#include "CInput.h"                     // DirectX Input Class Header File

// MTM Include
#include "CTrack.h"                     // Track Class Header File

// ODE Include
#include "CVehiclePhysics.h"	        // Vehicle Physics Header
#include "CGenericPhysics.h"            // Generic Physics Header

// Render Class
class CRender
{

    private:

		int Width;				                // Window Width
		int Height;				                // Window Height

        CWindowGL* cWindowGL;                   // WindowGL Pointer
		GL_Window* Window;						// Window Pointer
 		CConfig* cConfig;						// Config Pointer
 		CTimer* cTimer;                         // Timer Pointer

        CTrack* cTrack;                         // Track Pointer

        CQuadTree* cQuadTree;                   // QuadTree Class Object

        CInput cInput;
        CCamera cCamera;                        // Camera Class Object
        CFont cFont;                            // Font Class Object
        CFrustum cFrustum;                      // Frustum Class Object

		GLuint SaveState;						// Save State Display List
		GLuint RestoreState;					// Restore State Display List

		vector <GLuint> Textures;			    // OpenGL Texture Ids

        // Create Font Display List States
		bool BuildFontStates( );

        // Render HUD
        void HeadsUpDisplay( );

        // Load Bitmap Texture File
        bool LoadBitmap( string FileName, bool Alpha = false );

        // Intro Scene
        bool Intro( void );

        // Fade Textures
        void Fade( int Delay, int Texture );

		// Render Quadtree Node
		void Node( CNode *cNode, float xOffset, float zOffset );

		// Renders Objects
		void Objects( CNode *cNode, float xOffset, float zOffset );

        // Render SkyBox
        void SkyBox( const dReal Pos[4] );

    protected:

    public:

        CRender( );            // Constructor
        ~CRender( );           // Destructor

		// Initialize Renderer
		bool Initialize( CWindowGL* cWindowGL, GL_Window *Window,
                         CConfig* cConfig, CTimer* cTimer, CTrack* cTrack );

		// Render Scene
		void Scene( void );

};

#endif  // Endif
