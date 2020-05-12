
// Define
#ifndef CVECTOR_H
#define CVECTOR_H

// Include
#include<math.h>                // Math Header File

#include "Shared.h"             // Shared Header File

// Vector4 Class
class CVector4
{
    public:

        CVector4();                                        // Constructor.
        CVector4(float X, float Y, float Z);               // Overloaded constructor.
        CVector4(float X, float Y, float Z, float W);      // Overloaded constructor again.
        CVector4(const CVector4 &v);                       // Another overloaded constructor.

        void operator=(CVector4 v);                        // Overloaded = sign.
        CVector4 operator-(CVector4 v);                    // Overloaded - sign.
        CVector4 operator+(CVector4 v);                    // Overloaded + sign.
        CVector4 operator*(CVector4 v);                    // Overloaded * sign.
        CVector4 operator/(CVector4 v);                    // Overloaded / sign.
        CVector4 operator+(float f);                       // Overloaded + sign.
        CVector4 operator-(float f);                       // Overloaded - sign.
        CVector4 operator*(float f);                       // Overloaded * sign.
        CVector4 operator/(float f);                       // Overloaded / sign.

        void operator +=(CVector4 v);                      // Overloaded += sign.
        void operator -=(CVector4 v);                      // Overloaded -= sign.
        void operator *=(CVector4 v);                      // Overloaded *= sign.
        void operator /=(CVector4 v);                      // Overloaded /= sign.
        void operator +=(float f);                         // Overloaded += sign.
        void operator -=(float f);                         // Overloaded -= sign.
        void operator *=(float f);                         // Overloaded *= sign.
        void operator /=(float f);                         // Overloaded /= sign.

        bool operator ==(CVector4 v);                      // Overloaded == sign.
        bool operator !=(CVector4 v);                      // Overloaded != sign.

        void CrossProduct(CVector4 v1, CVector4 v2);                // Stores cross product of v1, v2.
        void CrossProduct3(CVector4 v1, CVector4 v2, CVector4 v3);  // Stores cross product of v1, v2, v3.
        float DotProduct3(CVector4 v1);                             // Dot 3 on v1 and this.
        float DotProduct4(CVector4 v1);                             // Returns dot product 4 of v1 and this.
        float GetLength();                                          // Return this objects length.
        void Normal();                                              // Normalize this vector.
        void Normalize(CVector4 Triangle[]);                        // Find the normal of a triangle.

        CVector4 GetRotatedX(double angle);                         // Rotate along x.
        CVector4 GetRotatedY(double angle);                         // Rotate alone y.
        CVector4 GetRotatedZ(double angle);                         // Rotate along z.
        CVector4 GetRotatedAxis(double angle, CVector4 axis);       // Rotate along an axis.
        void ClampTo01();                                           // Clamp this vector between 0 and 1.


        float x, y, z, w;                                       // vertex's x, y, and z info.

};

#endif
