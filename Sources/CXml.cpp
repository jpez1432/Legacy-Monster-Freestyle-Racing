
// Include
#include "CXml.h"                 // Xml Class Header File

// Constructor
CXmlParser::CXmlParser( )
{


}

// Destructor
CXmlParser::~CXmlParser( )
{



}

// Constructor
CXmlNode::CXmlNode( )
{



}

// Destructor
CXmlNode::~CXmlNode( )
{

    // Loop Node Data
    for ( unsigned int i = 0; i != Child.size( ); i++ )
    {
        // Empty Data Vectors
        Child[i].Attrib.empty( );
        Child[i].Value.empty( );

    }

    // Empty Child
    Child.empty( );

}

// Xml Find Parent & Node Match Value
string CXmlParser::FindValue(string ParentName, string Name, CXmlNode Node)
{

	static string Value;

	// Match Value
	if ( StringToUpper( Node.GetName( ) ) == Name &&
		 StringToUpper( Node.GetParent( ) ) == ParentName )

		// Value Text
		Value =  Node.GetText( );

	// Continue Recursivly
	for ( int i = 0; i != Node.ChildrenCount( ); i++ )
		FindValue( ParentName, Name, Node.GetChild( i ) );

	return Value;	// Value

}

// Find Attribute
int CXmlNode::AttributeLookup( string AttribName )
{

	// Loop Attributes
	for ( unsigned int i = 0; i != Attrib.size( ); i++)
		// Match Name, Return Index
		if (Attrib[i] == AttribName) return i;

	return 0;	// Not Found

}

// Get Attribute
string CXmlNode::GetAttribute( string AttribName, string Default )
{

	// Find Attribute
	int i = AttributeLookup( AttribName );

	if ( i > 0 )			// Bounds Check
		return Value[i];	// Return Value
	else
		return Default;		// Not Found

}


// Set Attribute
void CXmlNode::SetAttribute( string AttribName, string AttribValue )
{

	// Find Attribute
	int i = AttributeLookup( AttribName );

	if ( i > 0 )							// Bounds Check
		Value[i] = AttribValue;				// Set Attribute Value
	else
	{
		Attrib.push_back( AttribName );		// Add New Attribute
		Value.push_back( AttribValue );		// Add New Value
	}

}

// Parse Xml File
bool CXmlParser::ParseFile( string Filename )
{


	// Open Xml File
	iXml.open( Filename.c_str() );

	// Couldn't Open File
	if ( iXml.is_open( ) == true )
    {

        // Parse Xml File
        Node = ParseParentNode( );

        // Close Xml File
        iXml.close( );

        // Ok
        return true;

    }

    // Error
    return false;

}

// Abort Parse Error
void CXmlParser::Abort( ParserError ErrType, string Info )
{

	string Description;	// Error Description String
	char Buff[10];

	switch ( ErrType )
	{

		// Xml Expected
		case ( XmlExpected):
			{
				// Build Description String
				Description = "Expected: ";
				Description.append(Info);
				Description.append(" At Line ");

				_itoa(Line, Buff, 10);

				Description.append(Buff);
				Description.append("[");

				_itoa(Column, Buff, 10);

				Description.append(Buff);
				Description.append("]");
			}
			return;

			case ( MismatchedTag ):
			{

				// Build Description String
				Description = "Mismatched Tag: '";
				Description.append(Info);
				Description.append("' At Line ");

				_itoa(Line, Buff, 10);

				Description.append(Buff);
				Description.append("[");

				_itoa(Column-2, Buff, 10);

				Description.append(Buff);
				Description.append("]");

			}
			return;

	}

	// Display Error
	MessageBox( NULL, Description.c_str( ), "Error", MB_OK );
	//exit (0);	// Quit Program

}


// Get Single Character
void CXmlParser::GetChar( void )
{

	C = ReadString();	// Read Single Character From Xml String

}

// Matches Character
void CXmlParser::Match( string With )
{


	if ( C == With )				// Matched
		GetChar( );					// Get Another Character
	else
		Abort( XmlExpected, With );	// Abort Error

}

// Skips All White Spaces
void CXmlParser::SkipWhiteSpaces( void )
{

	// White Space, Get Single Character
	while ( isspace( C[0] ) ) GetChar( );

}

// Validate Name Strings
bool CXmlParser::ValidChar( void )
{

	// Alpha & Numeric & Special Characters Validation Check
	if ( ( isalpha( C[0] ) != 0 ) || ( isdigit(  C[0] ) != 0 ) ||  ( Instr( 0 , C, "`!A@$^_-:" ) != -1 ) )
		return true;	// True

	return false;		// False


}

// Skips Comment
void CXmlParser::SkipComment( void )
{

	if ( C == "<" )									// Match Opening Bracket
	{
		if ( ReadString( 3 ) == "!--" )				// Match Opening Comment
		{
			do
			{
				GetChar( );							// Get Single Character

				if ( C == "-" )
				{
					if ( ReadString( 2 )  == "->" ) // Match Ending Comment
					{
						GetChar( );					// Get Single Character
						SkipWhiteSpaces( );			// Skip All White Spaces
						//SkipComment( );				// Skip Comments
						break;
					}
					else
					{
						Position = iXml.tellg();				// Set File Position
						iXml.seekg( Position - 1, ios::cur );	// Seek Charcter Back
					}
				}
			} while ( !iXml.eof( ) );				// Loop End Of File
		}
		else
		{
			Position = iXml.tellg();				// Set File Position
			iXml.seekg( Position - 3, ios::beg );	// Seek 3 Characters Back
		}
	}

	SkipWhiteSpaces( );								// Skip All White Spaces

}


