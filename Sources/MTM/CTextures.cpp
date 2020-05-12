
// Include
#include "CTextures.h"                    // Textures Class Header File

extern string RootDir;				// Global Root Directory

// Constructor
CTextures::CTextures(  )
{

	// Build Texture Mirror, Rotations Coordinates
	double MirrorRotationCoords[2][2][4][4][2] = {
         {
           {
             {{TEX_0, TEX_1}, {TEX_0, TEX_0}, {TEX_1, TEX_0}, {TEX_1, TEX_1}},
             {{TEX_1, TEX_1}, {TEX_0, TEX_1}, {TEX_0, TEX_0}, {TEX_1, TEX_0}},
             {{TEX_1, TEX_0}, {TEX_1, TEX_1}, {TEX_0, TEX_1}, {TEX_0, TEX_0}},
             {{TEX_0, TEX_0}, {TEX_1, TEX_0}, {TEX_1, TEX_1}, {TEX_0, TEX_1}}
           },
           {
             {{TEX_0, TEX_0}, {TEX_0, TEX_1}, {TEX_1, TEX_1}, {TEX_1, TEX_0}},
             {{TEX_0, TEX_1}, {TEX_1, TEX_1}, {TEX_1, TEX_0}, {TEX_0, TEX_0}},
             {{TEX_1, TEX_1}, {TEX_1, TEX_0}, {TEX_0, TEX_0}, {TEX_0, TEX_1}},
             {{TEX_1, TEX_0}, {TEX_0, TEX_0}, {TEX_0, TEX_1}, {TEX_1, TEX_1}}
           }
         },
         {
           {
             {{TEX_1, TEX_1}, {TEX_1, TEX_0}, {TEX_0, TEX_0}, {TEX_0, TEX_1}},
             {{TEX_1, TEX_0}, {TEX_0, TEX_0}, {TEX_0, TEX_1}, {TEX_1, TEX_1}},
             {{TEX_0, TEX_0}, {TEX_0, TEX_1}, {TEX_1, TEX_1}, {TEX_1, TEX_0}},
             {{TEX_0, TEX_1}, {TEX_1, TEX_1}, {TEX_1, TEX_0}, {TEX_0, TEX_0}}
           },
           {
             {{TEX_1, TEX_0}, {TEX_1, TEX_1}, {TEX_0, TEX_1}, {TEX_0, TEX_0}},
             {{TEX_0, TEX_0}, {TEX_1, TEX_0}, {TEX_1, TEX_1}, {TEX_0, TEX_1}},
             {{TEX_0, TEX_1}, {TEX_0, TEX_0}, {TEX_1, TEX_0}, {TEX_1, TEX_1}},
             {{TEX_1, TEX_1}, {TEX_0, TEX_1}, {TEX_0, TEX_0}, {TEX_1, TEX_0}}
           }
         }
       };


	// Copy Memory Array
	memcpy( TextureCoords, MirrorRotationCoords, sizeof(TextureCoords) );

}

// Destructor
CTextures::~CTextures( )
{

    // Delete OpenGL Texture
    glDeleteTextures( Textures.size( ), &Textures[0] );

    // Release Textures Vector
    Textures.empty( );

}

// Load Bitmap Texture File
bool CTextures::LoadBitmap( string FileName, bool Alpha )
{
	HBITMAP hBMP;														// Handle Of The Bitmap
	BITMAP	BMP;

    Textures.resize( Textures.size( ) + 1 );                            // Resize Texture Vector
	glGenTextures(1, &Textures[Textures.size( )-1]);						// Create Texture

	hBMP=(HBITMAP)LoadImage(GetModuleHandle(NULL), FileName.c_str( ),   // Load Image
	IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE );

	if (!hBMP)															// Bitmap Exists
		return FALSE;													// Error, Return False

	GetObject(hBMP, sizeof(BMP), &BMP);									// Get The Object

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);								// Pixel Storage Mode

	glBindTexture(GL_TEXTURE_2D, Textures[Textures.size( )-1]);			// Bind Texture ID
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Min Filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// Linear Mag Filter

 	if ( Alpha == true )
        glTexImage2D(GL_TEXTURE_2D, 0, 4, BMP.bmWidth, BMP.bmHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, BMP.bmBits);
    else
        glTexImage2D(GL_TEXTURE_2D, 0, 3, BMP.bmWidth, BMP.bmHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, BMP.bmBits);

	DeleteObject(hBMP);													// Delete Object

	return TRUE;														// Load Successful

}

