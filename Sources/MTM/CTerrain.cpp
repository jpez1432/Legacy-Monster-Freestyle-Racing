
// Include
#include "CTerrain.h"                   // Terrain Class Header File

extern string RootDir;				    // Global Root Directory
extern float Tint[4];				    // Weather Tint

CTerrain::CTerrain( )
{


}

CTerrain::~CTerrain()
{

    SAFE_DELETE( Heightmap );
    SAFE_DELETE( Indices );
    SAFE_DELETE( Rotations );
    SAFE_DELETE( Lighting );

    // Delete List
    glDeleteLists( TerrainIds[0], TerrainIds.size( ) );

	// Empty Terrain Id Vector
	TerrainIds.empty( );

}

// Return Heightmap Value
float CTerrain::Height( int x, int z )
{

	// Wrap Values
	if (x == Dimensions) x = 0;
	if (z == Dimensions) z = 0;

	return (float)Heightmap[z+Dimensions*x] / 16.0f;

}

// Return Texture Index
int CTerrain::Index( int x, int z )
{

	// Wrap Values
	if (x == Dimensions) x = 0;
	if (z == Dimensions) z = 0;

	return (int)Indices[z+Dimensions*x] + (int)((Rotations[z+Dimensions*x] & 1 ) << 8);

}

// Return Texture Rotation
int CTerrain::Rotation( int x, int z )
{

	// Wrap Values
	if (x == Dimensions) x = 0;
	if (z == Dimensions) z = 0;

	return (int)Rotations[z+Dimensions*x];

}

// Return Terrain Light Value
float CTerrain::Light( int x, int z )
{

	// Wrap Values
	if (x == Dimensions) x = 0;
	if (z == Dimensions) z = 0;

	return (float)Lighting[z+Dimensions*x] / 256.0f;

}

// Load Heightmap
bool CTerrain::LoadHeightmap( string Filename, CPodIni *cPodIni )
{

	string Temp;				// Temp String
	ifstream Fs;				// Filestream

	// Find Pod File Information
	if ( cPodIni->FileInfo( "Data\\" + Filename, PodFile, Size, Offset ) == true )
	{

		PodFile = RootDir + PodFile;						// Build Heightmap Filename
		Fs.open( PodFile.c_str( ), ios::binary | ios::in );	// Open Pod File
		Fs.seekg( Offset );									// Seek Track File Offset
		Dimensions = (int)sqrt((double)Size);				// Calculate Square Heightmap Size

	} else {

		PodFile = RootDir + "Data\\" + Filename;			// Build Heightmap Filename
		Fs.open( PodFile.c_str( ), ios::binary | ios::in );	// Open Track File

		Fs.seekg( 0, ios::end );							// Seek End Of File
		Dimensions = Fs.tellg( );							// Get File Length
		Fs.seekg( 0, ios::beg );							// Seek Beginning Of File
		Dimensions = (int)sqrt((double) Dimensions);	// Calculate Square Heightmap Size

	}

    // Valid Heightmap
    if ( Fs.is_open( ) )
    {

        // Allocate Heightmap Memory
        Heightmap = new unsigned char[Dimensions * Dimensions];

        // Read Heightmap File
        Fs.read((char*)Heightmap, Dimensions * Dimensions );

        // Close Heightmap
        Fs.close( );

        return true;	// Return Ok

	}
	else
	{

        // Close Heightmap
        Fs.close( );

        return false;	// Return  Error

    }

}

