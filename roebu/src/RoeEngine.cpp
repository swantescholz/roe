#include "RoeEngine.h"

#ifdef ROE_COMPILE_PHYSICS

#include "RoeMaterial.h"
#include "RoeUtil.h"
#include "RoeTexture.h"
#include "RoeException.h"
#include "RoeHACDCompoundShape.h"
//#include <BulletCollision/NarrowPhaseCollision/btVoronoiSimplexSolver.h>

namespace roe {namespace physics {
	
	Engine::Engine() {
		m_dElapsed = -1.0;
		m_dStepSize = 1.0/60.0;
		m_iNumMaxSubSteps = 3;
		m_pBroadphase             = new btDbvtBroadphase();
		m_pCollisionConfiguration = new btDefaultCollisionConfiguration();
		m_pDispatcher             = new btCollisionDispatcher(m_pCollisionConfiguration);
		m_pSolver                 = new btSequentialImpulseConstraintSolver;
		//m_pSolver                 = new btVoronoiSimplexSolver();
		m_pDynamicsWorld          = new btDiscreteDynamicsWorld(m_pDispatcher,m_pBroadphase,m_pSolver,m_pCollisionConfiguration);
		setGravity(Vector3(0,-9.81,0));
		
		//register GIMPACT algorithm
		//btCollisionDispatcher *dispatcher = static_cast<btCollisionDispatcher*>(m_pDynamicsWorld->getDispatcher());
		//btGImpactCollisionAlgorithm::registerAlgorithm(dispatcher);
	}
	Engine::~Engine() {
		for(auto it = m_mpStaticTrimeshes.begin(); it != m_mpStaticTrimeshes.end(); ++it) {
			ROE_SAFE_DELETE(it->second); //delete trimesh shapes
		}
		m_mpStaticTrimeshes.clear();
		for(auto it = m_mpTrimeshes.begin(); it != m_mpTrimeshes.end(); ++it) {
			ROE_SAFE_DELETE(it->second); //delete trimesh shapes
		}
		m_mpTrimeshes.clear();
		//delete for bvh saved indexVertexArrays
		while(!m_vDump1.empty()) {
			delete m_vDump1.back();
			m_vDump1.pop_back();
		}
		while(!m_vDump2.empty()) {
			delete[] m_vDump2.back();
			m_vDump2.pop_back();
		}
		while(!m_vDump3.empty()) {
			delete[] m_vDump3.back();
			m_vDump3.pop_back();
		}
		delete m_pDynamicsWorld;
		delete m_pSolver;
		delete m_pDispatcher;
		delete m_pCollisionConfiguration;
		delete m_pBroadphase;
	}
	void Engine::setGravity (const Vector3& gravity) {
		m_vGravity = gravity;
		m_pDynamicsWorld->setGravity(btVector3(m_vGravity.x,m_vGravity.y,m_vGravity.z));
	}
	
	void Engine::addRigidBody(Splinter *b) {
		addRigidBody(b->getRigidBody());
		b->setEngine(this); //notify of current world
	}
	void Engine::addRigidBody(Body *b) {
		addRigidBody(b->getRigidBody());
		b->setEngine(this); //notify of current world
	}
	void Engine::addRigidBody(btRigidBody *b) {
		m_pDynamicsWorld->addRigidBody(b);
	}
	void Engine::step() {
		const time_point_t newTimePoint = std::chrono::monotonic_clock::now();
		if (m_dElapsed < 0.0) { //first step, do no time stepping
			m_LastTimePoint = newTimePoint;
			m_dElapsed = 1.0;
		}
		else { //not first step, so normal stepping
			m_Duration = newTimePoint - m_LastTimePoint;
			m_dElapsed = std::chrono::duration<double>(m_Duration).count();
			m_pDynamicsWorld->stepSimulation(m_dElapsed, m_iNumMaxSubSteps, m_dStepSize);
			m_LastTimePoint = newTimePoint;
		}
	}
	
	bool Engine::doPhysics()
	{
		step();
		return true;
	}
	
	btCollisionShape* Engine::shapePlane(const Vector3& up, float d) {
		return new btStaticPlaneShape(btVector3(up.x,up.y,up.z), d);
	}
	btCollisionShape* Engine::shapeSphere(float r) {
		return new btSphereShape(r);
	}
	btCollisionShape* Engine::shapeCuboid(const Vector3& halfSize) {
		return new btBoxShape(halfSize.bt());
	}
	btCollisionShape* Engine::shapeCylinder(const Vector3& halfSize) {
		return new btCylinderShapeZ(halfSize.bt());
	}
	btCollisionShape* Engine::shapeCone(float r, float h) {
		return new btConeShapeZ(r, h);
	}
	btCollisionShape* Engine::shapeCapsule(float r, float h) {
		return new btCapsuleShape(r, h);
	}
	btCollisionShape* Engine::shapeTrimesh(const Model& trimeshModel) {
		btCollisionShape *shape = getTrimesh(trimeshModel);
		if(!shape) roe_except("Trimesh '"+trimeshModel.getPath()+"' not yet loaded", "shapeTrimesh");
		return shape;
	}
	btCollisionShape* Engine::shapeStaticTrimesh(const Model& trimeshModel) {
		btCollisionShape *shape = getStaticTrimesh(trimeshModel);
		if(!shape) roe_except("(Static) Trimesh '"+trimeshModel.getPath()+"' not yet loaded", "shapeTrimesh");
		return shape;
	}
	
