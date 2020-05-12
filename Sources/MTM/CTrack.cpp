
// Include
#include "CTrack.h"                 // MTM Track Header File

extern string RootDir;              // Global Root Resources Directory

// Constructor
CTrack::CTrack( )
{


}

// Destructor
CTrack::~CTrack( )
{

	// Empty Vehicle Vector
	Vehicles.empty( );

	// Empty Model Vector
	Models.empty( );

    // Delete Objects
    SAFE_DELETE( cModels );
    SAFE_DELETE( cTerrain );
    SAFE_DELETE( cGroundBoxes );
    SAFE_DELETE( cTextures );

}

// Load Track
bool CTrack::LoadTrack( string Filename, CPodIni *cPodIni, CLogger *cLogger, CConfig *cConfig )
{

	string Temp;				// Temp String
	ifstream Fs;				// Filestream

	// Find Pod Track File Information
	if ( cPodIni->FileInfo( "World\\" + Filename + ".Sit", PodFile, Size, Offset ) == true )
	{
		PodFile = RootDir + PodFile;					    // Build Track Filename
		Fs.open( PodFile.c_str( ) );					    // Open Pod File
		Fs.seekg( Offset );								    // Seek Track File Offset

        // Prioritize Track Pod
		cPodIni->Prioritize( PodFile );

	}
	else
	{
        Temp = RootDir + "World\\" + Filename + ".Sit";     // Build Track Filename
		Fs.open(  Temp.c_str( ) );					        // Open Track File
	}

    // Valid Track
    if ( Fs.is_open( ) )
    {

        getline( Fs, Level );							// Get Level Filename
        getline( Fs, Temp );							// Skip Line
        getline( Fs, Name );							// Get Track Name

        do { getline( Fs, Temp ); }						// Skip Lines
        while ( Temp != "*** Vehicles ***" );			// Found Vehicles

        getline( Fs, Temp );							// Get Number Of Vehicles
        int NumVehicles = atoi( Temp.c_str( ) );		// Convert String To Integer
        getline( Fs, Temp );							// Skip Line

        // Allocate Vehicle Memory
        Vehicles.resize( NumVehicles );

        // Loop Vehicles
        for ( int i = 0; i != NumVehicles; i++ )
        {
            getline( Fs, Temp );							// Skip Line
            getline( Fs, Vehicles[i].Filename );			// Get Vehicle Filename
            getline( Fs, Temp );							// Skip Line

            getline( Fs, Temp );							// Get Truck Position
            Vehicles[i].Position = ParseVector( Temp );	    // Parse Vehicle Position

            getline( Fs, Temp );							// Skip Line
            getline( Fs, Temp );							// Skip Line
            getline( Fs, Temp );							// Skip Line
            getline( Fs, Temp );							// Get Truck Rotation

            Vehicles[i].Rotation = ParseVector( Temp );	// Parse Vehicle Position

            do { getline( Fs, Temp ); }					// Skip Lines
            while ( Temp != "*********************************************" && // Marker
                    Temp != "*** Ramps ***" );			// Found End Of Vehicles

        }

        do { getline( Fs, Temp ); }						// Skip Lines
        while ( Temp != "*** Boxes ***" );				// Found Boxes

        getline( Fs, Temp );							// Get Number Of Vehicles
        int NumModels = atoi( Temp.c_str( ) );			// Convert String To Integer
        getline( Fs, Temp );							// Skip Line

        // Allocate Model Memory
        Models.resize( NumModels );

        // Loop Models
        for ( int i = 0; i != NumModels; i++ )
        {

            getline( Fs, Temp );							// Skip Line
            getline( Fs, Temp );							// Get Model Position
            Models[i].Position = ParseVector( Temp );		// Parse Model Position

            getline( Fs, Temp );							// Skip Line
            getline( Fs, Temp );							// Get Model Position
            Models[i].Rotation = ParseVector( Temp );		// Parse Model Rotation

            getline( Fs, Temp );							// Skip Line

            if ( Temp == "model" )
                getline( Fs, Models[i].Filename );		// Get Model Filename
            else											// Not Model, Box
            {
                Models[i].Filename = "0";					// Set Empty Filename
                getline( Fs, Temp );						// Get Model Dimensions
                Models[i].Dimensions = ParseVector( Temp );// Parse Model Dimensions
            }

            getline( Fs, Temp );							// Skip Line
            getline( Fs, Temp );							// Get Model Mass
            Models[i].Mass = atof( Temp.c_str( ) );			// Convert String To Double

            getline( Fs, Temp );							// Skip Line
            getline( Fs, Temp );							// Skip Line
            getline( Fs, Temp );							// Skip Line
            getline( Fs, Temp );							// Skip Line
            getline( Fs, Temp );							// Skip Line

            getline( Fs, Models[i].TypeFlags );				// Get Model Type, Flags

            getline( Fs, Temp );							// Skip Line
            getline( Fs, Temp );							// Get Model Priority
            Models[i].Priority = atoi( Temp.c_str( ) );		// Convert String To Integer

            do { getline( Fs, Temp ); }						// Skip Lines
            while ( Temp != "*********************************************" &&	// Marker
                    Temp != "*** Cylinders ***");			// Found End Of Boxes

        }

        do { getline( Fs, Temp ); }							// Skip Lines
        while ( Temp != "*** Backdrop ***" );				// Found Backdrop

        getline( Fs, Temp );								// Skip Line
        getline( Fs, Temp );								// Skip Line
        getline( Fs, Temp );								// Skip Line

        getline( Fs, BackDrop );							// Get Backdrop Model

        Fs.close( );		                                // Close Track File

        // Log Parsed Track File
	    cLogger->Text( "Parsed Track " + Filename + ".Sit File", true );

    }
    else
	{
	    // Log Error Parsing Track File
        cLogger->Error( "Error Parsing Track " + Filename + ".Sit File", true );
	}

	// Find Pod Level File Information
	if ( cPodIni->FileInfo( "Levels\\" + Level, PodFile, Size, Offset ) == true )
	{
		PodFile = RootDir + PodFile;						// Build Level Filename
		Fs.open( PodFile.c_str( ) );						// Open Pod File
		Fs.seekg( Offset );									// Seek Level File Offset
	}
	else
	{
		PodFile = RootDir + "Levels\\" + Level;			    // Build Level Filename
		Fs.open( PodFile.c_str( ) );						// Open Level File
	}

    // Valid Level
    if ( Fs.is_open( ) )
    {

        getline( Fs, Temp );								// Skip Line
        getline( Fs, Temp );								// Skip Line

        getline( Fs, Heightmap );							// Get Heightmap Filename
        getline( Fs, TextureInfo );						    // Get Texture Information Filename
        getline( Fs, Palette );							    // Get Palette Filename
        getline( Fs, TextureList );						    // Get Texture List Filename

        // Skip 10 Lines
        for ( unsigned int  i = 0; i != 10; i++) getline( Fs, Temp );	// Skip Lines

        getline( Fs, Lightmap );							// Get Lightmap Filename
        getline( Fs, Temp );								// Get Sun Position
        CVector4 LightPos = ParseVector( Temp );			// Set Sun Position

        WaterHeight = 0;									// Default Water Height
        do { getline( Fs, Temp ); }							// Get Line
        while ( Temp != "!waterHeight" && !Fs.eof( ) );		// Found Marker

        if ( !Fs.eof() )									// Not End Of File
        {
            getline( Fs, Temp );											    // Get Water Level Height
            WaterHeight = (float)atof( Temp.c_str( ) ) / (4.0f * 16.0f);	    // Convert String To Float
        }

        // Log Parsed Level File
	    cLogger->Text( "Parsed Track " + Level + " File", true );

    }
    else
	{
	    // Log Error Parsing Level File
        cLogger->Error( "Error Parsing Track " + Level + " File", true );
	}

    // Build Weather Tints
    BuildWeatherTints( cConfig->Weather );

    // Build Unique Model List
    BuildUniqueList( ListModels );

    // Load Track Resources
    LoadResources( cPodIni, cLogger, cConfig );

    return true;    // Ok

}

