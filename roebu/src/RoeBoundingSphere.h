#ifndef ROEBOUNDINGSPHERE_H_
#define ROEBOUNDINGSPHERE_H_

#include <string>

#include "RoeVector3.h"
#include "RoePlane.h"
#include "RoeUtil.h"

namespace roe {

	class BoundingSphere
	{
	public:
		BoundingSphere(float r, const Vector3& v) : radiusSq(r*r), radius(r), center(v) {}
		BoundingSphere(const Vector3& v) : radiusSq(1.0), radius(1.0), center(v) {}
		BoundingSphere(const BoundingSphere& rhs) : radiusSq(rhs.radiusSq), radius(rhs.radius), center(rhs.center) {}
		BoundingSphere& operator=(const BoundingSphere& rhs) {radiusSq = rhs.radiusSq; radius = rhs.radius; center = rhs.center; return *this;}
		~BoundingSphere() {}
		
		inline bool containsPoint(const Vector3& point) const {
			return Vector3::distance2(point, center) <= radiusSq;
		};
		
		//updating bbox with new vector
		inline void add(const Vector3& v) {
			radiusSq = std::max(center.distance2(v), radiusSq);
		};
		
		//updating bbox with another bs
		inline void add(const BoundingSphere& bs) {
			radiusSq = std::max((float)std::pow(center.distance(bs.center)+std::sqrt(bs.radiusSq), 2), radiusSq);
		}
		//if radius is calculated, then faster:
		inline void addR(const BoundingSphere& bs) {
			radius = std::max(center.distance(bs.center)+bs.radius, radius);
			radiusSq = radius*radius;
		}
		
		inline void translate(const Vector3& v) {
			center += v;
		}
		
		inline void updateRadius() {radius = std::sqrt(radiusSq);}
		inline Vector3 getCenter() const {return center;}
		
		//view frustrum culling:
		inline bool isVisible(const Plane *planes) {
			for (int i = 0; i < 6; ++i) {
				if (util::classifySphereSq(center,radiusSq,planes[i]) == 1) return false;
			}
			return true;
		}
		
		inline std::string toString() const {char c[280]; sprintf(c,"BSphere(%s, %f, %f)",center.toString().c_str(),radius,radiusSq); return std::string(c);}
		
		//########################
		float radiusSq, radius;
		Vector3 center;
	};
	
} // namespace roe

#endif /* ROEBOUNDINGSPHERE_H_ */
