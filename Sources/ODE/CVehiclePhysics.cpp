
// Include
#include "CVehiclePhysics.h"                    // Vehicle Physics Class Header File

extern GL_Window Window;                        // Window
extern CTruck *cTruck;                          // Truck

// Constructor
CVehiclePhysics::CVehiclePhysics( )
{

    // Zero Data Structure
    ZeroMemory( &Data, sizeof( Data ) );

    // Setup Gear Box
	Data.GearBox.Gear = 1;				// First Gear
    Park = false;                       // Start Parked

    // Gear Box Scaling
	float Scale = 1500;

    // Min RPMs
	Data.GearBox.GearA[0][1] = 0 * Scale;
	Data.GearBox.GearA[1][1] = 3.059000 * Scale;
	Data.GearBox.GearA[2][1] = 1.625000 * Scale;
	Data.GearBox.GearA[3][1] = 1.000000 * Scale;

    // Max RPMs
	Data.GearBox.GearA[1][2] = 0 * Scale;
	Data.GearBox.GearA[1][2] = 1.100000 * Scale;
	Data.GearBox.GearA[2][2] = 0.900000 * Scale;
	Data.GearBox.GearA[3][2] = 0.700000 * Scale;

    // Zero
	Data.GearBox.GearA[0][3] = 0;
	Data.GearBox.GearA[1][3] = 0;
	Data.GearBox.GearA[2][3] = 0;
	Data.GearBox.GearA[3][3] = 0;

    // Zero
	Data.GearBox.GearU[0][1] = 0;
	Data.GearBox.GearU[1][1] = 0;
	Data.GearBox.GearU[2][1] = 0;
	Data.GearBox.GearU[3][1] = 0;

    // Set Scale Up
	Scale = 0.1;

    // Min Speed
	Data.GearBox.GearU[0][2] = 0;
	Data.GearBox.GearU[1][2] = 50.0f * Scale;
	Data.GearBox.GearU[2][2] = 80.0f * Scale;
	Data.GearBox.GearU[3][2] = 110.0f * Scale;

    // Set Scale Up
	Scale = 0.12;

    // Max Speed
	Data.GearBox.GearU[0][3] = 0;
	Data.GearBox.GearU[1][3] = 70.0f * Scale;
	Data.GearBox.GearU[2][3] = 110.0f * Scale;
	Data.GearBox.GearU[3][3] = 155.0f * Scale;

}

// Destructor
CVehiclePhysics::~CVehiclePhysics( )
{

    // Delete Xml Parser
    SAFE_DELETE( cXml );

    // Loop Axles
    for ( unsigned int i = 0; i != Data.Axles.size( ); i++ )
    {
        // Loop  Wheels
        for ( unsigned int j = 0; j != Data.Axles[i].Wheels.size( ); j++ )
        {


            // Delete Wheels, Joints
            if ( Data.Axles[i].Joints[j] )
            {
                dJointDestroy( Data.Axles[i].Joints[j] );
                Data.Axles[i].Joints[j] = 0;
            }

            // Delete Wheel Body
            if ( Data.Axles[i].Wheels[j].Body )
            {
                dBodyDestroy( Data.Axles[i].Wheels[j].Body );
                Data.Axles[i].Wheels[j].Body = 0;
            }

            // Delete Wheel Geom
            if ( Data.Axles[i].Wheels[j].Geom )
            {
                dGeomDestroy( Data.Axles[i].Wheels[j].Geom );
                Data.Axles[i].Wheels[j].Geom = 0;
            }

            // Empty Axle Wheel, Joint Data
            Data.Axles[i].Wheels.empty( );
            Data.Axles[i].Joints.empty( );

        }

    }

    // Empty Axle Vector
    Data.Axles.empty( );

    // Loop Geoms
    for ( unsigned int i = 0; i != Geoms.size( ); i++ )
    {
        // Delete Vehicle COG Geom
        if ( Geoms[i] )
        {
            dGeomDestroy( Geoms[i] );
            Geoms[i] = 0;
        }
    }

    // Empty Geom Vector
    Geoms.empty( );


	// Delete Vehicle Space
    if ( SpaceVeh )
	{
		dSpaceDestroy( SpaceVeh );
		SpaceVeh = 0;
	}

	// Delete COG Joint
    if ( JointCOG )
	{
		dJointDestroy( JointCOG );
		JointCOG = 0;
	}

	// Delete Vehicle Body
	if ( BodyVeh )
	{
		dBodyDestroy( BodyVeh );
		BodyVeh = 0;
	}

	// Delete COG Body
	if ( BodyCOG )
	{
		dBodyDestroy( BodyCOG );
		BodyCOG = 0;
	}

}

