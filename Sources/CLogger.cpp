
// Include
#include "CLogger.h"                    // Logger Class Header File

extern bool Looping;                    // Global Main Looping Flag

// File Stream
ofstream oFile;

// Constructor
CLogger::CLogger( )
{

    // Open Default Log File
    OpenLog( "MFR.Log" );

}

// Destructor
CLogger::~CLogger( )
{

    // Close Log File
    CloseLog( );

}

// Open Log File
bool CLogger::OpenLog( string Filename )
{

	// File Open Already
	if ( oFile.is_open( ) )
	{
		// Close Log File
		oFile.close( );

		// Clear Log File
		oFile.clear( );
	}

    // Open Log File
    oFile.open( Filename.c_str( ), ios::out );

    // Validate File
    if ( oFile.good( ) == false )
    {
        // Error Occured
        return false;
    }

    // Return Ok
    return true;

}

// Close Log File
bool CLogger::CloseLog( )
{


	// Validate File
	if ( oFile.is_open( ) )
	{
        // Close Log
        oFile.close( );

        // Return Ok
        return true;
	}

    // Error Occured
    return false;

}

// Log Text
void CLogger::Text( string Text, bool Console )
{

    // Add Line
	Text += "\r\n";

	// Validate File
	if ( oFile.is_open( ) )
	{
		// Write Text To Log
		oFile.write( Text.c_str( ) , (int)Text.length( ) );

		// Write Console Text
        if ( Console == true ) cout << Text.c_str( );

	}

}

// Log Error
void CLogger::Error( string Error, bool ExitApp )
{

	Error += "\r\n";	// Add New Line

	// Validate File
	if ( oFile.is_open( ) )
	{
		// Write Error To Log
		oFile.write( Error.c_str( ), (int)Error.length( ) );
	}

	// Exit App Flag
	if ( ExitApp == true )
	{
		// Display Error
		MessageBox( NULL, Error.c_str( ), "Error", MB_OK | MB_ICONEXCLAMATION );

		// Write Console Text
        cout << Error.c_str( );

        // Exit
		exit(0);
	}

}


