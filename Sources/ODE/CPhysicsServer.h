
// Define
#ifndef CPHYSICSSERVER_H
#define CPHYSICSSERVER_H

// Include
#include "Shared.h"                     // Common Header File

#include "CPhysicsClient.h"		        // Physics Client Header

#include "CTrack.h"                     // Track Class Header File
#include "CXml.h"                       // Xml Class Header File

// Geom Attributes
struct PhysGeomData
{
	dReal Slip;     // Slip
};

// Physics Server Class
class CPhysicsServer
{

    private:

		// Check Collisions
		void CheckCollisions( void *data, dGeomID o1, dGeomID o2 );

		// Instance
		static CPhysicsServer Instance;

		// List Physics Clients
		list<CPhysicsClient*> ClientList;

		dWorldID World;						// ODE World
		dSpaceID Space;						// World Space
		dSpaceID EnvSpace;					// Enviroment Space
		dJointGroupID ContactGroup;			// Contact Groups
		dGeomID HeightField;				// Heightfield Geom

		PhysGeomData EnvData;				// Enviroment Attributes

		float MassScale;					// World Mass Scale
		float Gravity;						// World Gravity
		float ERP;							// World ERP
		float CFM;							// World CFM
		float Slip;							// World Slip
        float Bounce;                       // World Bounce
        float Soft_CFM;                     // World Soft CFM
        float Soft_ERP;                     // World Soft ERP

		CXmlParser *cXml;		            // Xml Class Object

    public:

        CPhysicsServer( );                  // Constructor
        virtual ~CPhysicsServer( );         // Destructor

		// Initialize Server
		bool Initialize( string Filename, int DimensionsX, int DimensionsZ );

		// Shutdown Server
		bool Shutdown( );

		// Register Pyhsics Client
		bool RegisterClient( CPhysicsClient *Client );

        // Unregister Physics Client
        bool UnregisterClient( CPhysicsClient *Client );

		// Update Simulation
		bool Update( dReal Step );

		// Get Server Instance
		static CPhysicsServer *GetInstance( ) { return &Instance; }

		// Collision Callback
		static void CollisionCallback( void *data, dGeomID o1, dGeomID o2 );

		// Returns Mass Scale
		dReal GetMassScale( void ) { return MassScale; }

		// Returns Slip
		dReal GetSlip( void ) { return Slip; }


};

#endif
