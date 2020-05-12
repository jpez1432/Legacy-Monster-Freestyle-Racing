
// Define
#ifndef CTEXTURES_H
#define CTEXTURES_H

// Include
#include "Shared.h"				            // Common Header File
#include "CLogger.h"                        // Logger Class Header File

#include "CPods.h"				            // Pods Class Header File

// Textures Class
class CTextures
{


    private:

		#define TEX_0 0.03123f					// Lower Texture Coordinate
		#define TEX_1 1.0 - 0.03123f			// Upper Texture Coordinate

		unsigned char Pallete[768];			// Act Pallete Memory
		unsigned char MetalCr2[768];			// MetalCr2 Palette

		double TextureCoords[2][2][4][4][2];	// Mirror, Rotation Values

		int Dimensions;						// Texture Dimensions
		int Filter;							// Texture Filter

		vector <GLuint> TextureIds;			// OpenGL Texture Ids

		string PodFile;							// Pod Filename
		int Size, Offset;						// Pod Filesize, Offset

        vector<GLuint> Textures;                // Texture Vector

    protected:

    public:

        CTextures( );                       // Constructor
        ~CTextures( );                      // Destructor

        // Load Bitmap Texture File
        bool LoadBitmap( string FileName, bool Alpha = false );

        // Get OpenGL Texture
        GLuint GLTexture( int Index ) { return Textures[Index]; }

		// Load Terrain, Box Textures
		bool LoadTextures( string Filename, CLogger *cLogger, CPodIni *cPodIni );

		// Load Default MetalCr2 Palette
		bool LoadMetalCr2( CPodIni *cPodIni );

		// Load Raw Texture
		GLuint LoadRawFile( string Filename, CPodIni *cPodIni );

		// Returns Mirror, Rotated Texture Coordinates
		float GetTexCoords( int Mir_X, int Mir_Y, int Rotation, int Corner, int UV );

		// Set Texture Filter
		void SetFilter( int Value ) { Filter = Value; }

		// Get Texture Id
		GLuint TextureId( int Index ) { return TextureIds[Index]; }
};

#endif // End ifndef
