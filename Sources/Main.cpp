
// Include
#include "Shared.h"                     // Shared Header File
#include "CWindowGL.h"                  // OpenGL Window Class Header File
#include "CLogger.h"                    // Logger Class Header File
#include "CTimer.h"                     // Timer Class Header File
#include "CRender.h"                    // Render Class Header File
#include "CConfig.h"                    // Config Class Header File
#include "Resources.h"                  // Resources Header File
#include "CSounds.h"					// Sounds Class Header File

// MTM Include
#include "CPods.h"                      // MTM Pod Ini Class Header File
#include "CTrack.h"                     // MTM Track Class Header File
#include "CTruck.h"                     // MTM Truck Class Header File

// Open Dynamics Engine Include
#include "CVehiclePhysics.h"			// Vehicle Physics Header
#include "CPhysicsServer.h"				// Physics Server Header

// Class Objects
CWindowGL *cWindowGL = NULL;			// OpenGL Window Class Object
CLogger *cLogger = NULL;                // Logger Class Object
CTimer *cTimer = NULL;                  // Timer Class Object
CRender *cRender = NULL;                // Render Class Object
CConfig *cConfig = NULL;                // Config Class Object
CSound cSounds;							// Sound Engine Object

// MTM Class Objects
CPodIni *cPodIni = NULL;                // MTM Pod Ini Class Object
CTrack *cTrack = NULL;                  // MTM Track Class Object
CTruck *cTruck = NULL;                  // MTM Truck Class Object

// ODE Objects
CVehiclePhysics *cVehiclePhysics = NULL;// Vehicle Physics Object
CGenericPhysics *Boxes[512];	        // Boxes Generic Physics Object
CGenericPhysics *GroundBoxes[512];      // Ground Boxes Generic Physics Object

// Variables
string RootDir = "";					// Root Resource Directory
bool Looping = true;			        // Main Loop Flag
bool LoadConfig = false;                // Loaded Config Flag
bool Fullscreen = false;			    // Fullscreen Creation Flag
bool Paused = false;				    // Loop Paused Flag
bool MessagePumpActive = false;         // Message Pump Active Flag
string Version = "Version - 0.4.76";     // MFR Version String

// Structures
Application App;					    // Application Structure
GL_Window Window;					    // Window Structure
KeyEvents Keys;					        // Key Structure
MSG Msg;                                // Window Messages

// Function Prototypes
bool InitializeMFR( HINSTANCE );                            // Initialize MFR
bool DeinitializeMFR( GL_Window *Window );                  // Deinitialize MFR
bool InitializeODE( );                                      // Initialize ODE
bool DeinitializeODE( );                                    // Deinitialize ODE
bool ReloadVehicleODE( );                                   // Reload Vehicle ODE
LRESULT CALLBACK WindowProc( HWND, UINT, WPARAM, LPARAM );  // Window Procedure Callback
BOOL CALLBACK ConfigProc( HWND, UINT, WPARAM, LPARAM );     // Dialog Launcher Callback

