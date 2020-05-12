
// Include
#include "CGroundBoxes.h"                   // Ground Boxes Class Header File

extern string RootDir;				        // Global Root Directory
extern float Tint[4];				        // Weather Tint

// Constructor
CGroundBoxes::CGroundBoxes( )
{


}

// Destructor
CGroundBoxes::~CGroundBoxes( )
{

    SAFE_DELETE( BoxLower );
    SAFE_DELETE( BoxUpper );
    SAFE_DELETE( BoxTextures );

    // Delete OpenGL Display List
    glDeleteLists( BoxIds[0], BoxIds.size( ) );

    // Release Box Id Vector
    BoxIds.empty( );

}

// Return Box Lower Height
float CGroundBoxes::Lower( int x, int z )
{

	// Wrap Values
	if (x == Dimensions) x = 0;
	if (z == Dimensions) z = 0;

	return (float)BoxLower[z+Dimensions*x] / 16.0f;

}


// Return Box Upper Height
float CGroundBoxes::Upper( int x, int z )
{

	// Wrap Values
	if (x == Dimensions) x = 0;
	if (z == Dimensions) z = 0;

	return (float)BoxUpper[z+Dimensions*x] / 16.0f;

}


// Return Texture Index
int CGroundBoxes::Index( int x, int z, int side )
{

	// Wrap Values
	if (x == Dimensions) x = 0;
	if (z == Dimensions) z = 0;

	return (int)BoxTextures[z+Dimensions*x].Indices[side] + (int)((BoxTextures[z+Dimensions*x].Rotations[side] & 1 ) << 8);

}

// Return Texture Rotation
int CGroundBoxes::Rotation( int x, int z, int side )
{

	// Wrap Values
	if (x == Dimensions) x = 0;
	if (z == Dimensions) z = 0;

	return (int)BoxTextures[z+Dimensions*x].Rotations[side];

}

// Load Boxes Lower, Upper Heights
bool CGroundBoxes::LoadBoxHeights( string LowerFilename, string UpperFilename, CPodIni *cPodIni )
{

	string Temp;				// Temp String
	ifstream Fs;				// Filestream

	// Find Pod File Information
	if ( cPodIni->FileInfo( "Data\\" + LowerFilename, PodFile, Size, Offset ) == true )
	{

		PodFile = RootDir + PodFile;						// Build GroudBox Filename
		Fs.open( PodFile.c_str( ), ios::binary | ios::in );	// Open Pod File
		Fs.seekg( Offset );									// Seek Track File Offset
		Dimensions = (int)sqrt((double)Size);				// Calculate Square Heightmap Size

	} else {

		PodFile = RootDir + "Data\\" + LowerFilename;		// Build GroundBox Filename
		Fs.open( PodFile.c_str( ), ios::binary | ios::in );	// Open Track File

		Fs.seekg( 0, ios::end );							// Seek End Of File
		Dimensions = Fs.tellg( );							// Get File Length
		Fs.seekg( 0, ios::beg );							// Seek Beginning Of File
		Dimensions = (int)sqrt((double) Dimensions);	// Calculate Square Heightmap Size

	}

    // Valid Ground Box Lower File
    if ( Fs.is_open( ) )
    {

        // Allocate Box Lower Memory
        BoxLower = new unsigned char[Dimensions * Dimensions];

        // Read Box Lower File
        Fs.read((char*)BoxLower, Dimensions * Dimensions );

        // Close Ground Box Lower File
        Fs.close( );

	}
	else
	{

        // Close Ground Box Lower File
        Fs.close( );

        return false;	// Return  Error

    }


	// Find Pod File Information
	if ( cPodIni->FileInfo( "Data\\" + UpperFilename, PodFile, Size, Offset ) == true )
	{

		PodFile = RootDir + PodFile;						// Build GroudBox Filename
		Fs.open( PodFile.c_str( ), ios::binary | ios::in );	// Open Pod File
		Fs.seekg( Offset );									// Seek Track File Offset
		Dimensions = (int)sqrt((double)Size);				// Calculate Square Heightmap Size

	} else {

		PodFile = RootDir + "Data\\" + UpperFilename;		// Build GroundBox Filename
		Fs.open( PodFile.c_str( ), ios::binary | ios::in );	// Open Track File

		Fs.seekg( 0, ios::end );							// Seek End Of File
		Dimensions = Fs.tellg( );							// Get File Length
		Fs.seekg( 0, ios::beg );							// Seek Beginning Of File
		Dimensions = (int)sqrt((double) Dimensions);	// Calculate Square Heightmap Size

	}

    // Valid Ground Box Upper File
    if ( Fs.is_open( ) )
    {

        // Allocate Box Upper Memory
        BoxUpper= new unsigned char[Dimensions * Dimensions];

        // Read Box Lower File
        Fs.read((char*)BoxUpper, Dimensions * Dimensions );

        // Close Ground Box Upper File
        Fs.close( );

	}
	else
	{

        // Close Ground Box Upper File
        Fs.close( );

        return false;	// Return  Error

    }

    return true;    // Ok

}

