
// Define
#ifndef CTRUCK_H
#define CTRUCK_H

// Include
#include "Shared.h"                 // Common Header File

#include "CPods.h"                  // Pod Class Header File
#include "CModels.h"                // Models Class Header File

// Truck Class
class CTruck
{

    private:

		string TruckName;		    // Truck Name
		string ModelName;		    // Truck Model Name
		string TireName;		    // Truck Tire Name
		string AxleName;		    // Truck Axle Name

		CVector4 FLTire;		    // Front Left Tire
		CVector4 FRTire;		    // Front Right Tire
		CVector4 RLTire;		    // Rear Left Tire
		CVector4 RRTire;		    // Rear Right Tire

        CVector4 AxleBarOffset;     // Axle Bar Offset
        CVector4 FrontAxle;         // Front Axle Position
        CVector4 RearAxle;          // Rear Axle Position

		CVector4 ScrapePoints[13];	// Truck Scrape Points

    protected:

    public:

        CTruck( );                      // Constructor
        ~CTruck( );                     // Destructor

		CBin cTruckModel;				// Truck Model
		CBin cLeftTire;					// Truck Tire Model
		CBin cRightTire;				// Truck Tire Model
        CBin cAxleModel;                // Truck Axle Model

		// Load MTM Truck File
		bool LoadTruck( string Filename, CPodIni *cPodIni );

		// Load Truck, Tire, Axle Models
		bool LoadTruckModels( string TruckModel, string TireModel, string AxleModel, CPodIni *cPodIni );

        // Parse X, Y, Z String
        CVector4 ParseVector( string Line );

        // Get Axle Bar Offset
        CVector4 GetAxleBarOffset( void ) { return AxleBarOffset; }

		// Get Front Left Tire Position
		CVector4 GetFLTire( void ) { return FLTire; }

		// Get Front Right Tire Position
		CVector4 GetFRTire( void ) { return FRTire; }

		// Get Rear Left Tire Position
		CVector4 GetRLTire( void ) { return RLTire; }

		// Get Rear Right Tire Position
		CVector4 GetRRTire( void ) { return RRTire; }

        // Get Rear Axle Position
        CVector4 GetRearAxlePos( void ) { return CVector4( 0, RLTire.y/32, RLTire.z/32); }

        // Get Front Axle Position
        CVector4 GetFrontAxlePos( void ) { return CVector4( 0, FLTire.y/32, FLTire.z/32); }

		// Get Scrape Point
		CVector4 GetScrapePoint( int Index ){ return ScrapePoints[Index]; }

		// Get Truck Model Name
		string GetModelName( void ) { return ModelName; }

		// Get Tire name
		string GetTireName( void ) { return TireName; }

        // Get Truck Model Display List
        GLuint GetTruckModel( void ) { return cTruckModel.ModelList( ); }

        // Get Truck Left Tire Display List
        GLuint GetLeftTireModel( void ) { return cLeftTire.ModelList( ); }

        // Get Truck Model Display List
        GLuint GetRightTireModel( void ) { return cRightTire.ModelList( ); }

        // Get Axle Model Display List
        GLuint GetAxleModel( void ) { return cAxleModel.ModelList( ); }


};

#endif // End ifndef
