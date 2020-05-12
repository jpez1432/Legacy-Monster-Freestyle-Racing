
// Include
#include "CPhysicsServer.h"                    // Physics Server Class Header File

extern CTrack *cTrack;                         // Track Class Object

// Create Static Instance
CPhysicsServer CPhysicsServer::Instance;

// Constructor
CPhysicsServer::CPhysicsServer( )
{


}

// Destructor
CPhysicsServer::~CPhysicsServer( )
{


}

// HeightField Callback
dReal HeightField_Callback( void* pUserData, int x, int z )
{

	// Return Height
	return (dReal) cTrack->cTerrain->Height( x, z );

}

// Initialize Server
bool CPhysicsServer::Initialize( string Filename, int DimensionsX, int DimensionsZ )
{

    // Create New Parser Object
    cXml = new CXmlParser( );

	// Parse Physics File
	if ( !cXml->ParseFile( Filename.c_str( ) ) )
		return false;	// Error

	// Read World Attributes
	Gravity = ( atof( cXml->FindValue( "WORLD","GRAVITY", cXml->GetNode( ) ).c_str( ) ) );
	ERP = ( atof( cXml->FindValue( "WORLD","ERP", cXml->GetNode( ) ).c_str( ) ) );
	CFM = ( atof( cXml->FindValue( "WORLD","CFM", cXml->GetNode( ) ).c_str( ) ) );
	Slip = ( atof( cXml->FindValue( "WORLD","SLIP", cXml->GetNode( ) ).c_str( ) ) );
	MassScale = ( atof( cXml->FindValue( "WORLD","MASSSCALE", cXml->GetNode( ) ).c_str( ) ) );
	Bounce = ( atof( cXml->FindValue( "COLLISIONS","BOUNCE", cXml->GetNode( ) ).c_str( ) ) );
	Soft_CFM = ( atof( cXml->FindValue( "COLLISIONS","SOFT_CFM", cXml->GetNode( ) ).c_str( ) ) );
	Soft_ERP = ( atof( cXml->FindValue( "COLLISIONS","SOFT_ERP", cXml->GetNode( ) ).c_str( ) ) );

	// Initialize ODE
	dInitODE2( 0 );

	// Create Global World
	World = dWorldCreate( );

	dVector3 Position; Position[0] = 0; Position[1] = 0; Position[2] = 0;
	dVector3 Extents; Extents[0] = 256; Extents[1] = 256; Extents[2] = 256;

	// Create Global Space
	//Space = dHashSpaceCreate( 0 );
	Space = dQuadTreeSpaceCreate( 0, Position, Extents, 4 );

	// Contact Group Handling Collision
	ContactGroup = dJointGroupCreate( 0 );

	// Enviroment Attributes
	EnvData.Slip = dReal( Slip );

	// Setup World Attributes
	dWorldSetGravity( World, 0, dReal( Gravity * MassScale ), 0 );
	dWorldSetERP( World, dReal( ERP ) );
	dWorldSetCFM( World, dReal( CFM ) );

	//dWorldSetCFM( World, dReal( 1e-5 ) );
	dWorldSetAutoDisableFlag ( World, 0 );
	dWorldSetContactMaxCorrectingVel ( World, (dReal)0.1 );
	dWorldSetContactSurfaceLayer ( World, (dReal)0.01 );

	// Create Enviroment Space
	EnvSpace = dSimpleSpaceCreate( Space );

	// Create Heightfield Data
	dHeightfieldDataID HeightID = dGeomHeightfieldDataCreate( );

	// Create Finite Heightfield
	dGeomHeightfieldDataBuildCallback( HeightID, NULL, HeightField_Callback,
		(dReal)DimensionsX -1, (dReal)DimensionsZ -1, DimensionsX, DimensionsZ,
		dReal( 1.0 ), dReal( 0.0 ), dReal( 0.0 ), 1 );

	// Create Bounds
	dGeomHeightfieldDataSetBounds( HeightID, dReal( -128.0 ), dReal( 128.0 ) );

	// Create HeightField
	HeightField = dCreateHeightfield( Space, HeightID, 1 );

	// Set HeightField Position
	dGeomSetPosition( HeightField, dReal( 128 + 7.5), dReal( 0.0 ), dReal( 128 + 7.5 ) );

	// Set HeightField Attributes
	dGeomSetData( HeightField, (void*)&EnvData );

	return true;    // Ok

}