// Load Texture Information
bool CTerrain::LoadTextureInfo( string Filename, CPodIni *cPodIni )
{

	string Temp;				// Temp String
	ifstream Fs;				// Filestream

	// Find Pod File Information
	if ( cPodIni->FileInfo( "Data\\" + Filename, PodFile, Size, Offset ) == true )
	{

		PodFile = RootDir + PodFile;						// Build Texture Info Filename
		Fs.open( PodFile.c_str( ), ios::binary | ios::in );	// Open Pod File
		Fs.seekg( Offset );									// Seek Track File Offset
		Dimensions = Size / 2;							// Calculate Difference
		Dimensions = (int)sqrt((double)Dimensions);				// Calculate Square Heightmap Size

	} else {

		PodFile = RootDir + "Data\\" + Filename;			// Build Texture Info Filename
		Fs.open( PodFile.c_str( ), ios::binary | ios::in );	// Open Track File

		Fs.seekg( 0, ios::end );							// Seek End Of File
		Dimensions = Fs.tellg( );							// Get File Length
		Fs.seekg( 0, ios::beg );							// Seek Beginning Of File
		Dimensions /= 2;									// Calculate Difference
		Dimensions = (int)sqrt((double) Dimensions);	// Calculate Square Heightmap Size

	}

    // Valid Texture Information File
    if ( Fs.is_open( ) )
    {

        // Allocate Lighting Memory
        Indices = new unsigned char[Dimensions * Dimensions];
        Rotations = new unsigned char[Dimensions * Dimensions];

        // Read Indices,Rotation Values
        for (int x = 0; x < Dimensions; x++)
        {
            for (int z = 0; z < Dimensions; z++)
            {
                Indices[z+Dimensions*x] = Fs.get();			// Read Index Value
                Rotations[z+Dimensions*x] = Fs.get();		// Read Rotation Value
            }
        }


        // Close Texture Information File
        Fs.close( );

        return true;	// Return Ok

	}
	else
	{

        // Close Texture Information File
        Fs.close( );

        return false;	// Return  Error

    }

}


// Load Lighting
bool CTerrain::LoadLightmap( string Filename, CPodIni *cPodIni )
{

	string Temp;				// Temp String
	ifstream Fs;				// Filestream

	// Find Pod File Information
	if ( cPodIni->FileInfo( "Data\\" + Filename, PodFile, Size, Offset ) == true )
	{

		PodFile = RootDir + PodFile;						// Build Lightmap Filename
		Fs.open( PodFile.c_str( ), ios::binary | ios::in );	// Open Pod File
		Fs.seekg( Offset );									// Seek Track File Offset
		Dimensions = Size / 7;							// Calculate Difference
		Dimensions = (int)sqrt((double)Dimensions);		// Calculate Square Heightmap Size

	} else {

		PodFile = RootDir + "Data\\" + Filename;			// Build Lightmap Filename
		Fs.open( PodFile.c_str( ), ios::binary | ios::in );	// Open Track File

		Fs.seekg( 0, ios::end );							// Seek End Of File
		Dimensions = Fs.tellg( );							// Get File Length
		Fs.seekg( 0, ios::beg );							// Seek Beginning Of File
		Dimensions /= 7;									// Calculate Difference
		Dimensions = (int)sqrt((double) Dimensions);	// Calculate Square Heightmap Size

	}

    // Valid Texture Information File
    if ( Fs.is_open( ) )
    {

        // Allocate Lighting Memory
        Lighting = new unsigned char[Dimensions * Dimensions];

        // Read Lighting Values
        for (int x = 0; x < Dimensions; x++)
        {
            for (int z = 0; z < Dimensions; z++)
            {
                Lighting[z+Dimensions*x] = Fs.get();	// Read Lighting Value
                Fs.seekg(6, ios::cur);						// Skip Boxes Lighting
            }
        }

        // Close Lighting File
        Fs.close( );

        return true;	// Return Ok

	}
	else
	{

        // Close Lighting File
        Fs.close( );

        return false;	// Return  Error

    }

}

