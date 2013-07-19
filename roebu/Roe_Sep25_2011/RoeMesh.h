#ifndef ROEMESH_H_
#define ROEMESH_H_

#include "RoeVector2.h"
#include "RoeVector3.h"
#include "RoeVertex.h"
#include "RoeCommon.h"
#include "RoeVertexBufferObject.h"
#include "RoeBoundingBox.h"

namespace roe {
	
	class MeshNode {
	public:
		MeshNode():up(nullptr),rt(nullptr),dn(nullptr),lt(nullptr),v(Vector3::ZERO,Vector3::Y,Vector2::ZERO) {}
		~MeshNode(){}
		MeshNode *up, *rt, *dn, *lt;
		Vertex v;
		
		void disconnect() {
			if(up) up->dn = nullptr;
			if(rt) rt->lt = nullptr;
			if(dn) dn->up = nullptr;
			if(lt) lt->rt = nullptr;}	
	private:
		
	};
	
	class Mesh {
	public:
		Mesh();
		~Mesh();
		
		void build(const Vector3 *vertices, int w, int h = -1);
		void fillVBO(VertexBufferObject& vbo);
		void fillVertexArray(Vertex *pv);
		
		//ystep should probably be negative
		void setTexCoords(const Vector2 base, const float xstep, const float ystep) {setTexCoords(base, xstep*Vector2::X, ystep*Vector2::Y);}
		void setTexCoords(Vector2 base, const Vector2& xstep, const Vector2& ystep);
		void calculateNormals();
		void simplify(const float simplyfyRadAngle = 0.35f);
		void destroy(); //deletes all nodes
		std::string toString();
		
	private:
		
		MeshNode *m_upltNode; //upper left node
	};
	
} // namespace roe

#endif /* ROEMESH_H_ */
