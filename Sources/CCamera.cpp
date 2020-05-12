
// Include
#include "CCamera.h"					// Camera Class Header File

// Constructor
CCamera::CCamera( )
{

	// Set Position Vectors
	vPosition = CVector4( 0, 0, 0 );
	vVelocity = CVector4( 0, 0, 0 );
	vAcceleration = CVector4( 0, 0, 0 );

	// Set View Vectors
	vDir = CVector4( 0, 0, 1 );
	vSide = CVector4( 1, 0, 0 );
	vUp = CVector4( 0, 1, 0 );

}

// Deconstructor
CCamera::~CCamera( )
{


}

// Position Camera Based On Paramters
void CCamera::PositionCamera( float PosX, float PosY, float PosZ, float ViewX, float ViewY, float ViewZ, float UpX, float UpY, float UpZ )
{

	// Set Position Vector
	vPosition = CVector4( PosX, PosY, PosZ );
	vUp = CVector4( UpX, UpY, UpZ );
	vDir = CVector4( ViewX, ViewY, ViewZ );

	// Reset Velocity & Acceleration
	vVelocity = CVector4( 0, 0, 0 );
	vAcceleration = CVector4( 0, 0, 0 );

	// Set View Vectors
	vSide = CVector4( 1, 0, 0 );

};

// Update Chase Camera
void CCamera::UpdateChase( GL_Window* Window, CVehiclePhysics *cVehiclePhysics,
                           CConfig *cConfig, double FrameInterval )
{

	// Get Geom Position & Rotation
	float Matrix[16];
	const dReal *Rot = cVehiclePhysics->GetRotation( );
	const dReal *Pos = cVehiclePhysics->GetPosition( );

	static float radius = 1.0;
	static float theta  = -(3.0f * PI) / 2.0f;
	static float phi = 1.81803399f;

	float Cx, Cy, Cz;

    if ( Window->Keys->Keys[(cConfig->RotateLeft[0])] )
		theta += 0.5f * FrameInterval;

    if ( Window->Keys->Keys[(cConfig->RotateRight[0])] )
		theta -= 0.5f * FrameInterval;

    if ( Window->Keys->Keys[(cConfig->RotateUp[0])] )
		phi += 0.5f * FrameInterval;

    if ( Window->Keys->Keys[(cConfig->RotateDown[0])] )
		phi -= 0.5f * FrameInterval;

    if ( Window->Keys->Keys[(cConfig->ZoomIn[0])] )
		radius -= 0.01f;

    if ( Window->Keys->Keys[(cConfig->ZoomOut[0])] )
		radius += 0.01f;

	// Crop Angles
	if ( phi >= PI )
		phi = PI;
	else if ( phi <= 1.570796326794896 )
		phi = 1.570796326794896;

	Cx = radius * cosf(theta) * sinf(phi);
	Cy = radius * cosf(phi);
	Cz = radius * sinf(theta) * sinf(phi);

    // Camera Position
	CVector4 Position( Cx, -Cy, Cz);

	// Convert ODE Matrix To OpenGL
	ODEtoOGL( Matrix, Pos, Rot );

	static CVector4 Res;			    // Camera Resultant Vector

	// Calculate Camera Vector
	Res.x = Position.x * Matrix[0] + Position.y * Matrix[4] + Position.z * Matrix[8];
	Res.y = Position.x * Matrix[1] + Position.y * Matrix[5] + Position.z * Matrix[9];
	Res.z = Position.x * Matrix[2] + Position.y * Matrix[6] + Position.z * Matrix[10];

	// Follow Vehicle
	gluLookAt( Pos[0] + Res.x, Pos[1] + Position.y /*+ Res.y*/, Pos[2] + Res.z,
			   Pos[0], Pos[1], Pos[2], 0, 1, 0 );


}

