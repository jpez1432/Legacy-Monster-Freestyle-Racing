
// Include
#include "CGenericPhysics.h"                    // Generic Physics Class Header File

// Constructor
CGenericPhysics::CGenericPhysics( )  : Body(0), Geom(0)
{


}

// Destructor
CGenericPhysics::~CGenericPhysics( )
{

    if ( Body )
	{
		dBodyDestroy( Body );
		Body = 0;
	}

    if ( Geom )
	{
		dGeomDestroy( Geom );
		Geom = 0;
	}


}

// Update Simulation
bool CGenericPhysics::Update( dReal Step )
{

	// Body, Geom
	if (Body == 0 || Geom == 0)
		return false;	// Error

	return true;

}

// Render
void CGenericPhysics::Render( )
{

	// Variables
	dVector3 Sides;
	float Matrix[16];

    // Model Flag
    if ( Model == false )
    {
        // Get Geom Dimensions
        dGeomBoxGetLengths ( Geom, Sides );
    }

	// Get Geom Position & Rotation
	const dReal *Pos = dGeomGetPosition( Geom );
	const dReal *Rot = dGeomGetRotation( Geom );

	// Convert ODE Matrix To OpenGL
	ODEtoOGL( Matrix, Pos, Rot );

	// Push Matrix
	glPushMatrix();

		// Multiply Position, Rotation Matrix
		glMultMatrixf( Matrix );

        // Model Flag
		if ( Model == true )
		{
		    // Call Model List
		    glCallList( ModelId );
		}
		else
		{
            // Render Box
            DrawBox( &Sides[0] );
		}

	// Pop Matrix
	glPopMatrix();

}

// Create Box Dimensions, Dynamic Or Static
bool CGenericPhysics::CreateBox( dReal Width, dReal Height, dReal Length, bool isStatic )
{

	// Enviroment
	if ( World == 0 || Space == 0 )
		return false;	// Error

	// Create Geometry
	Geom = dCreateBox( 0, Width, Height, Length );

	// Zero, Set Geom Attributes
	ZeroMemory( &EnvData, sizeof (EnvData) );
	EnvData.Slip = 0.00002;
	dGeomSetData( Geom, (void*)&EnvData );

	// Not Static, Attach Geometry
	if ( !isStatic )
	{

		// Create Body
		Body = dBodyCreate( World );

		// Setup Bodies Mass
		dMass m;
		dMassSetBox( &m, 1, Width, Height, Length );
		dBodySetMass( Body, &m );
		dGeomSetBody( Geom, Body );

	}

	return true;    // Ok

}

// Create TriMesh, Dynamic Or Static
bool CGenericPhysics::CreateTriMesh( CTrack* cTrack, int BinIndex, float Mass, bool isStatic )
{

	// Enviroment
	if ( World == 0 || Space == 0 )
		return false;	// Error

	// Zero, Set Geom Attributes
	ZeroMemory( &EnvData, sizeof (EnvData) );
	//dGeomSetData( Geom, (void*)&Data );

	int FaceCount = cTrack->cModels->Models[BinIndex].Faces.size();
    //FaceCount += cTrack->cModels->Models[BinIndex].SpecialColors.size();

    ModelId=glGenLists( 1 );			// Generate Model List
    glNewList( ModelId, GL_COMPILE );	// New Model List
    glDisable( GL_TEXTURE_2D );
    glColor3f( 1.0, 0.0, 0.0 );
    glBegin(GL_TRIANGLES);               // Triangles

	int VertexCount = cTrack->cModels->Models[BinIndex].Header.NumVerts;
	int IndexCount = FaceCount * 3;

	dVector3 *Vertices;
	Vertices = new dVector3[VertexCount];
	dTriIndex *Indices;
	Indices = new dTriIndex[IndexCount];

	for ( int i = 0; i != VertexCount; i++ )
	{
		Vertices[i][0] = dReal( cTrack->cModels->Models[BinIndex].Vertices[i].X * 0.000125);
		Vertices[i][1] = dReal( cTrack->cModels->Models[BinIndex].Vertices[i].Y * 0.000125);
		Vertices[i][2] = dReal( cTrack->cModels->Models[BinIndex].Vertices[i].Z * 0.000125);
	}

	int v = 0;
	for ( unsigned int i = 0; i != cTrack->cModels->Models[BinIndex].Faces.size(); i++)
	{

        Indices[v++] = cTrack->cModels->Models[BinIndex].Faces[i].Vertices[2].VertIndex;
        Indices[v++] = cTrack->cModels->Models[BinIndex].Faces[i].Vertices[1].VertIndex;
        Indices[v++] = cTrack->cModels->Models[BinIndex].Faces[i].Vertices[0].VertIndex;

        glVertex3f( Vertices[cTrack->cModels->Models[BinIndex].Faces[i].Vertices[0].VertIndex][0],
                    Vertices[cTrack->cModels->Models[BinIndex].Faces[i].Vertices[0].VertIndex][1],
                    Vertices[cTrack->cModels->Models[BinIndex].Faces[i].Vertices[0].VertIndex][2]);

        glVertex3f( Vertices[cTrack->cModels->Models[BinIndex].Faces[i].Vertices[1].VertIndex][0],
                    Vertices[cTrack->cModels->Models[BinIndex].Faces[i].Vertices[1].VertIndex][1],
                    Vertices[cTrack->cModels->Models[BinIndex].Faces[i].Vertices[1].VertIndex][2]);

        glVertex3f( Vertices[cTrack->cModels->Models[BinIndex].Faces[i].Vertices[2].VertIndex][0],
                    Vertices[cTrack->cModels->Models[BinIndex].Faces[i].Vertices[2].VertIndex][1],
                    Vertices[cTrack->cModels->Models[BinIndex].Faces[i].Vertices[2].VertIndex][2]);

	}

	glEnd( );
	glEndList( );
	glEnable(GL_TEXTURE_2D) ;

	dTriMeshDataID Data = dGeomTriMeshDataCreate();

	dGeomTriMeshDataBuildSimple(Data, *Vertices, VertexCount, Indices, IndexCount);

	Geom = dCreateTriMesh(0, Data, 0, 0, 0);

	// Enviroment Attributes
    float Slip = 0.0000005;
	EnvData.Slip = dReal( Slip );
	dGeomSetData( Geom, (void*)&EnvData );

	// Not Static, Attach Geometry
	if ( !isStatic )
	{

		// Create Body
		Body = dBodyCreate( World );

        CVector4 Max;
        CVector4 Min;

        // Get Dimensions
		cTrack->cModels->Models[BinIndex].Dimensions( Min, Max );

        // Get Model Dimensions
        CVector4 Dimensions = Max - Min;

		// Setup Bodies Mass
		dMass m;
		dMassSetBoxTotal( &m, Mass, Dimensions.x, Dimensions.y, Dimensions.z );
		dBodySetMass( Body, &m );
		dGeomSetBody( Geom, Body );


	}

	return true;

}


