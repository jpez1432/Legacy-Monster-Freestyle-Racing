
// Define
#ifndef CVEHICLEPHYSICS_H
#define CVEHICLEPHYSICS_H

// Include
#include "Shared.h"                     // Common Header File
#include "CWindowGL.h"                  // Window Class Header
#include "CConfig.h"                    // Config Class Header

#include "CPhysicsServer.h"				// Physics Server Header
#include "CPhysicsClient.h"				// Physics Client Header

#include "CTruck.h"                     // Truck Class Header File

// Vehicle GearBox
struct Transmission
{
	float GearA[4][4];					// Gears A
	float GearU[4][4];					// Gears U
	int Gear;							// Current Gear
	int Rpm;							// Rpms
	int Mph;							// Miles Per Hour
	dReal Acceleration;					// Acceleration
	dReal Decceleration;				// Decceleration
};

// Vehicle Wheel
struct Wheel
{
	dReal Radius;						// Wheel Radius
	dReal Width;						// Wheel Width
	dReal Mass;							// Wheel Mass
	dReal SteerMul;						// Steering Multiplier
	dReal DriveMul;						// Drive Multiplier
	dReal Shock;						// Shock Density
	dReal Error;						// Shock Error
	dReal StopERP;                      // Stop Error
	dReal StopCFM;                      // Stop CFM
	dReal Bounce;                       // Bounce
	dReal CFM;                          // CFM
	dReal FMax;                         // FMax
	dReal FudgeFactor;                  // Fudge Factor
	dReal Slip;                         // Wheel Slip
	dGeomID Geom;						// Wheel Geom
	dBodyID Body;						// Wheel Body
	PhysGeomData Data;					// Wheel Geom Attributes
	CVector4 Position;					// Wheel Position
};

// Vehicle Axle
struct Axle
{
	CVector4 Position;					// Axle Position
	CVector4 Rotation;					// Axle Rotation
	dReal Mass;							// Axle Mass
	dGeomID Geom;						// Axle Geom
	dBodyID Body;						// Axle Body
	vector<Wheel> Wheels;				// Vector Type Wheel
	vector<dJointID> Joints;			// Vector Type Joint
};

// Vehicle
struct Vehicle
{
	dReal Mass;							// Vehicle Mass
	CVector4 Dimensions;				// Vehicle Dimensions
	CVector4 COG;				        // Vehicle Center Of Gravity
	dReal COGScale;                     // Center Of Gravity Scale
	Transmission GearBox;				// Vehicle GearBox
	vector<Axle> Axles;				    // Vector Type Axle
	dReal MinSteering;					// Minimum Steering Angle
	dReal MaxSteering;					// Maximum Steering Angle
	dReal SteerSpeed;					// Steering Speed
	dReal FrontSteerAngle;				// Front Steering Angle
	dReal RearSteerAngle;				// Rear Steering Angle
	dReal FrontCurrentSteer;			// Current Front Steering Speed
	dReal RearCurrentSteer;			    // Current Rear Steering Speed
	dReal Brake;						// Brake Force
	dReal Drag;							// Drag Force
	dReal MaxSpeed;						// Maximum Speed
	dReal RevSpeed;						// Reverse Speed
	dReal WheelForce;					// Wheel Force
	dReal Speed;						// Vehicle Speed
};

// Vehicle Physics Class
class CVehiclePhysics : public CPhysicsClient
{

    private:

        bool Park;

		// LamDa Down
		float LamDa_Down( int K );

		// LamDa Up
		float LamDa_Up( int K );

		// Best Gear
		float BestGear( int G );

		// Automatic Gear
		void AutomaticGear( );

		// Check Gear
		void CheckGear( float Step );

        // Config Pointer
        CConfig *Config;

	private:

		// Get Geom Position
		const dReal* GetGeomPosition( dGeomID Geom );

		// Set Geom Position
		const dReal* GetGeomRotation( dGeomID Geom );

		// Add Geom Relative Position Rotation Body
		void AddGeom( dGeomID Geom, dReal xOffset, dReal yOffset, dReal zOffset, dMatrix3 R );

		Vehicle Data;		            // Vehicle Data
		float Velocity;					// Vehicle Speed

		dGeomID GeomCOG;                // Center Of Gravity Geom
		dBodyID BodyCOG;	            // Center Of Gravity Body
		dJointID JointCOG;			    // Center Of Gravity Joint

		dJointID JointFront;			// Front Axle Joint
		dJointID JointRear;			    // Rear Axle Joint
		dGeomID FrontAxle;			    // Front Axle Geom
		dGeomID RearAxle;			    // Rear Axle Geom

		PhysGeomData BodyData;			// Vehicle Body Attributes
		dGeomID GeomVeh;                // Vehicle Body Geom
		dBodyID BodyVeh;			    // Vehicle Body
		dSpaceID SpaceVeh;			    // Vehicle Space

		vector<dGeomID> Geoms;	        // Geoms

        float MassScale;                // Mass Scaling

		CXmlParser *cXml;		        // Xml Class Object

    public:

        int Gear;                       // Current Gear
        float Gas;                      // Sound

        CVehiclePhysics( );             // Constructor
        virtual ~CVehiclePhysics( );    // Destructor

		// Create Vehicle
		bool Create( string Filename, CConfig *cConfig );

        // Loads Vehicle Config
        bool LoadVehicle( string Filename );

        // Load Vehicle Wheels
        void LoadWheels(string ParentName, string Name, CXmlNode Node);

		// Update Simulation
		virtual bool Update( dReal Step );

		// Render
		virtual void Render( );

		// Returns Vehicle Body Position
		const dReal* GetPosition( );

		// Returns Vehicle Body Rotation
		const dReal* GetRotation( );

		// Set Body Position
		void SetPosition( dReal x, dReal y, dReal z );

		// Set Body Rotation
		void SetRotation( dReal x, dReal y, dReal z );

		// Position Vehicle
		void PositionVehicle( CVector4 Position, CVector4 Rotation );

		// Position
		void Position( dReal x, dReal y, dReal z );

		// Steer Wheel
		void SteerWheel(int AxleID, int WheelID, dReal Angle, int Left);

		// Get Vehicle Speed
		float GetSpeed( void );

		// Get GearBox Acceleration
		float GetGearBoxSpeed( void ) { return Data.GearBox.Acceleration; }

		// Return Speed
		float Speed( void ) { return (Velocity * 3600.0f / 1000.0f); }


};

#endif