// Build Terrain OpenGL Display Lists
bool CTerrain::BuildTerrainLists( CTextures *cTextures, CPodIni *cPodIni, float Tint[4] )
{

	// Variables
	int Rotate = 0;
	int Mir_x = 0;
	int Mir_y = 0;
	int Rot = 0;
	float Lights = 0;
	int xx = 0;
	int zz = 0;

	// Flag
	bool Unique = false;

	// Unique Texture List
	vector <int> Lists;

	// Terrain Lists
	TerrainIds.resize(256);

	// Loop Map Chunks
	for ( int x = 0; x != 16; x++ )
	{
		for ( int z = 0; z != 16; z++ )
		{

			// Terrain List
			TerrainIds[z + 16 * x] = glGenLists( 1 );
			glNewList( TerrainIds[z + 16 * x], GL_COMPILE );

			// Disable Blending
			glDisable(GL_BLEND);
			glDisable(GL_ALPHA_TEST);
			//glEnable(GL_CULL_FACE);

			// Loop Chunk Grids
			for ( int cx = x * 16; cx != x * 16 + 16; cx++ )
			{
				for ( int cz = z * 16; cz != z * 16 + 16; cz++ )
				{

					Unique = false;	// Default False

					// Loop List
					for ( unsigned int i = 0; i != Lists.size( ); i++ )
					{
						// Matches
						if ( Index( cx, cz ) == Lists[i] )
							Unique = true;	// Unqiue Index
					}

					// Matches,
					if ( Unique == false )
						Lists.push_back( Index( cx, cz ) );	// Push Back Index

				}
			}

			// Loop Unique Textures
			for ( unsigned int i = 0; i != Lists.size( ); i++ )
			{

				// Bind List OpenGL Texture
				glBindTexture( GL_TEXTURE_2D, cTextures->TextureId( Lists[i] ) );

				// Draw Match Texture Quads
				glBegin(GL_QUADS);

				// Loop Chunk Grid
				for ( float cx = 0; cx < 16; cx++ )
				{
					for ( float cz = 0; cz < 16; cz++ )
					{

						// Set Variables
						xx = int(x * 16 + cx);
						zz = int(z * 16 + cz);

						// Match Texture Index
						if ( Index( xx, zz ) == Lists[i] )
						{

							// Get Rotation Value
							Rotate = Rotation( xx, zz );
							Mir_x = (Rotate & 32) >> 5;	// Calculate Mirror X
							Mir_y = (Rotate & 16) >> 4;	// Calculate Mirror Y
							Rot = (Rotate & 192) >> 6;	// Calculate Rotation

							// Texture Coordinate, Vertex 1, Lighting Corner
							glTexCoord2f(cTextures->GetTexCoords( Mir_x,Mir_y,Rot,0,0 ),cTextures->GetTexCoords( Mir_x,Mir_y,Rot,0,1 ));
							Lights = Light( xx, zz );
							glColor3f(Lights * Tint[0], Lights * Tint[1], Lights * Tint[2]);
							glVertex3f( ( cx ), Height(xx, zz ), ( cz ) );

							// Texture Coordinate, Vertex 2, Lighting Corner
							glTexCoord2f(cTextures->GetTexCoords( Mir_x,Mir_y,Rot,1,0 ),cTextures->GetTexCoords( Mir_x,Mir_y,Rot,1,1 ));
							Lights = Light( xx + 1, zz );
							glColor3f(Lights * Tint[0], Lights * Tint[1], Lights * Tint[2]);
							glVertex3f( ( cx+1 ), Height( xx + 1, zz ), ( cz ) );

							// Texture Coordinate, Vertex 3, Lighting Corner
							glTexCoord2f(cTextures->GetTexCoords( Mir_x,Mir_y,Rot,2,0 ),cTextures->GetTexCoords( Mir_x,Mir_y,Rot,2,1 ));
							Lights = Light( xx + 1, zz + 1 );
							glColor3f(Lights * Tint[0], Lights * Tint[1], Lights * Tint[2]);
							glVertex3f( ( cx+1 ), Height( xx + 1, zz + 1 ), ( cz+1 ) );

							// Texture Coordinate, Vertex 4, Lighting Corner
							glTexCoord2f(cTextures->GetTexCoords( Mir_x,Mir_y,Rot,3,0 ),cTextures->GetTexCoords( Mir_x,Mir_y,Rot,3,1 ));
							Lights = Light( xx, zz + 1 );
							glColor3f(Lights * Tint[0], Lights * Tint[1], Lights * Tint[2]);
							glVertex3f( ( cx ), Height( xx, zz + 1 ), ( cz+1 ) );

						}
					}
				}

				// Quads Finished
				glEnd( );

				// Empty Unique
				Lists.empty( );

			}

			glEnable(GL_BLEND);
			glEnable(GL_ALPHA_TEST);
            glDisable(GL_CULL_FACE);

			// List Finished
			glEndList( );

		}
	}

	return true;	// Return Ok

}
