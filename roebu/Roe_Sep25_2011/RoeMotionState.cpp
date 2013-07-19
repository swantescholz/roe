#include "RoeMotionState.h"
#include "RoeRandom.h"

#ifdef ROE_COMPILE_PHYSICS

namespace roe {
namespace physics {

	void MotionState::getWorldTransform(btTransform &worldTrans) const {
		worldTrans = m_Transform;
	}
	void MotionState::setWorldTransform(const btTransform &worldTrans) {
		if(!m_pObject) return; // silently return before we set a node
		m_Transform = worldTrans;
		m_pObject->setRotation(this->getMatrix());
		m_pObject->setPosition(this->getPosition());
	}
	
} // namespace physics
} // namespace roe

#endif //ROE_COMPILE_PHYSICS
