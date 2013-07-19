#ifndef ROEVECTOR3_H_
#define ROEVECTOR3_H_

#include <cmath>
#include <cstdio>
#include <string>
#include "RoeRandom.h"
#include "RoeCommon.h"
#ifdef ROE_COMPILE_PHYSICS
	#include <btBulletDynamicsCommon.h>
#endif

namespace roe {
	
	//forward declaration
	class  Vector3;
	inline Vector3 operator - (const Vector3& a, const Vector3& b);
	inline Vector3 operator + (const Vector3& a, const Vector3& b);
	inline Vector3 operator * (const float f, const Vector3& v);
	inline Vector3 operator * (const Vector3& v, const float f);
	inline Vector3 operator / (const Vector3& v, const float f);
	inline bool    operator ==(const Vector3& a, const Vector3& b);
	inline bool    operator !=(const Vector3& a, const Vector3& b);
	
	class Vector3
	{
	public:
		//variables
		union
		{
			struct
			{
				float x;  // cooridinates
				float y;
				float z;
			};
			
			struct
			{
				float u;  // texture coordinates
				float v;
				float w;
			};
			
			float rg[3]; // array
		};
		
		// constructors
		Vector3()                                                                                     {}
		Vector3(const Vector3& v) : x(v.x), y(v.y), z(v.z)                                            {}
		Vector3(const float f) : x(f), y(f), z(f)                                                     {}
		Vector3(const float _x, const float _y, const float _z) : x(_x), y(_y), z(_z)                 {}
		Vector3(const float* pfComponent) : x(pfComponent[0]), y(pfComponent[1]), z(pfComponent[2])   {}
#ifdef ROE_COMPILE_PHYSICS
		Vector3(const btVector3 v) : x(v.x()), y(v.y()), z(v.z()) {}
		Vector3& operator=(const btVector3& v) {x = v.x(); y = v.y(); z = v.z(); return *this;}
		operator btVector3() const {return btVector3(x,y,z);}
		btVector3 bt() const {return btVector3(x,y,z);}
#endif
		
		//destructors
		~Vector3() {}
		
		// casting operators
		operator float* () {return (float*)(rg);}
		
		// assignment operators
		Vector3& operator  = (const Vector3& v) {x  = v.x; y  = v.y; z  = v.z; return *this;}
		Vector3& operator += (const Vector3& v) {x += v.x; y += v.y; z += v.z; return *this;}
		Vector3& operator -= (const Vector3& v) {x -= v.x; y -= v.y; z -= v.z; return *this;}
		Vector3& operator *= (const Vector3& v) {x *= v.x; y *= v.y; z *= v.z; return *this;}
		Vector3& operator *= (const float f)    {x *= f;   y *= f;   z *= f;   return *this;}
		Vector3& operator /= (const Vector3& v) {x /= v.x; y /= v.y; z /= v.z; return *this;}
		Vector3& operator /= (const float f)    {x /= f;   y /= f;   z /= f;   return *this;}
		
		//other methods
		std::string toString   () const {char c[80]; sprintf(c,"Vector3(%f, %f, %f)",x,y,z); return std::string(c);}
		Vector3 cross          (const Vector3& v) const {return cross(*this, v);}
		float   dot            (const Vector3& v) const {return dot  (*this, v);}
		float   angleBetween   (const Vector3& v) const {return angleBetween(*this, v);}
		float   distance       (const Vector3& v) const {return std::sqrt((x-v.x)*(x-v.x)+(y-v.y)*(y-v.y)+(z-v.z)*(z-v.z));}
		float   distance2      (const Vector3& v) const {return           (x-v.x)*(x-v.x)+(y-v.y)*(y-v.y)+(z-v.z)*(z-v.z) ;}
		Vector3 midPoint       (const Vector3& v) const {return Vector3(x*v.x*0.5+y*v.y*0.5+z*v.z*0.5);}
		float   volume         ()                 const {return x*y*z;}
		float   length         ()                 const {return std::sqrt(x*x+y*y+z*z);}
		float   length2        ()                 const {return x*x+y*y+z*z;}
		void    normalize      ()                       {(*this) /= sqrt(x*x+y*y+z*z);}
		void    makeFloor      (const Vector3& v)       {if(v.x<x)x=v.x;if(v.y<y)y=v.y;if(v.z<z)z=v.z;}
		void    makeCeil       (const Vector3& v)       {if(v.x>x)x=v.x;if(v.y>y)y=v.y;if(v.z>z)z=v.z;}
		void    makeRandom     ()                       {(*this) = createRandomUnit();}
		
