
// Include
#include "CRender.h"                        // Render Class Header File

extern string Version;                      // Global Version String
extern string RootDir;                      // Global Resource Directory String

extern CVehiclePhysics *cVehiclePhysics;	// Global Vehicle Physics Object
//extern CGenericPhysics *Boxes[512];	        // Boxes Generic Physics Object

#define PHYS_STEP_SCALE 4.0             // Step Scaling Time
#define PHYS_UPDATE_RATE 140.0          // Update Physics Rate

// Physics Timing
static const double	PhysDelta = 1.0f / PHYS_UPDATE_RATE;
static const double	PhysStep = 0.04f * (60.0f / PHYS_UPDATE_RATE);
double PhysTime =  0.0f;
double DeltaTime = 0.0f;


// Constructor
CRender::CRender( )
{



}

// Destructor
CRender::~CRender( )
{

	// Delete Display List State Memory
    if ( glIsList( SaveState ) ) glDeleteLists( SaveState, 1 );
    if ( glIsList( RestoreState ) ) glDeleteLists( RestoreState, 1 );

    // Delete QuadTree
    SAFE_DELETE( cQuadTree );

}

// Initialize Renderer
bool CRender::Initialize( CWindowGL* cWindowGL, GL_Window *Window,
                          CConfig* cConfig, CTimer* cTimer, CTrack *cTrack )
{

    // Copy Pointers
    this->cWindowGL = cWindowGL;                    // Copy WindowGL Pointer
	this->Window = Window;				            // Copy Window Pointer
	this->cConfig = cConfig;				        // Copy Config Pointer
    this->cTimer = cTimer;                          // Copy Timer Pointer
    this->cTrack = cTrack;                          // Copy Track Pointer

	// Get Window Client Dimensions
	RECT rWindow;									// Window Rect
	GetClientRect( Window->hWnd, &rWindow );		// Get Window Client Area
	Width = rWindow.right - rWindow.left;			// Calculate Width
	Height = rWindow.bottom - rWindow.top;			// Calculate Height

	// Build OpenGL Font
	cFont.CreateFontGL( Window, "Arial", 15, true );

    // Create QuadTree Object
    cQuadTree = new CQuadTree( );

    // Initialize QuadTree
    cQuadTree->Initialize( 256, 16, cTrack );

    // Get Truck 0 Position
    CVector4 Cam = cTrack->TruckPosition( 0 );

    // Position Camera
    cCamera.PositionCamera( Cam.z / 32 + 8, Cam.y / 32, Cam.x / 32 + 8,
                            0.0f, 0.0f, 0.0f,
                            0.0f, 1.0f, 0.0f );

    // Bind Camera Keys
    cCamera.BindKeys( cConfig->Gas[0], cConfig->Brake[0],
                      cConfig->FrontSteerLeft[0], cConfig->FrontSteerRight[0] );

	// Build Font Display States
	if ( !BuildFontStates( ) ) return false;

    // Save Display State
	glCallList( SaveState );

    // Render Loading Text
    cFont.RenderText( Window, "Loading...", 1, 1, 1, 1, 10, 10 );

    // Swap Double Buffer
    SwapBuffers( Window->hDC );

    // Load Logos
    LoadBitmap( RootDir + "Natas8_Logo.Bmp" );
    LoadBitmap( RootDir + "Black_Logo.Bmp" );
    LoadBitmap( RootDir + "MFR_Logo.Bmp" );
    LoadBitmap( RootDir + "\\Hud\\" + "Gauges.Bmp", true );

    // Render Intro Scene
    if ( cConfig->SkipIntro == false ) Intro( );

    // Reset OpenGL States
	glAlphaFunc( GL_GEQUAL, 0.8f );

    // Initialize Direct Input
    if ( cConfig->UseJoystick == true ) cInput.InitializeDirectInput( Window->hWnd );

    // Reset Timer (ODE)
    cTimer->Reset( );


    return true;    // Return Ok

}

