
// Define
#ifndef CMODELS_H
#define CMODELS_H

// Include
#include "Shared.h"                     // Common Header File
#include "CLogger.h"                    // Logger Class Header File

#include "CTextures.h"					// Textures Class Header File
#include "CPods.h"						// Pods Class Header File

// MTM Bin Class
class CBin
{

	private:


		typedef struct BinHeader		// Bin Header
		{
			int BinScale;				// Bin Scale Size
			int Unknown1;				// Unknown Value
			int Unknown2;				// Unknown Value
			int NumVerts;				// Bin Number Of Vertices
		};

		typedef struct Vertex			// Bin Vertex
		{
			int X;						// Vertex X Coordinate
			int Y;						// Vertex Y Coordinate
			int Z;						// Vertex Z Coordinate
		};

		typedef struct Normal			// Bin Vertex Normal
		{
			int NX;						// Vertex Normal X Component
			int NY;						// Vertex Normal Y Component
			int NZ;						// Vertex Normal Z Component
		};

		typedef struct TextureBlock		// Bin Texture Block
		{
			int Unknown1;				// Unknown Value
			char TextureName[16];		// Texture Filename
		};

		typedef struct AnimTexBlockHeader// Bin Animated Texture Block Header
		{
			int Unknown1;				// Unknown Value
			int NumTextures;			// Number Of Textures
			int Unknown2;				// Unknown Value
			int Delay;					// Animation Rate
			int Unknown3;				// Unknown Value
			int Unknown4;				// Unknown Value
		};

		typedef struct AnimTexFilenames	// Bin Animated Textures
		{
			char TextureNames[32];		// Texture Filenames
		};


		typedef struct AnimTextureBlock		// Bin Animated Texture Block
		{
			AnimTexBlockHeader Header;		// Anim Texture Block Header
			vector<AnimTexFilenames> Names;	// Texture Filenames
		};

		typedef struct ColorBlock		// Bin Color Block
		{
			unsigned char Red;			// Red Component
			unsigned char Green;		// Green Component
			unsigned char Blue;			// Blue Component
			unsigned char Alpha;		// Alpha Component
		};

		typedef struct FaceBlockHeader	// Bin Face Block Header
		{
			int NumVerts;				// Face Number Of Vertices
			int NormalX;				// Face Normal X Component
			int NormalY;				// Face Normal Y Component
			int NormalZ;				// Face Normal Z Component
			int Magic;					// Face Magic Value
		};

		typedef struct VertexBlock		// Face Vertex Block
		{
			int VertIndex;				// Vertice Index
			int TexCoordU;				// Texture Coordinate X
			int TexCoordV;				// Texture Coordinate Y
		};

		typedef struct FaceBlock			// Bin Face Block
		{
			int FaceType;					// Face Type
			FaceBlockHeader Header;			// Face Header
			vector <VertexBlock> Vertices;	// Face Vertices
			int TextureIndex;				// Texture Block Index
		};

		typedef struct FaceBlock19		// Bin Special Block Color
		{
			int NumVerts;				// Face Number Of Vertices
			int NormalX;				// Face Normal X Component
			int NormalY;				// Face Normal Y Component
			int NormalZ;				// Face Normal Z Component
			int Magic;					// Face Magic Value
			vector <int> Verts;			// Face Vertices
			int ColorIndex;				// Face Color Block Index
		};

	public:

		string Name;									// Bin Filename

		int BinId;										// Bin Id

		BinHeader Header;								// Bin Header

		vector <Vertex> Vertices;						// Bin Vertices
		vector <Normal> Normals;						// Bin Normal Values

		vector <TextureBlock> Textures;					// Textures Blocks
		vector <AnimTextureBlock> AnimTextures;			// Animated Texture Blocks
		vector <ColorBlock> Colors;						// Color Blocks
		vector <FaceBlock> Faces;						// Faces Blocks
		vector <FaceBlock19> SpecialColors;				// Special Color Blocks

		vector <GLuint> TextureIds;						// OpenGL Texture Ids
		GLuint ModelId;									// OpenGL Model Display List Id

		string PodFile;									// Pod Filename
		int Size, Offset;								// Pod Filesize, Offset

		CTextures cTextures;							// Texture Class Header File

        CVector4 Min, Max;                              // Min, Max Dimension Vectors

		// Build OpenGL Display List
		bool BuildDisplayList( float AddTint[4] );

	public:

		CBin( );										// Constructor
		~CBin();										// Destructor

		// Load Bin Model
		bool LoadBin( string Filename, CPodIni *cPodIni, float Tiont[4] );

		// Get OpenGL Display List
		GLuint ModelList( void ) { return ModelId; }

        // Return Model Dimensions
        void Dimensions( CVector4 &MinVec, CVector4 &MaxVec );

};

// MTM Animated Bin Model Class
class CAnimBin
{

	private:

		// Animated Bin Model
		typedef struct Header
		{
			int Unknown1;					// Unknown Value
			int NumFrames;					// Number Of Animation Frames
			int Delay;						// Animation Delay
			int Unknown2;					// Unknown Value
			int Unknown3;					// Unknown Value
		};

	public:

		Header m_Header;					// Animated Bin Header
		vector <CBin> Frames;				// Model Bin Frames

	public:

		// Load Bin Model
		bool LoadBin( string Filename, CPodIni *cPodIni, float Tint[4] );

		// Get OpenGL Display List
		GLuint ModelList( int Index ) { return Frames[Index].ModelList( ); }

};

// MTM Models Class
class CModels
{

	public:

		vector <CBin> Models;					// Bin Models
		vector <CAnimBin> AnimModels;			// Animated Bin Models

		CBin cSkyBox;							// Sky Box Model
		CBin cSkyDome;							// Sky Dome Model

		string PodFile;							// Pod Filename
		int Size, Offset;						// Pod Filesize, Offset

        GLuint WaterList;					    // OpenGL Water Display List

	public:

		~CModels( );							// Destructor

		// Loads Unique List Of Models
		bool LoadModels( vector <string> ListModels, CLogger *cLogger, CTextures *cTextures, CPodIni* cPodIni, float Tint[4] );

		// Loads Backdrop Models
		bool LoadBackDrops( string Filename, CTextures *cTextures, CPodIni* cPodIni, int Weather, float Tint[4] );

        // Build Water Display List
        bool BuildWaterList( float Tint[4], float WaterHeight, CTextures *cTextures, CPodIni *cPodIni );

};

#endif // End ifndef
