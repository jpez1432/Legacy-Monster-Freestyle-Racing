
// Define
#ifndef CPODS_H
#define CPODS_H

// Include
#include "Shared.h"                 // Shared Header File
#include "CLogger.h"                // Logger Header File

// Pod Class
class CPod
{

    private:

        char Comment[80];				// Pod File Comment

        typedef struct Entry {          // Entry
            char Filename[32];		    // Filename
            int Filesize;				// Filesize
            int FileOffset;			    // File Offset
        };

        vector<Entry> Entries;			// Pod Files

    public:

        CPod( );                        // Constructor
        ~CPod( );                       // Destructor

        // Load Pod File
        bool LoadPod( string Filename );

        // Return Number Of Files
        int NumberOfFiles( void ) { return Entries.size( ); }

        // Return Filename
        string Filename( int Index ) { return Entries[Index].Filename; }

        // Return Filesize
        int Filesize( int Index ) { return Entries[Index].Filesize; }

        // Return File Offset
        int FileOffset( int Index ) { return Entries[Index].FileOffset; }

};

// MTM Pod Ini Class
class CPodIni
{

    public:

        vector<CPod> Pods;					// Pods
        vector<string> Filenames;			// Pod Filenames

        bool UsePods;						// Use Pod Files Flag

    public:

        CPodIni( );              // Constructor
        ~CPodIni( );             // Destructor

        // Load Pod Ini
        bool LoadPodIni( string Filename, CLogger *cLogger, bool UsePods = true );

        // Get File Information
        bool FileInfo( string File, string &Pod, int &Size, int &Offset );

        // Prioritize Pod File
        void Prioritize( string Filename );

};

#endif  // Endif
