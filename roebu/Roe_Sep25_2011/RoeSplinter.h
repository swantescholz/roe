#ifndef ROESPLINTER_H_
#define ROESPLINTER_H_

#include "RoeCommon.h"

#ifdef ROE_COMPILE_PHYSICS

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionShapes/btConvexPolyhedron.h>
#include <BulletCollision/CollisionShapes/btShapeHull.h>
#include "RoeVector3.h"
#include "RoeBody.h"
#include <vector>

namespace roe {
namespace physics {
	
	class Engine;
	class Splinter : public Object {
		
	public:
		Splinter(Engine *engine = nullptr);
		~Splinter();
		Splinter(const Splinter& rhs) = delete;
		Splinter& operator= (const Splinter& rhs) = delete;
		
		void addPoint(const Vector3& v);
		void finalize(Vector3 pos = Vector3::ZERO, float density = 1.0) {finalize(btTransform(btQuaternion(0,0,0,1),pos.bt()),density);}
		void finalize(Vector3 pos, Quaternion quat, float density = 1.0);
		void finalize(btTransform transform, float density = 1.0);
		void addToWorld(short group = (1<<14), short mask = (~(short)0)^(1<<14));
		void render(); //uses glBegin -> slow!
		void split(const Plane& plane); //splits this splinter (or its childs if already splitted)
		
		float getMass() const {return m_fMass;}
		bool isCreated() {return m_bIsCreated;}
		void setEngine (Engine *engine) {m_pEngine = engine;}
		Engine            * getEngine          () const {return m_pEngine;}
		btCollisionShape  * getCollisionShape  () const {return dynamic_cast<btCollisionShape*>(m_pShape);}
		btRigidBody       * getRigidBody       () const {return m_pRigidBody;}
		MotionState       * getMotionState     () const {return m_pMotionState;}
		btRigidBody::btRigidBodyConstructionInfo* getConstructionInfo() const {return m_pRigidBodyCI;}
	private:
		Splinter *m_pChild1, *m_pChild2; //bintree childs
		
		btCollisionShape     *m_pShape;
		btRigidBody          *m_pRigidBody;
		MotionState          *m_pMotionState;
		btRigidBody::btRigidBodyConstructionInfo *m_pRigidBodyCI;
		Engine               *m_pEngine;   //the dynamics world this object is inserted into
		float m_fMass, m_fDensity, m_fRadius;
		bool m_bIsCreated, m_bIsInWorld;
		short m_sGroup, m_sMask; //collision filtering group and mask
		std::vector<Vector3> m_vPoints;
		std::vector<Vector3> m_vVertices, m_vNormals;
		std::vector<unsigned int> m_vIndices;
	};
	
}
}

#endif //ROE_COMPILE_PHYSICS
#endif /* ROESPLINTER_H_ */