// Create Vehicle
bool CVehiclePhysics::Create( string Filename, CConfig *cConfig )
{

    // Copy Config Pointer
    this->Config = cConfig;

	dMass m;		// Mass
	dMatrix3 R1;	// Rotation Matrix

	// World, Space
	if (World == 0 || Space == 0)
		return false;	// Error

    // Load Vehicle Physics File
    LoadVehicle( Filename );

	// Set Wheel Position To MTM Truck
	Data.Axles[0].Wheels[0].Position = cTruck->GetFLTire( ) / 32;
	Data.Axles[0].Wheels[1].Position = cTruck->GetFRTire( ) / 32;
	Data.Axles[1].Wheels[0].Position = cTruck->GetRRTire( ) / 32;
	Data.Axles[1].Wheels[1].Position = cTruck->GetRLTire( ) / 32;


    CVector4 Max;
    CVector4 Min;
    cTruck->cTruckModel.Dimensions( Min, Max );

	// Get Model Dimensions
	Data.Dimensions = Max - Min;

	// Create Vehicle Body
	BodyVeh = dBodyCreate( World );

	// Calculate Vehicle Body Mass
	dMassSetBoxTotal( &m, Data.Mass * MassScale, (dReal)Data.Dimensions.x, (dReal)Data.Dimensions.y, (dReal)Data.Dimensions.z );

	// Set Vehicle Mass
	dBodySetMass( BodyVeh, &m );

	// Create Vehicle Space
	SpaceVeh = dSimpleSpaceCreate( Space );

	// Setup Rotation
	dRSetIdentity( R1 );

	// Create Geom Body
	GeomVeh = dCreateSphere( 0, 0.05f );

	// Add Geometry
	AddGeom( GeomVeh, 0, 0, 0, R1 );

//	int FaceCount = cTruck->cTruckModel.Faces.size();
//	int VertexCount = cTruck->cTruckModel.Header.NumVerts;
//	int IndexCount = FaceCount * 3;
//
//	dVector3 *Vertices;
//	Vertices = new dVector3[VertexCount];
//	dTriIndex *Indices;
//	Indices = new dTriIndex[IndexCount];
//
//	for ( int i = 0; i != VertexCount; i++ )
//	{
//		Vertices[i][0] = dReal( cTruck->cTruckModel.Vertices[i].X * 0.000125);
//		Vertices[i][1] = dReal( cTruck->cTruckModel.Vertices[i].Y * 0.000125);
//		Vertices[i][2] = dReal( cTruck->cTruckModel.Vertices[i].Z * 0.000125);
//	}
//
//	int v = 0;
//	for ( unsigned int i = 0; i != cTruck->cTruckModel.Faces.size(); i++)
//	{
//
//        Indices[v++] = cTruck->cTruckModel.Faces[i].Vertices[2].VertIndex;
//        Indices[v++] = cTruck->cTruckModel.Faces[i].Vertices[1].VertIndex;
//        Indices[v++] = cTruck->cTruckModel.Faces[i].Vertices[0].VertIndex;
//
//        glVertex3f( Vertices[cTruck->cTruckModel.Faces[i].Vertices[0].VertIndex][0],
//                    Vertices[cTruck->cTruckModel.Faces[i].Vertices[0].VertIndex][1],
//                    Vertices[cTruck->cTruckModel.Faces[i].Vertices[0].VertIndex][2]);
//
//        glVertex3f( Vertices[cTruck->cTruckModel.Faces[i].Vertices[1].VertIndex][0],
//                    Vertices[cTruck->cTruckModel.Faces[i].Vertices[1].VertIndex][1],
//                    Vertices[cTruck->cTruckModel.Faces[i].Vertices[1].VertIndex][2]);
//
//        glVertex3f( Vertices[cTruck->cTruckModel.Faces[i].Vertices[2].VertIndex][0],
//                    Vertices[cTruck->cTruckModel.Faces[i].Vertices[2].VertIndex][1],
//                    Vertices[cTruck->cTruckModel.Faces[i].Vertices[2].VertIndex][2]);
//
//	}
//
//	dTriMeshDataID TriData = dGeomTriMeshDataCreate();
//	dGeomTriMeshDataBuildSimple(TriData, *Vertices, VertexCount, Indices, IndexCount);
//	GeomVeh = dCreateTriMesh(0, TriData, 0, 0, 0);
//
//	 Add Geometry
//	AddGeom( GeomVeh, 0, 0, 0, R1 );


	// Setup Rotation
	dRSetIdentity( R1 );
	dRFromEulerAngles( R1, 0, 0, 0 );

	// 12 Scrape Points
	dGeomID Sphere[12];

	// Add Scrape Point Collisions
	for ( int i = 0; i != 12; i++ )
	{
		// Create Sphere Geometry
		Sphere[i] = dCreateSphere( 0, 0.05f);

		// Add Geometry
		AddGeom( Sphere[i], (dReal)cTruck->GetScrapePoint(i).x / 32.0f, (dReal)cTruck->GetScrapePoint(i).y / 32.0f, (dReal)cTruck->GetScrapePoint(i).z / 32.0f, R1 );

	}

	// Create Center Of Gravity Body
	BodyCOG = dBodyCreate( World );

	Data.COG.x = 0;
	Data.COG.y = cTruck->GetFLTire( ).y / 32;
	Data.COG.z = ( cTruck->GetRLTire( ).z / 32 + cTruck->GetFLTire( ).z / 32 );

	// Position Body Center Of Gravity
	dBodySetPosition( BodyCOG, (dReal)Data.COG.x, (dReal)Data.COG.y, (dReal)Data.COG.z );

	// Set Body Mass
	dMassSetBoxTotal(&m, Data.Mass * Data.COGScale, (dReal)Data.Dimensions.x / 2, (dReal)Data.Dimensions.y / 2, (dReal)Data.Dimensions.z / 1.65 );

    // Set Center Of Gravity Mass
	dBodySetMass( BodyCOG, &m );

	// Create Fixed Gravity Joint
	JointCOG = dJointCreateFixed( World, 0 );

	// Attach Joint To Vehicle Body
	dJointAttach( JointCOG, BodyVeh, BodyCOG );

	// Set Fix Joint Attribute
	dJointSetFixed( JointCOG );

	// Loop Axles Per Vehicle
	for ( unsigned int j = 0; j != Data.Axles.size( ); j++ )
	{
		// Loop Wheels Per Axle
		for ( unsigned int i = 0; i != Data.Axles[j].Wheels.size( ); i++ )
		{
			// Create Wheel Body
			Data.Axles[j].Wheels[i].Body = dBodyCreate( World );

			// Finite Rotation Mode
			dBodySetFiniteRotationMode( Data.Axles[j].Wheels[i].Body, 1 );

			// Wheel Dimensions Vector
			CVector4 Max(0,0,0);
			CVector4 Min(0,0,0);

			// Get First Wheel Dimension
 			cTruck->cLeftTire.Dimensions( Min, Max );

			// Set Wheel Body Mass
			dMassSetCylinderTotal( &m, (dReal)Data.Axles[j].Wheels[i].Mass * MassScale, 1, (dReal)Max.y / 2, (dReal)Max.x );

			// Set Wheel Mass
			dBodySetMass(Data.Axles[j].Wheels[i].Body, &m);

			// Create Wheel Cylinder
			Data.Axles[j].Wheels[i].Geom = dCreateCylinder( 0, (dReal)Max.y / 2, (dReal)Max.x );

			// Set Wheel Data
			dGeomSetData( Data.Axles[j].Wheels[i].Geom, (void*)&Data.Axles[j].Wheels[i].Data );

			// Set Geom Wheel Body
			dGeomSetBody( Data.Axles[j].Wheels[i].Geom, Data.Axles[j].Wheels[i].Body );

			// Convert Degrees To Radians
			dReal phi = DEG2RAD( 0 );
			dReal theta = dReal(DEG2RAD( 90 ));
			dReal psi = DEG2RAD( 0 );

			// Setup Rotation
			dRSetIdentity( R1 );
			dRFromEulerAngles( R1, phi, theta, psi );
			dBodySetRotation( Data.Axles[j].Wheels[i].Body, R1 );

			// Add Wheel Geom To Vehicle Space
			dSpaceAdd( SpaceVeh, Data.Axles[j].Wheels[i].Geom );

		}
	}

	// Center Position
	Position( 0, 0, 0 );

	// Loop Axles Per Vehicle
	for ( unsigned int j = 0; j != Data.Axles.size( ); j++ )
	{

		// Loop Wheels Per Axle
		for ( unsigned int i = 0; i != Data.Axles[j].Wheels.size( ); i++ )
		{
			// Create Hinge Joint 2
			Data.Axles[j].Joints[i] = dJointCreateHinge2( World, 0 );

			// Attach Hinge To Wheel
			dJointAttach( Data.Axles[j].Joints[i], BodyCOG, Data.Axles[j].Wheels[i].Body );

			// Get Wheel Position
			const dReal *pos = dBodyGetPosition( Data.Axles[j].Wheels[i].Body );

			// Anchor Wheel Position
			dJointSetHinge2Anchor( Data.Axles[j].Joints[i], pos[0], pos[1], pos[2] );

			// Set Steering Axis
			dJointSetHinge2Axis1( Data.Axles[j].Joints[i], 0, 1, 0 );

			// Set Rotation Axis
			dJointSetHinge2Axis2( Data.Axles[j].Joints[i], 1, 0, 0 );

			// Lock Wheels
			dJointSetHinge2Param( Data.Axles[j].Joints[i], dParamLoStop, 0 );
			dJointSetHinge2Param( Data.Axles[j].Joints[i], dParamHiStop, 0 );

			dJointSetHinge2Param( Data.Axles[j].Joints[i], dParamSuspensionERP, Data.Axles[j].Wheels[i].Error );
			dJointSetHinge2Param( Data.Axles[j].Joints[i], dParamSuspensionCFM, Data.Axles[j].Wheels[i].Shock );

			dJointSetHinge2Param( Data.Axles[j].Joints[i], dParamStopERP, Data.Axles[j].Wheels[i].Error );
			dJointSetHinge2Param( Data.Axles[j].Joints[i], dParamStopCFM, Data.Axles[j].Wheels[i].StopCFM );

			dJointSetHinge2Param( Data.Axles[j].Joints[i], dParamBounce, Data.Axles[j].Wheels[i].Bounce );
			dJointSetHinge2Param( Data.Axles[j].Joints[i], dParamCFM, Data.Axles[j].Wheels[i].CFM );

			dJointSetHinge2Param( Data.Axles[j].Joints[i], dParamFMax, Data.Axles[j].Wheels[i].FMax );
			dJointSetHinge2Param( Data.Axles[j].Joints[i], dParamFudgeFactor, Data.Axles[j].Wheels[i].FudgeFactor );


			// Lock Wheels
			dJointSetHinge2Param( Data.Axles[j].Joints[i], dParamLoStop2, 0 );
			dJointSetHinge2Param( Data.Axles[j].Joints[i], dParamHiStop2, 0 );

			dJointSetHinge2Param( Data.Axles[j].Joints[i], dParamSuspensionERP2, Data.Axles[j].Wheels[i].Error );
			dJointSetHinge2Param( Data.Axles[j].Joints[i], dParamSuspensionCFM2, Data.Axles[j].Wheels[i].Shock );

			dJointSetHinge2Param( Data.Axles[j].Joints[i], dParamStopERP2, Data.Axles[j].Wheels[i].Error );
			dJointSetHinge2Param( Data.Axles[j].Joints[i], dParamStopCFM2, Data.Axles[j].Wheels[i].StopCFM );

			dJointSetHinge2Param( Data.Axles[j].Joints[i], dParamBounce2, Data.Axles[j].Wheels[i].Bounce );
			dJointSetHinge2Param( Data.Axles[j].Joints[i], dParamCFM2, Data.Axles[j].Wheels[i].CFM );

			dJointSetHinge2Param( Data.Axles[j].Joints[i], dParamFMax2, Data.Axles[j].Wheels[i].FMax );
			dJointSetHinge2Param( Data.Axles[j].Joints[i], dParamFudgeFactor2, Data.Axles[j].Wheels[i].FudgeFactor );



		}
	}


	return true;	// Return True

}