// Config Window Message Callbacks
BOOL CALLBACK ConfigProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{

	// Switch Window Messages
	switch( uMsg )													// Message
	{
        // Initialize Dialog
        case WM_INITDIALOG:
		{

			string Tracks, Trucks;

            for ( int i = 0; i != cPodIni->Pods.size( ); i++ )
            {
                for ( int v = 0; v != cPodIni->Pods[i].NumberOfFiles( ); v++ )
                {
                    if ( cPodIni->Pods[i].Filename(v).substr( 0, 6 ) == "TRUCK\\" )
                    {
                        SendMessage( GetDlgItem( hDlg, IDC_LISTTRUCKS ), LB_ADDSTRING, 0, (LPARAM)cPodIni->Pods[i].Filename(v).c_str());
                    }
                    if ( cPodIni->Pods[i].Filename(v).substr( 0, 6 ) == "WORLD\\" )
                    {
                        SendMessage( GetDlgItem( hDlg, IDC_LISTTRACKS ), LB_ADDSTRING, 0, (LPARAM)cPodIni->Pods[i].Filename(v).c_str());
                    }

                }
            }

            // Set Caption, Icon
			SetWindowText( hDlg, "MFR Launcher" );
            SendMessage( hDlg, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(GetModuleHandle(NULL),MAKEINTRESOURCE(IDI_ICON)));

            return true;
		}
		return 0;


        case WM_CLOSE:
            EndDialog(hDlg, 0);
            return TRUE;

		case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{

                case IDC_BTN_CANCEL:
					exit(0);
					return TRUE;

				case IDC_BTN_OK:

                    char buffer[33];

                    int selPos = SendMessage(GetDlgItem(hDlg, IDC_LISTTRUCKS), LB_GETCURSEL, 0, 0);
                    SendMessage(GetDlgItem(hDlg, IDC_LISTTRUCKS), LB_GETTEXT, selPos, (LPARAM)buffer);

                    if ( selPos == -1 )
                        return FALSE;

                    string Truck = buffer;
                    Truck = Truck.substr(6, 256);
                    cConfig->TruckName = Truck.substr(0, Truck.size( ) - 4 );

                    selPos = 0;
                    selPos = SendMessage(GetDlgItem(hDlg, IDC_LISTTRACKS), LB_GETCURSEL, 0, 0);
                    SendMessage(GetDlgItem(hDlg, IDC_LISTTRACKS), LB_GETTEXT, selPos, (LPARAM)buffer);

                    if ( selPos == -1 )
                        return FALSE;

                    string Track = buffer;
                    Track = Track.substr(6, 256);
                    cConfig->TrackName = Track.substr(0, Track.size( ) - 4 );

					EndDialog( hDlg, IDC_BTN_OK );

					return TRUE;
			}
        }
        return 0;
	}

	// Pass Unhandled Messages To DefWindowProc
	return DefWindowProc ( hDlg, uMsg, wParam, lParam);

}

