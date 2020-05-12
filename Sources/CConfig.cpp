
// Include
#include "CConfig.h"                // Config Class Header File

extern string RootDir;              // Global Root Resource Directory

// Constructor
CConfig::CConfig( )
{

	// Debug
	RenderInfo = true;				// Render Debug Information
	EnableConsole = true;			// Enable Debug Console
    SkipIntro = true;               // Skip Intro Flag

	// Display
	Fullscreen = false;				// Fullscreen Flag
	UseRefresh = false;				// Use Refresh Rate
	RefreshRate = 60;				// Refresh Rate
	DisplayWidth = 800;				// Resolution Width
	DisplayHeight = 600;			// Resolution Height
	DisplayDepth = 32;				// Display Bits Per Pixel

	// OpenGL
	FieldOfVision = 60;				// Field Of Vision
	ZDrawDistance = 32;			    // Z Draw Distance
	TextureFilter = 2;				// Texture Filtering

	// Camera
	RotateLeft = 'J';				// Camera Rotate Left
	RotateRight = 'L';				// Camera Rotate Right
	RotateUp = 'I';					// Camera Rotate Up
	RotateDown = 'K';				// Camera Rotate Down
    ZoomIn = 'N';                   // Camera Zoom In
    ZoomOut = 'M';                  // Camera Zoom Out

    // Sound
    UseSound = true;                // Use Sound Flag
    EngineSound = "Engine.Wav";     // Engine Sound String
    Volume = 100;                   // Sound Volume

	// Input
	UseJoystick = false;			// Use Joy/Wheel Flag
	Gas = 'W';						// Gas Key
	Brake = 'S';					// Brake Key
	FrontSteerLeft = 'A';			// Steer Front Left Key
	FrontSteerRight = 'D';			// Steer Front Right Key
	RearSteerLeft = 'A';			// Steer Rear Left Key
	RearSteerRight = 'D';			// Steer Rear Right Key

	// Game Options
	UsePodFiles = true;				// Use Pod Files Flag
	UseReflections = true;			// Use Reflections Flag
	UseLenseFlare = true;			// Use Lense Flare Flag
	WrapTrack = true;				// Wrap Track Flag
	Weather = 0;					// Weather Flag
	SceneryComplexity = 3;			// Complexity Flag
	TrackName = "MFRTrack";			// Default Track Name
	TruckName = "MFRTruck";			// Default Truck Name


}

// Destructor
CConfig::~CConfig( )
{

    // Delete Xml Parser Object
    SAFE_DELETE( cXml );

}

