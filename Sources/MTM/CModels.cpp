
// Include
#include "CModels.h"                    // Models Class Header File

extern string RootDir;					// Global Root Directory

GLuint EnvMap;                          // Enviroment Map Texture

// Constructor
CBin::CBin( )
{


}

// Destructor
CBin::~CBin( )
{

    // Empty Vectors
    Vertices.empty();					// Bin Vertices
    Normals.empty();					// Bin Normal Values
    Textures.empty();					// Textures Blocks
    AnimTextures.empty();				// Animated Texture Blocks
    Colors.empty();						// Color Blocks
    Faces.empty();						// Faces Blocks
    SpecialColors.empty();				// Special Color Blocks


    // Delete OpenGL Display List
    glDeleteLists( ModelId, 1 );

	// OpenGL Delete Textures
    glDeleteTextures( Textures.size( ), &TextureIds[0] );


}

// Destructor
CModels::~CModels( )
{

	// Empty Models Vector
	Models.empty( );
    AnimModels.empty( );

    // Delete OpenGL Water Display List
    glDeleteLists( WaterList, 1 );


}

// Load Bin Model
bool CBin::LoadBin( string ModelFile, CPodIni *cPodIni, float Tint[4] )
{

	int BlockId = -1;			// Block Id
	string Temp;				// Temp String
	ifstream Fs;				// Filestream

	// Find Pod File Information
	if ( cPodIni->FileInfo( "Models\\" + ModelFile, PodFile, Size, Offset ) == true )
	{

		PodFile = RootDir + PodFile;						// Build GroudBox Filename
		Fs.open( PodFile.c_str( ), ios::binary | ios::in );	// Open Pod File
		Fs.seekg( Offset );									// Seek Track File Offset

	} else {

		PodFile = RootDir + "Models\\" + ModelFile;			// Build GroundBox Filename
		Fs.open( PodFile.c_str( ), ios::binary | ios::in );	// Open Track File

	}

	// Validate
	if ( !Fs.good( ) )
		return false;				// Error Occured

	Fs.read( (char*)&BinId, sizeof(BinId) );		// Read Bin Id Number

	if ( BinId == 0x20 )
	{
		Fs.read( (char*)&BlockId, sizeof(int) );
		Fs.read( (char*)&BlockId, sizeof(int) );
		Fs.read( (char*)&BlockId, sizeof(int) );
		Fs.read( (char*)&BlockId, sizeof(int) );
		Fs.read( (char*)&BlockId, sizeof(int) );

		char Buffer[16];
		Fs.read( (char*)&Buffer, 16 );
		LoadBin( Buffer, cPodIni, Tint );
	}


	Fs.read( (char*)&Header, sizeof(Header) );		// Read Bin Header

	Vertices.resize( Header.NumVerts );
	Fs.read( (char*)&Vertices[0], sizeof(Vertex) * Header.NumVerts );		// Read Vertex

    FaceBlock Split, TriOne, TriTwo;

	// Loop Bin End
	while (BlockId != 0 && !Fs.eof() )
	{

		// Read Next Block Id
		Fs.read( (char*)&BlockId,sizeof(BlockId) );

		// Switch BlockId
		switch (BlockId)
		{
			case 0x0:	// End Of File
				break;

			case 0x0E:
			case 0x11:
			case 0x18:
			case 0x29:
			case 0x33:
			case 0x34:


                Split.FaceType = BlockId;
                Split.TextureIndex = Textures.size( )-1;

                Fs.read((char*)&Split.Header, sizeof( Split.Header ) );

                Split.Vertices.resize( Split.Header.NumVerts );
                Fs.read((char*)&Split.Vertices[0], sizeof( Split.Vertices[0] ) * Split.Header.NumVerts );

                if ( Split.Header.NumVerts == 3 )
                {
                    Faces.push_back( Split );
                }
                else
                {
                    TriOne.FaceType = Split.FaceType;
                    TriOne.TextureIndex = Split.TextureIndex;
                    TriOne.Header = Split.Header;
                    TriOne.Header.NumVerts = 3;
                    TriOne.Vertices.resize(3);
                    TriOne.Vertices[0] = Split.Vertices[0];
                    TriOne.Vertices[1] = Split.Vertices[1];
                    TriOne.Vertices[2] = Split.Vertices[2];
                    Faces.push_back( TriOne );

                    TriTwo.FaceType = Split.FaceType;
                    TriTwo.TextureIndex = Split.TextureIndex;
                    TriTwo.Header = Split.Header;
                    TriTwo.Header.NumVerts = 3;
                    TriTwo.Vertices.resize(3);
                    TriTwo.Vertices[0] = Split.Vertices[2];
                    TriTwo.Vertices[1] = Split.Vertices[3];
                    TriTwo.Vertices[2] = Split.Vertices[0];
                    Faces.push_back( TriTwo );

                }
                break;

			case 0x03:	// Vertex Normal Block

				Normals.resize( Header.NumVerts );
				Fs.read( (char*)&Normals[0], sizeof(Normal) * Header.NumVerts );		// Read Vertex
				Fs.seekg( 8, ios::cur );	// Skip Unknown
				break;

			case 0x0D:	// Texture Block

				Textures.resize( Textures.size( ) +1 );
				Fs.read( (char*)&Textures[ Textures.size( )-1 ], sizeof( Textures[Textures.size( )-1] ) );

				break;

			case 0x17:	// HellBender Unknown

				Fs.seekg( 8, ios::cur );	// Skip Unknown
				break;


			case 0x1D: // Animated Texture Block
				{

					AnimTextureBlock Anim;
					AnimTexFilenames Name;

					Fs.read( (char*)&Anim.Header, sizeof(Anim.Header) );	// Read Anim Texture Header

					for ( int i = 0; i < Anim.Header.NumTextures; i++ )
						Fs.read( (char*)&Name.TextureNames, sizeof(Name.TextureNames) );// Read Animated Names

					Anim.Names.push_back( Name );							// Push Back Names
					AnimTextures.push_back(Anim);							// Push Back Animate Textures

					break;
				}
			case 0x0A:


				Colors.resize( Colors.size( ) + 1 );
				Fs.read( (char*)&Colors[Colors.size( )-1 ], sizeof( Colors[0] ) );

				break;

			case 0x19:	// Special Face 19
				{

				    SpecialColors.resize( SpecialColors.size( ) + 1 );
				    SpecialColors[ SpecialColors.size( )-1 ].ColorIndex = Colors.size( ) - 1;

				    Fs.read( (char*)& SpecialColors[ SpecialColors.size( )-1 ].NumVerts, sizeof(int)*5 );

				    SpecialColors[ SpecialColors.size( ) -1].Verts.resize( SpecialColors[SpecialColors.size( )-1 ].NumVerts );
				    Fs.read( (char*)& SpecialColors[ SpecialColors.size( )-1 ].Verts[0], sizeof( int ) * SpecialColors[ SpecialColors.size( )-1 ].NumVerts );

				}
				break;

//			default:
//
//                FaceBlock Split;
//
//                Split.FaceType = BlockId;
//                Split.TextureIndex = Textures.size( )-1;
//
//                Fs.read((char*)&Split.Header, sizeof( Split.Header ) );
//
//                Split.Vertices.resize( Split.Header.NumVerts );
//                Fs.read((char*)&Split.Vertices[0], sizeof( Split.Vertices[0] ) * Split.Header.NumVerts );
//
//                FaceBlock TriOne, TriTwo;
//
//                if ( Split.Header.NumVerts == 3 )
//                {
//                    Faces.push_back( Split );
//                }
//                else
//                {
//                    TriOne.FaceType = Split.FaceType;
//                    TriOne.TextureIndex = Split.TextureIndex;
//                    TriOne.Header = Split.Header;
//                    TriOne.Header.NumVerts = 3;
//                    TriOne.Vertices.resize(3);
//                    TriOne.Vertices[0] = Split.Vertices[0];
//                    TriOne.Vertices[1] = Split.Vertices[1];
//                    TriOne.Vertices[2] = Split.Vertices[2];
//                    Faces.push_back( TriOne );
//
//                    TriTwo.FaceType = Split.FaceType;
//                    TriTwo.TextureIndex = Split.TextureIndex;
//                    TriTwo.Header = Split.Header;
//                    TriTwo.Header.NumVerts = 3;
//                    TriTwo.Vertices.resize(3);
//                    TriTwo.Vertices[0] = Split.Vertices[2];
//                    TriTwo.Vertices[1] = Split.Vertices[3];
//                    TriTwo.Vertices[2] = Split.Vertices[0];
//                    Faces.push_back( TriTwo );
//
//                }
//                Faces.resize( Faces.size( ) + 1 );
//                Faces[ Faces.size( ) - 1].FaceType = BlockId;
//                Faces[ Faces.size( ) - 1].TextureIndex = Textures.size( )-1;
//
//                Fs.read((char*)&Faces[ Faces.size( )-1].Header, sizeof( Faces[Faces.size( )-1 ].Header ) );
//
//                Faces[ Faces.size( )-1].Vertices.resize( Faces[Faces.size( )-1].Header.NumVerts );
//                Fs.read((char*)&Faces[ Faces.size( )-1].Vertices[0], sizeof( Faces[Faces.size( )-1].Vertices[0] ) * Faces[Faces.size( )-1].Header.NumVerts );



				break;

		}
	}

	// Close Bin Model File
	Fs.close( );

	// Clear Texture Ids
	TextureIds.resize( Textures.size() );

	// Linear Filter
	cTextures.SetFilter(2);

	// Load Default Palette
	cTextures.LoadMetalCr2( cPodIni );

	// Loop Textures
	for ( int i = 0; i < (int)Textures.size(); i++)
	{
		// Load Texture, Set GL Texture Id
		TextureIds[i] = cTextures.LoadRawFile( Textures[i].TextureName, cPodIni );
	}

	// Build Display List
	if ( !BuildDisplayList( Tint ) )
		return false;	// Error Occured

	return true;

}