// Load Terrain, Box Textures
bool CTextures::LoadTextures( string Filename, CLogger *cLogger, CPodIni *cPodIni )
{


	int NumTextures;			// Number Of Textures
	string Temp;				// Temp String
	ifstream Fs;				// Filestream

	// Find Pod File Information
	if ( cPodIni->FileInfo( "Data\\" + Filename, PodFile, Size, Offset ) == true )
	{

		PodFile = RootDir + PodFile;						// Build Texture Info Filename
		Fs.open( PodFile.c_str( ) );						// Open Pod File
		Fs.seekg( Offset );									// Seek Track File Offset

	} else {

		PodFile = RootDir + "Data\\" + Filename;				// Build Texture Filename
		Fs.open( PodFile.c_str( ) );							// Open Track File

	}

	// Validate File
	if ( !Fs.good( ) ) return false;	// Error Occured

	// Get Number Of Textures
	getline( Fs, Temp );

	// Convert String To Integer
	NumTextures = atoi(Temp.c_str());

	// Allocate Texture Memory
	TextureIds.resize( NumTextures );

    // Set Texture Filter
    Filter = 0;

	for ( int i = 0; i != NumTextures; i++ )
	{
		// Get Texture Filename
		getline( Fs, Temp );

		// Load Raw Texture, Assign Id
		TextureIds[i] = LoadRawFile( Temp, cPodIni );

        if ( TextureIds[i] != -1 )
        {
            // Log Loaded Raw Texture File
            cLogger->Text( "Loaded Raw Texture File " + Temp, true );
        }
        else
        {
            // Log Error Loading Raw Texture File
            cLogger->Error( "Error Loading Raw Texture File " + Temp, true );

            return false;	// Error Occured
        }
	}

	return true;	// Return Ok

}

// Load Default MetalCr2 Palette
bool CTextures::LoadMetalCr2( CPodIni *cPodIni )
{

	ifstream Fs;				// Filestream

	// Get File Information
	if ( cPodIni->FileInfo( "Art\\MetalCr2.Act", PodFile, Size, Offset ) == true  )
	{
		// Matched File In Pod
		PodFile = RootDir + PodFile;							// Build Pod Filename
		Fs.open( PodFile.c_str( ), ios:: binary | ios::in );	// Open Pod File
		Fs.seekg( Offset );										// Seek File Position
	}
	else // Couldn't Match File In Pod
	{
		return false;
	}

    // Valid Palette
    if ( Fs.is_open( ) )
    {

        // Read Pallete Data
        Fs.read((char*)MetalCr2, sizeof(MetalCr2));

        // Close Palette
        Fs.close( );

        return true;	// Return Ok

	}
	else
	{

        // Close Palette
        Fs.close( );

        return false;	// Return  Error

    }

}

