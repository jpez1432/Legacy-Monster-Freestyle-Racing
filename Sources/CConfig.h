
// Define
#ifndef CCONFIG_H
#define CCONFIG_H

// Include
#include "Shared.h"             // Shared Header File
#include "CXml.h"               // Xml Class Header File

// CConfig Class
class CConfig
{

	private:

		CXmlParser *cXml;		// Xml Class Object

	public:

		CConfig( );				// Constructor
		~CConfig( );			// Destructor

		// Load Configuration File
		bool LoadConfig( string Filename );

        // Save Configuration File
        bool SaveConfig( string Filename );

        // Convert Integer Number To String
        string NtoS( int i );

		// Return Boolean Value
		bool GetBool( int Value ) { if ( Value == 0 ) return false; else return true; }

		// Return Int Value
		int GetInt( bool Value ) { if ( Value == true ) return 1; else return 0; }

		// Debug
		bool RenderInfo;				// Render Debug Information
		bool EnableConsole;				// Enable Debug Console
        bool SkipIntro;                 // Skip Intro Flag

		// Disable
		bool Fullscreen;				// Fullscreen Flag
		bool UseRefresh;				// Use Refresh Rate
		int RefreshRate;				// Refresh Rate
		int DisplayWidth;				// Resolution Width
		int DisplayHeight;				// Resolution Height
		int DisplayDepth;				// Display Bits Per Pixel

		// OpenGL
		float FieldOfVision;			// Field Of Vision
		float ZDrawDistance;			// Z Draw Distance
		int TextureFilter;				// Texture Filtering

		// Camera
		string RotateLeft;				// Rotate Right Key
		string RotateRight;				// Rotate Left Key
		string RotateUp;				// Rotate Up Key
		string RotateDown;				// Rotate Down Key
		string ZoomIn;				    // Zoom In Key
		string ZoomOut;				    // Zoom Out Key

        // Sound
        bool UseSound;                  // Use Sound Flag
        string EngineSound;             // Engine Sound String
        int Volume;                     // Sound Volume

		// Input
		bool UseJoystick;				// Use Joy/Wheel Flag
		string Gas;						// Gas Key
		string Brake;					// Brake Key
		string FrontSteerLeft;			// Steer Front Left Key
		string FrontSteerRight;			// Steer Front Right Key
		string RearSteerLeft;			// Steer Rear Left Key
		string RearSteerRight;			// Steer Rear Right Key

		// Game Options
		bool UsePodFiles;				// Use Pod Files Flag
		bool UseReflections;			// Use Reflections Flag
		bool UseLenseFlare;				// Use Lense Flare Flag
		bool WrapTrack;					// Wrap Track Flag
		int Weather;					// Weather Flag
		int SceneryComplexity;			// Complexity Flag
		string TrackName;				// Track Name
		string TruckName;				// Truck Name

};

#endif  // Endif