// Loads Vehicle Config
bool CVehiclePhysics::LoadVehicle( string Filename )
{

    // Create New Parser Object
    cXml = new CXmlParser( );

	// Parse Physics File
	if ( !cXml->ParseFile( Filename.c_str( ) ) )
		return false;	// Error

	MassScale = (dReal)atof( cXml->FindValue( "WORLD","MASSSCALE", cXml->GetNode( ) ).c_str( ) );
	Data.Mass = MassScale * (dReal)atof( cXml->FindValue( "VEHICLE","MASS", cXml->GetNode( ) ).c_str( ) );

	Data.MinSteering = (dReal)atof( cXml->FindValue( "VEHICLE","MINSTEER", cXml->GetNode( ) ).c_str( ) );
	Data.MaxSteering = (dReal)atof( cXml->FindValue( "VEHICLE","MAXSTEER", cXml->GetNode( ) ).c_str( ) );
	Data.SteerSpeed	= (dReal)atof( cXml->FindValue( "VEHICLE","STEERSPEED", cXml->GetNode( ) ).c_str( ) );

	Data.Brake = (dReal)atof( cXml->FindValue( "VEHICLE","BRAKE", cXml->GetNode( ) ).c_str( ) );
	Data.Drag = (dReal)atof( cXml->FindValue( "VEHICLE","DRAG", cXml->GetNode( ) ).c_str( ) );
	Data.MaxSpeed = (dReal)atof( cXml->FindValue( "VEHICLE","MAXSPEED", cXml->GetNode( ) ).c_str( ) );
	Data.RevSpeed = (dReal)atof( cXml->FindValue( "VEHICLE","REVSPEED", cXml->GetNode( ) ).c_str( ) );

	Data.COGScale = (dReal)atof( cXml->FindValue( "VEHICLE","COGSCALE", cXml->GetNode( ) ).c_str( ) );

    float HorsePower = (dReal)atof( cXml->FindValue( "TRANSMISSION","HORSEPOWER", cXml->GetNode( ) ).c_str( ) );

    // Min RPMs
	Data.GearBox.GearA[0][1] = (dReal)atof( cXml->FindValue( "TRANSMISSION","GEAR0MIN", cXml->GetNode( ) ).c_str( ) ) * HorsePower;
	Data.GearBox.GearA[1][1] = (dReal)atof( cXml->FindValue( "TRANSMISSION","GEAR1MIN", cXml->GetNode( ) ).c_str( ) ) * HorsePower;
	Data.GearBox.GearA[2][1] = (dReal)atof( cXml->FindValue( "TRANSMISSION","GEAR2MIN", cXml->GetNode( ) ).c_str( ) ) * HorsePower;
	Data.GearBox.GearA[3][1] = (dReal)atof( cXml->FindValue( "TRANSMISSION","GEAR3MIN", cXml->GetNode( ) ).c_str( ) ) * HorsePower;

    // Max RPMs
	Data.GearBox.GearA[1][2] = (dReal)atof( cXml->FindValue( "TRANSMISSION","GEAR0MAX", cXml->GetNode( ) ).c_str( ) ) * HorsePower;
	Data.GearBox.GearA[1][2] = (dReal)atof( cXml->FindValue( "TRANSMISSION","GEAR1MAX", cXml->GetNode( ) ).c_str( ) ) * HorsePower;
	Data.GearBox.GearA[2][2] = (dReal)atof( cXml->FindValue( "TRANSMISSION","GEAR2MAX", cXml->GetNode( ) ).c_str( ) ) * HorsePower;
	Data.GearBox.GearA[3][2] = (dReal)atof( cXml->FindValue( "TRANSMISSION","GEAR3MAX", cXml->GetNode( ) ).c_str( ) ) * HorsePower;

    // Zero
	Data.GearBox.GearA[0][3] = 0;
	Data.GearBox.GearA[1][3] = 0;
	Data.GearBox.GearA[2][3] = 0;
	Data.GearBox.GearA[3][3] = 0;

    // Zero
	Data.GearBox.GearU[0][1] = 0;
	Data.GearBox.GearU[1][1] = 0;
	Data.GearBox.GearU[2][1] = 0;
	Data.GearBox.GearU[3][1] = 0;

    // Set Scale Up
	float ScaleLow = (dReal)atof( cXml->FindValue( "TRANSMISSION","SCALELOW", cXml->GetNode( ) ).c_str( ) );

    // Min Speed
	Data.GearBox.GearU[0][2] = (dReal)atof( cXml->FindValue( "TRANSMISSION","GEAR0LOW", cXml->GetNode( ) ).c_str( ) ) * ScaleLow;
	Data.GearBox.GearU[1][2] = (dReal)atof( cXml->FindValue( "TRANSMISSION","GEAR1LOW", cXml->GetNode( ) ).c_str( ) ) * ScaleLow;
	Data.GearBox.GearU[2][2] = (dReal)atof( cXml->FindValue( "TRANSMISSION","GEAR2LOW", cXml->GetNode( ) ).c_str( ) ) * ScaleLow;
	Data.GearBox.GearU[3][2] = (dReal)atof( cXml->FindValue( "TRANSMISSION","GEAR3LOW", cXml->GetNode( ) ).c_str( ) ) * ScaleLow;

    // Set Scale Up
	float ScaleHigh = (dReal)atof( cXml->FindValue( "TRANSMISSION","SCALEHIGH", cXml->GetNode( ) ).c_str( ) );

    // Max Speed
	Data.GearBox.GearU[0][3] = (dReal)atof( cXml->FindValue( "TRANSMISSION","GEAR0HIGH", cXml->GetNode( ) ).c_str( ) ) * ScaleHigh;
	Data.GearBox.GearU[1][3] = (dReal)atof( cXml->FindValue( "TRANSMISSION","GEAR1HIGH", cXml->GetNode( ) ).c_str( ) ) * ScaleHigh;
	Data.GearBox.GearU[2][3] = (dReal)atof( cXml->FindValue( "TRANSMISSION","GEAR2HIGH", cXml->GetNode( ) ).c_str( ) ) * ScaleHigh;
	Data.GearBox.GearU[3][3] = (dReal)atof( cXml->FindValue( "TRANSMISSION","GEAR3HIGH", cXml->GetNode( ) ).c_str( ) ) * ScaleHigh;


    // Loop Axles
    for ( unsigned int i = 0; i != Data.Axles.size( ); i++ )
    {
        // Loop  Wheels
        for ( unsigned int j = 0; j != Data.Axles[i].Wheels.size( ); j++ )
        {


            // Delete Wheels, Joints
            if ( Data.Axles[i].Joints[j] )
            {
                dJointDestroy( Data.Axles[i].Joints[j] );
                Data.Axles[i].Joints[j] = 0;
            }

            // Delete Wheel Body
            if ( Data.Axles[i].Wheels[j].Body )
            {
                dBodyDestroy( Data.Axles[i].Wheels[j].Body );
                Data.Axles[i].Wheels[j].Body = 0;
            }

            // Delete Wheel Geom
            if ( Data.Axles[i].Wheels[j].Geom )
            {
                dGeomDestroy( Data.Axles[i].Wheels[j].Geom );
                Data.Axles[i].Wheels[j].Geom = 0;
            }

            // Empty Axle Wheel, Joint Data
            Data.Axles[i].Wheels.empty( );
            Data.Axles[i].Joints.empty( );

        }

    }

    // Empty Axle Vector
    Data.Axles.empty( );

    // Loop Geoms
    for ( unsigned int i = 0; i != Geoms.size( ); i++ )
    {
        // Delete Vehicle COG Geom
        if ( Geoms[i] )
        {
            dGeomDestroy( Geoms[i] );
            Geoms[i] = 0;
        }
    }

    // Empty Geom Vector
    Geoms.empty( );


	// Load Axles & Wheels
	LoadWheels( "VEHICLE", "AXLE", cXml->GetNode( ) );

	return true;

}


