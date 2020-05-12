
// Include
#include "CFrustum.h"                 // Frustum Class Header File

// Constructor
CFrustum::CFrustum( )
{


}

// Destructor
CFrustum::~CFrustum( )
{


}

// Calculate Viewing Frustum
void CFrustum::CalculateFrustum( )
{
	GLfloat   Proj[16];
	GLfloat   Modl[16];
	GLfloat   Clip[16];

	//Get OpenGL Projection Matrix
	glGetFloatv( GL_PROJECTION_MATRIX, Proj );

	// Get OpenGL Modelview Matrix
	glGetFloatv( GL_MODELVIEW_MATRIX, Modl );

	// Multiply Projection By Modelview
	Clip[ 0] = Modl[ 0] * Proj[ 0] + Modl[ 1] * Proj[ 4] + Modl[ 2] * Proj[ 8] + Modl[ 3] * Proj[12];
	Clip[ 1] = Modl[ 0] * Proj[ 1] + Modl[ 1] * Proj[ 5] + Modl[ 2] * Proj[ 9] + Modl[ 3] * Proj[13];
	Clip[ 2] = Modl[ 0] * Proj[ 2] + Modl[ 1] * Proj[ 6] + Modl[ 2] * Proj[10] + Modl[ 3] * Proj[14];
	Clip[ 3] = Modl[ 0] * Proj[ 3] + Modl[ 1] * Proj[ 7] + Modl[ 2] * Proj[11] + Modl[ 3] * Proj[15];

	Clip[ 4] = Modl[ 4] * Proj[ 0] + Modl[ 5] * Proj[ 4] + Modl[ 6] * Proj[ 8] + Modl[ 7] * Proj[12];
	Clip[ 5] = Modl[ 4] * Proj[ 1] + Modl[ 5] * Proj[ 5] + Modl[ 6] * Proj[ 9] + Modl[ 7] * Proj[13];
	Clip[ 6] = Modl[ 4] * Proj[ 2] + Modl[ 5] * Proj[ 6] + Modl[ 6] * Proj[10] + Modl[ 7] * Proj[14];
	Clip[ 7] = Modl[ 4] * Proj[ 3] + Modl[ 5] * Proj[ 7] + Modl[ 6] * Proj[11] + Modl[ 7] * Proj[15];

	Clip[ 8] = Modl[ 8] * Proj[ 0] + Modl[ 9] * Proj[ 4] + Modl[10] * Proj[ 8] + Modl[11] * Proj[12];
	Clip[ 9] = Modl[ 8] * Proj[ 1] + Modl[ 9] * Proj[ 5] + Modl[10] * Proj[ 9] + Modl[11] * Proj[13];
	Clip[10] = Modl[ 8] * Proj[ 2] + Modl[ 9] * Proj[ 6] + Modl[10] * Proj[10] + Modl[11] * Proj[14];
	Clip[11] = Modl[ 8] * Proj[ 3] + Modl[ 9] * Proj[ 7] + Modl[10] * Proj[11] + Modl[11] * Proj[15];

	Clip[12] = Modl[12] * Proj[ 0] + Modl[13] * Proj[ 4] + Modl[14] * Proj[ 8] + Modl[15] * Proj[12];
	Clip[13] = Modl[12] * Proj[ 1] + Modl[13] * Proj[ 5] + Modl[14] * Proj[ 9] + Modl[15] * Proj[13];
	Clip[14] = Modl[12] * Proj[ 2] + Modl[13] * Proj[ 6] + Modl[14] * Proj[10] + Modl[15] * Proj[14];
	Clip[15] = Modl[12] * Proj[ 3] + Modl[13] * Proj[ 7] + Modl[14] * Proj[11] + Modl[15] * Proj[15];

	// Extract Right Clipping Plane
	Planes[Right].A = Clip[ 3] - Clip[ 0];
	Planes[Right].B = Clip[ 7] - Clip[ 4];
	Planes[Right].C = Clip[11] - Clip[ 8];
	Planes[Right].D = Clip[15] - Clip[12];

	// Normalize Right Clipping Plane
	Normalize( Right );

	// Extract Left Clipping Plane
	Planes[Left].A = Clip[ 3] + Clip[ 0];
	Planes[Left].B = Clip[ 7] + Clip[ 4];
	Planes[Left].C = Clip[11] + Clip[ 8];
	Planes[Left].D = Clip[15] + Clip[12];

	// Normalize Left Clipping Plane
	Normalize( Left );

	// Extract Bottom Clipping Plane
	Planes[Bottom].A = Clip[ 3] + Clip[ 1];
	Planes[Bottom].B = Clip[ 7] + Clip[ 5];
	Planes[Bottom].C = Clip[11] + Clip[ 9];
	Planes[Bottom].D = Clip[15] + Clip[13];

	// Normalize Bottom Clipping Plane
	Normalize( Bottom );

	// Extract Top Clipping Plane
	Planes[Top].A = Clip[ 3] - Clip[ 1];
	Planes[Top].B = Clip[ 7] - Clip[ 5];
	Planes[Top].C = Clip[11] - Clip[ 9];
	Planes[Top].D = Clip[15] - Clip[13];

	// Normalize Top Clipping Plane
	Normalize( Top );

	// Extract Back Clipping Plane
	Planes[Back].A = Clip[ 3] - Clip[ 2];
	Planes[Back].B = Clip[ 7] - Clip[ 6];
	Planes[Back].C = Clip[11] - Clip[10];
	Planes[Back].D = Clip[15] - Clip[14];

	// Normalize Back Clipping Plane
	Normalize( Back );

	// Extract Front Clipping Plane
	Planes[Front].A = Clip[ 3] + Clip[ 2];
	Planes[Front].B = Clip[ 7] + Clip[ 6];
	Planes[Front].C = Clip[11] + Clip[10];
	Planes[Front].D = Clip[15] + Clip[14];

	// Normalize Front Clipping Plane
	Normalize( Front );

}

