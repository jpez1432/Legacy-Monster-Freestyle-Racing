
// Include
#include "Shared.h"                 // Shared Header File

// Converts A String To Upper Case
string StringToUpper(string str)
{

	// Convert Upper
	for ( unsigned int i = 0; i != str.length(); i++)
		str[i] = toupper( str[i] );

	return str;

}

// Instr Function
int Instr(int startPOS, string strSearch, string strFind)
{

	// Convert Uppercase
	strSearch = StringToUpper(strSearch);

	// Loop Length Of String
	for ( unsigned int j = 1; j != strFind.length(); j++ )
	{
		// Find Character
		int i = (int)strSearch.find( strFind.substr(j,1), startPOS );
		if ( i != -1 ) return i;	// Found Start Character
	}

	return -1;	// Error Occured

}

// String Replace Function
string Replace(string Base, string Old, string New)
{

    unsigned int iIndex = (int)Base.find(Old, 0);

	if ( iIndex == string::npos )
    return Base;

    int iLength = (int)Old.length();

	Base = Base.erase(iIndex, iLength);
    Base = Base.insert(iIndex, New);

    return Base;

}

// Convert ODE Matrix To OpenGL Matrix
void ODEtoOGL( float* M, const dReal* P, const dReal* R )
{

    M[0]  = (float)R[0]; M[1]  = (float)R[4]; M[2]  = (float)R[8];  M[3]  = 0.0f;
    M[4]  = (float)R[1]; M[5]  = (float)R[5]; M[6]  = (float)R[9];  M[7]  = 0.0f;
    M[8]  = (float)R[2]; M[9]  = (float)R[6]; M[10] = (float)R[10]; M[11] = 0.0f;
    M[12] = (float)P[0]; M[13] = (float)P[1]; M[14] = (float)P[2];  M[15] = 1.0f;

}

// Convert ODE Matrix To OpenGL Matrix
void OGLtoODE( const float* M, const dReal* P, dReal* R )
{

    R[0]  = (float)M[0]; R[4]  = (float)M[1]; R[8]  = (float)M[2];  R[3]  = 0.0f;
    R[1]  = (float)M[4]; R[5]  = (float)M[5]; R[9]  = (float)M[6];  R[7]  = 0.0f;
    R[2]  = (float)M[8]; R[6]  = (float)M[9]; R[10] = (float)M[10]; R[11] = 0.0f;
    R[12] = (float)P[0]; R[13] = (float)P[1]; R[14] = (float)P[2];  R[15] = 1.0f;

}
