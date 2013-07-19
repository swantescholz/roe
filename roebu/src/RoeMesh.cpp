#include "RoeMesh.h"
#include "RoeUtil.h"
#include "RoeException.h"

namespace roe {
	
	Mesh::Mesh() {
	
	}
	
	Mesh::~Mesh() {
		destroy();
	}
	
	std::string Mesh::toString() {
		std::string s;
		MeshNode *node1 = m_upltNode, *node2;
		while(node1) {
			node2 = node1;
			while(node2) {
				s += util::toString(node2->v.index) + " " + (int)(bool)(node2->up) + " " + (int)(bool)(node2->rt) +
						" " + (int)(bool)(node2->dn) + " " + (int)(bool)(node2->lt) + "\n";
				node2 = node2->rt;
			}
			node1 = node1->dn;
		}
		return s;
	}
	
	void Mesh::build(const Vector3 *vertices, int w, int h) {
		if (h < 0) h = w;
		m_upltNode = new MeshNode();
		m_upltNode->v.p = vertices[0];
		m_upltNode->v.index = 0;
		MeshNode *node1 = m_upltNode, *node2, *node3;
		long index = 1;
		//creating:
		for (int y = 0; y < h; ++y) {
			node2 = node1;
			for (int x = 1; x < w; ++x) {
				node2->rt = new MeshNode();
				node2->rt->lt = node2;
				node2 = node2->rt;
				node2->v.index = index++;
				node2->v.p = vertices[y*w+x];
			}
			if (y < h-1) {
				node1->dn = new MeshNode();
				node1->dn->up = node1;
				node1 = node1->dn;
				node1->v.index = index++;
				node1->v.p = vertices[(y+1)*w];
			}
		}
		//linking:
		node1 = m_upltNode;
		while(node1->dn) {
			node2 = node1;
			node3 = node1->dn;
			while(node2->rt && node3->rt) {
				node2 = node2->rt;
				node3 = node3->rt;
				node2->dn = node3;
				node3->up = node2;
			}
			node1 = node1->dn;
		}
		
		//cout << toString() << endl;
	}
	void Mesh::simplify(const float simplyfyRadAngle) {
		
	}
	void Mesh::fillVBO(VertexBufferObject& vbo) {
		auto& vboP = vbo.getPosition();
		auto& vboN = vbo.getNormal();
		auto& vboTC= vbo.getTexCoord();
		auto& vboI = vbo.getIndex();
		
		MeshNode *node1 = m_upltNode, *node2;
		
		//indices:
		while(node1->dn) {
			node2 = node1;
			while(node2->rt) {
				vboI.push_back(node2->v.index);
				vboI.push_back(node2->dn->v.index);
				vboI.push_back(node2->dn->rt->v.index);
				vboI.push_back(node2->v.index);
				vboI.push_back(node2->rt->dn->v.index);
				vboI.push_back(node2->rt->v.index);
				node2 = node2->rt;
			}
			node1 = node1->dn;
		}
		
		//vertices:
		node1 = m_upltNode;
		while (node1) {
			node2 = node1;
			while(node2) {
				vboP.push_back(node2->v.p);
				vboN.push_back(node2->v.n);
				vboTC.push_back(node2->v.tc);
				node2 = node2->rt;
			}
			node1 = node1->dn;
		}
		cout << "index count: "  << (long)vboI.size() << endl;
		cout << "vertex count: " << (long)vboP.size() << endl;
	}
	
	void Mesh::fillVertexArray(Vertex *pv) {
		long index = 0;
		MeshNode *node1 = m_upltNode, *node2;
		while (node1) {
			node2 = node1;
			while(node2) {
				pv[index++] = node2->v;
				node2 = node2->rt;
			}
			node1 = node1->dn;
		}
	}
	
	//ystep should probably be negative
	void Mesh::setTexCoords(Vector2 base, const Vector2& xstep, const Vector2& ystep) {
		MeshNode *node1 = m_upltNode, *node2;
		Vector2 tcTmp;
		while (node1) {
			tcTmp = base;
			node2 = node1;
			while(node2) {
				node2->v.tc = tcTmp;
				tcTmp += xstep;
				node2 = node2->rt;
			}
			base += ystep;
			node1 = node1->dn;
		}
	}
	
	void Mesh::calculateNormals() { 
		MeshNode *node1 = m_upltNode, *node2;
		
		//borders
		node1->v.n = Vector3::normalize(Vector3::cross(node1->rt->v.p - node1->v.p, node1->v.p - node1->dn->v.p));
		node1 = node1->rt;
		while (node1->rt) {
			node1->v.n = Vector3::normalize(Vector3::cross(node1->rt->v.p - node1->lt->v.p, node1->v.p - node1->dn->v.p));
			node1 = node1->rt;
		}
		node1->v.n = Vector3::normalize(Vector3::cross(node1->lt->v.p - node1->v.p, node1->dn->v.p - node1->v.p));
		node1 = node1->dn;
		while (node1->dn) {
			node1->v.n = Vector3::normalize(Vector3::cross(node1->lt->v.p - node1->v.p, node1->dn->v.p - node1->up->v.p));
			node1 = node1->dn;
		}
		node1->v.n = Vector3::normalize(Vector3::cross(node1->up->v.p - node1->v.p, node1->lt->v.p - node1->v.p));
		node1 = node1->lt;
		while (node1->lt) {
			node1->v.n = Vector3::normalize(Vector3::cross(node1->up->v.p - node1->v.p, node1->lt->v.p - node1->rt->v.p));
			node1 = node1->lt;
		}
		node1->v.n = Vector3::normalize(Vector3::cross(node1->rt->v.p - node1->v.p, node1->up->v.p - node1->v.p));
		node1 = node1->up;
		while (node1->up) {
			node1->v.n = Vector3::normalize(Vector3::cross(node1->rt->v.p - node1->v.p, node1->up->v.p - node1->dn->v.p));
			node1 = node1->up;
		}
		
		//core
		node1 = m_upltNode->dn->rt;
		while (node1->dn) {
			node2 = node1;
			while(node2->rt) {
				node2->v.n = Vector3::normalize(Vector3::cross(
					node2->rt->v.p - node2->lt->v.p, node2->up->v.p - node2->dn->v.p));
				node2 = node2->rt;
			}
			node1 = node1->dn;
		}
	}
	
	void Mesh::destroy() { //deletes complete rt-dn rectangle
		if(!m_upltNode) return;
		MeshNode *node1 = m_upltNode, *node2;
		m_upltNode->up = m_upltNode->lt = nullptr;
		for(;;) { //go down
			node2 = node1;
			while(node2->rt) node2=node2->rt; //go right
			while(node2->lt) { //go back left
				node2 = node2->lt;
				delete node2->rt;
			}
			if (node1->dn) node1 = node1->dn;
			else break;
		}
		while(node1->up) {
			node1 = node1->up;
			delete node1->dn;
		}
		delete m_upltNode; m_upltNode = nullptr;
	}
	
	
} // namespace roe
