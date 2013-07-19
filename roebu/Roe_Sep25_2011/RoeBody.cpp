#include "RoeBody.h"

#ifdef ROE_COMPILE_PHYSICS

#include "RoeTextfile.h"
#include "RoeException.h"
#include "RoeUtil.h"
#include "RoeEngine.h"

namespace roe {
namespace physics {
		
		Body::Body(float mass, Vector3 pos) {
			init(mass, pos);
		}
		Body::Body(const Model& gfxModel, float mass, Vector3 pos) {
			init(mass, pos);
			setGfxModel(gfxModel);
		}
		Body::Body(Engine *engine, float mass, Vector3 pos) {
			init(mass, pos);
			setEngine(engine);
		}
		Body::Body(Engine *engine, const Model& gfxModel, float mass, Vector3 pos) {
			init(mass, pos);
			setEngine(engine);
			setGfxModel(gfxModel);
		}
		Body::~Body() {
			if (m_pEngine) {
				m_pEngine->getWorld()->removeRigidBody(m_pRigidBody);
				m_pEngine = nullptr;
			}
			ROE_SAFE_DELETE(m_pRigidBody);
			ROE_SAFE_DELETE(m_pRigidBodyCI);
			ROE_SAFE_DELETE(m_pMotionState);
			ROE_SAFE_DELETE(m_pGfxModel);
			if(m_bWillDeleteShape) { 
				ROE_SAFE_DELETE(m_pShape); //only delete if its a std shape
			}
			else m_pShape = nullptr;
		}
		/*Body::Body(const Body& rhs) {
			(*this) = rhs;
		}
		Body& Body::operator=(const Body& rhs) {
			init(rhs.m_fMass, rhs.getMotionState()->getPosition());
			return *this;
		}//*/
		
		void Body::init(float mass, Vector3 pos) {
			m_pGfxModel    = new Model();
			m_pShape       = nullptr;
			m_pMotionState = new MotionState(btTransform(btQuaternion(0,0,0,1),pos.bt()), dynamic_cast<Object*>(m_pGfxModel));
			m_pRigidBodyCI = nullptr;
			m_pRigidBody   = nullptr;
			m_pEngine      = nullptr;
			m_bIsStatic = m_bWillDeleteShape = m_bIsCreated = false;
			
			if (mass > 0.0f) {
				m_fMass = mass;
			} else {
				m_bIsStatic = true;
				m_fMass = 0.0f;
			}
		}
		void Body::create() {
			assert(m_pShape && m_pMotionState && !m_pRigidBodyCI && !m_pRigidBody);
			btVector3 inertia(0,0,0);
			m_pShape->calculateLocalInertia(m_fMass, inertia);
			m_pRigidBodyCI = new btRigidBody::btRigidBodyConstructionInfo(m_fMass, m_pMotionState, m_pShape, inertia);
			m_pRigidBody   = new btRigidBody(*m_pRigidBodyCI);
			m_bIsCreated = true;
		}
		void Body::render() {
			m_pGfxModel->render();
		}
		void Body::setGfxModel(const Model& gfxModel) {
			(*m_pGfxModel) = gfxModel;
			if(m_pMotionState) {
				m_pGfxModel->setRotation(m_pMotionState->getMatrix());
				m_pGfxModel->setPosition(m_pMotionState->getPosition());
			}
		}
		Vector3 Body::getPos() const {return m_pMotionState->getPosition();}
		Vector3 Body::getVel() const {return Vector3(.0);} //TODO
		Quaternion Body::getQuaternion() const {
			return m_pMotionState->getQuaternion();
		}
		Matrix Body::getRotationMatrix() const {
			return Matrix(m_pMotionState->getMatrix());
		}
} // namespace physics
} // namespace roe

#endif //ROE_COMPILE_PHYSICS
