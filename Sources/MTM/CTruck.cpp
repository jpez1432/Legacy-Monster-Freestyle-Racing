
// Include
#include "CTruck.h"                     // Truck Class Header File

extern string RootDir;				    // Global Root Directory

// Constructor
CTruck::CTruck( )
{


}

// Destructor
CTruck::~CTruck( )
{


}

// Load MTM Truck File
bool CTruck::LoadTruck( string Filename, CPodIni *cPodIni )
{
	string Pod;			// Pod Filename
	int Size;			// File Size
	int Offset;			// File Offset

	ifstream Fs;		// File Stream
	string Line;		// Temp String

	if ( cPodIni->FileInfo("Truck\\" + Filename + ".Trk", Pod, Size, Offset ) == true )
	{
		// Build Pod Filename
		Line =  RootDir + Pod;
		// Open Pod File
		Fs.open( Line.c_str( ) );
		// Seek File Offset
		Fs.seekg( Offset );

	// Read File Dir
	} else {

		// Build Truck Filename
		Line =  RootDir + "Truck\\" + Filename + ".Trk";
		// Open Track
		Fs.open( Line.c_str( ) );
	}

    // Pod Valid
    if ( Fs.is_open( ) )
    {

        getline( Fs, Line );				// Skip Line
        getline( Fs, TruckName );			// Read Truck Name
        getline( Fs, Line );				// Skip Line
        getline( Fs, ModelName );			// Read Truck Model Name
        getline( Fs, Line );				// Skip Line
        getline( Fs, TireName );			// Read Tire Name
        getline( Fs, Line );				// Skip Line
        getline( Fs, AxleName );			// Read Axle Name

        for (int i = 0; i < 4; i++ )		// Loop Lines
                getline( Fs, Line );		// Skip Lines

        getline( Fs, Line );
        getline( Fs, Line );
        AxleBarOffset = ParseVector( Line );

        getline( Fs, Line );
        getline( Fs, Line );

        // Read Tire X Positions
        getline( Fs, Line );
        getline( Fs, Line );
        FRTire.x = atof( Line.c_str( ) );

        getline( Fs, Line );
        getline( Fs, Line );
        FLTire.x = atof( Line.c_str( ) );

        getline( Fs, Line );
        getline( Fs, Line );
        RRTire.x = atof( Line.c_str( ) );

        getline( Fs, Line );
        getline( Fs, Line );
        RLTire.x = atof( Line.c_str( ) );

        // Read Tire Y Positions
        getline( Fs, Line );
        getline( Fs, Line );
        FRTire.y = atof( Line.c_str( ) );

        getline( Fs, Line );
        getline( Fs, Line );
        FLTire.y = atof( Line.c_str( ) );

        getline( Fs, Line );
        getline( Fs, Line );
        RRTire.y = atof( Line.c_str( ) );

        getline( Fs, Line );
        getline( Fs, Line );
        RLTire.y = atof( Line.c_str( ) );

        // Read Tire Z Positions
        getline( Fs, Line );
        getline( Fs, Line );
        FRTire.z = atof( Line.c_str( ) );

        getline( Fs, Line );
        getline( Fs, Line );
        FLTire.z = atof( Line.c_str( ) );

        getline( Fs, Line );
        getline( Fs, Line );
        RRTire.z = atof( Line.c_str( ) );

        getline( Fs, Line );
        getline( Fs, Line );
        RLTire.z = atof( Line.c_str( ) );

        // Read Scrape Points
        for ( int i = 0; i != 13; i++ )
        {
            getline( Fs, Line );						// Skip Marker
            getline( Fs, Line );						// Get Scrape Point
            ScrapePoints[i] = ParseVector( Line );		// Parse Scrape Vector
        }

        // Close File
        Fs.close( );

    }
    else
    {

        // Close File
        Fs.close( );

        return false;   // Error

    }

	// Load Tires
	if ( !LoadTruckModels( ModelName, TireName, AxleName, cPodIni ) )
		return false;

	return true;	// Ok

}

// Load Truck & Tire Models
bool CTruck::LoadTruckModels( string TruckModel, string TireModel, string AxleModel, CPodIni *cPodIni )
{

	float Tint[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	// Extension String
	string  Ext = "";

	// Add Necessary Bin Extension
	if ( StringToUpper(TruckModel).find(".BIN") == string::npos )
		Ext = ".BIN";

	// Load Bin Model
	if ( !cTruckModel.LoadBin( TruckModel + Ext, cPodIni, Tint ) )
		return false;	// Error

	// Build OpenGL Model Display List
	if ( !cTruckModel.BuildDisplayList( Tint ) )
		return false;	// Error

	// Reset Extension
	Ext = "";

	// Add Necessary Bin Extension
	if ( StringToUpper(TireModel).find(".BIN") == string::npos )
		Ext = ".BIN";

	// Load Bin Model
	if ( !cLeftTire.LoadBin( TireModel + "16L" + Ext, cPodIni, Tint ) )
		return false;	// Error

	// Build OpenGL Model Display List
	if ( !cLeftTire.BuildDisplayList( Tint ) )
		return false;	// Error

	// Load Bin Model
	if ( !cRightTire.LoadBin( TireModel + "16R" + Ext, cPodIni, Tint ) )
		return false;	// Error

	// Build OpenGL Model Display List
	if ( !cRightTire.BuildDisplayList( Tint ) )
		return false;	// Error

	// Reset Extension
	Ext = "";

	// Add Necessary Bin Extension
	if ( StringToUpper(AxleModel).find(".BIN") == string::npos )
		Ext = ".BIN";

	// Load Bin Model
	if ( !cAxleModel.LoadBin( AxleModel + Ext, cPodIni, Tint ) )
		return false;	// Error

	// Build OpenGL Model Display List
	if ( !cAxleModel.BuildDisplayList( Tint ) )
		return false;	// Error

	return true;	// True

}

// Parse X, Y, Z String
CVector4 CTruck::ParseVector( string Line )
{

	// Temp Vector
	CVector4 Tmp;

	// Parse Comma Delimited Values
	Tmp.x = atof( Line.substr(0, Line.find_first_of(",") ).c_str( ) );
	Tmp.y = atof( Line.substr( Line.find_first_of(",")+ 1,Line.find_last_of(",") - 1 - Line.find_first_of(",")+ 1).c_str( ) );
	Tmp.z = atof( Line.substr(Line.find_last_of(",") + 1, Line.length( ) ).c_str( ) );

	return Tmp;

}