// Main Entry Point
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{

    // Root Resource Directory
    RootDir = "../Resources/";

    // Create New Log Object
    cLogger = new CLogger( );

    // Creation Loop
	while ( Looping == true )
	{

        // Initialize MFR
        if ( InitializeMFR( hInstance ) == false )
        {
            // Deinitialize MFR
            if ( DeinitializeMFR( &Window ) == false )
                // Log Deinitialization Error
                cLogger->Error( "Error Deinitializing MFR", true );
            else
                // Log Deinitialization
                cLogger->Text( "Deinitialized MFR", true );
        }
        else
        {

            // Log Initialization
            cLogger->Text( "Initialized MFR", true );

            // Set Message Pump Flag
            MessagePumpActive = true;

            // Main Game Loop
            while( MessagePumpActive == true )
            {
                // Check Window Messages
                if ( PeekMessage( &Msg, Window.hWnd, 0, 0, PM_REMOVE ) != 0 )
                {
                    // Check Quit Message
                    if ( Msg.message != WM_QUIT )
                    {
                        // Dispatch Message
                        DispatchMessage( &Msg );
                    }
                    else
                    {
                        // Set Message Pump Flag
                        MessagePumpActive = false;
                    }
                }
                else    // No Messages
                {
                    // Check Window Visible Flag
                    if ( Window.Visible == false )
                    {
                        // Window Not Visible, Wait
                        WaitMessage( );
                    }
                    else    // Looping
                    {

                        // Check Paused Flag
                        if ( Paused == false )
                        {

                            // Render Scene
                            cRender->Scene( );

                            // Update Sound
                            if ( cConfig->UseSound == true ) cSounds.Update( );

                            // Swap Double Buffer
                            SwapBuffers( Window.hDC );

                        }

                    }
                }
            }

            // Deinitialize MFR
            if ( DeinitializeMFR( &Window ) == false )
                // Log Deinitialization Error
                cLogger->Error( "Error Deinitializing MFR", true );
            else
                // Log Deinitialization
                cLogger->Text( "Deinitialized MFR", true );

        }
	}

    // Return Message
    return Msg.wParam;

}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	// Get The Window Context
	GL_Window* Window = (GL_Window*)(GetWindowLong (hWnd, GWL_USERDATA));

	// Switch Window Messages
	switch( uMsg )													// Message
	{
		case WM_SYSCOMMAND:											// System Commands
		{
			switch( wParam )										// Switch System Calls
			{
				case SC_SCREENSAVE:									// Screen Saver Starting
				case SC_MONITORPOWER:								// Monitor Power Saving
				return 0;											// Cancel
			}
			break;													// Exit
		}
		return 0;													// Return

		case WM_CREATE:												// Window Creation
		{
			CREATESTRUCT* Creation = (CREATESTRUCT*)(lParam);		// Window Structure Pointer
			Window = (GL_Window*)(Creation->lpCreateParams);
			SetWindowLong (hWnd, GWL_USERDATA, (LONG)(Window));
		}
		return 0;													// Return

		case WM_CLOSE:
		{
			PostMessage ( Window->hWnd, WM_QUIT, 0, 0 );			// Sent WM_QUIT Message
			Looping = false;										// Stop Looping
		}
		return 0;

		case WM_SIZE:												// Window Resized
		{
			switch( wParam )
			{
				case SIZE_MINIMIZED:								// Window Minimized
					Window->Visible = false;						// Set Visible Flag
				return 0;											// Return

				case SIZE_MAXIMIZED:								// Window Maximized
					Window->Visible = true;							// Set Visible Flag
				return 0;

				case SIZE_RESTORED:									// Window Restored
					Window->Visible = true;							// Set Visible Flag
				return 0;

			}
		}
		break;

		case WM_KILLFOCUS:
            Paused = true;                                          // Losing Focus, Pause
            return 0;

		case WM_KEYDOWN:											// Key Down Flag
		{
			switch ( wParam )
			{

				case VK_ESCAPE:										// Escape Key
				{
                    Paused = !Paused;		                                    // Toggle Paused
                    cTimer->Reset( );                                           // Reset Timer
                    if ( cConfig->Fullscreen  == false ) ShowCursor( true );	// Toggle Cursor Visible
				}
				return 0;

				case VK_F2:
				{
					cConfig->Fullscreen = !cConfig->Fullscreen;     	// Fullscreen
					PostMessage( hWnd, WM_QUIT, 0, 0 );                 // Restart Application
				}
				return 0;

				case VK_F6:
				{
				    // Deinitialize ODE
                    DeinitializeODE( );
				    // Reinitialize ODE
                    InitializeODE( );
				}
				return 0;


				case VK_F5:
				{
				    // Reload Vehicle
                    ReloadVehicleODE( );
                    // Reposition Vehicle
                    cVehiclePhysics->PositionVehicle( cTrack->TruckPosition( 0 ), cTrack->TruckRotation( 0 ) );
				}
				return 0;


				case 'R':
				{
                    // Reposition Vehicle
				    const dReal *Pos = cVehiclePhysics->GetPosition( );
				    CVector4 Position; Position.z = Pos[0] * 32 - 8 * 32; Position.y = Pos[1] * 32 + 1; Position.x = Pos[2] * 32 - 8 * 32;
                    cVehiclePhysics->PositionVehicle( Position, cTrack->TruckRotation( 0 ) );
				}
				return 0;

				case 'Q':
				{
					PostMessage ( Window->hWnd, WM_QUIT, 0, 0 );			// Sent WM_QUIT Message
					Looping = false;										// Stop Looping
				}
				return 0;

			}

			if (( wParam >= 0 ) && ( wParam <= 255 ) )				// Bounds
			{
				Window->Keys->Keys[wParam] = true;					// Set Key Down Flag
				return 0;											// Return
			}
		}
		return 0;

		case WM_KEYUP:												// Key Up Flag
		{
			if (( wParam >= 0 ) && ( wParam <= 255 ) )				// Bounds
			{
				Window->Keys->Keys[wParam] = false;					// Set Key Up Flag
				return 0;
			}
		}

	}

	// Pass Unhandled Messages To DefWindowProc
	return DefWindowProc( hWnd, uMsg, wParam, lParam );

}

