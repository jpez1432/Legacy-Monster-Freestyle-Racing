
// Define
#ifndef CGENERICPHYSICS_H
#define CGENERICPHYSICS_H

// Include
#include "Shared.h"                     // Common Header File

#include "CPhysicsServer.h"		        // Physics Server Header
#include "CPhysicsClient.h"		        // Physics Client Header

// Generic Physics Class, Inherits Physics Client
class CGenericPhysics : public CPhysicsClient
{

    private:

		dBodyID Body;							// Geom Body
		dGeomID Geom;							// Geom
		PhysGeomData EnvData;					// Geom Data

        bool Model;                             // Model Box Flag
        GLuint ModelId;                         // OpenGL TriMesh Display List

    protected:

    public:

        CGenericPhysics( );                     // Constructor
        virtual ~CGenericPhysics( );            // Destructor

		// Update Simulation
		virtual bool Update( dReal Step );

		// Render
		virtual void Render( );

        // Render Box
        void DrawBox( const float sides[3] );

		// Create Box Dimensions, Dynamic Or Static
		bool CreateBox( dReal Width, dReal Height, dReal Length, bool isStatic = false );

		// Create TriMesh, Dynamic Or Static
		bool CreateTriMesh( CTrack* cTrack, int BinIndex, float Mass, bool isStatic = false );

        // Set Client Mass
		void SetMass( dReal Mass_Scale, dReal mass );

		// Sets Geom Data
		void SetGeomData( const PhysGeomData *pData );

		// Set Client Position
		void SetPosition( dReal x, dReal y, dReal z );

		// Rotate Client
		void SetRotation( dReal x, dReal y, dReal z, bool Box  );

        // Set Model Box Flag
        void IsModel( bool Flag ) { Model = Flag; }

		// Add Client To Space
		void AddEnviroment( );

};

#endif