// Load Track Resources
bool CTrack::LoadResources( CPodIni *cPodIni, CLogger *cLogger, CConfig *cConfig )
{

    // Copy Track Name
    string TrackName = cConfig->TrackName;

    // Create Texture Object
    cTextures = new CTextures( );

	// Load MetalCr2 Default Palette
	if ( !cTextures->LoadMetalCr2( cPodIni ) )
        cLogger->Error( "Error Loading MetalCr2.Act, Default Palette File", true );
	else
	    cLogger->Text( "Loaded MetalCr2.Act, Default Palette File", true );

	// Load Textures
	cTextures->SetFilter( 2 );
	if ( !cTextures->LoadTextures( TextureList, cLogger, cPodIni ) )
        cLogger->Error( "Error Loading Texture List: " + TextureList, true );
    else
	    cLogger->Text( "Loaded Texture List: " + TextureList, true );

    // Create Ground Box Object
    cGroundBoxes = new CGroundBoxes( );

	// Load Ground Box Heights
	if ( !cGroundBoxes->LoadBoxHeights( TrackName + ".Ra0", TrackName + ".Ra1", cPodIni ) )
        cLogger->Error( "Error Loading Ground Box Height Files: " + TrackName + ".Ra0, " + TrackName + ".Ra1", true );
	else
	    cLogger->Text( "Loaded Ground Box Height Files: " + TrackName + ".Ra0, " + TrackName + ".Ra1", true );

	// Load Ground Box Texture Info
	if ( !cGroundBoxes->LoadTextureInfo( TrackName + ".Cl0", cPodIni ) )
        cLogger->Error( "Error Loading Ground Box Texture Information: " + TrackName + ".Cl0", true );
	else
	    cLogger->Text( "Loaded Ground Box Texture Information: " + TrackName + ".Cl0", true );

    // Create Terrain Object
    cTerrain = new CTerrain( );

	// Load Terrain Heightmap
	if ( !cTerrain->LoadHeightmap( Heightmap, cPodIni ) )
        cLogger->Error( "Error Loading Heightmap File: " + Heightmap, true );
	else
	    cLogger->Text( "Loaded Heightmap File: " + Heightmap, true );

	// Load Terrain Texture Information
	if ( !cTerrain->LoadTextureInfo( TextureInfo, cPodIni ) )
        cLogger->Error( "Error Loading Texture Information File: " + TextureInfo, true );
	else
	    cLogger->Text( "Loaded Texture Information File: " + TextureInfo, true );

	// Load Terrain Lighting Information
	if ( !cTerrain->LoadLightmap( Lightmap, cPodIni ) )
        cLogger->Error( "Error Loading Lighting Information File: " + Lightmap, true );
	else
	    cLogger->Text( "Loaded Lighting Information File: " + Lightmap, true );

	// Build Terrain Display Lists
	if ( !cTerrain->BuildTerrainLists( cTextures, cPodIni, Tint ) )
        cLogger->Error( "Error Building OpenGL Terrain Display Lists", true );
	else
	    cLogger->Text( "Built OpenGL Terrain Display Lists", true );

	// Build GroundBox Display Lists
	if ( !cGroundBoxes->BuildGroundBoxLists( cTextures, cTerrain, cPodIni, Tint ) )
        cLogger->Error( "Error Building OpenGL Ground Boxes Display Lists", true );
	else
	    cLogger->Text( "Built OpenGL Ground Boxes Display Lists", true );

    // Create Models Object
    cModels = new CModels( );

	// Load Models
	if ( !cModels->LoadModels( ListModels, cLogger, cTextures, cPodIni, Tint ) )
        cLogger->Error( "Error Loading Track Models, And Building OpenGL Display Lists", true );
    else
	    cLogger->Text( "Loaded Track Models, Built OpenGL Display Lists", true );

	// Load BackDrop Models
	if ( !cModels->LoadBackDrops( BackDrop, cTextures, cPodIni, cConfig->Weather, Tint ) )
        cLogger->Error( "Error Loading Track Backdrop, And Building OpenGL Display List", true );
	else
	    cLogger->Text( "Loaded Track Backdrop, Built OpenGL Display List", true );

	// Build Water Display Lists
	if ( !cModels->BuildWaterList( Tint, WaterHeight, cTextures, cPodIni ) )
        cLogger->Error( "Error Loading Water Texture, And Building OpenGL Display List", true );
	else
	    cLogger->Text( "Loaded Water Texture, Built OpenGL Display List", true );

    return true;

}

