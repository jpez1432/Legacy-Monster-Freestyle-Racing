
// Include
#include "CPhysicsClient.h"                    // Physics Client Class Header File

// Constructor
CPhysicsClient::CPhysicsClient( )
{


}

// Destructor
CPhysicsClient::~CPhysicsClient( )
{


}

// Initialize Physics Client
bool CPhysicsClient::Init( dWorldID World, dSpaceID Space, dSpaceID EnvSpace )
{
	// World, Space, Enviroment
	if ( World == 0 || Space == 0 || EnvSpace == 0 )
	{
		return false;	// Error
	}

	this->World = World;					// Set World
	this->Space = Space;					// Set Space
	this->EnvSpace = EnvSpace;			    // Set Enviroment

	return true;    // Ok

}