// Load Boxes Texture Information
bool CGroundBoxes::LoadTextureInfo( string Filename, CPodIni *cPodIni )
{

	string Temp;				// Temp String
	ifstream Fs;				// Filestream

	// Find Pod File Information
	if ( cPodIni->FileInfo( "Data\\" + Filename, PodFile, Size, Offset ) == true )
	{

		PodFile = RootDir + PodFile;						// Build GroudBox Info Filename
		Fs.open( PodFile.c_str( ), ios::binary | ios::in );	// Open Pod File
		Fs.seekg( Offset );									// Seek Track File Offset
		Dimensions = Size / 12;							// Calculate Box/Sides Difference
		Dimensions = (int)sqrt((double)Dimensions);		// Calculate Square Texture Info Size

	} else {

		PodFile = RootDir + "Data\\" + Filename;				// Build GroundBox Filename
		Fs.open( PodFile.c_str( ), ios::binary | ios::in );		// Open Track File

		Fs.seekg( 0, ios::end );								// Seek End Of File
		Dimensions = Fs.tellg( );								// Get File Length
		Fs.seekg( 0, ios::beg );								// Seek Beginning Of File
		Dimensions /= 12;										// Calculate Texture Info Difference
		Dimensions = (int)sqrt((double)Dimensions);			// Calculate Square Texture Information Size

	}

    // Valid Ground Box Upper File
    if ( Fs.is_open( ) )
    {

        // Allocate Lighting Memory
        BoxTextures = new TextureInfo[Dimensions * Dimensions];

        // Read Indices, Rotation Values
        for (int x = 0; x != Dimensions; x++)
        {
            for (int z = 0; z != Dimensions; z++)
            {
                for (int s = 0; s != 6; s++)
                {
                    BoxTextures[z+Dimensions*x].Indices[s] = Fs.get();			// Read Index Value
                    BoxTextures[z+Dimensions*x].Rotations[s] = Fs.get();		// Read Rotation Value
                }
            }
        }

        // Close Texture Information File
        Fs.close( );

	}
	else
	{

        // Close Texture Information File
        Fs.close( );

        return false;	// Return  Error

    }

	return true;	// Return Ok

}