// Set Box Mass
void CGenericPhysics::SetMass( dReal Mass_Scale, dReal mass )
{

	// Body, Geom
	if (Body == 0 || Geom == 0)
	{
		return;	// Error
	}

	// Setup Body Mass
	dMass m;
	dBodyGetMass( Body, &m );
	dMassAdjust( &m, mass );//* (dReal)Mass_Scale );
	dBodySetMass( Body, &m );

}

// Sets Geom Data
void CGenericPhysics::SetGeomData( const PhysGeomData *pData )
{

    // Copy Geom Data
	memcpy( &EnvData, pData, sizeof( PhysGeomData ) );

}

// Position Box
void CGenericPhysics::SetPosition( dReal x, dReal y, dReal z )
{

	// Position Geom
	dGeomSetPosition( Geom, x, y, z );

}

// Rotate Box
void CGenericPhysics::SetRotation( dReal x, dReal y, dReal z, bool Box )
{

    float Modl[16];

    if ( Box == true )
    {

        glLoadIdentity( );
        glPushMatrix( );
        glTranslated( 0,0,0 );
        glRotated(270 - z  * (180 / PI), 0, 1, 0);
        glRotated( -x * (180 / PI), 1, 0, 0 );
        glRotated( y * (180 / PI), 0, 0, 1 );
        glScalef(1,1,-1);
        glGetFloatv( GL_MODELVIEW_MATRIX, Modl );
        glPopMatrix( );

    }
    else
    {

        glLoadIdentity( );
        glPushMatrix( );
        glTranslated( 0,0,0 );
        glRotated(270 - z  * (180 / PI), 0, 1, 0);
        glRotated( -x * (180 / PI), 1, 0, 0 );
        glRotated( y * (180 / PI), 0, 0, 1 );
        glScalef(1,1,-1);
        glGetFloatv( GL_MODELVIEW_MATRIX, Modl );
        glPopMatrix( );

    }

    const dReal *Pos = dGeomGetPosition( Geom );
	dMatrix3 R;
	dRSetIdentity( R );
	OGLtoODE( Modl, Pos, R );
	dGeomSetRotation( Geom, R );

}

// Add To Space
void CGenericPhysics::AddEnviroment( )
{

	// Add Geom To Space
	dSpaceAdd( Space, Geom );

}

void CGenericPhysics::DrawBox( const float sides[3] )
{

    float lx = sides[0]*0.5f;
    float ly = sides[1]*0.5f;
    float lz = sides[2]*0.5f;

    // sides
    glColor3f( 0.0, 0.0, 1.0 );
    //glLineWidth(4);
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glDisable(GL_TEXTURE_2D);
    glBegin (GL_TRIANGLE_STRIP);
        //glNormal3f (-1,0,0);
        glVertex3f (-lx,-ly,-lz);
        glVertex3f (-lx,-ly,lz);
        glVertex3f (-lx,ly,-lz);
        glVertex3f (-lx,ly,lz);
        //glNormal3f (0,1,0);
        glVertex3f (lx,ly,-lz);
        glVertex3f (lx,ly,lz);
        //glNormal3f (1,0,0);
        glVertex3f (lx,-ly,-lz);
        glVertex3f (lx,-ly,lz);
        //glNormal3f (0,-1,0);
        glVertex3f (-lx,-ly,-lz);
        glVertex3f (-lx,-ly,lz);
    glEnd();

    // top face
    glBegin (GL_TRIANGLE_FAN);
        //glNormal3f (0,0,1);
        glVertex3f (-lx,-ly,lz);
        glVertex3f (lx,-ly,lz);
        glVertex3f (lx,ly,lz);
        glVertex3f (-lx,ly,lz);
    glEnd();

    // bottom face
    glBegin (GL_TRIANGLE_FAN);
        //glNormal3f (0,0,-1);
        glVertex3f (-lx,-ly,-lz);
        glVertex3f (-lx,ly,-lz);
        glVertex3f (lx,ly,-lz);
        glVertex3f (lx,-ly,-lz);
    glEnd();
    glEnable(GL_TEXTURE_2D);
    //glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

}

