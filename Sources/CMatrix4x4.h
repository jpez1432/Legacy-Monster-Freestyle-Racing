
// Define
#ifndef CMATRIX4X4_H
#define CMATRIX4X4_H

// Include
#include "Shared.h"                 // Shared Header File

// Matrix4x4 Class
class CMatrix4x4
{

    public:

      CMatrix4x4();                                // Constructor.
      CMatrix4x4(const CMatrix4x4 &m);             // Overloaded constructors.
      CMatrix4x4(float r11, float r12, float r13, float r14,
                 float r21, float r22, float r23, float r24,
                 float r31, float r32, float r33, float r34,
                 float r41, float r42, float r43, float r44);
      ~CMatrix4x4();                               // Destructor.

      void Clear();                                // Reset the matrix.
      void Zero();                                 // Reset the matrix to all 0s.

      void operator=(CMatrix4x4 m);                // Overloaded = sign.
      CMatrix4x4 operator-(CMatrix4x4 m);          // Overloaded - sign.
      CMatrix4x4 operator+(CMatrix4x4 m);          // Overloaded + sign.
      CMatrix4x4 operator*(CMatrix4x4 m);          // Overloaded * sign.
      CMatrix4x4 operator/(CMatrix4x4 m);          // Overloaded / sign.

      void operator +=(CMatrix4x4 m);              // Overloaded += sign.
      void operator -=(CMatrix4x4 m);              // Overloaded -= sign.
      void operator *=(CMatrix4x4 m);              // Overloaded *= sign.
      void operator /=(CMatrix4x4 m);              // Overloaded /= sign.

      CMatrix4x4 operator-(float f);               // Overloaded - sign.
      CMatrix4x4 operator+(float f);               // Overloaded + sign.
      CMatrix4x4 operator*(float f);               // Overloaded * sign.
      CMatrix4x4 operator/(float f);               // Overloaded / sign.

      CVector4 operator *(CVector4 v);             // CVector4 * matrix overloaded *.

      void operator -=(float f);                   // Overloaded -= sign.
      void operator +=(float f);                   // Overloaded += sign.
      void operator *=(float f);                   // Overloaded *= sign.
      void operator /=(float f);                   // Overloaded /= sign.

      bool operator ==(CMatrix4x4 v);              // Overloaded == sign.
      bool operator !=(CMatrix4x4 v);              // Overloaded != sign.

      void Translate(CVector4 Translate);          // Translate a point in 3D.
      void Translate(float x, float y, float z);   // Translate a point in 3D.
      void Rotate(double angle, int x, int y,
                  int z);                          // Rotate a 3D point.
      bool inverseMatrix(CMatrix4x4 m);            // Inverse the current matrix data.
      bool invertMatrix(CMatrix4x4 m);             // Invert a matrix.
      CVector4 VectorMatrixMultiply(CVector4 v);
      CVector4 VectorMatrixMultiply3x3(CVector4 v);
      CVector4 transformPoint(CVector4 v);         // Transform a point against the matrix.
      bool CreateShadowMatrix(CVector4 planeNormal,
                              CVector4 lightPos);  // Create a shadow matrix.

      float matrix[16];                            // The 4x4 matrix in a 1D array.
};

#endif  // Endif