// Render Scene
void CRender::Scene( void )
{

    // Clear Screen And Depth Buffer
    glClear( GL_COLOR_BUFFER_BIT |  GL_DEPTH_BUFFER_BIT );

    // Update Timer
    cTimer->Update( );

//	// Computer Delta
//	static double LastTime = cTimer->FrameTimer( );
//	double CurTime = cTimer->FrameTimer( );
//	DeltaTime = (CurTime - LastTime);
//	PhysTime += DeltaTime;
//
//	// Fixed Time Step
//	// Update Physics
//	if ( PhysTime >= PhysDelta)
//	{
//		CPhysicsServer::GetInstance()->Update( (dReal)PhysStep );
//		PhysTime -= PhysDelta;
//	}

    // Update Direct Input
    if ( cConfig->UseJoystick == true ) cInput.UpdateInputState( );

	// Time Based Modeling Physics Update
	CPhysicsServer::GetInstance()->Update( cTimer->FrameInterval( ) );

    // Update Chase Camera
    cCamera.UpdateChase( Window, cVehiclePhysics, cConfig, cTimer->FrameInterval( ) );

    // Update Camera
    //cCamera.UpdateFrame( Window, 0.001f, cTimer->FrameInterval( ) );

    // Camera LookAt
    //cCamera.LookAt( );

    // Calculate Viewing Frustum
    cFrustum.CalculateFrustum( );

	// Render SkyBox
	SkyBox( cVehiclePhysics->GetPosition( ) );

    // Render Wrapping Terrain, Boxes, Models
    if ( cConfig->WrapTrack == true )
    {
        Node( cQuadTree->GetRoot( ), 0, 0 );			// Render Middle Quadtree Root Node
        Node( cQuadTree->GetRoot( ), -256, -256 );	    // Render Top Left Quadtree Root Node
        Node( cQuadTree->GetRoot( ), 256, 256 );		// Render Bottom Right Quadtree Root Node
        Node( cQuadTree->GetRoot( ), 256, -256 );	    // Render Bottom Left Quadtree Root Node
        Node( cQuadTree->GetRoot( ), -256, 256 );	    // Render Top Right Quadtree Root Node
        Node( cQuadTree->GetRoot( ), 0, -256 );		    // Render Middle Top Quadtree Root Node
        Node( cQuadTree->GetRoot( ), 0, 256 );		    // Render Middle Bottom Quadtree Root Node
        Node( cQuadTree->GetRoot( ), -256, 0 );		    // Render Top Middle Quadtree Root Node
        Node( cQuadTree->GetRoot( ), 256, 0 );		    // Render Bottom Middle Quadtree Root Node
    }
    else
    {
        // Render Node
        Node( cQuadTree->GetRoot( ), 0, 0 );
    }


	// Render Vehicle
	cVehiclePhysics->Render();

    // Render Water
    glCallList( cTrack->cModels->WaterList );


//    for ( int i= 0; i != 512; i++ )
//        if ( Boxes[i] )
//            Boxes[i]->Render();

    // Render HUD
    HeadsUpDisplay( );

    // Flush GL Pipeline
    glFlush( );

}

// Render Quadtree Node
void CRender::Node( CNode *cNode, float xOffset, float zOffset )
{

	// Validate Node
	if ( cNode == NULL )
		return;

	// Node
	if ( !cNode->Leaf )
	{
		// Loop Children Nodes
		for (int i = 0; i < 4; ++i)
		{
			// Render Children Node
			Node( cNode->Child[i], xOffset, zOffset );
		}
	}
	else	// Leaf
	{

		// Setup Center Vector Offsets
		CVector4 CenterOffset( cNode->Center.x + xOffset, cNode->Center.y, cNode->Center.z + zOffset );

		// Leaf In View Frustum
		if ( cFrustum.SphereInFrustum(CenterOffset, 24.0f ) == 1 )
		{

			// Push Matrix
			glPushMatrix( );

                // Translate Chunk Position
                glTranslated( cNode->Center.x + xOffset, 0 , cNode->Center.z + zOffset );

                // Call Terrain List
                glCallList( cTrack->cTerrain->TerrainList( (int)cNode->Center.x / 16, (int)cNode->Center.z / 16 ) );

                // Call Terrain List
                glCallList( cTrack->cGroundBoxes->BoxList( (int)cNode->Center.x / 16, (int)cNode->Center.z / 16 ) );

			// Pop Matrix
			glPopMatrix( );

			// Render Objects
			Objects( cNode, xOffset, zOffset );

		}

	}

}

