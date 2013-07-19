#ifndef ROEBOUNDINGBOX_H_
#define ROEBOUNDINGBOX_H_

#include <string>

#include "RoeVector3.h"
#include "RoePlane.h"
#include "RoeUtil.h"

namespace roe {

	class BoundingBox
	{
	public:
		BoundingBox() : min(-1,-1,-1), max(1,1,1) {}
		BoundingBox(const vec3& pv) : min(pv), max(pv) {}
		BoundingBox(const vec3& pmin, const vec3& pmax) : min(pmin), max(pmax) {}
		BoundingBox(const BoundingBox& rhs) : min(rhs.min), max(rhs.max) {}
		BoundingBox& operator=(const BoundingBox& rhs) {min = rhs.min; max = rhs.max; return *this;}
		~BoundingBox() {}
		
		inline bool containsPoint(const vec3& point) const {
			return (point.x>=min.x && point.y>=min.y && point.z>=min.z && point.x<=max.x && point.y<=max.y && point.z<=max.z);
		};
		
		//updating bbox with new vector
		inline void add(const Vector3& v) {
			if(v.x > max.x) max.x = v.x;
			if(v.x < min.x) min.x = v.x;
			if(v.y > max.y) max.y = v.y;
			if(v.y < min.y) min.y = v.y;
			if(v.z > max.z) max.z = v.z;
			if(v.z < min.z) min.z = v.z;
		};
		
		//updating bbox with another bbox
		inline void add(const BoundingBox& bb) {
			if(bb.max.x > max.x) max.x = bb.max.x;
			if(bb.min.x < min.x) min.x = bb.min.x;
			if(bb.max.y > max.y) max.y = bb.max.y;
			if(bb.min.y < min.y) min.y = bb.min.y;
			if(bb.max.z > max.z) max.z = bb.max.z;
			if(bb.min.z < min.z) min.z = bb.min.z;
		}
		
		inline void translate(const Vector3& v) {
			min += v;
			max += v;
		}
		inline Vector3 getCenter() const {return (max+min)*0.5;}
		
		//view frustrum culling:
		inline bool isVisible(const Plane *planes) {
			for (int i = 0; i < 6; ++i) {
				if (util::classifyAABox(min,max,planes[i]) == 1) return false;
			}
			return true;
		}
		
		inline std::string toString() {return "BBox("+min.toString()+", "+max.toString()+")";}
		
		//########################
		Vector3 min, max;
	};
	
} // namespace roe

#endif /* ROEBOUNDINGBOX_H_ */