// Load Vehicle Wheels
void CVehiclePhysics::LoadWheels(string ParentName, string Name, CXmlNode Node)
{

	// Found New Axle
	if ( StringToUpper( Node.GetName( ) ) == "AXLE" && StringToUpper( Node.GetParent( ) ) == "VEHICLE" )
	{

        Data.Axles.resize( Data.Axles.size( ) + 1 );


	}	// Found New Wheel
	else if ( StringToUpper( Node.GetName( ) ) == "WHEEL"  && StringToUpper( Node.GetParent( ) )== "AXLE" )
	{

        Data.Axles[Data.Axles.size( )-1].Wheels.resize( Data.Axles[Data.Axles.size( )-1].Wheels.size( ) + 1 );
        Data.Axles[Data.Axles.size( )-1].Joints.resize( Data.Axles[Data.Axles.size( )-1].Joints.size( ) + 1 );

		Data.Axles[Data.Axles.size( )-1].Wheels[ Data.Axles[Data.Axles.size( )-1].Wheels.size( ) - 1 ].Mass = MassScale * (dReal)atof( cXml->FindValue( "WHEEL", "MASS", Node ).c_str( ) );
		Data.Axles[Data.Axles.size( )-1].Wheels[ Data.Axles[Data.Axles.size( )-1].Wheels.size( ) - 1 ].Shock = (dReal)atof( cXml->FindValue( "WHEEL", "SHOCK", Node ).c_str( ) );
		Data.Axles[Data.Axles.size( )-1].Wheels[ Data.Axles[Data.Axles.size( )-1].Wheels.size( ) - 1 ].Error = (dReal)atof( cXml->FindValue( "WHEEL", "ERP", Node ).c_str( ) );
		Data.Axles[Data.Axles.size( )-1].Wheels[ Data.Axles[Data.Axles.size( )-1].Wheels.size( ) - 1 ].Data.Slip = (dReal)atof( cXml->FindValue( "WHEEL", "SLIP", Node ).c_str( ) );

		Data.Axles[Data.Axles.size( )-1].Wheels[ Data.Axles[Data.Axles.size( )-1].Wheels.size( ) - 1 ].DriveMul = (dReal)atof( cXml->FindValue( "WHEEL", "DRIVEMUL", Node ).c_str( ) );
		Data.Axles[Data.Axles.size( )-1].Wheels[ Data.Axles[Data.Axles.size( )-1].Wheels.size( ) - 1 ].SteerMul = (dReal)atof( cXml->FindValue( "WHEEL", "STEERMUL", Node ).c_str( ) );

		Data.Axles[Data.Axles.size( )-1].Wheels[ Data.Axles[Data.Axles.size( )-1].Wheels.size( ) - 1 ].StopERP = (dReal)atof( cXml->FindValue( "WHEEL", "STOPERP", Node ).c_str( ) );
		Data.Axles[Data.Axles.size( )-1].Wheels[ Data.Axles[Data.Axles.size( )-1].Wheels.size( ) - 1 ].StopCFM = (dReal)atof( cXml->FindValue( "WHEEL", "STOPCFM", Node ).c_str( ) );

		Data.Axles[Data.Axles.size( )-1].Wheels[ Data.Axles[Data.Axles.size( )-1].Wheels.size( ) - 1 ].Bounce = (dReal)atof( cXml->FindValue( "WHEEL", "BOUNCE", Node ).c_str( ) );
		Data.Axles[Data.Axles.size( )-1].Wheels[ Data.Axles[Data.Axles.size( )-1].Wheels.size( ) - 1 ].CFM = (dReal)atof( cXml->FindValue( "WHEEL", "CFM", Node ).c_str( ) );

		Data.Axles[Data.Axles.size( )-1].Wheels[ Data.Axles[Data.Axles.size( )-1].Wheels.size( ) - 1 ].FMax = (dReal)atof( cXml->FindValue( "WHEEL", "FMAX", Node ).c_str( ) );
		Data.Axles[Data.Axles.size( )-1].Wheels[ Data.Axles[Data.Axles.size( )-1].Wheels.size( ) - 1 ].FudgeFactor = (dReal)atof( cXml->FindValue( "WHEEL", "FUDGEFACTOR", Node ).c_str( ) );

	}

	// Continue Recursively
	for ( int i = 0; i != Node.ChildrenCount( ); i++ )
		LoadWheels( ParentName, Name, Node.GetChild( i ) );

}