// Update Camera Frame
void CCamera::UpdateFrame( GL_Window* Window, double CamSmooth, double FrameInterval )
{

	// Calculate Current Speed
	double Speed = CamSmooth * FrameInterval;

	double Strafe = 0.0f;
	double Move = 0.0f;

	// Calculate Speeds, Strafe & Move
	Strafe += ( Window->Keys->Keys[KeyLeft] ) ? + Speed : 0.0f;
	Strafe += ( Window->Keys->Keys[KeyRight] ) ? - Speed : 0.0f;
	Move += ( Window->Keys->Keys[KeyBackwards] ) ? - Speed : 0.0f;
	Move += ( Window->Keys->Keys[KeyForwards] ) ? + Speed : 0.0f;

	POINT MousePos;										// Mouse Cursor Position

	// Window Center Screen X/Y Position
	int ScreenHalfX = ( GetSystemMetrics( SM_CXSCREEN ) / 2);
	int ScreenHalfY = ( GetSystemMetrics( SM_CYSCREEN ) / 2);

	GetCursorPos(&MousePos);							// Get Mouse Position
	SetCursorPos(ScreenHalfX, ScreenHalfY);				// Set Mouse Position

	// Calculate Direction Of Movement
	double Rot = (float)( (ScreenHalfX - MousePos.x) ) / ( CamSmooth );		// Scale X
	double Elev = (float)( (ScreenHalfY - MousePos.y) ) / ( CamSmooth );	// Scale Y

	// Clip Values
	Elevation += (Elev * Speed ); // Z Rotation
	Rotation -= ( Rot  * Speed ); // Y Rotation

	// Set Yaw, Pitch
	double Cos_Yaw = (double) cos(Rotation);
	double Sin_Yaw = (double) -sin(Rotation);
	double Cos_Pitch = (double) cos(Elevation);
	double Sin_Pitch = (double) sin(Elevation);

	vSide = CVector4( Cos_Yaw, 0.0f, -Sin_Yaw );								// Calculate Side Vector
	vDir = CVector4( Sin_Yaw * Cos_Pitch, Sin_Pitch, Cos_Yaw * Cos_Pitch );		// Calculate Direction Vector
	vUp.CrossProduct( vDir, vSide );											// Calculate Up Vector

	// Calculate Forces
	float Force = 1000.0f;
	float Drag  = 6.0f;

	CVector4 Accel( 0, 0, 0 );						// Reset Accel Vector

	Accel = Accel + vSide * Strafe;					// Strafe, Left/Right
	Accel = Accel + vDir  * Move;					// Move, Forward/Backwards

	// Add Forces To Vectors
	AddForce(Accel * Force);
	AddForce(vVelocity * -Drag);

	vVelocity = vVelocity + vAcceleration * FrameInterval;
	vAcceleration = CVector4(0, 0, 0);

	//vPosition = vPosition + vVelocity / (MoveSpeed / FrameInterval);
	vPosition = vPosition + vVelocity / ( FrameInterval );

	// Wrap Level Boundaries
	if ( vPosition.x > 256 ) vPosition.x = 0;
	if ( vPosition.y > 256 ) vPosition.y = 0;
	if ( vPosition.z > 256 ) vPosition.z = 0;
	if ( vPosition.x < 0 ) vPosition.x = 256;
	if ( vPosition.y < 0 ) vPosition.y = 256;
	if ( vPosition.z < 0 ) vPosition.z = 256;

};

// Add Force To Acceleration
void CCamera::AddForce( const CVector4 &vForce )
{

	vAcceleration = vAcceleration + vForce;

};

// GluLookAt Camera
void CCamera::LookAt()
{

	// Calculate View Vector
	CVector4 vView = vPosition + vDir;

	gluLookAt( vPosition.x, vPosition.y, vPosition.z,
				vView.x, vView.y, vView.z,
				vUp.x, vUp.y, vUp.z );

};

// Bind Config Movement Keys
bool CCamera::BindKeys( int Forwards, int Backwards, int Left, int Right )
{

	// Bind Key Forwards
	KeyForwards = Forwards;

	// Bind Key Backwards
	KeyBackwards = Backwards;

	// Bind Key Left
	KeyLeft = Left;

	// Bind Key Right
	KeyRight = Right;

	return true;

}