		static bool    isNaN        (const Vector3& v)                   {return v.x!=v.x || v.y!=v.y || v.z!=v.z;}
		static bool    isNearlyZero (const Vector3& v, float eps = 0.0001f) {return std::abs(v.x)<=eps && std::abs(v.y)<=eps && std::abs(v.z)<=eps;}
		static float   distance     (const Vector3& a, const Vector3& b)  {return (b-a).length ();}
		static float   distance2    (const Vector3& a, const Vector3& b)  {return (b-a).length2();}
		static Vector3 midPoint     (const Vector3& a, const Vector3& b)  {return 0.5*(a+b);}
		static float   length       (const Vector3& v)                   {return std::sqrt(v.x*v.x+v.y*v.y+v.z*v.z);}
		static float   length2      (const Vector3& v)                   {return v.x*v.x+v.y*v.y+v.z*v.z;}
		static Vector3 normalize    (const Vector3& v)                   {return v / std::sqrt(v.x*v.x+v.y*v.y+v.z*v.z);}
		static Vector3 floor        (const Vector3& a, const Vector3& b) {return Vector3((a.x<b.x)?a.x:b.x, (a.y<b.y)?a.y:b.y, (a.z<b.z)?a.z:b.z);}
		static Vector3 ceil         (const Vector3& a, const Vector3& b) {return Vector3((a.x>b.x)?a.x:b.x, (a.y>b.y)?a.y:b.y, (a.z>b.z)?a.z:b.z);}
		static Vector3 cross        (const Vector3& a, const Vector3& b) {return Vector3(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);}
		static float   dot          (const Vector3& a, const Vector3& b) {return a.x*b.x + a.y*b.y + a.z*b.z;}
		static float   angleBetween (const Vector3& a, const Vector3& b) {
			return acos((a.x*b.x + a.y*b.y + a.z*b.z) /
				sqrt((a.x*a.x+a.y*a.y+a.z*a.z) * (b.x*b.x+b.y*b.y+b.z*b.z)));} //returns angle as radian, works CCW
		static Vector3 triangleNormal(const Vector3& a, const Vector3& b, const Vector3& c) { //CCW
			return Vector3::cross(b-a, c-a);
		}
		static Vector3 makePerpendicularTo(const Vector3& v1, const Vector3& v2) { //makes v1 perpendicular to v2 (return value)
			return v1.length()*Vector3::normalize(Vector3::cross(Vector3::cross(v2,v1), v2));}
		static Vector3 createRandomBox(const Vector3& min, const Vector3& max) {
			const float x = Random::getSingletonPtr()->uniformReal(min.x,max.x);
			const float y = Random::getSingletonPtr()->uniformReal(min.y,max.y);
			const float z = Random::getSingletonPtr()->uniformReal(min.z,max.z);
			return Vector3(x,y,z);}
		static Vector3 createRandomUnit() {
			//const float z = Random::getSingletonPtr()->uniformReal(-1.0f,1.0f);
			//const float t = Random::getSingletonPtr()->uniformReal(0.0f, 2*3.14159265359f);
			//const float r = std::sqrt(1.0f - z*z);
			//return Vector3(r*std::cos(t), r*std::sin(t), z);
			Vector3 v;
			do {
				v.x = Random::getSingletonPtr()->uniformReal(-1.0,1.0);
				v.y = Random::getSingletonPtr()->uniformReal(-1.0,1.0);
				v.z = Random::getSingletonPtr()->uniformReal(-1.0,1.0);
			} while (v.length2() > 1.0);
			v.normalize();
			return v;
		}
		
		//special points
		static const Vector3 ZERO;
		static const Vector3 X;
		static const Vector3 Y;
		static const Vector3 Z;
		static const Vector3 XY;
		static const Vector3 XZ;
		static const Vector3 YZ;
		static const Vector3 NX;
		static const Vector3 NY;
		static const Vector3 NZ;
		static const Vector3 ONE;
		static const Vector3 GRAVITY; //normal earth gravity(9.81m/s^2)
	};
	
	typedef Vector3 vec3;
	
	// comparison operators
	inline bool operator == (const Vector3& a, const Vector3& b) {return a.x==b.x && a.y==b.y && a.z==b.z;}
	inline bool operator != (const Vector3& a, const Vector3& b) {return a.x!=b.x || a.y!=b.y || a.z!=b.z;}
	inline bool operator <= (const Vector3& a, const Vector3& b) {return a.x*a.x+a.y*a.y+a.z*a.z <= b.x*b.x+b.y*b.y+b.z*b.z;}
	inline bool operator <  (const Vector3& a, const Vector3& b) {return a.x*a.x+a.y*a.y+a.z*a.z <  b.x*b.x+b.y*b.y+b.z*b.z;}
	inline bool operator >= (const Vector3& a, const Vector3& b) {return a.x*a.x+a.y*a.y+a.z*a.z >= b.x*b.x+b.y*b.y+b.z*b.z;}
	inline bool operator >  (const Vector3& a, const Vector3& b) {return a.x*a.x+a.y*a.y+a.z*a.z >  b.x*b.x+b.y*b.y+b.z*b.z;}
	
	// arithmetic operators
	inline Vector3 operator + (const Vector3& a, const Vector3& b)  {return Vector3(a.x + b.x, a.y + b.y, a.z + b.z);}
	inline Vector3 operator - (const Vector3& a, const Vector3& b)  {return Vector3(a.x - b.x, a.y - b.y, a.z - b.z);}
	inline Vector3 operator - (const Vector3& v)                    {return Vector3(-v.x, -v.y, -v.z);}
	inline Vector3 operator * (const Vector3& a, const Vector3& b)  {return Vector3(a.x * b.x, a.y * b.y, a.z * b.z);}
	inline Vector3 operator * (const Vector3& v, const float f)     {return Vector3(v.x * f, v.y * f, v.z * f);}
	inline Vector3 operator * (const float f,    const Vector3& v)  {return Vector3(v.x * f, v.y * f, v.z * f);}
	inline Vector3 operator / (const Vector3& a, const Vector3& b)  {return Vector3(a.x / b.x, a.y / b.y, a.z / b.z);}
	inline Vector3 operator / (const Vector3& v, const float f)     {return Vector3(v.x / f, v.y / f, v.z / f);}
	
} // namespace roe

#endif /*ROEVECTOR3_H_*/