// Build OpenGL Display List
bool CBin::BuildDisplayList( float Tint[4] )
{

	ModelId=glGenLists( 1 );			// Generate Model List
	glNewList( ModelId, GL_COMPILE );	// New Model List
	{

		// Enable Texture Mapping
		glEnable(GL_TEXTURE_2D);
        // Enable Culling
        glEnable( GL_CULL_FACE );
        // Colorize
        glColor3f( Tint[0], Tint[1], Tint[2] );
        // Scale
        glScalef(0.000125,0.000125,-0.000125);

		// Loop Textures
		for ( int  t = 0; t < (int)Textures.size(); t++)
		{

			// Bind Bin Texture
			glBindTexture(GL_TEXTURE_2D, TextureIds[t]);

			// Disable Blending
			glDisable(GL_BLEND);
			glDisable(GL_ALPHA_TEST);

            // Triangles
            glBegin(GL_TRIANGLES);

			// Loop Faces
			for (int f = 0; f < (int)Faces.size(); f++)
			{

				// Non-Transparent Faces
				if ( ( Faces[f].FaceType == 0x34 ) ||
					 ( Faces[f].FaceType == 0x18 ) ||
					 ( Faces[f].FaceType == 0x29 ) )
				{

					// Match Texture Index
					if (Faces[f].TextureIndex == t )
					{

                        // Texture Coordinates
                        glTexCoord2f( (GLfloat)Faces[f].Vertices[0].TexCoordU / 16777216,
                                      (GLfloat)Faces[f].Vertices[0].TexCoordV / 16777216 );


                        // Vertex Coordinates
                        glVertex3f( (GLfloat)Vertices[Faces[f].Vertices[0].VertIndex].X,
                                    (GLfloat)Vertices[Faces[f].Vertices[0].VertIndex].Y,
                                    (GLfloat)Vertices[Faces[f].Vertices[0].VertIndex].Z);


                        // Texture Coordinates
                        glTexCoord2f( (GLfloat)Faces[f].Vertices[1].TexCoordU / 16777216,
                                      (GLfloat)Faces[f].Vertices[1].TexCoordV / 16777216 );


                        // Vertex Coordinates
                        glVertex3f( (GLfloat)Vertices[Faces[f].Vertices[1].VertIndex].X,
                                    (GLfloat)Vertices[Faces[f].Vertices[1].VertIndex].Y,
                                    (GLfloat)Vertices[Faces[f].Vertices[1].VertIndex].Z);


                        // Texture Coordinates
                        glTexCoord2f( (GLfloat)Faces[f].Vertices[2].TexCoordU / 16777216,
                                      (GLfloat)Faces[f].Vertices[2].TexCoordV / 16777216 );


                        // Vertex Coordinates
                        glVertex3f( (GLfloat)Vertices[Faces[f].Vertices[2].VertIndex].X,
                                    (GLfloat)Vertices[Faces[f].Vertices[2].VertIndex].Y,
                                    (GLfloat)Vertices[Faces[f].Vertices[2].VertIndex].Z);

					}
				}
			}

            glEnd();

			// Enable Blending
			glEnable(GL_BLEND);
			glEnable(GL_ALPHA_TEST);

            // Triangles
            glBegin(GL_TRIANGLES);

			// Loop Faces
			for ( int f = 0; f < (int)Faces.size(); f++)
			{

				// Transparent Faces
				if ( ( Faces[f].FaceType == 0x11 ) ||
					 ( Faces[f].FaceType == 0x33 ) )
				{

					// Match Texture Index
					if (Faces[f].TextureIndex == t )
					{

                        // Texture Coordinates
                        glTexCoord2f( (GLfloat)Faces[f].Vertices[0].TexCoordU / 16777216,
                                      (GLfloat)Faces[f].Vertices[0].TexCoordV / 16777216 );


                        // Vertex Coordinates
                        glVertex3f( (GLfloat)Vertices[Faces[f].Vertices[0].VertIndex].X,
                                    (GLfloat)Vertices[Faces[f].Vertices[0].VertIndex].Y,
                                    (GLfloat)Vertices[Faces[f].Vertices[0].VertIndex].Z);


                        // Texture Coordinates
                        glTexCoord2f( (GLfloat)Faces[f].Vertices[1].TexCoordU / 16777216,
                                      (GLfloat)Faces[f].Vertices[1].TexCoordV / 16777216 );


                        // Vertex Coordinates
                        glVertex3f( (GLfloat)Vertices[Faces[f].Vertices[1].VertIndex].X,
                                    (GLfloat)Vertices[Faces[f].Vertices[1].VertIndex].Y,
                                    (GLfloat)Vertices[Faces[f].Vertices[1].VertIndex].Z);


                        // Texture Coordinates
                        glTexCoord2f( (GLfloat)Faces[f].Vertices[2].TexCoordU / 16777216,
                                      (GLfloat)Faces[f].Vertices[2].TexCoordV / 16777216 );


                        // Vertex Coordinates
                        glVertex3f( (GLfloat)Vertices[Faces[f].Vertices[2].VertIndex].X,
                                    (GLfloat)Vertices[Faces[f].Vertices[2].VertIndex].Y,
                                    (GLfloat)Vertices[Faces[f].Vertices[2].VertIndex].Z);

					}
				}
			}

            glEnd( );

		}



		// Color Blocks
		glDisable(GL_TEXTURE_2D);

		// Loop Faces
		for ( int f = 0; f < (int)SpecialColors.size(); f++)
		{

			// Number Of Vertices
			if (SpecialColors[f].NumVerts == 3)
			{
				// Triangles
				glBegin(GL_TRIANGLES);
			}
			else // Quads
			{
				glBegin(GL_QUADS);
			}

			// Winding Order
			for ( int v = 0; v < SpecialColors[f].NumVerts; v++)
			{

				// Colorize
				glColor3ub( Colors[SpecialColors[f].ColorIndex].Red * 255 * (unsigned char)Tint[0],
						   Colors[SpecialColors[f].ColorIndex].Green * 255 * (unsigned char)Tint[1],
						   Colors[SpecialColors[f].ColorIndex].Blue * 255 * (unsigned char)Tint[2] );

				// Vertex Coordinates
				glVertex3f( (GLfloat)Vertices[SpecialColors[f].Verts[v]].X,
							(GLfloat)Vertices[SpecialColors[f].Verts[v]].Y,
							(GLfloat)Vertices[SpecialColors[f].Verts[v]].Z);
			}

			// End
			glEnd();
		}

		// Enable Texture Mapping
		glEnable(GL_TEXTURE_2D);
		// Enable Culling
		glDisable( GL_CULL_FACE );

		// End List
		glEndList();

	}

	return true;

}