// Renders Objects
void CRender::Objects( CNode *cNode, float xOffset, float zOffset )
{

	// Get Camera Position
	const dReal *Pos = cVehiclePhysics->GetPosition( );
	CVector4 CamPos( Pos[0], Pos[1], Pos[2] );

	// Loop Models In Node Leaf
	for ( int i = 0; i != cNode->NumModels( ); i++ )
	{

		// Skip Boxes
		if ( cTrack->ModelName( i ) == "0" )
			continue;

		// Get Model Index
		int Index = cNode->ModelIndex( i );

		// Scenery Complexity
		if ( cTrack->ModelPriority( Index ) >= cConfig->SceneryComplexity )
			continue;

		// Skip Drawing CheckPoint Box
		if ( cTrack->ModelTypeFlags( Index ) == "6,0" )
			continue;

		// Get Model Position
		CVector4 ModelPos = cTrack->ModelPosition( Index );

		// Get Model Rotation
		CVector4 Rotation = cTrack->ModelRotation( Index );

		// Reposition
		ModelPos.z = ModelPos.z / 32 + 8 + zOffset;
		ModelPos.y = ModelPos.y / 32;
		ModelPos.x = ModelPos.x / 32 + 8 + xOffset;

		// Setup Matrix
		glPushMatrix();

		// Translate Model Position
		glTranslated( ModelPos.z, ModelPos.y, ModelPos.x );

		// Billboard Flags
		if  ( cTrack->ModelTypeFlags( Index ) != "8,0" &&
			  cTrack->ModelTypeFlags( Index ) != "9,0" )
		{
			// Rotate Model Normally
			glRotated(270 - Rotation.z  * (180 / PI), 0, 1, 0);

		} else {

			// Swap X/Z
			double Theta;
			if ( ModelPos.x - CamPos.z == 0 )

				if ( ModelPos.z > CamPos.x )
					Theta = 0;
				else
					Theta = (float)( PI );
				else
				{
					Theta = atan( (ModelPos.z - CamPos.x) / (ModelPos.x - CamPos.z) );

					if ( ModelPos.x > CamPos.z )
						Theta = (float)( PI ) / 2.0f - Theta;
					else
						Theta = (float)( PI ) * 1.5f - Theta;

			};

			glRotated(270 - Theta  * (180 / PI), 0, 1, 0);

		}

		// Rotate
		glRotated( -Rotation.x * (180 / PI), 1, 0, 0 );
		glRotated( Rotation.y * (180 / PI), 0, 0, 1 );

		// Render Model
		glCallList( cTrack->cModels->Models[ cTrack->ModelIndex(Index) ].ModelList( ) );

		// PopMatrix
		glPopMatrix( );

	}


}

// Render SkyBox
void CRender::SkyBox( const dReal Pos[4] )
{

	// Push Matrix
	glPushMatrix( );

	// Translate Model Position
	glTranslated( Pos[0], Pos[1], Pos[2] );

	// Disable Depth Testing
	glDisable( GL_DEPTH_TEST );

	// Enable Blending
	glEnable( GL_BLEND );

	// Render SkyDome Display List
	glCallList( cTrack->cModels->cSkyDome.ModelList( ) );

    // ReScale Skybox
    glScalef(8000,8000,-8000);

	// Render SkyBox Display List
	glCallList( cTrack->cModels->cSkyBox.ModelList( ) );

	// Disable Blending
	glDisable( GL_BLEND );

	// Enable Depth Testing
	glEnable( GL_DEPTH_TEST );

	// Pop Matrix
	glPopMatrix( );

}


// Render Heads Up Display
void CRender::HeadsUpDisplay( )
{

    // Restore Display State
	glCallList( SaveState );

	// Save Display State
	glCallList( SaveState );

    // Enable States
    glEnable( GL_BLEND);
    glEnable( GL_TEXTURE_2D );
    glDisable( GL_ALPHA_TEST );

    // Bind Fading Texture
    glBindTexture( GL_TEXTURE_2D, Textures[3] );

    // Calculate Screen Ratio
    float W = ( Width / 100 * 40 );
    float H = ( Height / 100 * 40 );

    // Additional Ratio
    if ( Width < Height )
        W = W + ( Height / Width ) * 100;
    else
        H = H + ( Width / Height ) * 100;

    // GL Quads
    glBegin(GL_QUADS);

        // Set Alpha Color
        glColor4f( 1.0f, 1.0f, 1.0f, 1.0 );

        glTexCoord2f( 0, 0 );
        glVertex3f( 0, 0, 0 );

        glTexCoord2f( 0, 1 );
        glVertex3f( 0, H, 0 );

        glTexCoord2f( 1, 1 );
        glVertex3f( W, H, 0 );

        glTexCoord2f( 1, 0 );
        glVertex3f( W, 0, 0 );

    glEnd();

    // Enable States
    glDisable( GL_BLEND);
    glDisable( GL_TEXTURE_2D );
    glEnable( GL_ALPHA_TEST );

	// Y Position
	GLfloat PosY = 6;

	// Render Version Flag
    PosY += 16;
    cFont.RenderText( Window, Version, 1, 1, 1, 1, 10, Height - PosY );

	// Render Frames Per Second Flag
    PosY += 16;
    cFont.RenderText( Window, cTimer->Fps( ), 1, 1, 1, 1, 10, Height - PosY );

    // Save Display State
	glCallList( RestoreState );

}

