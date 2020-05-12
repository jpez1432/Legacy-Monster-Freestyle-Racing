

// Define
#ifndef CTERRAIN_H
#define CTERRAIN_H

// Include
#include "Shared.h"             // Common Header File

#include "CPods.h"				// Pods Class Header File
#include "CTextures.h"			// Textures Class Header File

// Terrain Class
class CTerrain
{

    private:

		unsigned char *Indices;						    // Terrain Texture Indices
		unsigned char *Rotations;						// Terrain Texture Rotations
		unsigned char *Heightmap;						// Terrain Heightmap
		unsigned char *Lighting;						// Terrain Lighting

		int Dimensions;								    // Terrain Dimensions

		string PodFile;									// Pod Filename
		int Size, Offset;								// Pod Filesize, Offset

		vector<GLuint> TerrainIds;						// Terrain Display List Ids

    protected:

    public:

        CTerrain( );                      // Constructor
        ~CTerrain( );                     // Destructor

		// Load Heightmap
		bool LoadHeightmap( string Filename, CPodIni *cPodIni );

		// Load Texture Information
		bool LoadTextureInfo( string Filename, CPodIni *cPodIni );

		// Load Lighting
		bool LoadLightmap( string Filename, CPodIni *cPodIni );

		// Build Terrain OpenGL Display Lists
		bool BuildTerrainLists( CTextures *cTextures, CPodIni *cPodIni, float Tint[4] );

		// Return Terrain Display List Id
		GLuint TerrainList( int x, int z ) { return TerrainIds[z + 16 * x]; }

		// Get Lighting ( Ground Boxes )
		unsigned char GetLight( int x, int z ) { return Lighting[z + 16 * x]; }

		int Index( int x, int z );						// Return Texture Index
		int Rotation( int x, int z );					// Return Texture Rotation
		float Light( int x, int z );					// Return Terrain Light Value
		float Height( int x, int z );					// Return Heightmap Value

};

#endif
