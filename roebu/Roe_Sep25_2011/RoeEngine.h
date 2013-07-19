#ifndef ROEPHYSICSENGINE_H_
#define ROEPHYSICSENGINE_H_

#include "RoeCommon.h"

#ifdef ROE_COMPILE_PHYSICS

#include <btBulletDynamicsCommon.h>
#include <string>
#include <map>
#include <chrono>
#include <tuple>
#include "RoeVector3.h"
#include "RoeBody.h"
#include "RoeSplinter.h"

namespace roe {
namespace physics {
	
	class Engine {
	private:
		typedef std::chrono::monotonic_clock::time_point time_point_t;
		typedef std::chrono::monotonic_clock::duration   duration_t;
	public:
		Engine();
		~Engine();
		
		void    setMaxSubSteps(int i) {m_iNumMaxSubSteps = i;}
		void    setStepSize(double d) {m_dStepSize = d;}
		void    setGravity (const Vector3& gravity);
		int     getMaxSubSteps() const {return m_iNumMaxSubSteps;}
		double  getStepSize   () const {return m_dStepSize;}
		Vector3 getGravity    () const {return m_vGravity;}
		
		btBroadphaseInterface               *getBroadphase            () {return m_pBroadphase;}
		btCollisionConfiguration            *getCollisionConfiguration() {return m_pCollisionConfiguration;}
		btCollisionDispatcher               *getCollisionDispatcher   () {return m_pDispatcher;}
		btConstraintSolver                  *getSolver                () {return m_pSolver;}
		btDiscreteDynamicsWorld             *getWorld                 () {return m_pDynamicsWorld;}
		
		//creating geometry shapes (need to be deleted by the user!!! EXCEPT: trimeshes and hightfields)
		btCollisionShape* shapePlane(const Plane& p) {return shapePlane(p.n, p.d);}
		btCollisionShape* shapePlane(const Vector3& up, float d = 1.0);
		btCollisionShape* shapeSphere  (float r);
		btCollisionShape* shapeCube    (float a) {return shapeCuboid(Vector3(a,a,a));}
		btCollisionShape* shapeCuboid  (const Vector3& halfSize);
		btCollisionShape* shapeCylinder(const Vector3& halfSize);
		btCollisionShape* shapeCone    (float r, float h);
		btCollisionShape* shapeCapsule (float r, float h);
		btCollisionShape* shapeTrimesh (const Model& trimeshModel);
		btCollisionShape* shapeStaticTrimesh (const Model& trimeshModel);
		
		//adding objects to world
		void addRigidBody(Splinter *b);
		void addRigidBody(Body *b);
		void addRigidBody(btRigidBody *b);
		
		bool doPhysics();
		
		void loadTrimesh(const Model& trimeshModel, bool isStatic = false);
		void loadHeightmap(std::string imagePath, Vector3 vScale = Vector3(1,1,1)); //creates a physical terrain from an image
		void deleteTrimesh(const std::string& path);
		void deleteTrimesh(const Model& model) {deleteTrimesh(model.getPath());}
		
		btCollisionShape* getTrimesh(const std::string& path);
		btCollisionShape* getTrimesh(const Model& model) {return getTrimesh(model.getPath());}
		btCollisionShape* getStaticTrimesh(const std::string& path);
		btCollisionShape* getStaticTrimesh(const Model& model) {return getStaticTrimesh(model.getPath());}
		
		
	private:
		void step(); //take one physics step
		
		double       m_dElapsed;      //time since last frame
		duration_t   m_Duration;      //same as elapsed but with chrono-type
		time_point_t m_LastTimePoint; //last time_point of updating
		double       m_dStepSize;
		int          m_iNumMaxSubSteps; //maximum number of substeps
		Vector3 m_vGravity;
		btBroadphaseInterface               *m_pBroadphase;
		btCollisionConfiguration            *m_pCollisionConfiguration;
		btCollisionDispatcher               *m_pDispatcher;
		btConstraintSolver                  *m_pSolver;
		btDiscreteDynamicsWorld             *m_pDynamicsWorld;
		std::vector<btTriangleIndexVertexArray*> m_vDump1;
		std::vector<float*> m_vDump2;
		std::vector<int*> m_vDump3;
		
		std::map<std::string, btCollisionShape*> m_mpTrimeshes, m_mpStaticTrimeshes;
		//TODO
	};
	
} // namespace physics
} // namespace roe

#endif //ROE_COMPILE_PHYSICS
#endif /* ROEPHYSICSENGINE_H_ */
