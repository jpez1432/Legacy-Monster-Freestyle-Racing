
// Include
#include "CSounds.h"					    // Sound Engine Class Header

extern string RootDir;					    // Global Root Directory

extern CVehiclePhysics *cVehiclePhysics;	// Global Vehicle Physics Object

extern CSoundEngine* SoundEngine = 0;	    // Global Pointer Sound Engine Class

// Constructor
CSoundEngine::CSoundEngine( )
{

	DS = 0;			// Direct Sound
	NumSounds = 0;	// Number Of Sounds
	Sounds = 0;		// Sounds

}

// Create Sound Engine
bool CSoundEngine::Create( HWND hWnd )
{

	// Create Direct Sound Object
	if ( DirectSoundCreate( 0, &DS, 0 ) != DS_OK )
		return false;	// Error Occured

	// Set Priority
	if ( DS->SetCooperativeLevel( hWnd, DSSCL_EXCLUSIVE ) != DS_OK )
		return false;	// Error Occured

	// Set Format Primary Buffer
	LPDIRECTSOUNDBUFFER DSBPrimary;
	DSBUFFERDESC dsbd;
	ZeroMemory( &dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = DSBCAPS_PRIMARYBUFFER;
	dsbd.dwBufferBytes = 0;
	dsbd.lpwfxFormat = 0;

	if ( DS->CreateSoundBuffer( &dsbd, &DSBPrimary, 0 ) != DS_OK )
		return false;

	WAVEFORMATEX wfx;
	ZeroMemory( &wfx, sizeof(WAVEFORMATEX ));
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = 1;
	wfx.nSamplesPerSec = 44100;
	wfx.wBitsPerSample = 16;
	wfx.nBlockAlign = (wfx.wBitsPerSample >> 3 ) * wfx.nChannels;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

	if ( DSBPrimary->SetFormat(&wfx) != DS_OK )
		return false;

	DSBPrimary->Release();

	return false;

}

CSoundSample* CSoundEngine::AddSound( void )
{
	CSoundSample **NewSounds = new CSoundSample*[NumSounds + 1];

	if ( !NewSounds )
		return 0;

	if ( Sounds )
	{
		memcpy( NewSounds, Sounds, sizeof(CSoundSample * ) * NumSounds );
		delete [] Sounds;
	}

	Sounds = NewSounds;

	CSoundSample *Handle = new CSoundSample;

	if ( !Handle )
		return 0;

	Sounds[NumSounds] = Handle;
	NumSounds++;

	Handle->Buffer = 0;
	return Handle;

}

void CSoundEngine::Free( void )
{

	if ( Sounds )
	{
		for ( int i = 0; i < NumSounds; i++ )
		{
			if ( Sounds[i] )
			{
				if ( Sounds[i]->Buffer )
				{
					int RefCount = Sounds[i]->Buffer->Release();
				}
				delete Sounds[i];
			}
		}
		delete [] Sounds;
		Sounds = 0;

	}
	NumSounds = 0;

	if ( DS )
	{
		int RefCount = DS->Release( );
		DS = 0;
	}

}

CSoundSample * CSoundEngine::CreateSoundFromWAV( string Filename )
{

	CSoundSample *Handle = AddSound( );
	if ( !Handle )
		return 0;

	FILE *fp = fopen( Filename.c_str( ), "rb" );

    if (!fp)
        return 0;
    char type[4];
    if (fread(type, 1, 4, fp) != 4)
        return 0;
    if (memcmp(type, "RIFF", 4))
        return 0;
    UL chunkLen;
    if (fread(&chunkLen, sizeof(UL), 1, fp) != 1)
        return 0;
    if (fread(type, 1, 4, fp) != 4)
        return 0;
    if (memcmp(type, "WAVE", 4))
        return 0;
    if (fread(type, 1, 4, fp) != 4)
        return 0;
    if (memcmp(type, "fmt ", 4))
        return 0;
    if (fread(&chunkLen, sizeof(UL), 1, fp) != 1)
        return 0;
    if (chunkLen != sizeof(PCMWAVEFORMAT))
        return 0;

	PCMWAVEFORMAT WaveFormat;
	if ( fread(&WaveFormat, sizeof(PCMWAVEFORMAT), 1,fp ) != 1 )
		return 0;

	if ( WaveFormat.wf.wFormatTag != WAVE_FORMAT_PCM )
		return 0;

	if ( fread(type, 1, 4, fp ) != 4 )
		return 0;

	if ( memcmp(type, "data", 4 ) )
		return 0;

	UL sampleLen;
	if ( fread(&sampleLen, sizeof(UL), 1, fp ) != 1)
		return 0;

    DSBUFFERDESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.dwSize = sizeof(desc);
    desc.dwFlags = DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLVOLUME | DSBCAPS_STATIC;
    desc.dwBufferBytes = sampleLen;
    desc.lpwfxFormat = (LPWAVEFORMATEX) &WaveFormat;

	if ( DS->CreateSoundBuffer(&desc, &Handle->Buffer, 0) != DS_OK)
        return 0;

    void *audioPtr1, *audioPtr2;
    DWORD audioBytes1, audioBytes2;

	if (Handle->Buffer->Lock(0, 0, &audioPtr1, &audioBytes1, &audioPtr2, &audioBytes2, DSBLOCK_ENTIREBUFFER) != DS_OK)
        return 0;

    int error = 0;

    if (audioBytes1 != sampleLen)
        error = 1;

    else if (fread(audioPtr1, 1, sampleLen, fp) != sampleLen)
        error = 1;

    fclose(fp);

    if (error)
	{
        Handle->Buffer->Unlock(audioPtr1, 0, audioPtr2, 0);
        return 0;
    }

    if (Handle->Buffer->Unlock(audioPtr1, sampleLen, audioPtr2, 0) != DS_OK)
        return 0;

	return Handle;

}

CSoundSample * CSoundEngine::CreateSoundFromBuffer(int Length, int BitsPerSample, void *Buffer)
{
    CSoundSample *Handle = AddSound( );

    if (!Handle)
        return 0;

    PCMWAVEFORMAT waveFormat;
    waveFormat.wf.wFormatTag = WAVE_FORMAT_PCM;
    waveFormat.wf.nChannels = 1;
    waveFormat.wBitsPerSample = BitsPerSample;
    waveFormat.wf.nSamplesPerSec = 44100;
    waveFormat.wf.nBlockAlign = waveFormat.wf.nChannels * (waveFormat.wBitsPerSample >> 3);
    waveFormat.wf.nAvgBytesPerSec = waveFormat.wf.nBlockAlign * waveFormat.wf.nSamplesPerSec;

    DSBUFFERDESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.dwSize = sizeof(desc);
    desc.dwFlags = DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLVOLUME | DSBCAPS_STATIC;
    UL sampleLen = Length * (BitsPerSample >> 3);
    desc.dwBufferBytes = sampleLen;
    desc.lpwfxFormat = (LPWAVEFORMATEX) &waveFormat;

	if ( DS->CreateSoundBuffer(&desc, &Handle->Buffer, 0) != DS_OK )
        return 0;

    void *audioPtr1, *audioPtr2;
    DWORD audioBytes1, audioBytes2;

    if (Handle->Buffer->Lock(0, 0, &audioPtr1, &audioBytes1, &audioPtr2, &audioBytes2, DSBLOCK_ENTIREBUFFER) != DS_OK)
        return 0;

    if (audioBytes1 != sampleLen)
	{
        Handle->Buffer->Unlock(audioPtr1, 0, audioPtr2, 0);
        return 0;
    }

    memcpy(audioPtr1, Buffer, sampleLen);

    if (Handle->Buffer->Unlock(audioPtr1, sampleLen, audioPtr2, 0) != DS_OK)
        return 0;

    return Handle;
}

void CSoundEngine::RemoveSound(CSoundSample *Sound)
{

	int found = -1;
    for ( int i = 0; i < NumSounds; i++) {
        if (Sounds[i] == Sound) {
            found = i;
            break;
        }
    }
    if (found == -1)
        return;

    NumSounds--;
    Sounds[found] = Sounds[NumSounds];
    CSoundSample **newSounds = new CSoundSample*[NumSounds];
    memcpy(newSounds, Sounds, sizeof(CSoundSample *) * NumSounds);
    delete [] Sounds;
    Sounds = newSounds;

    if (Sound->Buffer)
        Sound->Buffer->Release();
}

bool CSound::Init( HWND hWnd, int Volume, string EngineWAV )
{

	SoundEngine = new CSoundEngine;

	if ( SoundEngine->Create( hWnd ) )
		return false;

    string Filename = RootDir + EngineWAV;

	EngineSound = SoundEngine->CreateSoundFromWAV( Filename );

	if ( !EngineSound )
		return false;

	MinEngineFreq = 16000 ;
	MaxEngineFreq = 28000;
	EngineVol = Volume;
	EngineOn = 1;

	return true;

}

void CSound::Free( void )
{

	if ( SoundEngine )
	{
		delete SoundEngine;
		SoundEngine = 0;
	}

}

void CSound::Play( void )
{

	EngineSound->Play( 0, DSBPLAY_LOOPING );

	if ( EngineOn )
		EngineSound->SetVolume( DSVOLUME_TO_DB(EngineVol) );
	else
		EngineSound->SetVolume( DSBVOLUME_MIN );

    EngineSound->SetFrequency( MinEngineFreq );

}

void CSound::Stop( void )
{

	EngineSound->Stop( );

}

void CSound::Update( void )
{

	static int Freq = MinEngineFreq;
	Freq = cVehiclePhysics->Gas;

	if ( Freq < MinEngineFreq )
		Freq = MinEngineFreq;
	else if  ( Freq > MaxEngineFreq )
		Freq = MaxEngineFreq;

	EngineSound->SetFrequency( Freq );

    EngineVol = cVehiclePhysics->Speed();

	if ( EngineOn )
		EngineSound->SetVolume( DSVOLUME_TO_DB(EngineVol) );
	else
		EngineSound->SetVolume( DSBVOLUME_MIN );

}