// Update Simulation
bool CVehiclePhysics::Update( dReal Step )
{

    // Get Vehicle Speed
    Velocity = GetSpeed( );

    // Calculate RPMs & MPHs
	Data.GearBox.Rpm = int( Velocity * 8000.0f / Data.GearBox.GearU[Data.GearBox.Gear][3] + 1000.0f);
	Data.GearBox.Mph = int( Velocity * 3600.0f / 1000.0f);

	// Character Buffer
	char Buffer[32];
	string Combine;

	if ( Park == true )
        Combine.append( "Parked " );
    else
    {
        sprintf(Buffer, "Gear - %i", ((int)Data.GearBox.Gear) );
        Combine.append( Buffer );
    }

	sprintf(Buffer, ", Rpms - %i", ((int)Data.GearBox.Rpm) );
	Combine.append( Buffer );
	sprintf(Buffer, ", Mph - %i", ((int)Data.GearBox.Mph) );
	Combine.append( Buffer );
	SetWindowText( Window.hWnd, Combine.c_str( ) );

    // Park
    if (Window.Keys->Keys['P'] )
    {   // Toggle
        Park = !Park;
        Window.Keys->Keys['P'] = false;
    }

	// Check Current Gear
	CheckGear( Step );

    // Park, Update
    if ( Park == true )
        return true;

    // Automatic Gear Box
	AutomaticGear( );

    // Gas
	if (Window.Keys->Keys[(int)Config->Gas[0]] )
	{
		Data.Speed = Data.MaxSpeed;
		Data.WheelForce = Data.GearBox.Acceleration;
		Gas += 1000;
	}
	// Brake
	else if (Window.Keys->Keys[(int)Config->Brake[0]])
	{
		Data.Speed = -Data.RevSpeed;
		Data.WheelForce = Data.GearBox.Acceleration;
		Gas -= 1000;
	}
	// Drag
	else
	{
		Data.Speed = Data.Drag;
		Data.WheelForce = Data.Drag;
		Gas = 0;
	}


	// Steer Front Left
	if(Window.Keys->Keys[(int)Config->FrontSteerLeft[0]])
	{
		Data.FrontCurrentSteer = Data.SteerSpeed; // * Step;
		Data.FrontSteerAngle += Data.FrontCurrentSteer;
	}
	// Steer Front Right
	else if (Window.Keys->Keys[(int)Config->FrontSteerRight[0]])
	{
		Data.FrontCurrentSteer = -Data.SteerSpeed; // * Step;
		Data.FrontSteerAngle += Data.FrontCurrentSteer;
	}
	// Center Front Steering
	else
	{
		if ( Data.FrontSteerAngle != 0 )
		{
			if ( Data.FrontSteerAngle <= 0 )
				Data.FrontSteerAngle += Data.SteerSpeed; // * Step;
			else if ( Data.FrontSteerAngle >= 0 )
				Data.FrontSteerAngle -= Data.SteerSpeed; // * Step;
		}
	}

	// Clip Steering Max
	if (Data.FrontSteerAngle >= Data.MaxSteering)
		Data.FrontSteerAngle = Data.MaxSteering;
	// Clip Steering Min
	else if (Data.FrontSteerAngle <= Data.MinSteering)
		Data.FrontSteerAngle = Data.MinSteering;

	// Steer Rear Left
	if(Window.Keys->Keys[(int)Config->RearSteerLeft[0]])
	{
		Data.RearCurrentSteer = Data.SteerSpeed; // * Step;
		Data.RearSteerAngle += Data.RearCurrentSteer;
	}
	// Steer Rear Right
	else if (Window.Keys->Keys[(int)Config->RearSteerRight[0]])
	{
		Data.RearCurrentSteer = -Data.SteerSpeed; // * Step;
		Data.RearSteerAngle += Data.RearCurrentSteer;
	}
	// Center Rear Steering
	else
	{
		if ( Data.RearSteerAngle != 0 )
		{
			if ( Data.RearSteerAngle <= 0 )
				Data.RearSteerAngle += Data.SteerSpeed; // * Step;
			else if ( Data.RearSteerAngle >= 0 )
				Data.RearSteerAngle -= Data.SteerSpeed; // * Step;
		}
	}

	// Clip Steering Max
	if (Data.RearSteerAngle >= Data.MaxSteering)
		Data.RearSteerAngle = Data.MaxSteering;
	// Clip Steering Min
	else if (Data.RearSteerAngle <= Data.MinSteering)
		Data.RearSteerAngle = Data.MinSteering;

	// Loop Axles Per Vehicle
	for ( unsigned int j = 0; j != Data.Axles.size( ); j++ )
	{
		// Loop Wheels Per Axle
		for ( unsigned int i = 0; i != Data.Axles[j].Wheels.size( ); i++ )
		{

            // Front Axle
            if ( j == 0 )
            {
                // Steering Left Multiplier
                if (Data.Axles[j].Wheels[i].SteerMul < 0)
                    SteerWheel (j,i, Data.FrontSteerAngle * Data.Axles[j].Wheels[i].SteerMul, 0);
                else // Steering Right Multiplier
                    SteerWheel (j,i, Data.FrontSteerAngle * Data.Axles[j].Wheels[i].SteerMul, 1);
            }
            else // Rear Axle
            {
                // Steering Left Multiplier
                if (Data.Axles[j].Wheels[i].SteerMul < 0)
                    SteerWheel (j,i, Data.RearSteerAngle * Data.Axles[j].Wheels[i].SteerMul, 0);
                else // Steering Right Multiplier
                    SteerWheel (j,i, Data.RearSteerAngle * Data.Axles[j].Wheels[i].SteerMul, 1);
            }

			// Wheel Multiplier
			if (Data.Axles[j].Wheels[i].DriveMul != 0)
			{
				// Set Parameters
				dJointSetHinge2Param( Data.Axles[j].Joints[i], dParamVel2, Data.Speed );
				dJointSetHinge2Param( Data.Axles[j].Joints[i], dParamFMax2, Data.WheelForce );
			}

			// Finite Rotations
			dVector3 Axis;
			dJointGetHinge2Axis2( Data.Axles[j].Joints[i], Axis );
			dBodySetFiniteRotationAxis( Data.Axles[j].Wheels[i].Body, Axis[0], Axis[1], Axis[2] );
			dBodySetFiniteRotationMode ( Data.Axles[j].Wheels[i].Body, 1 );

		}
	}

	return true;

}

