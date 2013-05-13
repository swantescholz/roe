#include "Quaternion.h"
#include <tdd/tdd.h>
#include <stdexcept>
#include <sstream>
#include <cmath>

using tdd::String;

namespace roe {

void sprintf(const String& s, std::stringstream& ss) {
	if (s.contains("%")) 
		throw std::runtime_error("invalid format string: missing arguments");
	ss << s;
}

template<typename T, typename... Args>
void sprintf(const String& format, std::stringstream& ss, const T& value, const Args&... args) {
	for (int i = 0; i < int(format.length()); i++) {
		char c = format.at(i);
		if (c == '%') {
			ss << value;
			sprintf(format.substr(i+1), ss, args...);
			return;
		}
		ss << c;
	}
	throw std::runtime_error("extra arguments provided to sprintf");
}

template<typename... Args>
std::string sprintf(const String& format, const Args&... args) {
	std::stringstream ss;
	sprintf(format, ss, args...);
	return ss.str();
}

template<typename... Args>
void printf(const String& s, const Args&... args) {
	std::cout << sprintf(s,args...);
}

// =======================================
Quaternion operator* (double k, const Quaternion& q) {return q * k;}
// ---------------------------------------

Quaternion::Quaternion(double w, double x, double y, double z)
: w(w), x(x), y(y), z(z) {}

std::string Quaternion::toString() const {return std::string(*this);}
Quaternion::operator std::string () const {
	return sprintf("(%, %, %, %)",w,x,y,z);
	
}

double Quaternion::length() const {
	return std::sqrt(length2());
}
double Quaternion::length2() const {
	return w*w+x*x+y*y+z*z;
}
void Quaternion::conjugate() {
	x = -x;
	y = -y;
	z = -z;
}
Quaternion Quaternion::conjugated() const {
	Quaternion q(*this);
	q.conjugate();
	return q;
}
void Quaternion::normalize() {
	*this /= length();
}
Quaternion Quaternion::normalized() const {
	Quaternion q(*this);
	q.normalize();
	return q;
}
void Quaternion::square() {*this *= *this;}
Quaternion Quaternion::squared() const {return *this * *this;}
void Quaternion::invert() {
	conjugate();
	*this /= length2();
}
Quaternion Quaternion::inverted() const {
	return conjugated() / length2();
}

bool Quaternion::operator== (const Quaternion& b) const {
	if (b.w != w) return false;
	if (b.x != x) return false;
	if (b.y != y) return false;
	return b.z == z;
}
bool Quaternion::operator!= (const Quaternion& b) const {
	return !(*this == b);
}

Quaternion Quaternion::operator- () const {
	return Quaternion(-w,-x,-y,-z);
}

Quaternion Quaternion::operator+ (const Quaternion& b) const {
	return Quaternion(w+b.w,x+b.x,y+b.y,z+b.z);
}

Quaternion Quaternion::operator- (const Quaternion& b) const {
	return Quaternion(w-b.w,x-b.x,y-b.y,z-b.z);
}

Quaternion Quaternion::operator* (double b) const {return Quaternion(w*b,x*b,y*b,z*b);}
Quaternion Quaternion::operator/ (double b) const {return *this * (1.0/b);}

Quaternion Quaternion::operator* (const Quaternion& b) const {
	return Quaternion(
		w*b.w - x*b.x - y*b.y - z*b.z,
		w*b.x + b.w*x + y*b.z - z*b.y,
		w*b.y + b.w*y + z*b.x - x*b.z,
		w*b.z + b.w*z + x*b.y - y*b.x);
}

Quaternion& Quaternion::operator+= (const Quaternion& b) {
	w += b.w;
	x += b.x;
	y += b.y;
	z += b.z;
	return *this;
}


Quaternion& Quaternion::operator-= (const Quaternion& b) {
	w -= b.w;
	x -= b.x;
	y -= b.y;
	z -= b.z;
	return *this;
}
Quaternion& Quaternion::operator*= (double b) {
	w *= b;
	x *= b;
	y *= b;
	z *= b;
	return *this;
}
Quaternion& Quaternion::operator/= (double b) {return *this *= (1.0/b);}

Quaternion& Quaternion::operator*= (const Quaternion& b) {
	*this = *this * b;
	return *this;
}


} // namespace roe