// Shutdown Server
bool CPhysicsServer::Shutdown( )
{

	// Clear Client List
	ClientList.empty( );

	// Destroy Contact Group
	if ( ContactGroup ) {
		dJointGroupDestroy( ContactGroup );
		ContactGroup = 0;
	}

	// Destroy Space
	if ( Space ) {
		dSpaceDestroy( Space );
		Space = 0;
	}

	// Destroy World
	if ( World ) {
		dWorldDestroy( World );
		World = 0;
	}

	// Close Ode
	dCloseODE();

	return true;

}

// Register Pyhsics Client
bool CPhysicsServer::RegisterClient( CPhysicsClient *Client )
{

	// Client
	if ( Client == NULL )
		return false;	// Error

	// Initiialize Client
	bool Ret = Client->Init( World, Space, EnvSpace );

	// Add Client To List
	if ( Ret )
		ClientList.push_back( Client );

	return Ret;

}

// Unregister Physics Client
bool CPhysicsServer::UnregisterClient( CPhysicsClient *Client )
{

    // Remove Client
    ClientList.remove( Client );

}

// Update Simulation
bool CPhysicsServer::Update( dReal Step )
{

	// World, Space
	if ( World == 0 || Space == 0 )
		return false;	// Error

	// Handle Collisions
	dSpaceCollide( Space, this, &CollisionCallback );

	// Update Physics Clients
	list <CPhysicsClient*>::const_iterator iter = ClientList.begin( );

	// Loop Clients
	while ( iter != ClientList.end( ) )
	{
		(*iter)->Update( Step );		// Update
		iter++;							// Increment
	}

	// Step World
	dWorldStep( World, Step );

	// Remove All Contact Joints
	dJointGroupEmpty( ContactGroup );

	return true;

}

// Check Collisions
void CPhysicsServer::CheckCollisions( void *data, dGeomID o1, dGeomID o2 )
{

	// Joined
    if( dGeomIsSpace( o1 ) || dGeomIsSpace( o2 ) )
	{
		dSpaceCollide2( o1, o2, data, &CollisionCallback );
	}
	else
	{
		// Init Variables
		int		Mode	= 0;
		dReal	Slip1	= 0;
		dReal	Slip2	= 0;

		// Get Bodies
		dBodyID Body1 = dGeomGetBody(o1);
		dBodyID Body2 = dGeomGetBody(o2);

		// Get Geom Bodies
		PhysGeomData* pData1 = (PhysGeomData*)dGeomGetData(o1);
		PhysGeomData* pData2 = (PhysGeomData*)dGeomGetData(o2);

		// Set Contact Parameters Data
		if (pData1 != NULL)
			Slip1 = pData1->Slip;


		if (pData2 != NULL)
			Slip2 = pData2->Slip;

		// set mode
		if (Slip1 != 0)
			Mode |= dContactSlip1;

		if (Slip2 != 0)
			Mode |= dContactSlip2;

		static const int N = 64; // Max Number Of Contact Points
		dContact contact[N];
		int n = dCollide (o1,o2,N,&contact[0].geom,sizeof(dContact));
		if (n > 0)
		{
			for (int i=0; i<n; i++)
			{

				contact[i].surface.mode = Mode |= dContactBounce | dContactSlip1 | dContactSlip2 |
                                                  dContactSoftERP | dContactSoftCFM | dContactApprox1 ;

				contact[i].surface.bounce = Bounce;
				contact[i].surface.mu = dInfinity;
				contact[i].surface.mu2 = dInfinity;
				contact[i].surface.slip1 = Slip1;
				contact[i].surface.slip2 = Slip2;
                contact[i].surface.soft_cfm = Soft_CFM;
                contact[i].surface.soft_erp = Soft_ERP;

				dJointID c = dJointCreateContact (World, ContactGroup, &contact[i]);
				dJointAttach (c, Body1, Body2);
			}
		}
	}


}

// Collision Callback
void CPhysicsServer::CollisionCallback( void *data, dGeomID o1, dGeomID o2 )
{

	// Get Server Pointer To Data
	CPhysicsServer* ServerPtr = (CPhysicsServer*)data;

	// Check Collisions
	ServerPtr->CheckCollisions(ServerPtr, o1, o2);

}
