
// Define
#ifndef CLOGGER_H
#define CLOGGER_H

// Include
#include "Shared.h"             // Shared Header File

// Logger Class
class CLogger
{

    private:

    public:

        CLogger( );                 // Constructor
        ~CLogger( );                // Destructor

        // Open Log File
        bool OpenLog( string Filename );

        // Close Log File
        bool CloseLog( );

		// Log Text
		void Text( string Text, bool Console = false );

		// Log Error
		void Error( string Error, bool ExitApp = false );


};

#endif  // Endif