// Load Configuration File
bool CConfig::LoadConfig( string Filename )
{

    // Create New Parser Object
    cXml = new CXmlParser( );

	// Parse Physics File
	if ( !cXml->ParseFile( Filename.c_str( ) ) )
		return false;	// Error

	// Debug
	RenderInfo = GetBool( atoi( cXml->FindValue( "DEBUG","INFO", cXml->GetNode( ) ).c_str( ) ) );
	EnableConsole = GetBool( atoi( cXml->FindValue( "DEBUG","CONSOLE", cXml->GetNode( ) ).c_str( ) ) );
	SkipIntro = GetBool( atoi( cXml->FindValue( "DEBUG","SKIPINTRO", cXml->GetNode( ) ).c_str( ) ) );

	// Display
	Fullscreen = GetBool( atoi( cXml->FindValue( "DISPLAY","FULLSCREEN", cXml->GetNode( ) ).c_str( ) ) );
	UseRefresh = GetBool( atoi( cXml->FindValue( "DISPLAY","INCLUDEREFRESH", cXml->GetNode( ) ).c_str( ) ) );
	RefreshRate = ( atoi( cXml->FindValue( "DISPLAY","SCREENREFRESH", cXml->GetNode( ) ).c_str( ) ) );
	DisplayWidth = ( atoi( cXml->FindValue( "DISPLAY","SCREENWIDTH", cXml->GetNode( ) ).c_str( ) ) );
	DisplayHeight = ( atoi( cXml->FindValue( "DISPLAY","SCREENHEIGHT", cXml->GetNode( ) ).c_str( ) ) );
	DisplayDepth = ( atoi( cXml->FindValue( "DISPLAY","SCREENDEPTH", cXml->GetNode( ) ).c_str( ) ) );

	// OpenGL
	FieldOfVision = (float)( atof( cXml->FindValue( "OPENGL","FIELDOFVISION", cXml->GetNode( ) ).c_str( ) ) );
	ZDrawDistance = (float)( atof( cXml->FindValue( "OPENGL","DRAWDISTANCE", cXml->GetNode( ) ).c_str( ) ) );
	TextureFilter = ( atoi( cXml->FindValue( "OPENGL","TEXTUREFILTER", cXml->GetNode( ) ).c_str( ) ) );

	// Camera
	RotateLeft = ( ( cXml->FindValue( "CAMERA","ROTATELEFT", cXml->GetNode( ) ).c_str( ) ) );
	RotateRight = ( ( cXml->FindValue( "CAMERA","ROTATERIGHT", cXml->GetNode( ) ).c_str( ) ) );
	RotateUp = ( ( cXml->FindValue( "CAMERA","ROTATEUP", cXml->GetNode( ) ).c_str( ) ) );
	RotateDown = ( ( cXml->FindValue( "CAMERA","ROTATEDOWN", cXml->GetNode( ) ).c_str( ) ) );
	ZoomIn = ( ( cXml->FindValue( "CAMERA","ZOOMIN", cXml->GetNode( ) ).c_str( ) ) );
	ZoomOut = ( ( cXml->FindValue( "CAMERA","ZOOMOUT", cXml->GetNode( ) ).c_str( ) ) );

    UseSound = GetBool( atoi( cXml->FindValue( "SOUND","SOUND", cXml->GetNode( ) ).c_str( ) ) );
    EngineSound = ( ( cXml->FindValue( "SOUND","ENGINESOUND", cXml->GetNode( ) ).c_str( ) ) );
    Volume = ( atoi( cXml->FindValue( "SOUND","VOLUME", cXml->GetNode( ) ).c_str( ) ) );

	// Input
	UseJoystick = GetBool( atoi( cXml->FindValue( "INPUT","JOYSTICK", cXml->GetNode( ) ).c_str( ) ) );
	Gas = ( ( cXml->FindValue( "INPUT","GAS", cXml->GetNode( ) ).c_str( ) ) );
	Brake = ( ( cXml->FindValue( "INPUT","BRAKE", cXml->GetNode( ) ).c_str( ) ) );
	FrontSteerLeft = ( ( cXml->FindValue( "INPUT","FRONTSTEERLEFT", cXml->GetNode( ) ).c_str( ) ) );
	FrontSteerRight = ( ( cXml->FindValue( "INPUT","FRONTSTEERRIGHT", cXml->GetNode( ) ).c_str( ) ) );
	RearSteerLeft = ( ( cXml->FindValue( "INPUT","REARSTEERLEFT", cXml->GetNode( ) ).c_str( ) ) );
	RearSteerRight = ( ( cXml->FindValue( "INPUT","REARSTEERRIGHT", cXml->GetNode( ) ).c_str( ) ) );

	// Game
	UsePodFiles = GetBool( atoi( cXml->FindValue( "GAME","PODFILES", cXml->GetNode( ) ).c_str( ) ) );
	UseReflections = GetBool( atoi( cXml->FindValue( "GAME","REFLECTIONS", cXml->GetNode( ) ).c_str( ) ) );
	UseLenseFlare = GetBool( atoi( cXml->FindValue( "GAME","LENSEFLARE", cXml->GetNode( ) ).c_str( ) ) );
	WrapTrack = GetBool( atoi( cXml->FindValue( "GAME","WRAPWORLD", cXml->GetNode( ) ).c_str( ) ) );
	Weather = ( atoi( cXml->FindValue( "GAME","WEATHER", cXml->GetNode( ) ).c_str( ) ) );
	SceneryComplexity = ( atoi( cXml->FindValue( "GAME","SCENERY", cXml->GetNode( ) ).c_str( ) ) );
	TrackName = ( ( cXml->FindValue( "GAME","TRACKNAME", cXml->GetNode( ) ).c_str( ) ) );
	TruckName = ( ( cXml->FindValue( "GAME","TRUCKNAME", cXml->GetNode( ) ).c_str( ) ) );

	return true;	// Ok

}