// Initialize MFR
bool InitializeMFR( HINSTANCE hInstance )
{

    // Log Version
    cLogger->Text( Version );

    // Create New Config Object
    cConfig = new CConfig( );

    // Load Config Once
    if ( LoadConfig == false )
    {
        // Set Flag
        LoadConfig = true;

        // Load Config File
        if ( cConfig->LoadConfig( RootDir + "Config.Xml" ) == false )
            // Log Error Loading Config
            cLogger->Error( "Error Loading Config.Xml", true );
        else
            // Log Loaded Config
            cLogger->Text( "Loaded Config.Xml", true );

    }

    // Create Pod Ini Object
    cPodIni = new CPodIni( );

    // Load Pod.Ini File
    if ( cPodIni->LoadPodIni( RootDir + "Pod.Ini", cLogger, cConfig->UsePodFiles ) == false )
        // Log Loading Pod.Ini Error
        cLogger->Error( "Error Loading Pod.Ini", true );
    else
        // Log Loaded Pod.Ini
        cLogger->Text( "Loaded Pod.Ini File", true );

	// Configuration Dialog Box
	if ( DialogBox( hInstance, MAKEINTRESOURCE( IDD_MAIN ), NULL, (DLGPROC)ConfigProc ) == FALSE )
        exit(0);

    // Save Track/Truck Selection
    cConfig->SaveConfig("Config.Xml");

	// Fillout Application Data
	App.ClassName = "MFR";										// Application Class Name
	App.hInstance = hInstance;									// Application Instance

	// Fillout Window Data
	ZeroMemory( &Window, sizeof( GL_Window ) );					// Zero Memory
	ZeroMemory( &Keys, sizeof( Keys ) );					    // Zero Memory
	Window.Init.App = &App;									    // Application Struct
	Window.Keys = &Keys;                                        // Keys Struct
	Window.Init.Width = cConfig->DisplayWidth;					// Window Width
	Window.Init.Height = cConfig->DisplayHeight;		        // Window Height
	Window.Init.BitsPerPixel = cConfig->DisplayDepth;			// Window Bpp
	Window.Init.Fullscreen = cConfig->Fullscreen;			    // Window Mode
	Window.Init.Vsync = cConfig->UseRefresh;				    // Vertical Sync
    Window.Init.Title = "Monster Freestyle Racing";             // Window Title

    // Create Window Object
    cWindowGL = new CWindowGL( );

	// Register Window Class
	if ( cWindowGL->RegisterWindowGL( &App, WindowProc ) == false )
        // Log Window Registration Error
        cLogger->Error( "Error Registering Window Class", true );
    else
        // Log Window Registration
        cLogger->Text( "Registered Window Class", true );

    // Create OpenGL Window
    if ( cWindowGL->CreateWindowGL( &Window ) == false )
        // Log Window Creation Error
        cLogger->Error( "Error Creating OpenGL Window", true );
    else
        // Log Window Creation
        cLogger->Text( "Created OpenGL Window", true );

	// Log OpenGL Information
    string LogStr;
	LogStr = "OpenGL Renderer: "; LogStr += ( (char *)glGetString(GL_RENDERER));
	cLogger->Text( LogStr, true );
	LogStr = "OpenGL Vendor: "; LogStr += ( (char *)glGetString(GL_VENDOR));
	cLogger->Text( LogStr, true );
	LogStr = "OpenGL Version: "; LogStr += ( (char *)glGetString(GL_VERSION));
	cLogger->Text( LogStr, true );

    // Create Timer Object
    cTimer = new CTimer( );

    // Initialize Timer
    if ( cTimer->Initialize( ) == false )
        // Log Timer Creation Error
        cLogger->Error( "Error Creating Performance Timer", true );
    else
        // Log Timer Creation
        cLogger->Text( "Created Performance Timer", true );

    // Create Track Object
    cTrack = new CTrack( );

    // Load Track File
    if ( cTrack->LoadTrack( cConfig->TrackName, cPodIni, cLogger, cConfig ) == false )
        // Log Loading Track Error
        cLogger->Error( "Error Loading " + cConfig->TrackName + ".Sit", true );
    else
        // Log Loaded Track
        cLogger->Text( "Loaded " + cConfig->TrackName + ".Sit", true );

    // Create Truck Object
    cTruck = new CTruck( );

    // Load Truck File
    if ( cTruck->LoadTruck( cConfig->TruckName, cPodIni ) == false )
        // Log Loading Truck Error
        cLogger->Error( "Error Loading " + cConfig->TruckName + ".Trk", true );
    else
        // Log Loaded Truck
        cLogger->Text( "Loaded " + cConfig->TruckName + ".Trk", true );


    // Create Render Object
    cRender = new CRender( );

    // Initialize Renderer
    if ( cRender->Initialize( cWindowGL, &Window, cConfig, cTimer, cTrack ) == false )
        // Log Renderer Creation Error
        cLogger->Error( "Error Creating Renderer", true );
    else
        // Log Renderer Creation
        cLogger->Text( "Created Renderer", true );

    // Initialize ODE
    if ( InitializeODE( ) == false )
        // Log ODE Initialization Error
        cLogger->Error( "Error Initializing ODE", true );
    else
        // Log ODE Initialization
        cLogger->Text( "Initialized ODE", true );

    // Use Sound Flag
    if ( cConfig->UseSound == true )
    {

        // Initialize Sound
        if ( !cSounds.Init( Window.hWnd, cConfig->Volume, cConfig->EngineSound ) )
            // Log ODE Initialization Error
            cLogger->Error( "Error Initializing Sound", true );
        else
            // Log ODE Initialization
            cLogger->Text( "Initialized Sound", true );

        cSounds.Play( );

    }

    // Hide Mouse Cursor
    //ShowCursor( true );

    return true;    // Ok

}

