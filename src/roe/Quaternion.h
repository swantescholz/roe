
/*

#ifndef ROE_H_
#define ROE_H_

namespace roe {

} // namespace roe

#endif
//*/

#ifndef ROE_H_QUATERNION
#define ROE_H_QUATERNION

#include <string>

namespace roe {
	
class Quaternion {
public:
	double w = 0.0;
	double x = 0.0;
	double y = 0.0;
	double z = 0.0;
	
	Quaternion() = default;
	Quaternion(const Quaternion& b) = default;
	Quaternion& operator= (const Quaternion& b) = default;
	~Quaternion() = default;
	
	Quaternion(double w, double x, double y, double z);
	
	std::string toString() const;
	operator std::string () const;
	
	double length() const;
	double length2() const;
	void conjugate();
	Quaternion conjugated() const;
	void normalize();
	Quaternion normalized() const;
	void square();
	Quaternion squared() const;
	void invert();
	Quaternion inverted() const;
	
	bool operator== (const Quaternion& b) const;
	bool operator!= (const Quaternion& b) const;
	
	Quaternion operator- () const;
	Quaternion operator+ (const Quaternion& b) const;
	Quaternion operator- (const Quaternion& b) const;
	Quaternion operator* (const Quaternion& b) const;
	Quaternion operator/ (const Quaternion& b) const;
	Quaternion operator* (double k) const;
	Quaternion operator/ (double k) const;
	Quaternion& operator+= (const Quaternion& b);
	Quaternion& operator-= (const Quaternion& b);
	Quaternion& operator*= (const Quaternion& b);
	Quaternion& operator/= (const Quaternion& b);
	Quaternion& operator*= (double k);
	Quaternion& operator/= (double k);
	
};

Quaternion operator* (double k, const Quaternion& q);
	
} // namespace roe

#endif