// LamDa Down
float CVehiclePhysics::LamDa_Down( int K )
{

	return -Data.GearBox.GearA[K][2] / (Data.GearBox.GearU[K][3] - Data.GearBox.GearU[K][2]);

}

// LamDa Up
float CVehiclePhysics::LamDa_Up( int K )
{

	return (Data.GearBox.GearA[K][2] - Data.GearBox.GearA[K][1]) / Data.GearBox.GearU[K][2];

}

// Best Gear
float CVehiclePhysics::BestGear( int G )
{

	if ( G == 0 )
		return 0;

	if ( G <= 3 && G >= 1 )
		return (Data.GearBox.GearA[G + 1][1] - Data.GearBox.GearA[G][2] + LamDa_Down(G) *
				Data.GearBox.GearU[G][2]) / (LamDa_Down(G) - LamDa_Up(G + 1));

	if ( G == 3 )
		return 1.2f * Data.GearBox.GearU[3][3];


	return 0;

}

// Automatic Gear
void CVehiclePhysics::AutomaticGear( )
{

    if ( Park == true )
        return;


	if ( (int)Velocity > BestGear( Data.GearBox.Gear ) && Data.GearBox.Gear <= 3 )
		Data.GearBox.Gear++;

	if ( Data.GearBox.Gear > 0 &&  (int)Velocity < BestGear( Data.GearBox.Gear - 1 ) )
		Data.GearBox.Gear--;


}

// Check Gear
void CVehiclePhysics::CheckGear( float Step )
{

    if ( Park == true )
        return;


	int K;
	int Temp;
	float LamDa;

	Temp = 0;
	for ( K = 1; K != 4; K++ )
		if ( (int)Velocity >= Data.GearBox.GearU[Data.GearBox.Gear][K] )
			Temp = K;

	if ( Temp == 0 )
	{
		Data.GearBox.Acceleration = -Data.Brake;
	}
	else
	{
		LamDa = (Data.GearBox.GearU[Data.GearBox.Gear][Temp + 1] - Data.GearBox.GearU[Data.GearBox.Gear][Temp]) /
				(Data.GearBox.GearA[Data.GearBox.Gear][Temp + 1] - Data.GearBox.GearA[Data.GearBox.Gear][Temp]);

		Data.GearBox.Acceleration = ( Velocity -
				Data.GearBox.GearU[Data.GearBox.Gear][Temp]) /
				LamDa + Data.GearBox.GearA[Data.GearBox.Gear][Temp];
	}


}

// Calculate Vehicle Speed
float CVehiclePhysics::GetSpeed( )
{

    // Get Velocity
	dReal const * cv = dBodyGetLinearVel( BodyVeh );
	float v2 = cv[0]*cv[0] + /*cv[1]*cv[1] +*/ cv[2]*cv[2];
	float v = sqrtf( v2 ) / 1000 * 3600;

	return v;   // Return Speed

}

