
// Include
#include "CQuadTree.h"					// QuadTree Class Header File

// Constructor
CNode::CNode( )
{

	for (int i = 0; i < 4; ++i)			// Loop Children
		Child[i] = 0;					// Set Child Null
		Leaf = false;					// Set Leaf Flag

}

// Return Bounding Box
void CQuadTree::GetBounds( CVector4 Out[4], CVector4 Offset, double Size, int I )
{

	CVector4 Shift;

	if	(I == 1) Shift = CVector4( Size, 0.0f, 0.0f );
	else if	(I == 2) Shift = CVector4( 0.0f, 0.0f, Size );
	else if	(I == 3) Shift = CVector4( Size, 0.0f, Size );

	// Calculate Bounding Box
	Out[0] = Offset	+ Shift;
	Out[1] = Offset + CVector4( Size, 0.0f, 0.0f ) + Shift;
	Out[2] = Offset + CVector4( 0.0f, 0.0f, Size ) + Shift;
	Out[3] = Offset + CVector4( Size, 0.0f, Size ) + Shift;

}

// Initialize Quadtree Node
CNode *CQuadTree::InitNode( CVector4 Bound[4], CTrack *cTrack )
{

	// Calculate Size
	double Size = Bound[1].x - Bound[0].x;

	// Create New Node
	CNode *newCNode = new CNode;
	newCNode->Center = (Bound[3] - Bound[0]) * 0.5 + Bound[0];

	// Set Boundaries
	for (int i = 0; i < 4; ++i)
		newCNode->B[i] = Bound[i];

	// Is Leaf Flag
	if ((int)Size == m_LeafSize)
	{
		// Set Leaf Flag
		newCNode->Leaf = true;

		// Loop Models
		for ( int i = 0; i != cTrack->NumberOfModels(  ); i++ )
		{

			// Get Model Position
			CVector4 Position = cTrack->ModelPosition( i );

			// Reposition
			Position.z = Position.z / 32;
			Position.y = Position.y / 32;
			Position.x = Position.x / 32;

			// Inside X Coordinates
			if ( Position.z >= newCNode->Center.x - 12 && Position.z <= newCNode->Center.x + 12 )
			{
				// Inside Z Coordinates
				if ( Position.x >= newCNode->Center.z - 12 && Position.x <= newCNode->Center.z + 12 )
				{
					// Push Back Model Index
					newCNode->ListModels.push_back( i );
				}
			}
		}

	}
	else	// Node
	{
		CVector4 B[4];	// Bound Vector

		// Loop Children
		for (int i = 0; i < 4; ++i)
		{
			// Get Bounds, Init Child Node
			GetBounds( B, Bound[0], Size/2.0f, i );
			newCNode->Child[i] = InitNode( B, cTrack );
		}
	}

	return newCNode;

}

// Closes Node
void CQuadTree::CloseNode( CNode *cNode )
{

	// Loop Child Nodes
	for (int i = 0; i != 4; ++i)
	{
		// Child Node
		if ( cNode->Child[i] != 0 )
		{
			// Close Node
			CloseNode( cNode->Child[i]);
		}
	}

	// Delete Node
	delete cNode;

}

// Initializes QuadTree
void CQuadTree::Initialize( int Size, int LeafSize, CTrack *cTrack )
{

	CVector4 B[4];
	m_LeafSize = LeafSize;									// Set Leaf Size
	GetBounds( B, CVector4(0.0f, 0.0f, 0.0f), Size, 0 );	// Get Initial Bounding Size
	m_Root = InitNode( B, cTrack );							// Initialize Root Node

}
