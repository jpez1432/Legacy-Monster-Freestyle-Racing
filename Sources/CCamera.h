
// Define
#ifndef CCAMERA_H
#define CCAMERA_H

// Include
#include "Shared.h"                 // Common Header File
#include "CVector4.h"				// Vector4 Class Header File
#include "CWindowGL.h"              // OpenGL Window Class Header File
#include "CConfig.h"                // Config Class Header File

// ODE Include
#include "CVehiclePhysics.h"        // Vehicle Physics Class Header File

// Camera Class
class CCamera
{

	private:

		// Camera Position
		CVector4 vVelocity;
		CVector4 vAcceleration;
		CVector4 vPosition;

		// Camera View
		CVector4 vDir;
		CVector4 vSide;
		CVector4 vUp;

		// Mouse Movement
		double Elevation;
		double Rotation;

		int KeyForwards;					// Move Forward Key
		int KeyBackwards;					// Move Backward Key
		int KeyLeft;						// Move Left Key
		int KeyRight;						// Move Right Key

	public:

		CCamera( );						// Constructor
		~CCamera( );					// Deconstructor

		// Return Camera Position
		CVector4 Position( ) { return vPosition; }

		// Set Camera Position
		void PositionCamera( float PosX, float PosY, float PosZ, float ViewX, float ViewY, float ViewZ, float UpX, float UpY, float UpZ );

		// Add Push Force (Inline)
		void AddForce( const CVector4 &vForce );

		// Update Camera Data
		void UpdateFrame( GL_Window* Window, double CamSmooth, double FrameInterval );

		// Update Chase Camera Data
		void UpdateChase( GL_Window* Window, CVehiclePhysics *cVehiclePhysics,
                          CConfig *cConfig, double FrameInterval );

		// GluLookat
		void LookAt( );

		// Bind Config Movement Keys
		bool BindKeys( int Forwards, int Backwards, int Left, int Right );




};

#endif