// Calculate Furthest Dimensions
void CBin::Dimensions( CVector4 &Min, CVector4 &Max )
{

	// Loop Vertices
	for ( unsigned int v = 0; v != Vertices.size(); v++ )
	{
		// Less Than X
		if ( Vertices[v].X < Min.x ) Min.x = (float)Vertices[v].X;
		// Less Than Y
		if ( Vertices[v].Y < Min.y ) Min.y = (float)Vertices[v].Y;
		// Less Than Z
		if ( Vertices[v].Z < Min.z ) Min.z = (float)Vertices[v].Z;

		// Greater Than X
		if ( Vertices[v].X > Max.x ) Max.x = (float)Vertices[v].X;
		// Greater Than Y
		if ( Vertices[v].Y > Max.y ) Max.y = (float)Vertices[v].Y;
		// Greater Than Z
		if ( Vertices[v].Z > Max.z ) Max.z = (float)Vertices[v].Z;
	}

	Min = Min / 4096;
	Max = Max / 4096;

}


// Loads Unique List Of Models
bool CModels::LoadModels( vector <string> ListModels, CLogger *cLogger, CTextures *cTextures, CPodIni* cPodIni, float Tint[4] )
{


	string Temp;				// Temp String
	ifstream Fs;				// Filestream

	// Allocate Model Memory
	Models.resize( ListModels.size( ) );

	// Loop Models
	for ( unsigned int i = 0; i != ListModels.size( ); i++ )
	{
		// Validate Model Object
		if ( ListModels[i] != "0" )
		{
			// Load Bin Model
			if ( Models[i].LoadBin( ListModels[i], cPodIni, Tint ) )
			{
                // Log Loaded Bin Model File
                cLogger->Text( "Loaded Bin Model File " + ListModels[i], true );
			}
			else
			{
                // Log Error Loading Bin Model File
                cLogger->Error( "Error Loading Bin Model File " + ListModels[i], true );

                return false;	// Error Occured
			}
		}
	}

	return true;	// Return Ok

}

