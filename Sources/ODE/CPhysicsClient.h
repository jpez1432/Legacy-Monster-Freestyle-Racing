
// Define
#ifndef CPHYSICSCLIENT_H
#define CPHYSICSCLIENT_H

// Include
#include "Shared.h"                     // Common Header File

// Physics Client Class
class CPhysicsClient
{

    private:

    protected:

		dWorldID	World;				// Client In World
		dSpaceID	Space;				// World Client Space
		dSpaceID 	EnvSpace;			// Enviroment Space

    public:

        CPhysicsClient( );              // Constructor
        virtual ~CPhysicsClient( );     // Destructor

		// Initialize Physics Client
		bool Init( dWorldID World, dSpaceID Space, dSpaceID EnvSpace );

		// Update Simulation
		virtual bool Update( dReal Step ) = 0;

		// Render
		virtual void Render( ) = 0;

};

#endif
