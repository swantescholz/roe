#ifndef ROEQUATERNION_H_
#define ROEQUATERNION_H_

#include <cmath>
#include <string>
#include "RoeVector3.h"
#include "RoeCommon.h"
#include "RoeMatrix.h"
#ifdef ROE_COMPILE_PHYSICS
	#include <btBulletDynamicsCommon.h>
#endif

namespace roe {
	class Matrix; //forward
	class Quaternion {
	public:
		union {
			struct
			{
				float x, y, z, w;
			};
			float rg[4];   // one-dimensional array
		};
		Quaternion();
		Quaternion(float px, float py, float pz, float pw = 1.0) : x(px), y(py), z(pz), w(pw) {}
		Quaternion(float *prg           ) : x(prg[0]), y(prg[1]), z(prg[2]), w(prg[3]) {}
		Quaternion(float *prg, float pw ) : x(prg[0]), y(prg[1]), z(prg[2]), w(pw    ) {}
		Quaternion(const Quaternion& rhs) : x(rhs.x ), y(rhs.y ), z(rhs.z ), w(rhs.w ) {}
		Quaternion(const Matrix& m                                     ) {setMatrix  (m);}
		Quaternion(const Vector3& axis, const float angle              ) {setRotation(axis, angle);}
		Quaternion(const float yaw, const float pitch, const float roll) {setEuler   (yaw, pitch, roll);}
		Quaternion& operator=(const Quaternion& rhs){x = rhs.x; y = rhs.y; z = rhs.z; w = rhs.w; return *this;}
		~Quaternion() {}
#ifdef ROE_COMPILE_PHYSICS
		Quaternion(const btQuaternion& q) : x(q.x()), y(q.y()), z(q.z()), w(q.getW()) {}
		Quaternion& operator=(const btQuaternion& q){x = q.x(); y = q.y(); z = q.z(); w = q.getW(); return *this;}
		operator btQuaternion() const {return btQuaternion(x,y,z,w);}
		btQuaternion bt() const {return btQuaternion(x,y,z,w);}
#endif
		
		operator float* () {return (float*)(rg);} // casting operator
		float  operator[](int i) const {assert(i<4 && i>=0); return *(&x+i);}
		float& operator[](int i)       {assert(i<4 && i>=0); return *(&x+i);}
		Quaternion& operator+=(const Quaternion& q) {x+=q.x; y+=q.y; z+=q.z; w+=q.w; return *this;}
		Quaternion& operator-=(const Quaternion& q) {x-=q.x; y-=q.y; z-=q.z; w-=q.w; return *this;}
		Quaternion& operator*=(const Quaternion& q);
		Quaternion& operator*=(const Vector3& v);
		Quaternion& operator*=(const float s        ) {x*=s    ; y*=s    ; z*=s    ; w*=s    ; return *this;}
		Quaternion& operator/=(const float s        ) {(*this)*=(1.0/s); return *this;}
		
		void setMatrix  (const Matrix& rot); //initializes the quat by a rotation matrix
		void setRotation(const Vector3& axis, const float angle); //initializes the quat by a rotation around an axis
		void setEuler   (const float yaw, const float pitch, const float roll); //initializes the quat by a euler rotation
		
		std::string toString() const;
		Matrix      toMatrix() const; //fills just the upper-left 3x3 submatrix and the lower-right element with a '1'
		Vector3 rotate   (const Vector3& v) const; //performs the actual quaternion rotation!
		Vector3 xAxis    () const {return rotate(Vector3::X);} //returns the new local x-axis
		Vector3 yAxis    () const {return rotate(Vector3::Y);} //returns the new local y-axis
		Vector3 zAxis    () const {return rotate(Vector3::Z);} //returns the new local z-axis
		Vector3 getAxis    () const; //returns the overall rotation axis
		float getAngle     () const {return 2.0*std::tan(w);} //returns the overall rotation angle
		float length       () const {return std::sqrt(x*x+y*y+z*z+w*w);}
		float squaredLength() const {return           x*x+y*y+z*z+w*w;}
		void normalize() {
			const float l2 = x*x+y*y+z*z+w*w; if(l2==0.0 || std::abs(l2-1.0) < 0.0001) return;
			const float linv = 1.0/std::sqrt(l2); x*=linv; y*=linv; z*=linv; w*=linv;}
		void invert() {x=-x; y=-y; z=-z;}
		
		//STATICS
		static float dot           (const Quaternion& a, const Quaternion& b) {return a.x*b.x+a.y*b.y+a.z*b.z+a.w*b.w;}
		static float angleBetween  (const Quaternion& a, const Quaternion& b) 
			{float s=std::sqrt(a.squaredLength()*b.squaredLength());assert(s!=0.0);return std::acos(dot(a,b)/s);}
		static Quaternion normalize(const Quaternion& q) {Quaternion t(q); t.normalize(); return t;}
		static Quaternion inverse  (const Quaternion& q) {Quaternion t(q); t.invert   (); return t;}
		static Quaternion slerp    (const Quaternion& a, const Quaternion& b, const float t); //perform spherical linear interpolation
		static Vector3    rotate   (const Quaternion& rotation, const Vector3& v) {return rotation.rotate(v);}
		
		//spectial quaternions (static)
		static const Quaternion IDENTITY, ZERO, ONE;
	};
	
	// comparison operators
	inline bool operator == (const Quaternion& a, const Quaternion& b) {return a.x==b.x && a.y==b.y && a.z==b.z && a.w==b.w;}
	inline bool operator != (const Quaternion& a, const Quaternion& b) {return a.x!=b.x || a.y!=b.y || a.z!=b.z || a.w!=b.w;}
	
	//other arithmetic operators
	inline Quaternion operator+(const Quaternion& a, const Quaternion& b) {Quaternion q(a); q+=b; return q;}
	inline Quaternion operator-(const Quaternion& a, const Quaternion& b) {Quaternion q(a); q-=b; return q;}
	inline Quaternion operator*(const Quaternion& a, const Quaternion& b) {Quaternion q(a); q*=b; return q;}
	inline Quaternion operator*(const Quaternion& a, const float s      ) {Quaternion q(a); q*=s; return q;}
	inline Quaternion operator*(const float s      , const Quaternion& a) {Quaternion q(a); q*=s; return q;}
	inline Quaternion operator*(const Quaternion& a, const Vector3& v   ) {Quaternion q(a); q*=v; return q;}
	inline Quaternion operator*(const Vector3& v   , const Quaternion& a) {Quaternion q(a); q*=v; return q;}
	inline Quaternion operator/(const Quaternion& a, const float s      ) {Quaternion q(a); q/=s; return q;}
	
	typedef Quaternion quat; //name simplification
} // namespace roe

#endif /* ROEQUATERNION_H_ */