// Builds Box OpenGL Display Lists
bool CGroundBoxes::BuildGroundBoxLists( CTextures *cTextures, CTerrain *cTerrain, CPodIni *cPodIni, float Tint[4] )
{

	// Variables
	float Low = 0, High = 0;
	int Rotate = 0, Mir_x = 0, Mir_y = 0;
	int Rot = 0;
	float Lights = 0;
	int xx = 0, zz = 0;

	// Allocate Display List Memory
	BoxIds.resize( 256 );

	// Loop Box Chunks
	for ( int x = 0; x < 16; x++ )
	{
		for ( int z = 0; z < 16; z++ )
		{
			// Generate Chunk List, Compile
			BoxIds[z + 16 * x]=glGenLists( 1 );
			glNewList( BoxIds[z + 16 * x], GL_COMPILE );

			glDisable(GL_BLEND);
			glDisable(GL_ALPHA_TEST);
            //glEnable( GL_CULL_FACE );

			// Loop Chunk Grid
			for ( float cx = 0; cx < 16; cx++ )
			{
				for ( float cz = 0; cz < 16; cz++ )
				{

					// Set Variables
					xx = int(x * 16 + cx);
					zz = int(z * 16 + cz);

					// Get Upper, Lower Heights
					High = Upper( xx, zz );
					Low = Lower( xx, zz );

					// No Ground Box
					if ( High == 0 && Low == 0 )
						continue;

					// Bind List OpenGL Texture
					glBindTexture( GL_TEXTURE_2D, cTextures->TextureId( Index( xx, zz, 0 ) ) );

					// Draw Quads
					glBegin(GL_QUADS);

						// Get Rotation Value
						Rotate = Rotation( xx, zz, 0 );
						Mir_x = (Rotate & 32) >> 5;	// Calculate Mirror X
						Mir_y = (Rotate & 16) >> 4;	// Calculate Mirror Y
						Rot = (Rotate & 192) >> 6;	// Calculate Rotation

						// Texture Coordinate, Vertex 1, Lighting Corner
						Lights = cTerrain->GetLight( xx, zz );
						glColor3f(/* Lights * */Tint[0], /* Lights * */Tint[1], /* Lights * */Tint[2]);
						glTexCoord2f(cTextures->GetTexCoords( Mir_x,Mir_y,Rot,0,0 ),cTextures->GetTexCoords( Mir_x,Mir_y,Rot,0,1 ));
						glVertex3f( ( cx ), Low, ( cz ) );

						// Texture Coordinate, Vertex 2, Lighting Corner
						Lights = cTerrain->GetLight( xx, zz );
						glColor3f(/* Lights * */Tint[0], /* Lights * */Tint[1], /* Lights * */Tint[2]);
						glTexCoord2f(cTextures->GetTexCoords( Mir_x,Mir_y,Rot,1,0 ),cTextures->GetTexCoords( Mir_x,Mir_y,Rot,1,1 ));
						glVertex3f( ( cx ), High, ( cz) );

						// Texture Coordinate, Vertex 3, Lighting Corner
						Lights = cTerrain->GetLight( xx, zz+1 );
						glColor3f(/* Lights * */Tint[0], /* Lights * */Tint[1], /* Lights * */Tint[2]);
						glTexCoord2f(cTextures->GetTexCoords( Mir_x,Mir_y,Rot,2,0 ),cTextures->GetTexCoords( Mir_x,Mir_y,Rot,2,1 ));
						glVertex3f( ( cx ), High, ( cz+1 ) );

						// Texture Coordinate, Vertex 4, Lighting Corner
						Lights = cTerrain->GetLight( xx, zz+1 );
						glColor3f(/* Lights * */Tint[0], /* Lights * */Tint[1], /* Lights * */Tint[2]);
						glTexCoord2f(cTextures->GetTexCoords( Mir_x,Mir_y,Rot,3,0 ),cTextures->GetTexCoords( Mir_x,Mir_y,Rot,3,1 ));
						glVertex3f( ( cx ), Low, ( cz+1 ) );

					// Quads Finished
					glEnd( );

					// Bind List OpenGL Texture
					glBindTexture( GL_TEXTURE_2D, cTextures->TextureId( Index( xx, zz, 1 ) ) );

					// Draw Quads
					glBegin(GL_QUADS);

						// Get Rotation Value
						Rotate = Rotation( xx, zz, 1 );
						Mir_x = (Rotate & 32) >> 5;	// Calculate Mirror X
						Mir_y = (Rotate & 16) >> 4;	// Calculate Mirror Y
						Rot = (Rotate & 192) >> 6;	// Calculate Rotation

						// Texture Coordinate, Vertex 1, Lighting Corner
						Lights = cTerrain->GetLight( xx+1, zz+1 );
						glColor3f(/* Lights * */Tint[0], /* Lights * */Tint[1], /* Lights * */Tint[2]);
						glTexCoord2f(cTextures->GetTexCoords( Mir_x,Mir_y,Rot,0,0 ),cTextures->GetTexCoords( Mir_x,Mir_y,Rot,0,1 ));
						glVertex3f( ( cx +1 ), Low, ( cz +1 ) );

						// Texture Coordinate, Vertex 2, Lighting Corner
						Lights = cTerrain->GetLight( xx+1, zz+1 );
						glColor3f(/* Lights * */Tint[0], /* Lights * */Tint[1], /* Lights * */Tint[2]);
						glTexCoord2f(cTextures->GetTexCoords( Mir_x,Mir_y,Rot,1,0 ),cTextures->GetTexCoords( Mir_x,Mir_y,Rot,1,1 ));
						glVertex3f( ( cx +1 ), High, ( cz +1) );

						// Texture Coordinate, Vertex 3, Lighting Corner
						Lights = cTerrain->GetLight( xx+1, zz );
						glColor3f(/* Lights * */Tint[0], /* Lights * */Tint[1], /* Lights * */Tint[2]);
						glTexCoord2f(cTextures->GetTexCoords( Mir_x,Mir_y,Rot,2,0 ),cTextures->GetTexCoords( Mir_x,Mir_y,Rot,2,1 ));
						glVertex3f( ( cx +1), High, ( cz ) );

						// Texture Coordinate, Vertex 4, Lighting Corner
						Lights = cTerrain->GetLight( xx+1, zz );
						glColor3f(/* Lights * */Tint[0], /* Lights * */Tint[1], /* Lights * */Tint[2]);
						glTexCoord2f(cTextures->GetTexCoords( Mir_x,Mir_y,Rot,3,0 ),cTextures->GetTexCoords( Mir_x,Mir_y,Rot,3,1 ));
						glVertex3f( ( cx +1 ), Low, ( cz) );

					// Quads Finished
					glEnd( );

					// Bind List OpenGL Texture
					glBindTexture( GL_TEXTURE_2D, cTextures->TextureId( Index( xx, zz, 2 ) ) );

					// Draw Quads
					glBegin(GL_QUADS);

						// Get Rotation Value
						Rotate = Rotation( xx, zz, 2 );
						Mir_x = (Rotate & 32) >> 5;	// Calculate Mirror X
						Mir_y = (Rotate & 16) >> 4;	// Calculate Mirror Y
						Rot = (Rotate & 192) >> 6;	// Calculate Rotation

						// Texture Coordinate, Vertex 1, Lighting Corner
						Lights = cTerrain->GetLight( xx, zz +1);
						glColor3f(/* Lights * */Tint[0], /* Lights * */Tint[1], /* Lights * */Tint[2]);
						glTexCoord2f(cTextures->GetTexCoords( Mir_x,Mir_y,Rot,0,0 ),cTextures->GetTexCoords( Mir_x,Mir_y,Rot,0,1 ));
						glVertex3f( ( cx ), Low, ( cz + 1 ) );

						// Texture Coordinate, Vertex 2, Lighting Corner
						Lights = cTerrain->GetLight( xx, zz+1 );
						glColor3f(/* Lights * */Tint[0], /* Lights * */Tint[1], /* Lights * */Tint[2]);
						glTexCoord2f(cTextures->GetTexCoords( Mir_x,Mir_y,Rot,1,0 ),cTextures->GetTexCoords( Mir_x,Mir_y,Rot,1,1 ));
						glVertex3f( ( cx ), High, ( cz +1) );

						// Texture Coordinate, Vertex 3, Lighting Corner
						Lights = cTerrain->GetLight( xx+1, zz+1);
						glColor3f(/* Lights * */Tint[0], /* Lights * */Tint[1], /* Lights * */Tint[2]);
						glTexCoord2f(cTextures->GetTexCoords( Mir_x,Mir_y,Rot,2,0 ),cTextures->GetTexCoords( Mir_x,Mir_y,Rot,2,1 ));
						glVertex3f( ( cx+1 ), High, ( cz+1 ) );

						// Texture Coordinate, Vertex 4, Lighting Corner
						Lights = cTerrain->GetLight( xx+1, zz+1 );
						glColor3f(/* Lights * */Tint[0], /* Lights * */Tint[1], /* Lights * */Tint[2]);
						glTexCoord2f(cTextures->GetTexCoords( Mir_x,Mir_y,Rot,3,0 ),cTextures->GetTexCoords( Mir_x,Mir_y,Rot,3,1 ));
						glVertex3f( ( cx+1 ), Low, ( cz+1 ) );

					// Quads Finished
					glEnd( );

					// Bind List OpenGL Texture
					glBindTexture( GL_TEXTURE_2D, cTextures->TextureId( Index( xx, zz, 3 ) ) );

					// Draw Quads
					glBegin(GL_QUADS);

						// Get Rotation Value
						Rotate = Rotation( xx, zz, 3 );
						Mir_x = (Rotate & 32) >> 5;	// Calculate Mirror X
						Mir_y = (Rotate & 16) >> 4;	// Calculate Mirror Y
						Rot = (Rotate & 192) >> 6;	// Calculate Rotation

						// Texture Coordinate, Vertex 1, Lighting Corner
						Lights = cTerrain->GetLight( xx+1, zz );
						glColor3f(/* Lights * */Tint[0], /* Lights * */Tint[1], /* Lights * */Tint[2]);
						glTexCoord2f(cTextures->GetTexCoords( Mir_x,Mir_y,Rot,0,0 ),cTextures->GetTexCoords( Mir_x,Mir_y,Rot,0,1 ));
						glVertex3f( ( cx + 1 ), Low, ( cz ) );

						// Texture Coordinate, Vertex 2, Lighting Corner
						Lights = cTerrain->GetLight( xx+1, zz);
						glColor3f(/* Lights * */Tint[0], /* Lights * */Tint[1], /* Lights * */Tint[2]);
						glTexCoord2f(cTextures->GetTexCoords( Mir_x,Mir_y,Rot,1,0 ),cTextures->GetTexCoords( Mir_x,Mir_y,Rot,1,1 ));
						glVertex3f( ( cx+1 ), High, ( cz ) );

						// Texture Coordinate, Vertex 3, Lighting Corner
						Lights = cTerrain->GetLight( xx, zz );
						glColor3f(/* Lights * */Tint[0], /* Lights * */Tint[1], /* Lights * */Tint[2]);
						glTexCoord2f(cTextures->GetTexCoords( Mir_x,Mir_y,Rot,2,0 ),cTextures->GetTexCoords( Mir_x,Mir_y,Rot,2,1 ));
						glVertex3f( ( cx ), High, ( cz ) );

						// Texture Coordinate, Vertex 4, Lighting Corner
						Lights = cTerrain->GetLight( xx, zz );
						glColor3f(/* Lights * */Tint[0], /* Lights * */Tint[1], /* Lights * */Tint[2]);
						glTexCoord2f(cTextures->GetTexCoords( Mir_x,Mir_y,Rot,3,0 ),cTextures->GetTexCoords( Mir_x,Mir_y,Rot,3,1 ));
						glVertex3f( ( cx ), Low, ( cz ) );

					// Quads Finished
					glEnd( );

					// Bind List OpenGL Texture
					glBindTexture( GL_TEXTURE_2D, cTextures->TextureId( Index( xx, zz, 4 ) ) );

					// Draw Quads
					glBegin(GL_QUADS);

						// Get Rotation Value
						Rotate = Rotation( xx, zz, 4 );
						Mir_x = (Rotate & 32) >> 5;	// Calculate Mirror X
						Mir_y = (Rotate & 16) >> 4;	// Calculate Mirror Y
						Rot = (Rotate & 192) >> 6;	// Calculate Rotation

						// Texture Coordinate, Vertex 1, Lighting Corner
						Lights = cTerrain->GetLight( xx, zz );
						glColor3f(/* Lights * */Tint[0], /* Lights * */Tint[1], /* Lights * */Tint[2]);
						glTexCoord2f(cTextures->GetTexCoords( Mir_x,Mir_y,Rot,0,0 ),cTextures->GetTexCoords( Mir_x,Mir_y,Rot,0,1 ));
						glVertex3f( ( cx ), High, ( cz ) );

						// Texture Coordinate, Vertex 2, Lighting Corner
						Lights = cTerrain->GetLight( xx +1, zz );
						glColor3f(/* Lights * */Tint[0], /* Lights * */Tint[1], /* Lights * */Tint[2]);
						glTexCoord2f(cTextures->GetTexCoords( Mir_x,Mir_y,Rot,1,0 ),cTextures->GetTexCoords( Mir_x,Mir_y,Rot,1,1 ));
						glVertex3f( ( cx+1 ), High, ( cz ) );

						// Texture Coordinate, Vertex 3, Lighting Corner
						Lights = cTerrain->GetLight( xx+1, zz+1 );
						glColor3f(/* Lights * */Tint[0], /* Lights * */Tint[1], /* Lights * */Tint[2]);
						glTexCoord2f(cTextures->GetTexCoords( Mir_x,Mir_y,Rot,2,0 ),cTextures->GetTexCoords( Mir_x,Mir_y,Rot,2,1 ));
						glVertex3f( ( cx+1 ), High, ( cz+1 ) );

						// Texture Coordinate, Vertex 4, Lighting Corner
						Lights = cTerrain->GetLight( xx, zz+1 );
						glColor3f(/* Lights * */Tint[0], /* Lights * */Tint[1], /* Lights * */Tint[2]);
						glTexCoord2f(cTextures->GetTexCoords( Mir_x,Mir_y,Rot,3,0 ),cTextures->GetTexCoords( Mir_x,Mir_y,Rot,3,1 ));
						glVertex3f( ( cx ), High, ( cz+1 ) );

					// Quads Finished
					glEnd( );

					// Bind List OpenGL Texture
					glBindTexture( GL_TEXTURE_2D, cTextures->TextureId( Index( xx, zz, 5 ) ) );

					// Draw Quads
					glBegin(GL_QUADS);

						// Get Rotation Value
						Rotate = Rotation( xx, zz, 5 );
						Mir_x = (Rotate & 32) >> 5;	// Calculate Mirror X
						Mir_y = (Rotate & 16) >> 4;	// Calculate Mirror Y
						Rot = (Rotate & 192) >> 6;	// Calculate Rotation

						// Texture Coordinate, Vertex 1, Lighting Corner
						Lights = cTerrain->GetLight( xx, zz );
						glColor3f(/* Lights * */Tint[0], /* Lights * */Tint[1], /* Lights * */Tint[2]);
						glTexCoord2f(cTextures->GetTexCoords( Mir_x,Mir_y,Rot,0,0 ),cTextures->GetTexCoords( Mir_x,Mir_y,Rot,0,1 ));
						glVertex3f( ( cx ), Low, ( cz ) );

						// Texture Coordinate, Vertex 2, Lighting Corner
						Lights = cTerrain->GetLight( xx+1, zz );
						glColor3f(/* Lights * */Tint[0], /* Lights * */Tint[1], /* Lights * */Tint[2]);
						glTexCoord2f(cTextures->GetTexCoords( Mir_x,Mir_y,Rot,3,0 ),cTextures->GetTexCoords( Mir_x,Mir_y,Rot,3,1 ));
						glVertex3f( ( cx ), Low, ( cz+1 ) );

						// Texture Coordinate, Vertex 3, Lighting Corner
						Lights = cTerrain->GetLight( xx+1, zz+1 );
						glColor3f(/* Lights * */Tint[0], /* Lights * */Tint[1], /* Lights * */Tint[2]);
						glTexCoord2f(cTextures->GetTexCoords( Mir_x,Mir_y,Rot,2,0 ),cTextures->GetTexCoords( Mir_x,Mir_y,Rot,2,1 ));
						glVertex3f( ( cx+1 ), Low, ( cz+1 ) );

						// Texture Coordinate, Vertex 4, Lighting Corner
						Lights = cTerrain->GetLight( xx+1, zz+1 );
						glColor3f(/* Lights * */Tint[0], /* Lights * */Tint[1], /* Lights * */Tint[2]);
						glTexCoord2f(cTextures->GetTexCoords( Mir_x,Mir_y,Rot,1,0 ),cTextures->GetTexCoords( Mir_x,Mir_y,Rot,1,1 ));
						glVertex3f( ( cx+1 ), Low, ( cz ) );

					// Quads Finished
					glEnd( );

				}
			}

			glEnable(GL_BLEND);
			glEnable(GL_ALPHA_TEST);
            glDisable( GL_CULL_FACE );

			// List Finished
			glEndList();

		}
	}

	return true;

}