// Save Configuration File
bool CConfig::SaveConfig( string Filename )
{

    // Generate Filename
    string File = RootDir + Filename;

    // Open Config File
    ofstream iFile( File.c_str( ) );

        iFile << "<Config>\n" << endl;

            iFile << "\t<Debug>\n" << endl;

                iFile << "\t\t<Info>" + NtoS( GetInt(RenderInfo) ) + "</Info>" << endl;
                iFile << "\t\t<Console>" + NtoS( GetInt(EnableConsole) ) + "</Console>\n" << endl;
                iFile << "\t\t<SkipIntro>" + NtoS( GetInt(SkipIntro) ) + "</SkipIntro>\n" << endl;

            iFile << "\t</Debug>\n" << endl;

            iFile << "\t<Display>\n" << endl;

                iFile << "\t\t<Fullscreen>" + NtoS( GetInt(Fullscreen) ) + "</Fullscreen>" << endl;
                iFile << "\t\t<ScreenRefresh>" + NtoS( (RefreshRate) ) + "</ScreenRefresh>" << endl;
                iFile << "\t\t<IncludeRefresh>" + NtoS( GetInt(UseRefresh) ) + "</IncludeRefresh>" << endl;
                iFile << "\t\t<ScreenWidth>" + NtoS( (DisplayWidth) ) + "</ScreenWidth>" << endl;
                iFile << "\t\t<ScreenHeight>" + NtoS( (DisplayHeight) ) + "</ScreenHeight>" << endl;
                iFile << "\t\t<ScreenDepth>" + NtoS( (DisplayDepth) ) + "</ScreenDepth>\n" << endl;

            iFile << "\t</Display>\n" << endl;

            iFile << "\t<OpenGL>\n" << endl;

                iFile << "\t\t<FieldOfVision>" + NtoS( (FieldOfVision) ) + "</FieldOfVision>" << endl;
                iFile << "\t\t<DrawDistance>" + NtoS( (ZDrawDistance) ) + "</DrawDistance>" << endl;
                iFile << "\t\t<TextureFilter>" + NtoS( (TextureFilter) ) + "</TextureFilter>\n" << endl;

            iFile << "\t</OpenGL>\n" << endl;

            iFile << "\t<Camera>\n" << endl;

                iFile << "\t\t<RotateLeft>" + ( (RotateLeft) ) + "</RotateLeft>" << endl;
                iFile << "\t\t<RotateRight>" + ( (RotateRight) ) + "</RotateRight>" << endl;
                iFile << "\t\t<RotateUp>" + ( (RotateUp) ) + "</RotateUp>" << endl;
                iFile << "\t\t<RotateDown>" + ( (RotateDown) ) + "</RotateDown>" << endl;
                iFile << "\t\t<ZoomIn>" + ( (ZoomIn) ) + "</ZoomIn>" << endl;
                iFile << "\t\t<ZoomOut>" + ( (ZoomOut) ) + "</ZoomOut>\n" << endl;

            iFile << "\t</Camera>\n" << endl;

           iFile << "\t<Sound>\n" << endl;

                iFile << "\t\t<Sound>" + NtoS( (UseSound) ) + "</Sound>" << endl;
                iFile << "\t\t<EngineSound>" + ( (EngineSound) ) + "</EngineSound>" << endl;
                iFile << "\t\t<Volume>" + NtoS( (Volume) ) + "</Volume>\n" << endl;

            iFile << "\t</Sound>\n" << endl;


            iFile << "\t<Input>\n" << endl;

                iFile << "\t\t<JoyStick>" + NtoS( GetInt(UseJoystick) ) + "</JoyStick>" << endl;
                iFile << "\t\t<Gas>" + ( (Gas) ) + "</Gas>" << endl;
                iFile << "\t\t<Brake>" + ( (Brake) ) + "</Brake>" << endl;
                iFile << "\t\t<FrontSteerLeft>" + ( (FrontSteerLeft) ) + "</FrontSteerLeft>" << endl;
                iFile << "\t\t<FrontSteerRight>" + ( (FrontSteerRight) ) + "</FrontSteerRight>" << endl;
                iFile << "\t\t<RearSteerLeft>" + ( (RearSteerLeft) ) + "</RearSteerLeft>" << endl;
                iFile << "\t\t<RearSteerRight>" + ( (RearSteerRight) ) + "</RearSteerRight>\n" << endl;

            iFile << "\t</Input>\n" << endl;

            iFile << "\t<Game>\n" << endl;

                iFile << "\t\t<PodFiles>" + NtoS( GetInt(UsePodFiles) ) + "</PodFiles>" << endl;
                iFile << "\t\t<Reflections>" + NtoS( GetInt(UseReflections) ) + "</Reflections>" << endl;
                iFile << "\t\t<LenseFlare>" + NtoS( GetInt(UseLenseFlare) ) + "</LenseFlare>" << endl;
                iFile << "\t\t<WrapWorld>" + NtoS( GetInt(WrapTrack) ) + "</WrapWorld>" << endl;
                iFile << "\t\t<Weather>" + ( NtoS(Weather) ) + "</Weather>" << endl;
                iFile << "\t\t<Scenery>" + ( NtoS(SceneryComplexity) ) + "</Scenery>" << endl;
                iFile << "\t\t<TrackName>" + ( (TrackName) ) + "</TrackName>" << endl;
                iFile << "\t\t<TruckName>" + ( (TruckName) ) + "</TruckName>\n" << endl;

            iFile << "\t</Game>\n" << endl;

        iFile << "</Config>\n" << endl;

    iFile.close( );

}

// Convert Int Number To String
string CConfig::NtoS( int i )
{

    string Combine;
    char buffer [33];
    itoa (i,buffer,10);
    Combine.append( buffer );
    return Combine;

}
