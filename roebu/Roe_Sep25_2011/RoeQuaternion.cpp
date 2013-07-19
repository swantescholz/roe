#include "RoeQuaternion.h"
#include <cstdio>

namespace roe {
	
	const Quaternion Quaternion::IDENTITY(0,0,0,1);
	const Quaternion Quaternion::ZERO    (0,0,0,0);
	const Quaternion Quaternion::ONE     (1,1,1,1);
	
	std::string Quaternion::toString() const {
		char c[150];
		sprintf(c,"Quaternion( %f, %f, %f, %f)",x,y,z,w);
		return std::string(c);
	}
	Matrix Quaternion::toMatrix() const {
		Quaternion q(Quaternion::normalize(*this));
		return Matrix(
			1.0-2.0*q.y*q.y-2.0*q.z*q.z, 2.0*q.x*q.y-2.0*q.z*q.w    , 2.0*q.x*q.z+2.0*q.y*q.w    , 0,
			2.0*q.x*q.y+2.0*q.z*q.w    , 1.0-2.0*q.x*q.x-2.0*q.z*q.z, 2.0*q.y*q.z-2.0*q.x*q.w    , 0,
			2.0*q.x*q.z-2.0*q.y*q.w    , 2.0*q.y*q.z+2.0*q.x*q.w    , 1.0-2.0*q.x*q.x-2.0*q.y*q.y, 0,
			0,0,0,1);
	}
	void Quaternion::setMatrix(const Matrix& rot) {
		Matrix m = rot.toRotationMatrix();
		float trace = 1 + m.m11 + m.m22 + m.m33;
		float s = 0;
		x = 0;
		y = 0;
		z = 0;
		w = 0;
		
		if (trace > 0.0000001) {
			s = std::sqrt(trace) * 2;
			x = (m.m23 - m.m32) / s;
			y = (m.m31 - m.m13) / s;
			z = (m.m12 - m.m21) / s;
			w = 0.25f * s;
		} else {
			if (m.m11 > m.m22 && m.m11 > m.m33) { // Column 0:
				s = std::sqrt(1.0 + m.m11 - m.m22 - m.m33) * 2;
				x = 0.25f * s;
				y = (m.m12 + m.m21) / s;
				z = (m.m31 + m.m13) / s;
				w = (m.m23 - m.m32) / s;
			} else if (m.m22 > m.m33) { // Column 1:
				s = std::sqrt(1.0 + m.m22 - m.m11 - m.m33) * 2;
				x = (m.m12 + m.m21) / s;
				y = 0.25f * s;
				z = (m.m23 + m.m32) / s;
				w = (m.m31 - m.m13) / s;
			} else { // Column 2:
				s = std::sqrt(1.0 + m.m33 - m.m11 - m.m22) * 2;
				x = (m.m31 + m.m13) / s;
				y = (m.m23 + m.m32) / s;
				z = 0.25f * s;
				w = (m.m12 - m.m21) / s;
			}
		}
		/*w = 0.5*std::sqrt(m.m11+m.m22+m.m33+1.0);
		if(w != 0.0) {
			const float inv = 1.0/(4.0*w);
			x = inv*(m.m32-m.m23);
			y = inv*(m.m13-m.m31);
			z = inv*(m.m21-m.m12);
		} else {
			if(m.m12 == 0.0 && m.m23 == 0.0) cout << "matrix2quat yaw error" << endl; //TODO
			if(m.m13 == 0.0 && m.m23 == 0.0) cout << "matrix2quat pitch error" << endl;
			if(m.m12 == 0.0 && m.m13 == 0.0) cout << "matrix2quat roll error" << endl;
			const float inv = 1.0/std::sqrt(m.m12*m.m12*m.m13*m.m13 + m.m12*m.m12*m.m23*m.m23 + m.m21*m.m21*m.m23*m.m23);
			x = inv*(m.m13*m.m12);
			y = inv*(m.m12*m.m23);
			z = inv*(m.m13*m.m23);
		}//*/
	}
	
	Quaternion& Quaternion::operator*=(const Quaternion& q) {
		x = w * q.x + x * q.w + y * q.z - z * q.y;
		y = w * q.y + y * q.w + z * q.x - x * q.z;
		z = w * q.z + z * q.w + x * q.y - y * q.x;
		w = w * q.w - x * q.x - y * q.y - z * q.z;
		return *this;
	}
	Quaternion& Quaternion::operator*=(const Vector3& v) {
		x =  w * v.x + y * v.z - z * v.y;
		y =  w * v.y + z * v.x - x * v.z;
		z =  w * v.z + x * v.y - y * v.x;
		w = -x * v.x - y * v.y - z * v.z;
		return *this;
	}
	
	void Quaternion::setRotation(const Vector3& axis, const float angle) {
		float d = axis.length();
		assert(d != 0.0);
		float s = std::sin(angle*0.5) / d;
		x = axis.x*s;
		y = axis.y*s;
		z = axis.z*s;
		w = std::cos(angle*0.5);
	}
	void Quaternion::setEuler(const float yaw, const float pitch, const float roll) {
		float halfYaw   = yaw   * 0.5;
		float halfPitch = pitch * 0.5;
		float halfRoll  = roll  * 0.5;
		float cosYaw    = std::cos(halfYaw);
		float sinYaw    = std::sin(halfYaw);
		float cosPitch  = std::cos(halfPitch);
		float sinPitch  = std::sin(halfPitch);
		float cosRoll   = std::cos(halfRoll);
		float sinRoll   = std::sin(halfRoll);
		x = cosRoll * sinPitch * cosYaw + sinRoll * cosPitch * sinYaw;
		y = cosRoll * cosPitch * sinYaw - sinRoll * sinPitch * cosYaw;
		z = sinRoll * cosPitch * cosYaw - cosRoll * sinPitch * sinYaw;
		w = cosRoll * cosPitch * cosYaw + sinRoll * sinPitch * sinYaw;
	}
	
	Vector3 Quaternion::getAxis() const {
		float s2 = 1.0 - std::pow(w, 2);
		if (s2 < 0.0001)
			return Vector3(1.0, 0.0, 0.0); //arbitrary
		float s = 1.0 / std::sqrt(s2);
		return Vector3(x*s, y*s, z*s);
	}
	Vector3 Quaternion::rotate(const Vector3& v) const {
		Quaternion q = *this * v;
		q *= Quaternion::inverse(*this);
		return Vector3(q.x,q.y,q.z);
	}
	
	
	//STATIC
	Quaternion Quaternion::slerp(const Quaternion& a, const Quaternion& b, const float t) {
		float theta = angleBetween(a,b);
		if (theta != 0.0) {
			float d  = 1.0 / std::sin(theta);
			float s0 = std::sin((1.0 - t) * theta);
			float s1 = std::sin(t * theta);
			if (dot(a,b) < 0) // long cases
				return Quaternion((a.x * s0 + -b.x * s1) * d,
				                  (a.y * s0 + -b.y * s1) * d,
				                  (a.z * s0 + -b.z * s1) * d,
				                  (a.w * s0 + -b.w * s1) * d);
			else
				return Quaternion((a.x * s0 + b.x * s1) * d,
				                  (a.y * s0 + b.y * s1) * d,
				                  (a.z * s0 + b.z * s1) * d,
				                  (a.w * s0 + b.w * s1) * d);
			
		}
		else {
			return a;
		}
	}
	
} // namespace roe