// Loads Backdrop Models
bool CModels::LoadBackDrops( string Filename, CTextures *cTextures, CPodIni* cPodIni, int Weather, float Tint[4] )
{

	string SkyDomeModel;					// SkyDome Model Filename
	GLfloat BackdropTint[3];				// Backdrop Tint Color

	// Switch Day Time
	switch ( Weather )
	{
		case 0:	// Clear SkyDome
			SkyDomeModel = "CLOUDY2.BIN";
			Tint[0]=1.0f;
			Tint[1]=1.0f;
			Tint[2]=1.0f;
			BackdropTint[0] = Tint[0];
			BackdropTint[1] = Tint[1];
			BackdropTint[2] = Tint[2];
			break;

		case 1:	// Cloudy SkyDome
			SkyDomeModel = "CLOUDY2.BIN";
			Tint[0]=0.8f;
			Tint[1]=0.8f;
			Tint[2]=0.8f;
			BackdropTint[0] = Tint[0];
			BackdropTint[1] = Tint[1];
			BackdropTint[2] = Tint[2];
			break;

		case 2:	// Fog Skydome
			SkyDomeModel = "CLOUDY2.BIN";
			Tint[0]=1.0f;
			Tint[1]=1.0f;
			Tint[2]=1.0f;
			BackdropTint[0] = Tint[0];
			BackdropTint[1] = Tint[1];
			BackdropTint[2] = Tint[2];
			break;

		case 3:	// Dense Fog Skydome
			SkyDomeModel = "CLOUDY2.BIN";
			Tint[0]=1.0f;
			Tint[1]=1.0f;
			Tint[2]=1.0f;
			BackdropTint[0] = Tint[0];
			BackdropTint[1] = Tint[1];
			BackdropTint[2] = Tint[2];
			break;

		case 4:	// Rainy SkyDome
			SkyDomeModel = "CCLOUDS.BIN";
			Tint[0]=0.5f;
			Tint[1]=0.5f;
			Tint[2]=0.5f;
			BackdropTint[0] = 0.3f;
			BackdropTint[1] = 0.3f;
			BackdropTint[2] = 0.3f;
			break;

		case 5:	// Dusk SkyDome
			SkyDomeModel = "DUSKSKY.BIN";
			Tint[0]=0.8f;
			Tint[1]=0.7f;
			Tint[2]=0.6f;
			BackdropTint[0] = 0.3f;
			BackdropTint[1] = 0.3f;
			BackdropTint[2] = 0.3f;
			break;

		case 6:	// Night SkyDome
			SkyDomeModel = "NITESKY.BIN";
			Tint[0]=0.3f;
			Tint[1]=0.3f;
			Tint[2]=0.3f;
			BackdropTint[0] = Tint[0];
			BackdropTint[1] = Tint[1];
			BackdropTint[2] = Tint[2];
			break;

		case 7:	// Night SkyDome (Pitch Black)
			SkyDomeModel = "NITESKY.BIN";
			Tint[0]=0.1f;
			Tint[1]=0.1f;
			Tint[2]=0.1f;
			BackdropTint[0] = Tint[0];
			BackdropTint[1] = Tint[1];
			BackdropTint[2] = Tint[2];
			break;

	}

	// Load Sky Dome
	if (!cSkyDome.LoadBin( SkyDomeModel, cPodIni, Tint ) )
		return false;	// Error

    // No Backdrop
    if ( Filename == "" )
        return true;

	// Load Sky Box
	if (!cSkyBox.LoadBin( Filename, cPodIni, BackdropTint ) )
		return false;	// Error

	return true;	// Return Ok

}

