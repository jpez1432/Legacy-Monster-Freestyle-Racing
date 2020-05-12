
// Define
#ifndef _CQUADTREE_H
#define _CQUADTREE_H

// Include
#include "Shared.h"             // Common Header File

#include "CTrack.h"			    // Track Class Header File

// QuadTree Node
class CNode
{

	public:

		CVector4 B[4], Center;					// Bounding Box, Center
		CNode *Child[4];						// Children Nodes
		bool Leaf;								// Leaf Flag

		// Return Number Of Models In Leaf
		int NumModels( void ) { return ListModels.size( ); }

		// Return Model Index From List Models In Leaf
		int ModelIndex( int Index ) { return ListModels[Index]; }

		// Index List Models
		vector <int> ListModels;				// List Models

		CNode( );								// Constructor

};

// QuadTree Class
class CQuadTree
{

	private:

		int m_LeafSize;     // Leaf Size
		CNode *m_Root;      // Root Node

		CNode *InitNode( CVector4 Bounds[4], CTrack *cTrack );					// Initializes Node
		void CloseNode( CNode *cNode );											// Close Node
		void GetBounds( CVector4 Out[4], CVector4 Offset, double Size, int I );	// Get Node Bounds


	public:

		CQuadTree( ) { m_LeafSize = 0; m_Root = 0; }			// Constructor
		~CQuadTree( ) { if (m_Root != 0) delete m_Root; }		// Destructor

		// Initialize QuadTree
		void Initialize( int Size, int LeafSize, CTrack *cTrack );

		// Get Root Node
		CNode *GetRoot( void ) { return m_Root; }

};

#endif
