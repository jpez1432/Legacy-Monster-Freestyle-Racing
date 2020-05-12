
// Define
#ifndef CFRUSTUM_H
#define CFRUSTUM_H

// Include
#include "Shared.h"             // Shared Header File
#include "CVector4.h"            // Vector4 Class Header File

// Plane Class
class CPlane
{

	public:

		float A,B,C,D;														// Normal, Intercept
		CPlane( ) : A(0.0f), B(0.0f), C(0.0f), D(0.0f) { }					// Constructor

};

// Frustum Class
class CFrustum
{

	private:

		CPlane Planes[6];													// Clipping Planes

		enum PlaneFaces{ Right=0, Left, Bottom, Top, Back, Front };			// Enumerate Planes
		enum Results{ Outside, Partially, Completly };						// Enumerate BoxInFrustum Results

	public:

		CFrustum();															// Constructor
		~CFrustum();														// Destructor

		void CalculateFrustum( );											// Calculates Viewing Frustum
		void Normalize( PlaneFaces Plane );									// Normalize Plane

		int BoxInFrustum( CVector4 Min, CVector4 Max );						// Checks If Bounding Box Is Visible
		int SphereInFrustum( CVector4 Position, float Size );				// Checks If Bounding Sphere Is Visible

};


#endif  // Endif
