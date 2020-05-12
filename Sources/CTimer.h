
// Define
#ifndef CTIMER_H
#define CTIMER_H

// Include
#include "Shared.h"             // Shared Header File

// Timer Class
class CTimer
{

    private:

		__int64 StartClock;					// Start Time
		double Rate_Inv;					// Inverse Frame Time

		double FrameTime;					// Frame Time
		double FrameStart;					// Start Frame
		double FrameEnd;					// End Frame
		double First;						// First Frame
		double FrameTimes;					// Frame Count

    public:

        CTimer( );              // Constructor
        ~CTimer( );             // Destructor

		bool Initialize( void );			                    // Initializes Performance Timer
		bool Reset( void );					                    // Resets Performance Timer
		void Update( void );				                    // Update Performance Timer

		string Fps( void );										// Returns Fps Count String
		double FrameTimer( void );								// Update Frame Timer (Elapsed)
		double FrameInterval( void ) { return FrameTime; }		// Return Frames Elapsed
		double FpsCount( void ) { return 1.0f / FrameTime; }	// Returns Fps Count

};

#endif  // Endif
