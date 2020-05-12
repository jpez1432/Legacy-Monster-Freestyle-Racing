
// Include
#include "CTimer.h"                 // Timer Class Header File

// Constructor
CTimer::CTimer( ) : StartClock(0), Rate_Inv(0), FrameTime(0),
					FrameStart(0), FrameEnd(0), First(0), FrameTimes(0)
{


}

// Destructor
CTimer::~CTimer( )
{



}

// Initializes Timer
bool CTimer::Initialize( void )
{

	// Large Integer Rate
	__int64 Rate;

	// Get Accuracy
	if(!QueryPerformanceFrequency((LARGE_INTEGER*)&Rate))
		return false;	// Win Error

	// Rate Normally (1193180)
	if ( !Rate )
		return false;	// Error

	// Calculate Rate Inverse
	Rate_Inv= ( 1.0/(double)Rate );

	// Get Start Clock
	if( !QueryPerformanceCounter( (LARGE_INTEGER*)&StartClock ) )
		return false;	// Error

	// Get Frame Start Time
	FrameStart = FrameTimer();

	// Copy First Frame Time
	First = FrameStart;

	return true;		// True

}

// Resets Timer
bool CTimer::Reset( void )
{

	Initialize( );	// Reinitialize Timer

	return true;	// True

}


// Updates Timer
void CTimer::Update( void )
{

	do										// Loop Frames
	{
		FrameEnd = FrameTimer();			// Get Frame Timer
	} while ( FrameEnd == FrameStart );		// While End != Start

    FrameTime = FrameEnd-FrameStart;		// Find Frametime Difference
    FrameStart = FrameEnd;					// Start To Previous Time

    FrameTimes += FrameTime;				// Increment Frame Times

}

// Returns Fps Count String
string CTimer::Fps( void )
{
	// Character Buffer
	char Buffer[32];

	// Copy Fps To String
	sprintf(Buffer, "Fps - %i", (int)(1.0f / FrameTime ) );

	return Buffer;	// Return Fps String

}


// Updates Elapsed Frame Timer
double CTimer::FrameTimer( void )
{

	// Large Integer End
	__int64 Endclock;

	// Query Performance Timer
	QueryPerformanceCounter( (LARGE_INTEGER*)&Endclock );

	// Return Differnce * Inverse
	return (double)( Endclock-StartClock ) * (double)Rate_Inv;

}
