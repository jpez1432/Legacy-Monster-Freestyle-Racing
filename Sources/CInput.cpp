
// Include
#include "CInput.h"         // Input Class Header File

// Joystick Callbacks
BOOL CALLBACK EnumAxesCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext );
BOOL CALLBACK EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance, VOID* pContext );

// Direct X Input
LPDIRECTINPUT8       DI;			    // Direct Input
LPDIRECTINPUTDEVICE8 Joystick;		    // Joystick
DIDEVCAPS            DevCaps;		    // Device Caps

// Buttons
bool JoyButtons[13];				    // JoyStick Buttons

// Keyboard
extern KeyEvents Keys;				    // Global Key Structure

// Constructor
CInput::CInput( )
{



}

// Overload Constructor
CInput::CInput( HWND hWnd )
{

    // Initialize DirectX Input
    InitializeDirectInput( hWnd );

}

// Destructor
CInput::~CInput( )
{

    // Deinitialize DirectX Input
    DeinitializeDirectInput( );

}
// Initialize Direct Input
bool CInput::InitializeDirectInput( HWND hWnd )
{

    HRESULT hr;	// Result

	// Create Direct Input
    if( FAILED( hr = DirectInput8Create( GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&DI, NULL ) ) )
	{	// Error Creating Direct Input
		MessageBox( NULL, "Error Creating Direct Input", "Error", MB_OK | MB_ICONEXCLAMATION );
        return false;	// Error Occured
	}

    // Enumerate Joysticks
    if( FAILED( hr = DI->EnumDevices( DI8DEVCLASS_GAMECTRL,  EnumJoysticksCallback, NULL, DIEDFL_ATTACHEDONLY ) ) )
	{   // Error Enumerating Joystick
		MessageBox( NULL, "Joystick not found. Please Check Config.Cfg", "Error", MB_OK | MB_ICONEXCLAMATION );
        return false;	// Error Occured
	}

    //Validate Joystick
    if( NULL == Joystick )
    {	// Error Getting Joystick
        MessageBox( NULL, "Joystick not found. Please Check Config.Cfg", "Error", MB_OK | MB_ICONEXCLAMATION );
        return false;	// Return Error
    }

	// Set Data Format
    if( FAILED( hr = Joystick->SetDataFormat( &c_dfDIJoystick2 ) ) )
    {	// Error Setting Data Format
        MessageBox( NULL, "Error Setting Joystick Data Format", "Error", MB_OK | MB_ICONEXCLAMATION );
        return false;	// Return Error
    }

	// Set Cooperative Level
    if( FAILED( hr = Joystick->SetCooperativeLevel( hWnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND ) ) )
    {	// Error Setting Cooperative Level
        MessageBox( NULL, "Error Setting Joystick Cooperative Level", "Error", MB_OK | MB_ICONEXCLAMATION );
        return false;	// Return Error
    }

	// Get Joystick Capabilities
    DevCaps.dwSize = sizeof(DIDEVCAPS);
    if ( FAILED( hr = Joystick->GetCapabilities( &DevCaps ) ) )
    {	// Error Retrieving Capabilities
        MessageBox( NULL, "Error Retrieving Joystick Capabilities", "Error", MB_OK | MB_ICONEXCLAMATION );
        return false;	// Return Error
    }

	// Enumerate Joystick Axis
    if ( FAILED( hr = Joystick->EnumObjects( EnumAxesCallback, (VOID*)hWnd, DIDFT_AXIS ) ) )
    {	// Error Enumerating Joystick Axis
        MessageBox( NULL, "Error Enumerating Joystick Axis", "Error", MB_OK | MB_ICONEXCLAMATION );
        return false;	// Return Error
    }

    return true;	// Return Ok
}

// Deinitialize Direct Input
void CInput::DeinitializeDirectInput()
{

	// Validate Joystick
	if( Joystick )
        Joystick->Unacquire();	// Unaquire

    // Release DirectInput Objects
    SAFE_RELEASE( Joystick );
    SAFE_RELEASE( DI );

}


