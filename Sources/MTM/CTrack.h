
// Define
#ifndef CTRACK_H
#define CTRACK_H

// Include
#include "Shared.h"             // Shared Header File
#include "CConfig.h"            // Config Class Header File
#include "CLogger.h"            // Logger Class Header File

#include "CPods.h"				// Pods Class Header File
#include "CTerrain.h"			// Terrain Class Header File
#include "CGroundBoxes.h"		// Ground Boxes Class Header File
#include "CTextures.h"			// Textures Class Header File
#include "CModels.h"			// Models Class Header File

// MTM Track Class
class CTrack
{

    private:

		// Vehicle
		typedef struct Vehicle
		{
			string Filename;				// Vehicle Filename
			CVector4 Position;			    // Vehicle Position
			CVector4 Rotation;			    // Vehicle Rotation
		};

		// Model
		typedef struct Model
		{
			string Filename;				// Model Filename
			CVector4 Position;			    // Model Position
			CVector4 Rotation;			    // Model Rotation
			CVector4 Dimensions;			// Model Dimensions
			double Mass;					// Model Mass
			string TypeFlags;				// Model Type, Flag
			int Priority;					// Model Priority
			int BinIndex;					// Model Bin Index
			bool Animated;				    // Model Animated Flag
		};

		string Name;						// Track Name
		string Level;						// Level Filename
		string Palette;					    // Palette Filename
		string Heightmap;					// Heightmap Filename
		string Lightmap;					// Lightmap Filename
		string TextureList;				    // Texture List Filename
		string TextureInfo;				    // Texture Info Filename
		string BackDrop;					// Backdrop Filename

		float WaterHeight;				    // Water Height
		float Tint[4];						// Weather Tint
		float Light[4];						// Light Position

        string PodFile;		                // Pod Filename
        int Size, Offset;		            // Pod Filesize, Offset

        vector <string> ListModels;         // Unique Model List

        // Parse X, Y, Z String
        CVector4 ParseVector( string Line );

        // Build Unique Model List
        void BuildUniqueList( vector<string> &ListModels );

		// Load Track Resources
		bool LoadResources( CPodIni *cPodIni, CLogger *cLogger, CConfig *cConfig );

        // Build Weather Tints
        void BuildWeatherTints( int Weather );

    protected:

    public:

        CTrack( );            // Constructor
        ~CTrack( );           // Destructor

		CTerrain *cTerrain;					// Terrain Class Object
		CGroundBoxes *cGroundBoxes;			// GroundBoxes Class Object
		CTextures *cTextures;				// Textures Class Object
		CModels *cModels;					// Models Class Object

		vector<Vehicle> Vehicles;			// Vehicles
		vector<Model> Models;				// Models

		// Load Track
		bool LoadTrack( string Filename, CPodIni *cPodIni, CLogger *cLogger, CConfig *cConfig );

		// Return Truck Position
		CVector4 TruckPosition( int Index ) { return Vehicles[Index].Position; }

		// Return Truck Rotation
		CVector4 TruckRotation( int Index ) { return Vehicles[Index].Rotation; }

		// Get Number Of Models
		int NumberOfModels( void ) { return Models.size( ); }

		// Model Name
		string ModelName( int Index ) { return Models[Index].Filename; }

		// Model Mass
		float ModelMass( int Index ) { return Models[Index].Mass; }

		// Get Model Position
		CVector4 ModelPosition( int Index ) { return Models[Index].Position; }

		// Get Model Rotation
		CVector4 ModelRotation( int Index ) { return Models[Index].Rotation; }

		// Get Model Dimensions
		CVector4 ModelDimensions( int Index ) { return Models[Index].Dimensions; }

		// Get Model Index
		int ModelIndex( int Index ) { return Models[Index].BinIndex; }

		// Get Model Priority
		int ModelPriority( int Index ) { return Models[Index].Priority; }

		// Get Model Type Flags
		string ModelTypeFlags( int Index ) { return Models[Index].TypeFlags; }


};

#endif  // Endif