// Render
void CVehiclePhysics::Render( )
{

	// Variables
	float Matrix[16];
    float MatrixWheel[16];

    // Get Vehicle Geom Position & Rotation
    const dReal *PosBody = dGeomGetPosition( Geoms[0] );
    const dReal *RotBody = dGeomGetRotation( Geoms[0] );

    // Convert ODE Matrix To OpenGL
    ODEtoOGL( Matrix, PosBody, RotBody );

    // Push Matrix
    glPushMatrix();

        // Multiply Position, Rotation Matrix
        glMultMatrixf( Matrix );

        // Call Truck Display List
        glCallList( cTruck->GetTruckModel( ) );

    // Pop Matrix
    glPopMatrix();

    // Push Matrix
    glPushMatrix();

        // Multiply Position, Rotation Matrix
        glMultMatrixf( Matrix );

        // Translate Front Axle Position
        glTranslatef( cTruck->GetFrontAxlePos().x, cTruck->GetFrontAxlePos().y, cTruck->GetFrontAxlePos().z );

        // Rotate Rear Backwards
        glRotatef(180, 0, 1, 0);

        // Call Truck Display List
        glCallList( cTruck->GetAxleModel( ) );


    // Pop Matrix
    glPopMatrix();

    // Push Matrix
    glPushMatrix();

        // Multiply Position, Rotation Matrix
        glMultMatrixf( Matrix );

        // Translate Front Axle Position
        glTranslatef( cTruck->GetRearAxlePos().x, cTruck->GetRearAxlePos().y, cTruck->GetRearAxlePos().z );

        // Call Truck Display List
        glCallList( cTruck->GetAxleModel( ) );


    // Pop Matrix
    glPopMatrix();

	// Loop Axles Per Vehicle
	for ( unsigned int j = 0; j != Data.Axles.size( ); j++ )
	{


		// Loop Wheels Per Axle
		for ( unsigned int i = 0; i != Data.Axles[j].Wheels.size( ); i++ )
		{

			// Get Position/Rotation
			const dReal *Pos = GetGeomPosition( Data.Axles[j].Wheels[i].Geom );
			const dReal *Rot = GetGeomRotation( Data.Axles[j].Wheels[i].Geom );

			// Convert ODE Matrix To OpenGL
			ODEtoOGL( MatrixWheel, Pos, Rot );

            // Push Matrix
			glPushMatrix();

                // Multiply Position, Rotation Matrix
                glMultMatrixf( Matrix );

                glColor3f( 0, 0, 0 );
                glLineWidth( 12 );
                glBegin(GL_LINES);

                    // Draw Four Link Bars
                    glVertex3f( -cTruck->GetAxleBarOffset( ).x / 32, cTruck->GetAxleBarOffset( ).y / 32, cTruck->GetAxleBarOffset( ).z / 32 );
                    glVertex3f( cTruck->GetFLTire().x / 64, cTruck->GetFLTire().y / 32, cTruck->GetFLTire().z / 32 );

                    glVertex3f( cTruck->GetAxleBarOffset( ).x / 32, cTruck->GetAxleBarOffset( ).y / 32, cTruck->GetAxleBarOffset( ).z / 32 );
                    glVertex3f( cTruck->GetFRTire().x / 64, cTruck->GetFRTire().y / 32, cTruck->GetFRTire().z / 32 );

                    glVertex3f( -cTruck->GetAxleBarOffset( ).x / 32, cTruck->GetAxleBarOffset( ).y / 32, cTruck->GetAxleBarOffset( ).z / 32 );
                    glVertex3f( cTruck->GetRLTire().x / 64, cTruck->GetRLTire().y / 32, cTruck->GetRLTire().z / 32 );

                    glVertex3f( cTruck->GetAxleBarOffset( ).x / 32, cTruck->GetAxleBarOffset( ).y / 32, cTruck->GetAxleBarOffset( ).z / 32 );
                    glVertex3f( cTruck->GetRRTire().x / 64, cTruck->GetRRTire().y / 32, cTruck->GetRRTire().z / 32 );


                glEnd( );

            glPopMatrix( );

            // Push Matrix
			glPushMatrix();


				// Multiply Position, Rotation Matrix
				glMultMatrixf( MatrixWheel );




				// Rotate Tire Y Up
				glRotatef(90, 0, 1, 0);

				if ( j == 0 && i == 0 )
					// Call Rear Right Tire Display List
					glCallList( cTruck->GetLeftTireModel( ) );
				else if ( j == 0 && i == 1 )
					// Call Rear Left Tire Display List
					glCallList( cTruck->GetRightTireModel( ) );
				else if ( j == 1 && i == 1 )
					// Call Front Right Tire Display List
					glCallList( cTruck->GetLeftTireModel( ) );
				else if ( j == 1 && i == 0 )
					// Call Front Left Tire Display List
					glCallList( cTruck->GetRightTireModel( ) );

            // Pop Matrix
            glPopMatrix( );

		}
	}

}

// Get Geom Position
const dReal* CVehiclePhysics::GetGeomPosition( dGeomID Geom )
{

	// Get Geom Position
	const dReal *Pos = dGeomGetPosition ( Geom );

	if ( dGeomGetClass( Geom ) != dGeomTransformClass )
		return Pos;

	// Handle Transformed Geom
	const dReal *R = dGeomGetRotation ( Geom );

	// Get Transform Geom
	dGeomID g2 = dGeomTransformGetGeom ( Geom );

	// Get Geom Position
	const dReal *Pos2 = dGeomGetPosition ( g2 );

	// Cacluate Actual Position
	static dVector3 Actual_Pos;
	dMULTIPLY0_331 (Actual_Pos, R, Pos2);

	// Add Position
	Actual_Pos[0] += Pos[0];
	Actual_Pos[1] += Pos[1];
	Actual_Pos[2] += Pos[2];

	// Return Position
	return Actual_Pos;

}

// Set Geom Position
const dReal* CVehiclePhysics::GetGeomRotation( dGeomID Geom )
{

	// Get Geom Rotation
	const dReal *Rot = dGeomGetRotation (Geom);

	if (dGeomGetClass(Geom) != dGeomTransformClass)
		return Rot;

	// Handle Transformed Geom
	dGeomID	g2 = dGeomTransformGetGeom ( Geom );

	// Get Geom Rotation
	const dReal *R2	= dGeomGetRotation ( g2 );

	// Setup Rotation Matrix
	static dMatrix3 Actual_R;
	dMULTIPLY0_333 ( Actual_R, Rot, R2 );

	// Copy Rotation Matrix
	dReal RR[12];
	memcpy( RR, Actual_R, sizeof( dMatrix3 ) );

	// Return Rotation
	return Actual_R;

}

// Add Geom Relative Position Rotation Body
void CVehiclePhysics::AddGeom( dGeomID Geom, dReal xOffset, dReal yOffset, dReal zOffset, dMatrix3 R )
{

	// Create Geom Transform To Encapsulte
	dGeomID xform = dCreateGeomTransform( SpaceVeh );
	dGeomTransformSetCleanup( xform, 1 );
	dGeomTransformSetInfo( xform, 0 );

	// Set Encapsulated Geom
	dGeomTransformSetGeom( xform, Geom );

	// Attach Transform To Body
	dGeomSetBody( xform, BodyVeh );

	// Set Translation & Rotation
	dGeomSetPosition( Geom, xOffset, yOffset, zOffset );
	dGeomSetRotation( Geom, R );

	// Insert Transform Into Geom Vector
	Geoms.push_back( xform );

}

// Returns Vehicle Body Position
const dReal* CVehiclePhysics::GetPosition( )
{

	// Geom 0 Position
	return GetGeomPosition( Geoms[0]);

}

// Returns Vehicle Body Rotation
const dReal* CVehiclePhysics::GetRotation( )
{

	// Geom 0 Rotation
	return GetGeomRotation( Geoms[0]);

}

// Set Body Position
void CVehiclePhysics::SetPosition( dReal x, dReal y, dReal z )
{

	// Reset Body Velocity
	dBodySetLinearVel( BodyVeh, 0, 0, 0 );
	dBodySetAngularVel( BodyVeh, 0, 0, 0 );

	// Loop Axles Per Vehicle
	for ( unsigned int j = 0; j != Data.Axles.size( ); j++ )
	{
		// Loop Wheels Per Axle
		for ( unsigned int i = 0; i != Data.Axles[j].Wheels.size( ); i++ )
		{
			// Reset Wheel Velocities
			dBodySetLinearVel( Data.Axles[j].Wheels[i].Body, 0, 0, 0);
			dBodySetAngularVel( Data.Axles[j].Wheels[i].Body, 0, 0, 0);
		}
	}

	// Position Vehicle Body
	dBodySetPosition( BodyVeh, x, y, z);

	// Loop Axles Per Vehicle
	for ( unsigned int j = 0; j != Data.Axles.size( ); j++ )
	{
		// Loop Wheels Per Axle
		for ( unsigned int i = 0; i != Data.Axles[j].Wheels.size( ); i++ )
		{
			// Set Wheel Body Position
			dBodySetPosition ( Data.Axles[j].Wheels[i].Body,
							   (dReal)Data.Axles[j].Wheels[i].Position.x,
							   (dReal)Data.Axles[j].Wheels[i].Position.y,
							   (dReal)Data.Axles[j].Wheels[i].Position.z );
		}
	}

}