// Enumerate Joysticks
BOOL CALLBACK EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance, VOID* pContext )
{
    HRESULT hr;	// Result

    // Obtain Interface To Joystick
    hr = DI->CreateDevice( pdidInstance->guidInstance, &Joystick, NULL );

	// Failed
	if( FAILED( hr ) )
        return DIENUM_CONTINUE;	// Continue Enumeration

	// Found Joystick
	return DIENUM_STOP;		// Return
}

// Enumerate Joystick Axis
BOOL CALLBACK EnumAxesCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext )
{

    DIPROPRANGE diprg;		// Axis Range
    diprg.diph.dwSize       = sizeof(DIPROPRANGE);	// Structure Size
    diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER); // Header Structure Size
    diprg.diph.dwHow        = DIPH_BYID;			// How
    diprg.diph.dwObj        = pdidoi->dwType;		// Specifiy Enumerate Axis
    diprg.lMin              = -10;					// Min Range
    diprg.lMax              = +10;					// Max Range

	// Set Axis Range
	if( FAILED( Joystick->SetProperty( DIPROP_RANGE, &diprg.diph ) ) )
		return DIENUM_STOP;		// Error Occured

    return DIENUM_CONTINUE;		// Return
}

// Update Input State
HRESULT CInput::UpdateInputState( )
{


    HRESULT     hr;		// Result
    DIJOYSTATE2 js;     // Direct Input Joystick State

	// Validate Joystick
    if ( NULL == Joystick )
        return S_OK;	// Error

    // Poll Device
    hr = Joystick->Poll();

	// Error Occured
    if( FAILED(hr) )
    {
		// Reaquire Joystick
		hr = Joystick->Acquire();

		// Loop Until Aquired
        while( hr == DIERR_INPUTLOST )
            hr = Joystick->Acquire();	// Aquire Joystick

		// Try Again
        return S_OK;
    }

    // Get Input Device States
    if( FAILED( hr = Joystick->GetDeviceState( sizeof(DIJOYSTATE2), &js ) ) )
        return hr; // Return Error


	for ( int i = 0; i != 13; i++ )
	{
		if ( js.rgbButtons[i] & 0x80 )
		{
			JoyButtons[i] = true;			// JoyStick Button Down
		}
		else
			JoyButtons[i] = false;			// JoyStick Button Up
	}


	// Calculate Gas, Brake, Steer
	if ( js.lX <=  -10 ) Keys.Keys['F'] = true; else Keys.Keys['F'] = false;	    // Turn Front Steer Left
	if ( js.lX >= 0 ) Keys.Keys['H'] = true; else Keys.Keys['H'] = false; 	    // Turn Front Steer Right

	//if ( js.lX <=  -10 ) Keys.Keys['W'] = true; else Keys.Keys['W'] = false;	    // Turn Front Steer Left
	//if ( js.lX >= 10 ) Keys.Keys['S'] = true; else Keys.Keys['S'] = false; 	    // Turn Front Steer Right

	if ( js.lY <=  -10 ) Keys.Keys['W'] = true; else Keys.Keys['W'] = false;	    // Turn Front Steer Left
	if ( js.lY >= 0 ) Keys.Keys['S'] = true; else Keys.Keys['S'] = false; 	    // Turn Front Steer Right

	if ( JoyButtons[0] == true ) Keys.Keys['A'] = true; else Keys.Keys['A'] = false;	    // Turn Front Steer Left
	if ( JoyButtons[3] == true ) Keys.Keys['D'] = true; else Keys.Keys['D'] = false; 	    // Turn Front Steer Right


//    // Gas
//	if ( JoyButtons[5] == true ) Keys.Keys['W'] = true; else Keys.Keys['W'] = false;
//
//	// Brake
//	if ( JoyButtons[4] == true ) Keys.Keys['S'] = true; else Keys.Keys['S'] = false;

//    // Gas
//	if ( JoyButtons[0] == true ) Keys.Keys['F'] = true; else Keys.Keys['F'] = false;
//
//	// Brake
//	if ( JoyButtons[3] == true ) Keys.Keys['H'] = true; else Keys.Keys['H'] = false;


    return S_OK;
;

}