// Deinitialize MFR
bool DeinitializeMFR( GL_Window *Window )
{

    // Destroy OpenGL Window
    if ( cWindowGL->DestroyWindowGL( Window ) == false )
        // Log Destroy Window Error
        cLogger->Error( "Error Destroying OpenGL Window", true );
    else
        // Log Destroy Window
        cLogger->Text( "Destroyed OpenGL Window", true );


	// Unregister Window Class
	UnregisterClass( App.ClassName, App.hInstance );
    // Log Unregistering Window
    cLogger->Text( "Unregistered Window Class", true );

    // Delete Objects
    SAFE_DELETE( cTimer );
    SAFE_DELETE( cWindowGL );
    SAFE_DELETE( cConfig );
    SAFE_DELETE( cRender );

    // Log Deletion Of Objects
    cLogger->Text( "Deleted Class Objects", true );

    // Delete MTM Objects
    SAFE_DELETE( cPodIni );
    SAFE_DELETE( cTrack );

    // Log Deletion Of Objects
    cLogger->Text( "Deleted MTM Class Objects", true );

    // Deinitialize ODE
    DeinitializeODE( );

    // Delete Logger Object
    SAFE_DELETE( cLogger );

    return true;    // Ok

}

// Initialize ODE
bool InitializeODE( )
{

	// Initialize Physics Server
	if ( !CPhysicsServer::GetInstance( )->Initialize( RootDir + "Physics.Xml", 256, 256 ) )
        cLogger->Error( "Error Initializing Physics Server", true );
	else
	    cLogger->Text( "Initialized Physics Server", true );

    // Create Vehicle Object
	cVehiclePhysics = new CVehiclePhysics( );

	// Register Vehicle With Server
	CPhysicsServer::GetInstance()->RegisterClient( cVehiclePhysics );

	// Create Vehicle
	if ( !(cVehiclePhysics->Create( RootDir + "Physics.Xml", cConfig ) ) )
	    // Log Error Creating Vehicle Physics Client
        cLogger->Error( "Error Creating Vehicle Physics Client", true );
    else
	    // Log Created Vehicle Physics Client
	    cLogger->Text( "Created Vehicle Physics Client", true );

	// Loop Boxes, Calculate Non-Models
	for ( int i = 0; i != cTrack->NumberOfModels(); i++ )
	{
		// Box
		if ( cTrack->ModelName( i ) == "0" )
		{
			// Box Attributes
			PhysGeomData BoxData;
			BoxData.Slip = dReal(CPhysicsServer::GetInstance()->GetSlip( ));

			// Create New Box Object
			Boxes[i] = new CGenericPhysics;

			// Register New Box With Server
			CPhysicsServer::GetInstance()->RegisterClient( Boxes[i] );

			// Get Box Properties
			CVector4 Rotation = cTrack->ModelRotation( i );
			CVector4 Position = cTrack->ModelPosition( i );
			CVector4 BoxDimensions = cTrack->ModelDimensions( i );

			// Create Box
			Boxes[i]->CreateBox( dReal(BoxDimensions.y / 32), dReal(BoxDimensions.z / 32), dReal(BoxDimensions.x / 32), true );
            Boxes[i]->IsModel( false );

			// Set Mass
			Boxes[i]->SetMass( CPhysicsServer::GetInstance()->GetMassScale( ), 1 );
			Boxes[i]->SetGeomData(&BoxData);

			// Set Position, Add To Enviroment
			Boxes[i]->SetPosition( dReal(Position.z / 32 + 8), dReal(Position.y / 32), dReal(Position.x / 32 + 8));
			Boxes[i]->SetRotation( dReal(Rotation.x), dReal(Rotation.y), dReal(Rotation.z), true );

			Boxes[i]->AddEnviroment( );

		}
		else
		{

			// Skip Animated
			if ( cTrack->cModels->Models[cTrack->Models[i].BinIndex].BinId == 0x20 )
				continue;

//            if ( cTrack->Models[i].TypeFlags[0] < 6 )
//                continue;

			if ( cTrack->Models[i].TypeFlags == "6,0" |
				 cTrack->Models[i].TypeFlags == "7,0" |
				 cTrack->Models[i].TypeFlags == "8,0" |
				 cTrack->Models[i].TypeFlags == "9,0" )
				continue;

			// Box Attributesa
			PhysGeomData BoxData;
			BoxData.Slip = dReal(CPhysicsServer::GetInstance()->GetSlip( ));

			// Create New Box Object
			Boxes[i] = new CGenericPhysics;

			// Register New Box With Server
			CPhysicsServer::GetInstance()->RegisterClient( Boxes[i] );

			// Get Box Properties
			CVector4 Rotation = cTrack->ModelRotation( i );
			CVector4 Position = cTrack->ModelPosition( i );

			// Has Mass
			bool Static = true;
			//if ( cTrack->ModelMass( i ) != 0 ) Static = false;

			// Create Box
			Boxes[i]->CreateTriMesh( cTrack, cTrack->Models[i].BinIndex, cTrack->ModelMass( i ), Static );
            Boxes[i]->IsModel( true );

			// Set Mass
			Boxes[i]->SetMass( dReal(CPhysicsServer::GetInstance()->GetMassScale( )), dReal(cTrack->ModelMass( i )) );
			Boxes[i]->SetGeomData(&BoxData);

			// Set Position, Rotation
			Boxes[i]->SetPosition( dReal(Position.z / 32 + 8), dReal(Position.y / 32), dReal(Position.x / 32 + 8));
			Boxes[i]->SetRotation( dReal(Rotation.x), dReal(Rotation.y), dReal(Rotation.z), false );

            // Add To Enviroment
			Boxes[i]->AddEnviroment( );

		}

	}

	int i = 0;	// Box Counter

	// Loop Ground Boxes
	for ( int x = 0; x != 256; x++ )
	{
		for ( int y = 0; y != 256; y++ )
		{

			// No Box
			if ( cTrack->cGroundBoxes->Lower(x,y) == 0 && cTrack->cGroundBoxes->Upper(x,y) == 0 )
				continue;

			i++;	// Increment Box Count

			// Box Attributes
			PhysGeomData BoxData;
			BoxData.Slip = dReal(CPhysicsServer::GetInstance()->GetSlip( ));

			// Create New Box Object
			GroundBoxes[i] = new CGenericPhysics;

			// Register New Box With Server
			CPhysicsServer::GetInstance()->RegisterClient( GroundBoxes[i] );

			// Create Box
			GroundBoxes[i]->CreateBox( 1,cTrack->cGroundBoxes->Upper(x,y)-cTrack->cGroundBoxes->Lower(x,y) ,1, true );

			// Set Mass
			GroundBoxes[i]->SetMass( CPhysicsServer::GetInstance()->GetMassScale( ), 1);
			GroundBoxes[i]->SetGeomData(&BoxData);

			// Set Position, Add To Enviroment
			GroundBoxes[i]->SetRotation( 0,0,0, true );
			GroundBoxes[i]->SetPosition( dReal(x + 8+0.5),(cTrack->cGroundBoxes->Lower(x,y) + cTrack->cGroundBoxes->Upper(x,y))/2, dReal(y + 8+0.5));

			GroundBoxes[i]->AddEnviroment( );

		}
	}

	// Position Vehicle
	cVehiclePhysics->PositionVehicle( cTrack->TruckPosition( 0 ), cTrack->TruckRotation( 0 ) );


    return true;    // Ok

}

