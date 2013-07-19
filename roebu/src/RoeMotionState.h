#ifndef ROEPHYSICSMOTIONSTATE_H_
#define ROEPHYSICSMOTIONSTATE_H_

#include "RoeCommon.h"

#ifdef ROE_COMPILE_PHYSICS

#include <btBulletDynamicsCommon.h>
#include "RoeObject.h"
#include "RoeVector3.h"
#include "RoeMatrix.h"
#include "RoeQuaternion.h"

namespace roe {
namespace physics {

class MotionState : public btMotionState {
public:
	MotionState(const btTransform& start, Object *obj = nullptr) {
		setObject(obj);
		setWorldTransform(start);
	}
	MotionState(const MotionState& rhs)
	: m_Transform(rhs.m_Transform)
	, m_pObject(rhs.m_pObject) {}
	MotionState& operator=(const MotionState& rhs) {
		m_Transform = rhs.m_Transform;
		m_pObject   = rhs.m_pObject;
		return *this;
	}
	virtual ~MotionState() {}
	
	void setObject(Object *obj) {
		m_pObject = obj;
	}
	virtual void getWorldTransform(btTransform &worldTrans) const;
	virtual void setWorldTransform(const btTransform &worldTrans);
	
	Vector3    getPosition  () const {return Vector3(m_Transform.getOrigin());} //returns the center of the object
	Matrix     getMatrix    () const {return Matrix(m_Transform.getBasis ());} //returns the rotation matrix
	Quaternion getQuaternion() const {return Quaternion(Matrix(m_Transform.getBasis()));} //return the rotation quaternion
	
private:
	btTransform m_Transform;
	Object *m_pObject; //movable object to update
};

} // namespace physics
} // namespace roe

#endif //ROE_COMPILE_PHYSICS
#endif /* ROEPHYSICSMOTIONSTATE_H_ */