// Set Body Rotation
void CVehiclePhysics::SetRotation( dReal x, dReal y, dReal z )
{

	// Reset Velocities
	dBodySetLinearVel( BodyVeh, 0, 0, 0 );
	dBodySetAngularVel( BodyVeh, 0, 0, 0 );

	// Convert To Radians
	dReal phi = dReal(DEG2RAD(x));
	dReal theta = dReal(DEG2RAD(y));
	dReal psi =dReal(DEG2RAD(z));

	// Set Rotation
	dMatrix3 R;
	dRFromEulerAngles ( R, phi, theta, psi );
	dBodySetRotation( BodyVeh, R );

	// Loop Axles Per Vehicle
	for ( unsigned int j = 0; j != Data.Axles.size( ); j++ )
	{
		// Loop Wheels Per Axle
		for ( unsigned int i = 0; i != Data.Axles[j].Wheels.size( ); i++ )
		{
			// Set Velocities
			dBodySetLinearVel( Data.Axles[j].Wheels[i].Body, 0, 0, 0 );
			dBodySetAngularVel( Data.Axles[j].Wheels[i].Body, 0, 0, 0 );

			dBodySetRotation( Data.Axles[j].Wheels[i].Body, R );
		}
	}


}

// Position
void CVehiclePhysics::Position( dReal x, dReal y, dReal z )
{

	// Reset Velocities
	dBodySetLinearVel( BodyVeh, 0, 0, 0);
	dBodySetAngularVel( BodyVeh, 0, 0, 0);
	dBodySetLinearVel( BodyCOG, 0, 0, 0);
	dBodySetAngularVel( BodyCOG, 0, 0, 0);

    // Position Body
	dBodySetPosition(BodyVeh, x, y, z);

	// Position Center Of Gravity Body
	dBodySetPosition( BodyCOG, x + (dReal)Data.COG.x,
                               y + (dReal)Data.COG.y,
							   z + (dReal)Data.COG.z );


	// Loop Axles Per Vehicle
	for ( unsigned int j = 0; j != Data.Axles.size( ); j++ )
	{
		// Loop Wheels Per Axle
		for ( unsigned int i = 0; i != Data.Axles[j].Wheels.size( ); i++ )
		{
			// Set Body Position
			dBodySetPosition ( Data.Axles[j].Wheels[i].Body,
							   (dReal)Data.Axles[j].Wheels[i].Position.x + x,
							   (dReal)Data.Axles[j].Wheels[i].Position.y + y,
							   (dReal)Data.Axles[j].Wheels[i].Position.z + z );

			dBodySetLinearVel( Data.Axles[j].Wheels[i].Body, 0, 0, 0);
			dBodySetAngularVel( Data.Axles[j].Wheels[i].Body, 0, 0, 0);

		}
	}

}

// Position Vehicle
void CVehiclePhysics::PositionVehicle( CVector4 Position, CVector4 Rotation )
{

    Position /= 32;
    Position.x += 8;
    Position.y += 0;
    Position.z += 8;

    float Modl[16];
    glLoadIdentity( );
    glPushMatrix( );
    glRotated( 270 - Rotation.z  * (180 / PI), 0, 1, 0);
    glRotated( -Rotation.x * (180 / PI), 1, 0, 0 );
    glRotated( Rotation.y * (180 / PI), 0, 0, 1 );
    glGetFloatv( GL_MODELVIEW_MATRIX, Modl );
    glPopMatrix( );

    dReal Pos[3]; Pos[0] = Pos[1] = Pos[2] = 0;

	dMatrix3 R;
	dRSetIdentity( R );
	OGLtoODE( Modl, Pos, R );

	dBodySetLinearVel( BodyVeh, 0, 0, 0);
	dBodySetAngularVel( BodyVeh, 0, 0, 0);

	dBodySetLinearVel( BodyCOG, 0, 0, 0);
	dBodySetAngularVel( BodyCOG, 0, 0, 0);

	// Position Vehicle Body
	dBodySetPosition( BodyVeh, Position.z, Position.y, Position.x );

	// Position Center Of Gravity Body
	dBodySetPosition( BodyCOG, Position.z + (dReal)Data.COG.x,
                               Position.y + (dReal)Data.COG.y,
                               Position.x + (dReal)Data.COG.z );

    dBodySetRotation( BodyVeh, R );
    dBodySetRotation( BodyCOG, R );


	// Loop Axles Per Vehicle
	for ( unsigned int j = 0; j != Data.Axles.size( ); j++ )
	{
		// Loop Wheels Per Axle
		for ( unsigned int i = 0; i != Data.Axles[j].Wheels.size( ); i++ )
		{


			dBodySetLinearVel( Data.Axles[j].Wheels[i].Body, 0, 0, 0);
			dBodySetAngularVel( Data.Axles[j].Wheels[i].Body, 0, 0, 0);

			// Set Wheel Body Position
			dBodySetPosition ( Data.Axles[j].Wheels[i].Body,
                             (dReal)Data.Axles[j].Wheels[i].Position.x + Position.z,
                             (dReal)Data.Axles[j].Wheels[i].Position.y + Position.y,
                             (dReal)Data.Axles[j].Wheels[i].Position.z + Position.x );

			dBodySetRotation( Data.Axles[j].Wheels[i].Body, R );


		}
	}

}

// Steer Wheel Helper Function
void CVehiclePhysics::SteerWheel(int AxleID, int WheelID, dReal Angle, int Left)
{

	// Steer Left
	if ( Left )
	{
		dJointSetHinge2Param (Data.Axles[AxleID].Joints[WheelID], dParamLoStop, Angle);	// Low
		dJointSetHinge2Param (Data.Axles[AxleID].Joints[WheelID], dParamHiStop, Angle);	// High
	}
	else // Steer Right
	{
		dJointSetHinge2Param (Data.Axles[AxleID].Joints[WheelID], dParamHiStop, Angle);	// High
		dJointSetHinge2Param (Data.Axles[AxleID].Joints[WheelID], dParamLoStop, Angle);	// Low
	}

}



