
// Define
#ifndef CXML_H
#define CXML_H

// Include
#include "Shared.h"             // Shared Header File

// Xml Node Class
class CXmlNode
{

    private:

        string Parent;              // Parent Name
        string Name;                // Name
		vector <CXmlNode> Child;	// Child Nodes
		vector <string> Attrib;		// Attributes
		vector <string> Value;		// Values
		string Text;				// Text

    public:

		CXmlNode( );				// Constructor
		~CXmlNode( );			    // Deconstructor

		void SetName( string N ) { Name = N; }			// Sets Node Name
		string GetName( void ) { return Name; }			// Gets Node Name

		void SetParent( string P ) { Parent = P; }		// Sets Node Parent Name
		string GetParent( void ) { return Parent; }		// Gets Node Parent Name

		void SetText( string T ) { Text = T; }			// Sets Node Text
		string GetText( void ) { return Text; }			// Gets Node Text

		string GetAttribute( string AttribName, string Default = "" );	// Get Attribute
		void SetAttribute( string AttribName, string AttribValue );		// Set Attribute

		int AttributeCount( void ) { return (int)Attrib.size(); }		// Get Attribute Count
		int ChildrenCount( void ) { return (int)Child.size(); }			// Get Children Count

		void AddChild( string Parent, CXmlNode Node ) { Node.Parent = Parent; Child.push_back( Node ); } // Add Child Node
		CXmlNode GetChild( int Index ) { return Child[Index]; }											 // Get Child Node

		string AttributeName( int Index ) { return Attrib[Index]; }		// Get Attribute Name
		int AttributeLookup( string AttribName );						// Find Attribute

};

// Xml Parser Class
class CXmlParser
{

    private:

        CXmlNode Node;			// Xml Node
		CXmlNode XmlNode;		// Xml Node

		string C;				// Character String

		int Position;			// File Position
		int Line;				// Line Number
		int Column;				// Column Number

		bool Xml;				// Xml Tag

		ifstream iXml;			// File Stream

		void GetChar( void );									// Get Single Character
		void Match( string With );								// Match Character
		void SkipWhiteSpaces( void );							// Skip All White Spaces
		void SkipComment( void );								// Skips Comment
		void SeekPosition( long Pos );							// Seek File Position
		bool ValidChar( void );									// Validates Characters

		enum ParserError		// Enumerate Parsing Errors
		{
			XmlExpected = 0,	// Xml Tag Expected
			MismatchedTag = 1	// Mismatch End Tag
		};

		void Abort( ParserError ErrType, string Info = "" );	// Abort Parse Error

    public:

		CXmlParser( );	                                        // Constructor
        ~CXmlParser( );                                         // Destructor

		bool ParseFile( string Filename );						// Parse Xml File

		string GetName( void );									// Gets Tag Name
		string GetValue( string Terminator );					// Gets Attribute Value

		CXmlNode ParseNode( string NodeName );					// Parses Node
		CXmlNode ParseParentNode( void );						// Parses Xml Parent Node

		void ReadAttributes( CXmlNode &Node );					// Read Node Attributes
		string ReadString( int Length = 1 );					// Read String Of Characters

		CXmlNode GetNode( void ) { return Node; }							// Get Node
		string FindValue(string ParentName, string Name, CXmlNode Node);	// Match Value


};


#endif  // Endif
