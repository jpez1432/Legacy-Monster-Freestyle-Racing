
// Define
#ifndef _CSOUNDS_H
#define _CSOUNDS_H

// Include
#include "Shared.h"					// Shared Header File
#include "CVehiclePhysics.h"		// Vehicle Physics Header File

#include <mmsystem.h>				// Multimedia Header File
#include <dsound.h>					// Direct Sound Header File

typedef unsigned long UL;

#define DSVOLUME_TO_DB(volume) ((DWORD)(-30*(100 - volume)))

// Sound Samples Class
class CSoundSample
{

	public:

		LPDIRECTSOUNDBUFFER Buffer;	// Pointer To Sound Buffer

		// Set Playback Frequency
		void SetFrequency(DWORD Freq) { Buffer->SetFrequency(Freq); }

		// Set Playback Volume
		void SetVolume(LONG Vol) { Buffer->SetVolume(Vol); }

		// Set Playback Pan
		void SetPan(LONG Pan) { Buffer->SetPan(Pan); }

		// Play Sound Sample
		void Play(DWORD Priority, DWORD Flags) { Buffer->Play(0, Priority, Flags); }

		// Stop Sound Sample
		void Stop(void) { Buffer->Stop(); }

};

// Sound Engine Class
class CSoundEngine
{

	public:

		CSoundEngine( );				// Constructor
		~CSoundEngine( ) { Free( ); }	// Deconstructor

		LPDIRECTSOUND DS;				// Direct Sound Object
		int NumSounds;					// Number Of Sounds
		CSoundSample **Sounds;           // Allocated Sounds

		// Allocate New Sound
		CSoundSample * AddSound( void );

		// Free All Sounds
		void Free( void );

		// Create Initial Sound
		bool Create( HWND hWnd );

		// Create Sound From Wave
		CSoundSample* CreateSoundFromWAV( string Filename );

		// Create Sound From Buffer
		CSoundSample* CreateSoundFromBuffer( int Length, int BitsPerSample, void *Buffer );

		// Remove Sound
		void RemoveSound( CSoundSample *Sound );

		// Get Device Caps
		int getCaps( LPDSCAPS DSCaps ) { return DS->GetCaps( DSCaps ); }

};

// Sound Class
class CSound
{

	public:

		CSoundSample* EngineSound;			// Engine Sound
		float EngineFreq;                   // Converts Rad/s Sound Sample Hz
		int MinEngineFreq, MaxEngineFreq;   // Min, Max Engine Sound Sample Hz
		int EngineVol;                      // Engine Volume
		int EngineOn;						// Engine Running

	public:

		// Initialize Sound
		bool Init( HWND hWnd, int Volume, string EngineWAV );

		// Free Sound
		void Free( void );

		// Update Sound
		void Update( void );

		// Stop Sound
		void Stop( void );

		// Play Sound
		void Play( void );

		// Set Engine On
		void SetEngine( bool EngineOn ) { this->EngineOn = EngineOn; }

};


#endif	// End Definex