// Build Unique Model List
void CTrack::BuildUniqueList( vector<string> &ListModels )
{

	bool Duplicate = false;		// Duplicate Flag

	// Loop All Boxes
	for ( int x = 0; x != (int)Models.size(); x++ )
	{

		Duplicate = false;	// Default False

		// Loop Previous
		for ( int z = 0; z != (int)ListModels.size(); z++ )
		{
			// Matched Duplicate
			if ( Models[x].Filename.substr(0,Models[x].Filename.find_first_of(".")) ==
				ListModels[z].substr(0,ListModels[z].find_first_of(".")) )
			{
				// Set Flag
				Duplicate = true;
			}
		}
		// Still No Match
		if ( Duplicate != true)
		{
			Models[x].Animated = false;
			ListModels.push_back( Models[x].Filename );
		}
	}

	// Loop Boxes
	for ( int x = 0; x < (int)Models.size(); x++ )
	{
		// Loop List Models
		for ( int z = 0; z < (int)ListModels.size(); z++ )
		{
			if ( Models[x].Filename.substr(0,Models[x].Filename.find_first_of(".")) ==
					ListModels[z].substr(0,ListModels[z].find_first_of(".")) )
			{
				Models[x].BinIndex = z;

			}
		}
	}

}

// Build Weather Tints
void CTrack::BuildWeatherTints( int Weather )
{

	// Switch Day Time
	switch ( Weather )
	{
		case 0:	// Clear
			Tint[0]=1.0f;
			Tint[1]=1.0f;
			Tint[2]=1.0f;
			break;

		case 1:	// Cloudy
			Tint[0]=0.8f;
			Tint[1]=0.8f;
			Tint[2]=0.8f;
			break;

		case 2:	// Fog
			Tint[0]=1.0f;
			Tint[1]=1.0f;
			Tint[2]=1.0f;
			break;

		case 3:	// Dense Fog
			Tint[0]=1.0f;
			Tint[1]=1.0f;
			Tint[2]=1.0f;
			break;

		case 4:	// Rainy
			Tint[0]=0.5f;
			Tint[1]=0.5f;
			Tint[2]=0.5f;
			break;

		case 5:	// Dusk
			Tint[0]=0.8f;
			Tint[1]=0.7f;
			Tint[2]=0.6f;
			break;

		case 6:	// Night
			Tint[0]=0.3f;
			Tint[1]=0.3f;
			Tint[2]=0.3f;
			break;

		case 7:	// Night (Pitch Black)
			Tint[0]=0.1f;
			Tint[1]=0.1f;
			Tint[2]=0.1f;
			break;

		default:// Clear
			Tint[0]=1.0f;
			Tint[1]=1.0f;
			Tint[2]=1.0f;
			break;

	}

}

// Parse X, Y, Z String
CVector4 CTrack::ParseVector( string Line )
{

	// Temp Vector
	CVector4 Tmp;

	// Parse Comma Delimited Values
	Tmp.x = atof( Line.substr(0, Line.find_first_of(",") ).c_str( ) );
	Tmp.y = atof( Line.substr( Line.find_first_of(",")+ 1,Line.find_last_of(",") - 1 - Line.find_first_of(",")+ 1).c_str( ) );
	Tmp.z = atof( Line.substr(Line.find_last_of(",") + 1, Line.length( ) ).c_str( ) );

	return Tmp;

}

