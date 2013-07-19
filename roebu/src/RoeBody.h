#ifndef ROEPHYSICSMODEL_H_
#define ROEPHYSICSMODEL_H_

#include "RoeCommon.h"

#ifdef ROE_COMPILE_PHYSICS

#include <btBulletDynamicsCommon.h>
#include "RoeModel.h"
#include "RoeVector3.h"
#include "RoeMatrix.h"
#include "RoeQuaternion.h"
#include "RoeObject.h"
#include "RoeMotionState.h"

namespace roe {
namespace physics {
	
	enum EPhysicsShapeType {
		  PST_PLANE = 0 //static
		, PST_SPHERE
		, PST_BOX
		, PST_CAPSULE
		, PST_CONE
		, PST_CYLINDER
		, PST_HEIGHTFIELD
		, PST_TRIMESH
	};
	
	class Engine;
	//a class for a physical movable body that is geometry-wise constant and renderable using a constant Model
	class Body {
		friend class Engine;
	public:
		Body(float mass = 1.0f, Vector3 pos = Vector3::ZERO);
		Body(const Model& gfxModel, float mass = 1.0f, Vector3 pos = Vector3::ZERO);
		Body(Engine *engine, float mass = 1.0f, Vector3 pos = Vector3::ZERO);
		Body(Engine *engine, const Model& gfxModel, float mass = 1.0f, Vector3 pos = Vector3::ZERO);
		Body(const Body& rhs) = delete;
		virtual ~Body();
		Body& operator= (const Body& rhs) = delete;
		
		void create(btCollisionShape *shape, bool shallDelete = true) {setCollisionShape(shape,shallDelete); create();}
		void create(); //shape and motion state specified; then create the body
		void render(); //renders model
		void setGfxModel(const Model& gfxModel); //attaches a renderable and movable graficls object model (won't delete it!)
		const Model& getGfxModel() const {assert(m_pGfxModel); return *m_pGfxModel;}
		float   getMass() const {return m_fMass;}
		Vector3 getPos () const;      //returns the current position
		Vector3 getVel () const;      //returns the current velocity
		Matrix     getRotationMatrix() const; //returns the rotation matrix
		Quaternion getQuaternion    () const; //returns the rotation quaternion
		
		void setStatic(bool b) {m_bIsStatic = b;}
		bool isCreated      () {return m_bIsCreated;}
		bool isStatic       () {return m_bIsStatic ;}
		bool willDeleteShape() {return m_bWillDeleteShape;}
		void setEngine        (Engine          *engine) {m_pEngine = engine;}
		void setCollisionShape(btCollisionShape *shape, bool shallDelete = true) {m_pShape  = shape; m_bWillDeleteShape = shallDelete;}
		Engine            * getEngine          () const {return m_pEngine;}
		btCollisionShape  * getCollisionShape  () const {return m_pShape;}
		btRigidBody       * getRigidBody       () const {return m_pRigidBody;}
		MotionState       * getMotionState     () const {return m_pMotionState;}
		btRigidBody::btRigidBodyConstructionInfo* getConstructionInfo() const {return m_pRigidBodyCI;}
		
	private:
		void init(float mass = 1.0f, Vector3 pos = Vector3::ZERO);
		
		
		btCollisionShape     *m_pShape;
		btRigidBody          *m_pRigidBody;
		MotionState          *m_pMotionState;
		btRigidBody::btRigidBodyConstructionInfo *m_pRigidBodyCI;
		Engine               *m_pEngine;   //the dynamics world this object is inserted into
		Model                *m_pGfxModel; //graphical model
		float m_fMass;
		bool m_bIsCreated;
		bool m_bIsStatic; //part of static enviroment?
		bool m_bWillDeleteShape; //will it delete the shape
	};

} // namespace physics
} // namespace roe

#endif //ROE_COMPILE_PHYSICS
#endif /* ROEPHYSICSMODEL_H_ */