// Create Font Display List States
bool CRender::BuildFontStates( )
{

    SaveState = glGenLists( 1 );
    RestoreState = glGenLists( 1 );

	// Save Mode
	glNewList( SaveState, GL_COMPILE );								// Start Display List
        glPushAttrib( GL_COLOR_BUFFER_BIT | GL_ENABLE_BIT );		// Push Attributes
		cWindowGL->ReshapeOrtho( Width, Height );				    // Reshape Ortho Projection
		glPushMatrix( );											// Push Matrix
        glLoadIdentity( );											// Reset Modelview Matrix
        glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );						// Full Color
        glEnable( GL_BLEND );										// Enable Blending
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );		// Blending Mode
        glDisable( GL_TEXTURE_2D );									// Disable Textures
		glDisable( GL_CULL_FACE );									// Disable Culling
    glEndList();													// End Display List

    // Restore Mode
    glNewList( RestoreState, GL_COMPILE );							// Start Display List
        glMatrixMode( GL_PROJECTION );								// Select Projection Matrix
        glPopMatrix( );												// Pop Matrix
        glMatrixMode( GL_MODELVIEW );								// Select Modelview Matrix
        glPopMatrix( );												// Pop Matrix
        glPopAttrib( );												// Pop Attributes
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );		// Blending Mode
		glEnable( GL_CULL_FACE );									// Enable Culling
		cWindowGL->ReshapePerspective( Width, Height,               // Reshape Perspective Projection
                                      cConfig->FieldOfVision, 0.1f,
                                      cConfig->ZDrawDistance );
	glEndList();

	return true;	// Return Ok

}

// Intro Scene
bool CRender::Intro( void )
{

	// Save Display State
	glCallList( SaveState );
    glEnable ( GL_TEXTURE_2D );

    Fade( 1000, 0 );
    Fade( 500, 1 );
    Fade( 1000, 2 );
    Fade( 500, 1 );

    // Restore Display State
	glCallList( RestoreState );

    return true;

}

// Fade Textures
void CRender::Fade( int Delay, int Texture )
{

    // Reset Timer (Intro)
    cTimer->Reset( );

    // Variables
    float Time = 0.0f;
    float Alpha = 0.0f;

    // Set GL States
    glDisable( GL_DEPTH_TEST );
    glEnable( GL_TEXTURE_2D );

    // Bind Fading Texture
    glBindTexture( GL_TEXTURE_2D, Textures[ Texture] );

    // Loop Delay
    for (;;)
    {

        // Increment Time
        Time += cTimer->FrameTimer();

        // GL Quads
        glBegin(GL_QUADS);

            // Set Alpha Color
            glColor4f( 1.0f, 1.0f, 1.0f, Alpha );

            glTexCoord2f( 0, 0 );
            glVertex3f( 0, 0, 1 );

            glTexCoord2f( 0, 1 );
            glVertex3f( 0, Height, 1 );

            glTexCoord2f( 1, 1 );
            glVertex3f( Width, Height, 1 );

            glTexCoord2f( 1, 0 );
            glVertex3f( Width, 0, 1 );

        glEnd();

        // Flush GL Pipeline
        glFlush();

        // Swap Double Buffer
        SwapBuffers( Window->hDC );

        // Calculate Alpha
        Alpha = ( Time / Delay );

        //Reached Alpha Break
        if ( Alpha > 1.0f )
        {
            break;
        }

    }

    // Set GL States
    glEnable( GL_DEPTH_TEST );

}

// Load Bitmap Texture File
bool CRender::LoadBitmap( string FileName, bool Alpha )
{
	HBITMAP hBMP;														// Handle Of The Bitmap
	BITMAP	BMP;

    Textures.resize( Textures.size( ) + 1 );                            // Resize Texture Vector
	glGenTextures(1, &Textures[Textures.size( )-1]);					// Create Texture

	hBMP=(HBITMAP)LoadImage(GetModuleHandle(NULL), FileName.c_str( ),   // Load Image
	IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE );

	if (!hBMP)															// Bitmap Exists
		return FALSE;													// Error, Return False

	GetObject(hBMP, sizeof(BMP), &BMP);									// Get The Object

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);								// Pixel Storage Mode

	glBindTexture(GL_TEXTURE_2D, Textures[Textures.size( )-1]);			// Bind Texture ID
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Min Filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// Linear Mag Filter

    // Alpha Channel
 	if ( Alpha == true )
 	{
 	    // BGRA Format
        glTexImage2D(GL_TEXTURE_2D, 0, 4, BMP.bmWidth, BMP.bmHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, BMP.bmBits);
 	}
    else
    {   // BGR Format
        glTexImage2D(GL_TEXTURE_2D, 0, 3, BMP.bmWidth, BMP.bmHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, BMP.bmBits);
    }

	DeleteObject(hBMP);													// Delete Object

	return TRUE;														// Load Successful

}



