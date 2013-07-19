#include "RoeSplinter.h"

#ifdef ROE_COMPILE_PHYSICS

#include <GL/gl.h>
#include "RoeEngine.h"
#include "RoeUtil.h"
#ifdef ROE_USE_SHADERS
	#include "RoeProgram.h"
#endif
#include <LinearMath/btConvexHullComputer.h>

#define ROE_SPLINTER_BS_FACTOR (0.7  *4.0/3.0*ROE_PI)
#define ROE_SPLINTER_SPLIT_MARGIN 0.02

namespace roe {	
namespace physics {
		
	Splinter::Splinter(Engine *engine) {
		m_pChild1 = m_pChild2 = nullptr;
		m_sGroup       = (short)(1<<14);
		m_sMask        = (~((short)0))^(short)(1<<14);
		m_pEngine      = engine;
		m_pShape       = nullptr;
		m_pRigidBodyCI = nullptr;
		m_pRigidBody   = nullptr;
		m_pMotionState = nullptr;
		m_fMass = m_fDensity = 1.0;
		m_bIsCreated = m_bIsInWorld = false;
	}
	Splinter::~Splinter() {
		ROE_SAFE_DELETE(m_pChild1);
		ROE_SAFE_DELETE(m_pChild2);
		if (m_pEngine && m_bIsInWorld) {
			m_pEngine->getWorld()->removeRigidBody(m_pRigidBody);
			m_pEngine = nullptr;
		}
		ROE_SAFE_DELETE(m_pRigidBody);
		ROE_SAFE_DELETE(m_pRigidBodyCI);
		ROE_SAFE_DELETE(m_pMotionState);
		ROE_SAFE_DELETE(m_pShape);
		m_vVertices.clear();
		m_vNormals.clear();
		m_vIndices.clear();
	}
	void Splinter::addPoint(const Vector3& v) {
		m_vPoints.push_back(v);
	}
	void Splinter::finalize(Vector3 pos, Quaternion quat, float density) {
		finalize(btTransform(quat.bt(), pos.bt()), density);
	}
	void Splinter::finalize(btTransform transform, float density) {
		assert(m_pEngine && !m_pShape && !m_pMotionState && !m_pRigidBodyCI && !m_pRigidBody && m_vPoints.size()>=4);
		m_fDensity = density;
		Vector3 center = Vector3::ZERO;
		const float inv = 1.0/m_vPoints.size();
		const Matrix mRot(transform.getBasis());
		const Vector3 origin(transform.getOrigin());
		for (unsigned int i = 0; i < m_vPoints.size(); ++i) {
			Matrix::transformCoords(m_vPoints[i], mRot);
			m_vPoints[i] += origin;
			center += inv*m_vPoints[i];
		}
		float rsq = 0.0; //squared radius
		//center to center
		for (unsigned int i = 0; i < m_vPoints.size(); ++i) {
			m_vPoints[i] -= center;
			rsq = std::max(m_vPoints[i].length2(), rsq);
		}
		m_fRadius = std::sqrt(rsq); //calculate bounding sphere radius
		m_fMass = m_fDensity*ROE_SPLINTER_BS_FACTOR*m_fRadius*m_fRadius*m_fRadius; //estimate mass
		btConvexHullShape *shape = new btConvexHullShape();
		for (unsigned int i = 0; i < m_vPoints.size(); ++i) {
			shape->addPoint(m_vPoints[i].bt());
		}
		//shape->initializePolyhedralFeatures(); //HERE LIES THE ERROR!!! //*/
		btVector3 inertia(0,0,0);
		shape->calculateLocalInertia(m_fMass, inertia);
		const Vector3 newCenter = Vector3::ZERO;
		//save hull points:
		/*m_pPolyhedron = (btConvexPolyhedron*)(shape->getConvexPolyhedron()); //OLD
		const long numVert = m_pPolyhedron->m_vertices.size(), numFaces = m_pPolyhedron->m_faces.size();
		m_vVertices.reserve(numVert);
		for(int i = 0; i < numVert; ++i) {
			m_vVertices.push_back(m_pPolyhedron->m_vertices[i]);
		}
		for(int i = 0; i < numFaces; ++i) {
			for(int j = 1; j < m_pPolyhedron->m_faces[i].m_indices.size()-1; ++j) {
				m_vIndices.push_back(m_pPolyhedron->m_faces[i].m_indices[0]);
				m_vIndices.push_back(m_pPolyhedron->m_faces[i].m_indices[j]);
				m_vIndices.push_back(m_pPolyhedron->m_faces[i].m_indices[j+1]);
			}
		}//*/
		//NEW (using the btConvexHullComputer to compute the convex hull manually)
		btConvexHullComputer *chc = new btConvexHullComputer();
		chc->compute((float*)m_vPoints.data(), sizeof(Vector3), m_vPoints.size(), 0, 0);
		m_vVertices.reserve(chc->vertices.size());
		for(int i = 0; i < chc->vertices.size(); ++i) { //inquire vertex data
			m_vVertices.push_back(chc->vertices[i]);
		}
		std::vector<int> vFaceIndices;
		for (int i = 0; i < chc->faces.size(); ++i) {
			int face = chc->faces[i];
			const btConvexHullComputer::Edge *firstEdge = &chc->edges[face];
			const btConvexHullComputer::Edge *edge = firstEdge;
			do { //inquire face indices
				vFaceIndices.push_back(edge->getTargetVertex());
				edge = edge->getNextEdgeOfFace();
			} while (edge!=firstEdge);
			//triangulate face indices
			m_vIndices.reserve((vFaceIndices.size()-2)*3);
			for(unsigned int j = 1; j < vFaceIndices.size()-1; ++j) {
				m_vIndices.push_back(vFaceIndices[0]);
				m_vIndices.push_back(vFaceIndices[j]);
				m_vIndices.push_back(vFaceIndices[j+1]);
			}
			vFaceIndices.clear();
		}
		for(unsigned int i = 0; i < m_vIndices.size(); i+=3) { //flip triangle indices to correct triangle orientation to the outside
			Vector3 n = Vector3::normalize(Vector3::triangleNormal(m_vVertices[m_vIndices[i+0]],m_vVertices[m_vIndices[i+1]],m_vVertices[m_vIndices[i+2]]));
			Plane p(m_vVertices[m_vIndices[i+0]], n);
			if (p.dotCoords(newCenter) > 0.0) { //swap indices:
				std::swap(m_vIndices[i+1], m_vIndices[i+2]);
				n *= -1;
			}
			m_vNormals.push_back(n);
		}
		delete chc;
		m_vPoints.clear();
		
		//main creation
		m_pShape = shape;
		m_pMotionState = new MotionState(btTransform(btQuaternion(0,0,0,1), center), dynamic_cast<Object*>(this));
		m_pRigidBodyCI = new btRigidBody::btRigidBodyConstructionInfo(m_fMass, m_pMotionState, m_pShape, inertia);
		m_pRigidBody   = new btRigidBody(*m_pRigidBodyCI);
		m_bIsCreated = true;
	}
	void Splinter::render() {
		assert(m_bIsCreated);
		if(m_pChild1 || m_pChild2) { //recursivly render
			if(m_pChild1) m_pChild1->render();
			if(m_pChild2) m_pChild2->render();
			return;
		}
		const Matrix m = getMatrix();
#ifdef ROE_USE_SHADERS
		const Matrix n = getRotationMatrix();
		Program::setStdTransformationMatrix(m);
		Program::setStdNormalMatrix(n);
#else
		glPushMatrix();
		glMultMatrixf(m.n);
#endif
		glBegin(GL_TRIANGLES);
		for(unsigned int i = 0; i < m_vNormals.size(); ++i) {
			glNormal3fv(m_vNormals[i].rg);
			for(int j = 0; j < 3; ++j)
				glVertex3fv((m_vVertices[m_vIndices[3*i+j]]).rg);
		}
		glEnd();
#ifndef ROE_USE_SHADERS
		glPopMatrix();
#endif
		//*/
	}
	void Splinter::addToWorld(short group, short mask) {
		m_sGroup = group;
		m_sMask  = mask;
		if(m_pChild1 || m_pChild2) { //recursivly adding to world
			if(m_pChild1) m_pChild1->addToWorld(m_sGroup, m_sMask);
			if(m_pChild2) m_pChild2->addToWorld(m_sGroup, m_sMask);
			return;
		}
		assert(m_pEngine && m_pRigidBody);
		if(!m_bIsInWorld) {
			m_pEngine->getWorld()->addRigidBody(m_pRigidBody, m_sGroup, m_sMask);
			m_bIsInWorld = true;
		} else {
			cout << "already in world!" << endl;
		}
	}
	void Splinter::split(const Plane& plane) {
		if(m_pChild1 || m_pChild2) { //recursivly split
			if(m_pChild1) m_pChild1->split(plane);
			if(m_pChild2) m_pChild2->split(plane);
			return;
		}
		
		const float d = plane.distance(getPosition());
		if(d > m_fRadius || d < -m_fRadius) return; //no split occurred
		const Matrix minv = Matrix::invert(getMatrix());
		const Plane p(Matrix::transformCoords(plane.n*plane.d, minv), Matrix::transformNormal(plane.n, minv));
		std::vector<int> ind1, ind2; //vert-indices for child1/2 
		for(unsigned int i = 0; i < m_vVertices.size(); ++i) {
			if(p.dotCoords(m_vVertices[i]) >= 0.0) {
				ind1.push_back(i);
			} else {
				ind2.push_back(i);
			}
		}
		if(ind1.empty() || ind2.empty()) {
			ind1.clear();
			ind2.clear();
			return; //no split occurred
		}
		//determine edge cuts
		std::vector<Vector3> vEdgeCuts;
		Vector3 hit;
		for(unsigned int i = 0; i < m_vIndices.size(); i+=3) {
			for (int j = 0; j < 3; ++j) {
				if(util::rayHitsPlane(m_vVertices[m_vIndices[i+j]],
				                      m_vVertices[m_vIndices[i+(j+1)%3]]-m_vVertices[m_vIndices[i+j]], p, &hit, true))
				{
					vEdgeCuts.push_back(hit);
				}
			}
		}
		
		//splitting:
		m_pChild1 = new Splinter(m_pEngine);
		m_pChild2 = new Splinter(m_pEngine);
		for(unsigned int i = 0; i < ind1.size(); ++i) m_pChild1->addPoint(m_vVertices[ind1[i]]);
		for(unsigned int i = 0; i < ind2.size(); ++i) m_pChild2->addPoint(m_vVertices[ind2[i]]);
		for(unsigned int i = 0; i < vEdgeCuts.size(); ++i) {
			m_pChild1->addPoint(vEdgeCuts[i]+ROE_SPLINTER_SPLIT_MARGIN*plane.n);
			m_pChild2->addPoint(vEdgeCuts[i]-ROE_SPLINTER_SPLIT_MARGIN*plane.n);
		}
		
		//cleaning up
		ind1.clear();
		ind2.clear();
		btTransform transform;
		m_pMotionState->getWorldTransform(transform);
		const btVector3& tmp = m_pRigidBody->getLinearVelocity();
		Vector3 velocity(tmp);
		if (m_pEngine) {
			m_pEngine->getWorld()->removeRigidBody(m_pRigidBody);
			m_pEngine = nullptr;
		}
		ROE_SAFE_DELETE(m_pRigidBody);
		ROE_SAFE_DELETE(m_pRigidBodyCI);
		ROE_SAFE_DELETE(m_pMotionState);
		ROE_SAFE_DELETE(m_pShape);
		m_vVertices.clear();
		m_vNormals.clear();
		m_vIndices.clear();
		
		m_pChild1->finalize(transform, m_fDensity);
		m_pChild2->finalize(transform, m_fDensity);
		m_pChild1->getRigidBody()->applyImpulse(velocity*m_pChild1->getMass(), Vector3::ZERO.bt());
		m_pChild2->getRigidBody()->applyImpulse(velocity*m_pChild2->getMass(), Vector3::ZERO.bt());
		if(m_bIsInWorld) {
			m_pChild1->addToWorld(m_sGroup, m_sMask);
			m_pChild2->addToWorld(m_sGroup, m_sMask);
			m_bIsInWorld = false;
		}
	}
	
}
}

#endif //ROE_COMPILE_PHYSICS
