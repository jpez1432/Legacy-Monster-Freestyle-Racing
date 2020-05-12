
// Define
#ifndef CGROUNDBOXES_H
#define CGROUNDBOXES_H

// Include
#include "Shared.h"             // Common Header File

#include "CPods.h"				// Pods Class Header File
#include "CTextures.h"			// Textures Class Header File
#include "CTerrain.h"			// Terrain Class Header File

// Ground Boxes Class
class CGroundBoxes
{


    private:

		typedef struct TextureInfo				// Texture Indices, Rotation
		{
			unsigned char Indices[6];			// Boxes Texture Indices
			unsigned char Rotations[6];			// Boxes Texture Rotations
		};

		TextureInfo *BoxTextures;				// Boxes Texture Information

		unsigned char *BoxLower;				// Boxes Lower Heights
		unsigned char *BoxUpper;				// Boxes Upper Heights

		int Dimensions;						// Ground Box Dimensions

		string PodFile;							// Pod Filename
		int Size, Offset;						// Pod Filesize, Offset

		vector<GLuint> BoxIds;					// GroundBox Display List Ids

    protected:

    public:

        CGroundBoxes( );                     // Constructor
        ~CGroundBoxes( );                    // Destructor

		// Load Boxes Lower, Upper Heights
		bool LoadBoxHeights( string LowerFilename, string UpperFilename, CPodIni *cPodIni );

		// Load Boxes Texture Information
		bool LoadTextureInfo( string Filename, CPodIni *cPodIni );

		// Builds Box OpenGL Display Lists
		bool BuildGroundBoxLists( CTextures *cTextures, CTerrain *cTerrain, CPodIni *cPodIni, float Tint[4] );

		// Return Ground Box Display List Id
		GLuint BoxList( int x, int z ) { return BoxIds[z + 16 * x]; }

		int Index( int x, int z, int side );	// Return Texture Index
		int Rotation( int x, int z, int side );	// Return Texture Rotation
		float Lower( int x, int z );			// Return Box Lower Height
		float Upper( int x, int z );			// Return Box Upper Height

};

#endif // End ifndef