// Normalizes Plane
void CFrustum::Normalize( PlaneFaces Plane )
{

	float N;	// Normalize Value

	// Calculate Normalize Value
	N = (GLfloat)sqrt( Planes[Plane].A * Planes[Plane].A + Planes[Plane].B
					   * Planes[Plane].B + Planes[Plane].C * Planes[Plane].C );

	// Normalize Normal & Intercept
	Planes[Plane].A /= N;
	Planes[Plane].B /= N;
	Planes[Plane].C /= N;
	Planes[Plane].D /= N;


}

// Returns 0 Box Outside, 1 Box Partially Inside, 2 Box Completly Inside
int CFrustum::BoxInFrustum( CVector4 Min, CVector4 Max )
{

	// Reset Variables
	int P, C, C2 = 0;

	// Loop Planes
	for( P = 0; P != 6; P++ )
	{
		C = 0;
		if( Planes[P].A * (Min.x) + Planes[P].B * (Min.y) + Planes[P].C * (Min.z) + Planes[P].D > 0 )
			C++;
		if( Planes[P].A * (Max.x) + Planes[P].B * (Min.y) + Planes[P].C * (Min.z) + Planes[P].D > 0 )
			C++;
		if( Planes[P].A * (Min.x) + Planes[P].B * (Max.y) + Planes[P].C * (Min.z) + Planes[P].D > 0 )
			C++;
		if( Planes[P].A * (Max.x) + Planes[P].B * (Max.y) + Planes[P].C * (Min.z) + Planes[P].D > 0 )
			C++;
		if( Planes[P].A * (Min.x) + Planes[P].B * (Min.y) + Planes[P].C * (Max.z) + Planes[P].D > 0 )
			C++;
		if( Planes[P].A * (Max.x) + Planes[P].B * (Min.y) + Planes[P].C * (Max.z) + Planes[P].D > 0 )
			C++;
		if( Planes[P].A * (Min.x) + Planes[P].B * (Max.y) + Planes[P].C * (Max.z) + Planes[P].D > 0 )
			C++;
		if( Planes[P].A * (Max.x) + Planes[P].B * (Max.y) + Planes[P].C * (Max.z) + Planes[P].D > 0 )
			C++;
		if( C == 0 )
			return 0;
		if( C == 8 )
			C2++;
	}
	  return (C2 == 6) ? 2 : 1; //Returns 0 If Outside, 1 If Partially, 2 If ComPletly

}

// Sphere In Frustum Check
int CFrustum::SphereInFrustum( CVector4 Position, float Size )
{

	int P;				// Plane
	double Distance;	// Plane Distance

	// Loop 6 Planes
	for( P = 0; P != 6; P++ )
	{
		// Calculate Distance
		Distance = Planes[P].A * Position.x + Planes[P].B * Position.y + Planes[P].C * Position.z + Planes[P].D;

		// Less Than
		if ( Distance < -Size )
		{
			return 0;	// Outside
		}

	}

	return 1;	// Inside

}