// Gets Tag Name
string CXmlParser::GetName( void )
{

	string N;			// Temp Name String

	// Validate Characters & Not End Of File
	while ( ValidChar( ) && !iXml.eof( ) )
	{
		N.append( C );	// Append Character
		GetChar( );		// Get Another Character
	}

	SkipWhiteSpaces( );	// Skip All White Spaces

	return N;			// Return Name

}

// Gets Attribute Value
string CXmlParser::GetValue( string Terminator )
{

	string V;			// Temp Value String

	// Not Terminator & Not End Of File
	while ( C != Terminator && !iXml.eof( ) )
	{
		V.append( C );	// Append Character
		GetChar( );		// Get Another Character
	}

	SkipWhiteSpaces( );	// Skip All White Spaces

	return V;	// Return Value

}

// Parses Node
CXmlNode CXmlParser::ParseNode( string NodeName )
{

	CXmlNode Ret;				// Return Node
	CXmlNode Child;				// Child Node

	string Name;				// Node Name
	bool Closed;				// Tag Closed Flag

	Ret.SetName( NodeName );	// Set Node Name
	Closed = false;				// Default False

	SkipWhiteSpaces( );			// Skip All White Spaces

	ReadAttributes( Ret );		// Read Node Attributes

	if ( C == ">" )				// Validate Bracket
	{
		Match( ">" );			// Match Closing Bracket
		SkipWhiteSpaces( );		// Skip All White Spaces
		//SkipComment( );			// Skip Comments

		if ( C== "<" )			// Match Open Bracket
		{
			do
			{
				Match( "<" );			// Match Closing Bracket

				SkipWhiteSpaces( );		// Skip All White Spaces

				if ( C == "/" )			// Match Slash
				{
					Match( "/" );		// Match Slash

					Name = GetName( );	// Get Name

					// Validate Name
					if ( Name == NodeName )
					{
						Match( ">" );	// Match Bracket
						Closed = true;	// Set Closed Flag
						break;			// Exit Loop
					}
					else	// Abort Error
						Abort( MismatchedTag, NodeName );

				}

				Name = GetName( );					// Get Name
				Child = ParseNode( Name );			// Parse Child Node
				Ret.AddChild( NodeName, Child );	// Add Child Node

				SkipWhiteSpaces( );					// Skip All White Spaces
				//SkipComment( );						// Skip Comments

			} while ( !iXml.eof( ) );				// Loop End Of File

		}
		else
			Ret.SetText( GetValue( "<" ) );			// Set Node Text

		if ( !Closed )								// Not Closed
		{
			Match( "<" );							// Match Opening Bracket
			Match( "/" );							// Match Slash

			Name = GetName( );						// Get Name
    Abort( MismatchedTag , NodeName );

			// Match Name
			if ( Name == NodeName )
				Match( ">" );						// Match Closing Bracket
			else
				Abort( MismatchedTag , NodeName );	// Display Error
		}
	}
	else
	{
		Match( "/" );								// Match Slash
		Match( ">" );								// Match Closing Bracket
	}

	SkipWhiteSpaces( );								// Skip All White Spaces

	return Ret;

}

// Parses Xml Parent Node
CXmlNode CXmlParser::ParseParentNode( void )
{

	string Name;				// Temp Name String

	GetChar( );					// Get Single Character
	SkipWhiteSpaces( );			// Skip All White Spaces
	//SkipComment( );				// Skip Comments

	Match( "<" );				// Match Open Bracket

	Xml = false;				// Default False

	while ( C == "?" )			// Loop Xml Declaration
	{
		if ( C == "?" )			// Found Xml Tag
		{
			Match( "?" );		// Match Question Mark

			Name = GetName( );	// Get Name

			if ( Name != "xml" )				// Xml Tag Not Found
				Abort( XmlExpected, "xml" );	// Display Error

			Xml = true;							// Set Xml Flag

			XmlNode.SetName( "xml" );			// Set Node Name

			ReadAttributes( XmlNode );			// Read Node Attributes

			GetChar( );							// Get Single Character
			SkipWhiteSpaces( );					// Skip All White Spaces
			//SkipComment( );						// Skip Comments

			Match( "<" );						// Match Open Bracket

		}
	}

	Name = GetName( );							// Get Node Name

	return ParseNode( Name );					// Recursively Parse Nodes

}

// Read Node Attributes
void CXmlParser::ReadAttributes( CXmlNode &Node )
{

	string Name;							// Temp Name String
	string Value;							// Temp Value String

	while ( C[0] != '/' && C[0] != '>' && !iXml.eof( ) )
	{

		SkipWhiteSpaces( );					// Skip All White Spaces

		Name = GetName( );					// Get Name

		Match( "=" );						// Match Equal Sign
		Match( "\"" );						// Match Qoute

		Value = GetValue( "\"" );			// Get Value

		Node.SetAttribute( Name, Value );	// Set Node Attributes

		Match( "\"" );						// Match Qoute

		SkipWhiteSpaces( );					// Skip All White Spaces

		if ( Xml && C[0] == '?' )			// Has Xml Tag
			Match( "?" );					// Match Question Mark

	}

}

// Seeks File Position
void CXmlParser::SeekPosition( long Pos )
{

	Position = Pos;						// Set Position Value

	iXml.seekg( Position, ios::beg);	// Seek File Position

}

// Read String Of Characters
string CXmlParser::ReadString( int Length )
{

	char cBuffer;							// Character
	string sBuffer;							// String Buffer

	// Loop Length
	for (int i = 1; i <= Length; i++)
	{
		cBuffer = iXml.get();				// Get Character
		sBuffer = sBuffer + cBuffer;		// Append Character
	}

	return sBuffer;			// Return String Buffer

}