// Load Raw Texture
GLuint CTextures::LoadRawFile( string Filename, CPodIni *cPodIni )
{

	ifstream Fs;		// File Stream
	string Line;		// Temp String

	GLuint TexId = -1;  // Temp Texture Id

	// Find Pod File Information
	if ( cPodIni->FileInfo( "Art\\" + Filename, PodFile, Size, Offset ) == true )
	{

		PodFile = RootDir + PodFile;							// Build Texture Info Filename
		Fs.open( PodFile.c_str( ), ios::binary | ios::in );		// Open Pod File
		Fs.seekg( Offset );										// Seek Track File Offset
		Dimensions = (int)sqrt((double)Size);					// Calculate Texture Dimensions

	} else {

		PodFile = RootDir + "Art\\" + Filename;					// Build Texture Filename
		Fs.open( PodFile.c_str( ), ios::binary | ios::in );		// Open Track File
		Fs.seekg( 0, ios::end );								// Seek End Of File
		Dimensions = Fs.tellg( );								// Get File Length
		Fs.seekg( 0, ios::beg );								// Seek Beginning Of File
		Dimensions = (int)sqrt((double)Dimensions);			    // Calculate Texture Dimensions
	}

    // Texture Buffer
    unsigned char *Buffer;

    // Valid Raw
    if ( Fs.is_open( ) )
    {

        // Allocate Texture & Buffer Memory
        Buffer = new unsigned char[Dimensions * Dimensions];

        // Read Texture Buffer
        Fs.read((char*)Buffer, Dimensions * Dimensions);

        // Close Palette
        Fs.close( );

	}
	else
	{

        // Close Palette
        Fs.close( );

        return TexId;	// Return  Error

    }

	// Replace Raw, Act Extension
	Filename = Replace( StringToUpper(Filename), ".RAW", ".ACT" );

	// Get File Information
	if ( cPodIni->FileInfo( "Art\\" + Filename, PodFile, Size, Offset ) )
	{

		PodFile = RootDir + PodFile;						// Build Filename
		Fs.open( PodFile.c_str( ), ios::binary | ios::in );	// Open Pod File
		Fs.seekg( Offset );									// Seek File Offset
	}
	else
	{

		PodFile = RootDir + "Art\\" + Filename;				// Build Filename
		Fs.open( PodFile.c_str( ), ios::binary | ios::in );	// Open Palette File
	}

	// Validate
	if ( !Fs.good( ) )
	{
		// Copy MetalCr2 To Pallete
		memcpy(Pallete, MetalCr2, sizeof(Pallete));

	}
	else
	{
		// Read Pallete Data
		Fs.read((char*)Pallete, 768);
	}

	// Close Palette File
	Fs.close( );

    unsigned int *Pal = new unsigned int[256];

	for ( int i = 0; i != 256; i++ )
	{
		Pal[i] = Pallete[i*3+2] << 16 | Pallete[i*3+1] << 8 | Pallete[i*3+0];

	    if ( Pallete[i*3+0] >8 || Pallete[i*3+1] >8 || Pallete[i*3+2] >8 )
	    {
            Pal[i] |= 0xFF000000;
	    }
    }

    unsigned int *Raw = new unsigned int[Dimensions * Dimensions * 4];

	// Loop Dimensions
	for (int z = 0; z < Dimensions; z++)
	{
		for (int x = 0; x < Dimensions; x++)
		{
			// Fill Out Raw Memory
			Raw[z+Dimensions*x] = Pal[Buffer[(z+Dimensions*x)]];
		}
	}

	glGenTextures (1, &TexId);					// Generate Texture
	glBindTexture (GL_TEXTURE_2D, TexId);		// Bind New Texture

	// Mix Color Modulate
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );


	switch (Filter)
	{
		case 0:	// Nearest
		{
			// Repeat Texture
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			// Max, Min Nearest Filtering
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

			// Create Texture Parameters
			glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, Dimensions, Dimensions, 0, GL_RGBA, GL_UNSIGNED_BYTE, Raw);

		}
		case 1: // Linear
		{
			// Repeat Texture
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			// Max, Min Linear Filtering
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			// Create Texture Parameters
			glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, Dimensions, Dimensions, 0, GL_RGBA, GL_UNSIGNED_BYTE, Raw);

		}
		case 2: // MipMaps
		{
			// Repeat Texture
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			// Max, Min  MipMap Linear Filtering
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

			// Build MipMaps
			gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, Dimensions, Dimensions, GL_RGBA, GL_UNSIGNED_BYTE, Raw);

		}

  }

	delete [] Raw;		// Delete Raw Buffer
	delete [] Buffer;	// Delete Temp Buffer

	return TexId;	// Texture ID

}

// Returns Mirror, Rotated Texture Coordinates
float CTextures::GetTexCoords( int Mir_X, int Mir_Y, int Rotation, int Corner, int UV )
{

	return (float)TextureCoords[Mir_X][Mir_Y][Rotation][Corner][UV];

}

