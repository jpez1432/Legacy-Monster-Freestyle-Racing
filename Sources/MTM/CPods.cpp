
// Include
#include "CPods.h"                  // MTM Pods Class Header File

extern string RootDir;              // Global Root Resources Directory

// Constructor
CPod::CPod( )
{

}

// Destructor
CPod::~CPod( )
{

	// Empty File Entries
	Entries.empty( );

}

// Constructor
CPodIni::CPodIni( )
{


}

// Destructor
CPodIni::~CPodIni( )
{

	// Empty Pod Vector
	Pods.empty( );

	// Empty Filenames Vector
	Filenames.empty( );

}

// Load Pod File
bool CPod::LoadPod( string Filename )
{

	// Number Of Pods
	int NumFiles;

	// Open File
    ifstream iFile( Filename.c_str( ), ios::binary | ios::in );

    // Validate File
    if ( iFile.is_open( ) )
    {

        // Get Number Of Pods
        iFile.read( (char*)&NumFiles, sizeof( NumFiles ) );

        // Get Pod Comment
        iFile.read( (char*)&Comment, sizeof( Comment ) );

        // Allocate Pod Files
        Entries.resize( NumFiles );

        // Get File Info
        iFile.read( (char*)&Entries[0], NumFiles * sizeof( Entry ) );

        // Close File
        iFile.close( );

        // Convert Uppercase Once
        for ( int i = 0; i != NumFiles; i++ )
        {
            // Convert Upper
            for ( unsigned int j = 0; j != 32; j++)
            {
                Entries[i].Filename[j] = toupper( Entries[i].Filename[j] );
            }
        }

        return true;	// Ok

    }
    else
    {

        // Close File
        iFile.close( );

        return false;   // Error

    }

}

// Load Pod.Ini
bool CPodIni::LoadPodIni( string Filename, CLogger *cLogger, bool UsePods )
{

	// Save Pod Flag
	this->UsePods = UsePods;

	string NumPods;			// Number Of Pods
	string PodFilename;		// Pod Filename

	// Open Pod.Ini
	ifstream iFile( Filename.c_str( ) );

    // Validate Pod.Ini
    if ( iFile.is_open( ) )
    {

        // Get Number Of Pods
        getline( iFile, NumPods );

        // Allocate Filename Memory
        Filenames.resize( atoi( NumPods.c_str( ) ) );

        // Loop Pod File List
        for ( int i = 0; i != atoi( NumPods.c_str( ) ); i++ )
        {

            // Get Pod Filename
            getline( iFile, PodFilename );

            // Set Filename
            Filenames[i] = PodFilename;

        }

        // Close Pod.Ini
        iFile.close( );

    }
    else
    {

        // Close Pod.Ini
        iFile.close( );

        return false;   // Error

    }

	// Allocate Pod Memory
	Pods.resize( atoi( NumPods.c_str( ) ) );

	// Loop Pods
	for ( int i = 0; i != atoi( NumPods.c_str( ) ); i++ )
	{

		// Set Pod Filename
		PodFilename = RootDir + Filenames[i].c_str( );

		// Load Pod
		if ( !Pods[i].LoadPod( PodFilename.c_str( ) ) )
            // Log Error Loading Pod File
            cLogger->Error( "Error Loading Pod File: " + Filenames[i], true );
        else
            // Log Loaded Pod File
            cLogger->Text( "Loaded Pod File: " + Filenames[i], true );

	}

	return true;	// Return Ok

}

// Get File Information
bool CPodIni::FileInfo( string File, string &Pod, int &Size, int &Offset )
{

	// Use Pods Flag
	if ( UsePods == false )
		return false;

	// Convert UpperCase
	string Filename = StringToUpper( File );

	// Loop Pods
	for ( unsigned int i = 0; i != Pods.size( ); i++ )
	{
		// Loop Pod Files
		for ( int j = 0; j != Pods[i].NumberOfFiles( ); j++ )
		{
			if ( Pods[i].Filename( j ) == Filename )
			{

				// Return Info
				Pod = Filenames[i];					// Return Filename
				Size = Pods[i].Filesize( j );		// Return Filesize
				Offset = Pods[i].FileOffset( j );	// Return File Offset
				return true;						// Return Ok
			}
		}
	}

	return false;	// File Not Found

}

// Prioritize Pod File
void CPodIni::Prioritize( string Filename )
{

	// Convert UpperCase
	string File = StringToUpper( Filename );

	// Swap Pod, First Pod
	CPod Swap, First;

	// Copy First Pod
	First = Pods[0];

	// Loop Pods
	for ( unsigned int i = 0; i != Pods.size( ); i++ )
	{
	    // Match Pod
        if ( Filenames[i] == File )
        {
            // Set Swap Pod
            Swap = Pods[i];

            // Set Pods
            Pods[0] = Swap;
            Pods[i] = First;

            return; // Ok
        }
	}

}