// Deinitialize ODE
bool DeinitializeODE( )
{

    // Delete Objects
	SAFE_DELETE( cVehiclePhysics );
    for ( unsigned int i = 0; i != 512; i++ )
    {
        if ( Boxes[i] )
        {
            CPhysicsServer::GetInstance()->UnregisterClient( Boxes[i] );
            SAFE_DELETE( Boxes[i] );
        }
        if ( GroundBoxes[i] )
        {
            CPhysicsServer::GetInstance()->UnregisterClient( GroundBoxes[i] );
            SAFE_DELETE( GroundBoxes[i] );
        }

    }

    // Log Deletion Of Objects
    cLogger->Text( "Deleted ODE Objects", true );

	// Shutdown Server
	CPhysicsServer::GetInstance()->Shutdown( );

    // Log ODE Server Shutdown
    cLogger->Text( "Shutdown Physics Server", true );


    return true;    // Ok

}

// Reload Vehicle ODE
bool ReloadVehicleODE( )
{

	// Unregister Vehicle Physics
	CPhysicsServer::GetInstance()->UnregisterClient( cVehiclePhysics );

    // Delete Vehicle Object
	SAFE_DELETE( cVehiclePhysics );

    // Log Deletion Of Objects
    cLogger->Text( "Deleted ODE Vehicle Object", true );

    // Create Vehicle Object
	cVehiclePhysics = new CVehiclePhysics( );

	// Register Vehicle With Server
	CPhysicsServer::GetInstance()->RegisterClient( cVehiclePhysics );

	// Create Vehicle
	if ( !(cVehiclePhysics->Create( RootDir + "Physics.Xml", cConfig ) ) )
	    // Log Error Creating Vehicle Physics Client
        cLogger->Error( "Error Creating Vehicle Physics Client", true );
    else
	    // Log Created Vehicle Physics Client
	    cLogger->Text( "Created Vehicle Physics Client", true );


}



