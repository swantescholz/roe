#ifndef ROETERRAIN_H_
#define ROETERRAIN_H_

#include <functional>

#include "RoeMesh.h"
#include "RoeVertexBufferObject.h"
#include "RoeObject.h"
#include "RoeTexture.h"
#include "RoeVertex.h"
#include "RoePlane.h"
#include "RoeCommon.h"
#include "RoeCamera.h"

namespace roe {
	
	//TODO: geomorphing, splatting, preceding simplification, irregular grid?
	class Terrain;
	
	class Quadtree {
		friend class Terrain;
	public:
		Quadtree(Terrain *pterrain, long start_index, long pw, long ph = -1);
		~Quadtree();
		void destroy();
		void build(int maxLevel);
		void buildToSize(long minEdgeLength);
		void setNeighbors();
		void updateLODs(const Plane *planes);
		void fillIndexVector(std::vector<long>& indices);
		void render();
		void renderLeaf();
		bool isLeaf() {return !uplt && !uprt && !dnlt && !dnrt;}
		Quadtree* getTopLeftLeafNode() {if(isLeaf()) return this; return uplt->getTopLeftLeafNode();}
		
	private:
		void createIndicesLeaf();
		void createBorderIndicesLeaf(long edgeW, long edgeH, const std::vector<long>& newIndexBase); //one new mipmap border level
		void setUpNeighbor(Quadtree *qt) {up=qt; if(isLeaf()) {return;} uplt->setUpNeighbor(qt->dnlt);uprt->setUpNeighbor(qt->dnrt);}
		void setRtNeighbor(Quadtree *qt) {rt=qt; if(isLeaf()) {return;} uprt->setRtNeighbor(qt->uplt);dnrt->setRtNeighbor(qt->dnlt);}
		void setDnNeighbor(Quadtree *qt) {dn=qt; if(isLeaf()) {return;} dnrt->setDnNeighbor(qt->uprt);dnlt->setDnNeighbor(qt->uplt);}
		void setLtNeighbor(Quadtree *qt) {lt=qt; if(isLeaf()) {return;} dnlt->setLtNeighbor(qt->dnrt);uplt->setLtNeighbor(qt->uprt);}
		
		static long   getSubIndex(long uplt, long w_size, long x, long y) {return uplt+y*w_size+x;}
		static int    s_iMinLeafEdgeLength;
		static const float s_fMaxScreenEpsilon; //maximum to-screen-projected cracking error
		static Camera s_Camera; //the camera
		
		Terrain *m_terrain;
		std::vector<std::vector<long>> m_vvIndicesCore;
		std::vector<std::vector<std::vector<long>>> m_vvvIndicesUpBorder, m_vvvIndicesRtBorder, m_vvvIndicesDnBorder, m_vvvIndicesLtBorder;
		Quadtree *uplt, *uprt, *dnlt, *dnrt; //subtrees
		Quadtree *up, *rt, *dn, *lt; //neighbors
		long m_upltIndex, m_uprtIndex, m_dnltIndex, m_dnrtIndex;
		long m_w, m_h;
		long m_iLOD;    //-2 -> visible non-leaf, >= 0-> visible leaf
		long m_iNumVertices;
		long m_iMaxLOD; //maximum LOD
		bool m_bIsVisible;
		BoundingBox m_bbox;
		float m_fInvDiagonalLength; //1/(length of the diagonal of the BBOX)
		//d = maximum error value, D = y-projected distance betrween eye and point, e = maximum projected error value
		//d/D < e   <=>  d^2/D^2 < e^2  <=>  D^2*(1.0/d^2) >= 1.0/e^2 <=> D^2 >= d^2/e^2
		std::vector<float> m_vfMaxDelta;   //the maximum vertical error value when going one level coarser (just leaf)
		std::vector<Vector3> m_vvMaxErrorPos; //the position vertex with the greatest error value
		std::vector<float> m_vfMinDistanceSq; //minimum y-projected squared distance
		//std::vector<float> m_vfMaxEpsilon; //the maximul projected error value when going one level coarser (just leaf)
	};

	//standard function for determining the height of the terrain with given color
	auto StdTerrainHeightFunc = [](const Color& c)->float{return c.brightness() * c.a;};
	
	//terrain class
	//terrain textures shall be: width=height=n^2+1
	class Terrain : public Object {
		friend class Quadtree;
	public:
		Terrain();
		~Terrain();
		
		void create(const Texture& texture, const Matrix& mTransform = Matrix::IDENTITY,
		            std::function<float(const Color&)> func = StdTerrainHeightFunc);
		void create(const Vector3 *vertices, int w, int h);
		void create(const Vertex *vertices, int w, int h);
		void destroy();
		void updateLODs(); //neighbor-quadtree-leaves shall only differ with one LOD
		void render(const Camera& cam, const Matrix& mCameraProjection);
		std::string quadtreeLODsToString();
		
	private:
		void createVBO();
		void createRoot(int min_edge_length = 64);
		long dirToLOD(long x, long y, int dir);
		void adjustLOD(long x, long y, int dir1); //comparison direction
		void adjustLOD(long x, long y, int dir1, int dir2);
		
		int m_w, m_h, m_iNumLeavesW, m_iNumLeavesH;
		VertexBufferObject m_VBO;
		Vertex* m_pVertices; // the differend lod levels of the vertices
		Quadtree *m_pRoot;
		Quadtree ***m_pLeaves; //all qt-leaves
		long     ***m_pLeavesLOD; //pointers to the lods of all the qt-leaves
		Camera m_Camera;
	};

} // namespace roe

#endif /* ROETERRAIN_H_ */