// Build Water Display List
bool CModels::BuildWaterList( float Tint[4], float WaterHeight, CTextures *cTextures, CPodIni *cPodIni )
{

	// Load Water Texture
	GLuint WaterTexture = cTextures->LoadRawFile( "RIPPL100.RAW", cPodIni );

	// Generate List & Compile
	WaterList = glGenLists( 1 );
	glNewList( WaterList, GL_COMPILE );
	{

		// Enable Blending
		glEnable(GL_BLEND);
		// Disable Alpha Test
		glDisable(GL_ALPHA_TEST);
		// Bind Water Texture
		glBindTexture( GL_TEXTURE_2D, WaterTexture );

		// Quads
		glBegin(GL_QUADS);
		{

			// Apply Global Tint
			glColor4f( (float)0.790625 * Tint[0], (float)0.86875 * Tint[1] , (float)0.9859375 * Tint[2], (float)0.459375 );

			// Vertex 1
			glTexCoord2f( 0.0f, 0.0f );
			glVertex3d( -256, WaterHeight, -256 );

			// Vertex 2
			glTexCoord2f( 0.0f, 256.0f );
			glVertex3d( 256, WaterHeight, -256 );

			// Vertex 3
			glTexCoord2f( 256.0f, 256.0f );
			glVertex3d( 256, WaterHeight, 256 );

			// Vertex 4
			glTexCoord2f( 256.0f, 0.0f );
			glVertex3d( -256, WaterHeight, 256 );

		}

		// Quad Finished
		glEnd();

	}

	// List Finished
	glEndList();

	return true;

}