	void Engine::loadTrimesh(const Model& trimeshModel, bool isStatic) {
		btCollisionShape *shape = nullptr;
		
		//creating the btTriangleIndexVertexArray
		Trimesh *trimesh = trimeshModel.getTrimesh();
		float *vertices = new float[trimesh->vertices.size()*3];
		int   *indices  = new int  [trimesh->indices.size ()*3];
		for(unsigned int i = 0; i < trimesh->vertices.size(); ++i) {
			vertices[i*3+0] = trimesh->vertices[i].x;
			vertices[i*3+1] = trimesh->vertices[i].y;
			vertices[i*3+2] = trimesh->vertices[i].z;
		}
		for(unsigned int i = 0; i < trimesh->indices.size(); ++i) {
			indices[i*3+0] = trimesh->indices[i][0];
			indices[i*3+1] = trimesh->indices[i][1];
			indices[i*3+2] = trimesh->indices[i][2];
		}
		btIndexedMesh indexedMesh;
		indexedMesh.m_numTriangles = (int)(trimesh->indices.size());
		indexedMesh.m_numVertices  = (int)(trimesh->vertices.size());
		indexedMesh.m_triangleIndexStride = 3*sizeof(int);
		indexedMesh.m_vertexStride = 3*sizeof(float);
		indexedMesh.m_triangleIndexBase = (const unsigned char*)indices;
		indexedMesh.m_vertexBase        = (const unsigned char*)vertices;
		btTriangleIndexVertexArray* tiva = new btTriangleIndexVertexArray();   //a btStridingMeshInterface* is enough...
		tiva->addIndexedMesh(indexedMesh);
		
		if(isStatic) { //BvhTriangleShape
			shape = new btBvhTriangleMeshShape(tiva, true, true);
			m_vDump1.push_back(tiva); //still needed
			m_vDump2.push_back(vertices);
			m_vDump3.push_back(indices);
			tiva = nullptr;
			vertices = nullptr;
			indices = nullptr;
			m_mpStaticTrimeshes.insert(std::make_pair(trimeshModel.getPath(), shape));
		}
		else { //HACD convex decomposed compound shape
			btHACDCompoundShape::Params params;
			params.maxHullVertices = 64;       // (16) Max number of vertices in each convex hull sub shape. This is the only parameter that can be tweaked in most cases...
			params.nClusters       = 1;        // (2) Min number of resulting sub shapes. Can be used to increase the number of sub shapes.
			params.concavity       = 100;      // (100) Higher seems to reduce the number of sub shapes and viceversa. Can be used to increase or decrease the number of sub shapes.
			params.compacityWeight = 0.5;      // (0.1) ???
			params.volumeWeight    = 0.0;      // (0.0) ???
			params.addExtraDistPoints      = false;//concaveMeshHasHoles;//false;   
			params.addNeighboursDistPoints = false;//concaveMeshHasHoles;//false;   
			params.addFacesPoints          = true;//concaveMeshHasHoles;//false;
			shape = new btHACDCompoundShape(tiva, params);
			delete tiva; //not needed anymore
			delete[] vertices;
			delete[] indices;
			m_mpTrimeshes.insert(std::make_pair(trimeshModel.getPath(), shape));
		}
	}
	
	void Engine::loadHeightmap(std::string imagePath, Vector3 vScale) {
		Body mod;
		//todo
	}
	void Engine::deleteTrimesh(const std::string& path) {
		auto mpi = m_mpTrimeshes.find(path);
		if (mpi == m_mpTrimeshes.end()) {
			roe_except("trimesh to be deleted does not exist: " + path, "deleteTrimesh");
		} else {
			ROE_SAFE_DELETE(mpi->second);
			m_mpTrimeshes.erase(mpi);
		}
	}
	btCollisionShape* Engine::getTrimesh(const std::string& path) {
		auto mpi = m_mpTrimeshes.find(path);
		if (mpi == m_mpTrimeshes.end()) {
			roe_except("inquired trimesh does not exist: " + path, "getTrimesh");
		}
		return mpi->second;
	}
	btCollisionShape* Engine::getStaticTrimesh(const std::string& path) {
		auto mpi = m_mpStaticTrimeshes.find(path);
		if (mpi == m_mpStaticTrimeshes.end()) {
			roe_except("inquired trimesh does not exist: " + path, "getTrimesh");
		}
		return mpi->second;
	}
	
} // namespace physics
} // namespace roe


#endif //ROE_COMPILE_PHYSICS
