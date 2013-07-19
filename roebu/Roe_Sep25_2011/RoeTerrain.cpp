#include "RoeTerrain.h"
#include "RoeUtil.h"
#include "RoeException.h"
#include "RoeRandom.h"
#include "RoeModel.h"
#ifdef ROE_USE_SHADERS
	#include "RoeProgram.h"
#endif
#include <array>
#include <algorithm>

//#define ROE_DEBUG_TERRAIN

namespace roe {
	
	int    Quadtree::s_iMinLeafEdgeLength = 3;
	const float  Quadtree::s_fMaxScreenEpsilon = 0.052;
	Camera Quadtree::s_Camera = Camera::STD_CAMERA;
	
#ifdef ROE_DEBUG_TERRAIN
#define debug_cout(str) std::cout << (str) << std::endl
#else
#define debug_cout(str)
#endif
	
	Quadtree::Quadtree(Terrain *pterrain, long start_index, long pw, long ph) : m_terrain(pterrain),
		uplt(nullptr), uprt(nullptr), dnlt(nullptr), dnrt(nullptr), //subnodes
		  up(nullptr),   rt(nullptr),   dn(nullptr),   lt(nullptr), //neighbors
		m_w(pw), m_h((ph==-1)?pw:ph), m_iLOD(-1), m_iNumVertices(m_w*m_h)
	{
		m_upltIndex = start_index;
		m_uprtIndex = m_upltIndex + m_w;
		m_dnltIndex = m_upltIndex + m_h*m_terrain->m_w;
		m_dnrtIndex = m_dnltIndex + m_w;
	}
	Quadtree::~Quadtree() {
		
	}
	void Quadtree::destroy() {
		if(!isLeaf()) {
			uplt->destroy();
			uprt->destroy();
			dnlt->destroy();
			dnrt->destroy();
			ROE_SAFE_DELETE(uplt);
			ROE_SAFE_DELETE(uprt);
			ROE_SAFE_DELETE(dnlt);
			ROE_SAFE_DELETE(dnrt);
		}
		m_vvIndicesCore.clear();
		m_vvvIndicesUpBorder.clear();
		m_vvvIndicesRtBorder.clear();
		m_vvvIndicesDnBorder.clear();
		m_vvvIndicesLtBorder.clear();
		m_vfMaxDelta.clear();
		m_vfMinDistanceSq.clear();
	}
	void Quadtree::buildToSize(long minEdgeLength) {
		int u = m_w, v = m_h, i;
		for(i = 0; i < 100; ++i) {
			u=(u-1)/2+1;
			v=(v-1)/2+1;
			if (u < minEdgeLength || v < minEdgeLength) break;
		}
		if (i <= 0) return;
		cout << i << " subdivisions" << endl;
		build(i);
	}
	void Quadtree::build(int numLevels) {
		m_iLOD = numLevels;
		if(numLevels <= 0) { //leaf
			createIndicesLeaf();
			return;
		}
		if(m_h < 3 || m_w < 3)
			roe_except("WH-size to small; rest levels: " + util::toString(numLevels), "build");
		if (!util::isPOT(m_w-1) || !util::isPOT(m_w-1))
			roe_except("texture width(" + util::toString(m_w) + ") or height(" + util::toString(m_h) + ") not POT+1", "build");
		
		//creating the nodes:
		const long wterr = m_terrain->m_w;
		const long w = (m_w-1)/2+1, h = (m_h-1)/2+1;
		long startIndices[4];
		startIndices[0] = getSubIndex(m_upltIndex, wterr,   0,   0);
		startIndices[1] = getSubIndex(m_upltIndex, wterr, w-1,   0);
		startIndices[2] = getSubIndex(m_upltIndex, wterr,   0, h-1);
		startIndices[3] = getSubIndex(m_upltIndex, wterr, w-1, h-1);
		
		/*cout << w << " " << h << endl;
		for(int i = 0; i < 4; ++i) {cout << startIndices[i] << endl;}
		cout << endl;*/
		
		//creating the sub-quadtrees
		uplt = new Quadtree(m_terrain, startIndices[0], w, h);
		uprt = new Quadtree(m_terrain, startIndices[1], w, h);
		dnlt = new Quadtree(m_terrain, startIndices[2], w, h);
		dnrt = new Quadtree(m_terrain, startIndices[3], w, h);
		
		//building recursively the sub-quadtrees:
		uplt->build(numLevels-1);
		uprt->build(numLevels-1);
		dnlt->build(numLevels-1);
		dnrt->build(numLevels-1);
		
		//not-leaf bbox determination:
		m_bbox.add(uplt->m_bbox);
		m_bbox.add(uprt->m_bbox);
		m_bbox.add(dnlt->m_bbox);
		m_bbox.add(dnrt->m_bbox);
		m_fInvDiagonalLength = 1.0/(m_bbox.max-m_bbox.min).length();
	}
	void Quadtree::setNeighbors() {
		if (!isLeaf()) {
			uplt->setRtNeighbor(uprt);
			dnlt->setRtNeighbor(dnrt);
			uprt->setLtNeighbor(uplt);
			dnrt->setLtNeighbor(dnlt);
			uplt->setDnNeighbor(dnlt);
			uprt->setDnNeighbor(dnrt);
			dnlt->setUpNeighbor(uplt);
			dnrt->setUpNeighbor(uprt);
			uplt->setNeighbors();
			uprt->setNeighbors();
			dnlt->setNeighbors();
			dnrt->setNeighbors();
		}
	}
	void Quadtree::createIndicesLeaf() {
		//initialising index basis
		std::vector<long> newIndexBase, oldIndexBase, vtmp;
		for(long y = 0; y < m_h; ++y) {
			for(long x = 0; x < m_w; ++x) {
				newIndexBase.push_back(getSubIndex(m_upltIndex, m_terrain->m_w, x, y));
			}
		}
		
		//updating bbox:
		m_bbox = BoundingBox(m_terrain->m_pVertices[newIndexBase.front()].p);
		for(auto it = newIndexBase.begin(); it != newIndexBase.end(); ++it) {
			m_bbox.add(m_terrain->m_pVertices[*it].p);
		}
		m_fInvDiagonalLength = 1.0/(m_bbox.max-m_bbox.min).length();
		
		//lets try triangle strips!!
		//mipmapping:
		for (long edgeW = m_w, edgeH = m_h; edgeW >= s_iMinLeafEdgeLength && edgeH >= s_iMinLeafEdgeLength; edgeW = (edgeW-1)/2+1, edgeH = (edgeH-1)/2+1) {
			//core:
			long x;
			for(long y = 1; y < edgeH-2; ++y) {
				vtmp.push_back(roe_pri);
				for(x = 1; x < edgeW-1; ++x) {
					vtmp.push_back(newIndexBase[(y  )*edgeW+x]);
					vtmp.push_back(newIndexBase[(y+1)*edgeW+x]);
				}
			}
			m_vvIndicesCore.push_back(vtmp);
			vtmp.clear();
			
			//borders:
			createBorderIndicesLeaf(edgeW, edgeH, newIndexBase);
			
			//downscaling:
			oldIndexBase.clear();
			oldIndexBase = newIndexBase;
			newIndexBase.clear();
			for(long y = 0; y < edgeH; y+=2) {
				for(long x = 0; x < edgeW; x+=2) {
					newIndexBase.push_back(oldIndexBase[y*edgeW+x]);
				}
			}
			
			//computing error values:
			std::vector<float> vftmp;
			std::vector<Vector3> vvtmp;
			for(long y = 0; y < (m_h-1)/2+1; ++y) {
				for(long x = 1; x < (m_w-1)/2+1; ++x) {
					vvtmp.push_back(m_terrain->m_pVertices[oldIndexBase[y*edgeW+x*2-1]].p);
					vftmp.push_back(0.5*(
							m_terrain->m_pVertices[newIndexBase[y*((m_w-1)/2+1)+x]].p.y+
							m_terrain->m_pVertices[newIndexBase[y*((m_w-1)/2+1)+x-1]].p.y)-
							m_terrain->m_pVertices[oldIndexBase[y*edgeW+x*2-1]].p.y);
				}
			}
			if (!vftmp.empty()) {
				float fMax = 0.0;
				Vector3 vMax = Vector3::ZERO;
				auto iv = vvtmp.begin();
				for (auto it = vftmp.begin(); it != vftmp.end(); ++it, ++iv) {
					if (std::abs(*it) > std::abs(fMax)) {
						fMax = *it;
						vMax = *iv;
					}
				}
				m_vfMaxDelta.push_back(fMax);
				m_vvMaxErrorPos.push_back(vMax);
				m_vfMinDistanceSq.push_back(fMax*fMax/(s_fMaxScreenEpsilon*s_fMaxScreenEpsilon));
			}
			vftmp.clear();
			vvtmp.clear();
		}
		m_iMaxLOD = (int)(m_vvIndicesCore.size())-1; //updating the maximum LOD
	}
	void Quadtree::createBorderIndicesLeaf(long edgeW, long edgeH, const std::vector<long>& newIndexBase) {
		long x;
		std::vector<long> vtmp;
		//borders:
		m_vvvIndicesUpBorder.push_back(std::vector<std::vector<long>>());
		m_vvvIndicesRtBorder.push_back(std::vector<std::vector<long>>());
		m_vvvIndicesDnBorder.push_back(std::vector<std::vector<long>>());
		m_vvvIndicesLtBorder.push_back(std::vector<std::vector<long>>());
		//up0
		vtmp.push_back(roe_pri);
		vtmp.push_back(newIndexBase[0]);
		vtmp.push_back(newIndexBase[edgeW+1]);
		vtmp.push_back(newIndexBase[1]);
		vtmp.push_back(roe_pri);
		for(x = 1; x < edgeW-1; ++x) {
			vtmp.push_back(newIndexBase[x]);
			vtmp.push_back(newIndexBase[edgeW+x]);
		}
		vtmp.push_back(newIndexBase[edgeW-1]);
		m_vvvIndicesUpBorder.back().push_back(vtmp);
		vtmp.clear();
		//up1
		for(x = 0; x < edgeW-3; x+=2) {
			vtmp.push_back(roe_pri);
			vtmp.push_back(newIndexBase[x]);
			vtmp.push_back(newIndexBase[edgeW+x+1]);
			vtmp.push_back(newIndexBase[x+2]);
			vtmp.push_back(newIndexBase[edgeW+x+2]);
			vtmp.push_back(newIndexBase[edgeW+x+3]);
		}
		vtmp.push_back(roe_pri);
		vtmp.push_back(newIndexBase[edgeW-3]);
		vtmp.push_back(newIndexBase[edgeW*2-2]);
		vtmp.push_back(newIndexBase[edgeW-1]);
		m_vvvIndicesUpBorder.back().push_back(vtmp);
		vtmp.clear();
		//down0
		vtmp.push_back(roe_pri);
		vtmp.push_back(newIndexBase[(edgeH)*edgeW-1]);
		vtmp.push_back(newIndexBase[(edgeH-1)*edgeW-2]);
		vtmp.push_back(newIndexBase[(edgeH)*edgeW-2]);
		vtmp.push_back(roe_pri);
		for(x = edgeW-2; x > 0; --x) {
			vtmp.push_back(newIndexBase[(edgeH-1)*edgeW+x]);
			vtmp.push_back(newIndexBase[(edgeH-2)*edgeW+x]);
		}
		vtmp.push_back(newIndexBase[(edgeH-1)*edgeW]);
		m_vvvIndicesDnBorder.back().push_back(vtmp);
		vtmp.clear();
		//down1
		for(x = edgeW-1; x > 2; x-=2) {
			vtmp.push_back(roe_pri);
			vtmp.push_back(newIndexBase[(edgeH-1)*edgeW+x]);
			vtmp.push_back(newIndexBase[(edgeH-2)*edgeW+x-1]);
			vtmp.push_back(newIndexBase[(edgeH-1)*edgeW+x-2]);
			vtmp.push_back(newIndexBase[(edgeH-2)*edgeW+x-2]);
			vtmp.push_back(newIndexBase[(edgeH-2)*edgeW+x-3]);
		}
		vtmp.push_back(roe_pri);
		vtmp.push_back(newIndexBase[(edgeH-1)*edgeW+2]);
		vtmp.push_back(newIndexBase[(edgeH-2)*edgeW+1]);
		vtmp.push_back(newIndexBase[(edgeH-1)*edgeW]);
		m_vvvIndicesDnBorder.back().push_back(vtmp);
		vtmp.clear();
		//left0
		vtmp.push_back(roe_pri);
		vtmp.push_back(newIndexBase[0]);
		for(long y = 1; y < edgeH-1; ++y) {
			vtmp.push_back(newIndexBase[y*edgeW]);
			vtmp.push_back(newIndexBase[y*edgeW+1]);
		}
		vtmp.push_back(newIndexBase[(edgeH-1)*edgeW]);
		m_vvvIndicesLtBorder.back().push_back(vtmp);
		vtmp.clear();
		//left1
		for(long y = edgeH-1; y > 2; y-=2) {
			vtmp.push_back(roe_pri);
			vtmp.push_back(newIndexBase[y*edgeW]);
			vtmp.push_back(newIndexBase[(y-1)*edgeW+1]);
			vtmp.push_back(newIndexBase[(y-2)*edgeW]);
			vtmp.push_back(newIndexBase[(y-2)*edgeW+1]);
			vtmp.push_back(newIndexBase[(y-3)*edgeW+1]);
		}
		vtmp.push_back(roe_pri);
		vtmp.push_back(newIndexBase[edgeW*2]);
		vtmp.push_back(newIndexBase[edgeW+1]);
		vtmp.push_back(newIndexBase[0]);
		m_vvvIndicesLtBorder.back().push_back(vtmp);
		vtmp.clear();
		//right0
		vtmp.push_back(roe_pri);
		vtmp.push_back(newIndexBase[edgeW-1]);
		for(long y = 1; y < edgeH-1; ++y) {
			vtmp.push_back(newIndexBase[(y+1)*edgeW-2]);
			vtmp.push_back(newIndexBase[(y+1)*edgeW-1]);
		}
		vtmp.push_back(newIndexBase[edgeH*edgeW-1]);
		m_vvvIndicesRtBorder.back().push_back(vtmp);
		vtmp.clear();
		//right1
		for(long y = 0; y < edgeH-3; y+=2) {
			vtmp.push_back(roe_pri);
			vtmp.push_back(newIndexBase[(y+1)*edgeW-1]);
			vtmp.push_back(newIndexBase[(y+2)*edgeW-2]);
			vtmp.push_back(newIndexBase[(y+3)*edgeW-1]);
			vtmp.push_back(newIndexBase[(y+3)*edgeW-2]);
			vtmp.push_back(newIndexBase[(y+4)*edgeW-2]);
		}
		vtmp.push_back(roe_pri);
		vtmp.push_back(newIndexBase[(edgeH-2)*edgeW-1]);
		vtmp.push_back(newIndexBase[(edgeH-1)*edgeW-2]);
		vtmp.push_back(newIndexBase[edgeH*edgeW-1]);
		m_vvvIndicesRtBorder.back().push_back(vtmp);
		vtmp.clear();
	}
	void Quadtree::updateLODs(const Plane *planes) {
		/*if (!m_bbox.isVisible(planes)) { //invisible, so discard:
			m_iLOD = -1;
			return;
		}//*/
		m_bIsVisible = m_bbox.isVisible(planes);
		if (isLeaf()) { //visible leaf, so determine LOD
			const float distanceSq = Vector2::distance2(Vector2(m_vvMaxErrorPos[m_iLOD].x, m_vvMaxErrorPos[m_iLOD].z),
			                                            Vector2(s_Camera.pos.x, s_Camera.pos.z));
			if (m_iLOD < 0) { //start-up, so find good starting value
				m_iLOD = 0;
				for (;;) {
					if (m_iLOD > m_iMaxLOD) {m_iLOD = m_iMaxLOD; break;}
					if (distanceSq >= m_vfMinDistanceSq[m_iLOD]) {
						++m_iLOD;
					}
					else break;
				}
			}
			else { //not starting, so incease or decrease or keep the same LOD
				if (distanceSq >= m_vfMinDistanceSq[m_iLOD] && m_iLOD < m_iMaxLOD) {
					++m_iLOD;
				} else if (m_iLOD >= 1) {
					const float distanceSq2 = Vector2::distance2(Vector2(m_vvMaxErrorPos[m_iLOD-1].x, m_vvMaxErrorPos[m_iLOD-1].z),
			                                                     Vector2(s_Camera.pos.x, s_Camera.pos.z));
					if (distanceSq2 < m_vfMinDistanceSq[m_iLOD-1]) {
						--m_iLOD; //higher level of detail needed
					}
				}
			}
		} else { //visible non-leaf
			m_iLOD = -2;
			//recursivly updating
			uplt->updateLODs(planes);
			uprt->updateLODs(planes);
			dnlt->updateLODs(planes);
			dnrt->updateLODs(planes);
		}
	}
	void Quadtree::fillIndexVector(std::vector<long>& indices) {
		static int iup, irt, idn, ilt;
		if (!m_bIsVisible) return; //invisible
		if(!isLeaf()) { //node
			uplt->fillIndexVector(indices);
			uprt->fillIndexVector(indices);
			dnlt->fillIndexVector(indices);
			dnrt->fillIndexVector(indices);
			return;
		}
#ifdef ROE_DEBUG_TERRAIN
		if(m_iLOD > m_iMaxLOD) 
			throw Exception("LOD (" + util::toString(m_iLOD) + ") bigger than maxLOD (" + util::toString(m_iMaxLOD) + ")",
			                "fillIndexVector", "RoeTerrain.cpp", ROE_LINE);
#endif
		//leaf
		//0-> this is equally detailed or coarser; 1-> this is more-detailed than neighbor
		iup = irt = idn = ilt = 1;
		if (up) iup = (up->m_iLOD-m_iLOD > 0) ? 1 : 0;
		if (rt) irt = (rt->m_iLOD-m_iLOD > 0) ? 1 : 0;
		if (dn) idn = (dn->m_iLOD-m_iLOD > 0) ? 1 : 0;
		if (lt) ilt = (lt->m_iLOD-m_iLOD > 0) ? 1 : 0;
		//if(roe_rand_unit < 0.01 && !rt && !dn) cout << "i: " << iup << irt << idn << ilt << up->m_iLOD << m_iLOD << endl;
		indices.insert(indices.end(), m_vvIndicesCore[m_iLOD].begin(), m_vvIndicesCore[m_iLOD].end());
		indices.insert(indices.end(), m_vvvIndicesUpBorder[m_iLOD][iup].begin(), m_vvvIndicesUpBorder[m_iLOD][iup].end());
		indices.insert(indices.end(), m_vvvIndicesRtBorder[m_iLOD][irt].begin(), m_vvvIndicesRtBorder[m_iLOD][irt].end());
		indices.insert(indices.end(), m_vvvIndicesDnBorder[m_iLOD][idn].begin(), m_vvvIndicesDnBorder[m_iLOD][idn].end());
		indices.insert(indices.end(), m_vvvIndicesLtBorder[m_iLOD][ilt].begin(), m_vvvIndicesLtBorder[m_iLOD][ilt].end());//*/
	}
	
	//*/#######################################################################
	
	Terrain::Terrain(){
		m_w = m_h = -1;
		m_pVertices  = nullptr;
		m_pRoot      = nullptr;
		m_pLeaves    = nullptr;
		m_pLeavesLOD = nullptr;
	}
	Terrain::~Terrain() {
		destroy();
	}
	void Terrain::destroy() {
		if (!m_pRoot) return;
		m_pRoot->destroy();
		ROE_SAFE_DELETE(m_pRoot);
		ROE_SAFE_DELETE_ARRAY(m_pVertices);
		m_VBO.destroy();
		m_w = m_h = -1;
		ROE_SAFE_DELETE_2ARRAY(m_pLeaves   , m_iNumLeavesW);
		ROE_SAFE_DELETE_2ARRAY(m_pLeavesLOD, m_iNumLeavesW);
	}
	void Terrain::create(const Texture& texture, const Matrix& mTransform, std::function<float(const Color&)> func) {
		static const float inv = 1.0f/255.0f;
		
		m_w = texture.getWidth();
		m_h = texture.getHeight();
		
		if (!util::isPOT(m_w-1) || !util::isPOT(m_w-1))
			roe_except("texture width(" + util::toString(m_w) + ") or height(" + util::toString(m_h) + ") not POT+1", "create");
		
		Vector3 pos;
		const int bpp = texture.getBPP();
		byte* pixels = texture.getData();
		
		std::vector<Vector3> vP;
		Color col;
		
		//positions
		for (int i = m_h-1; i >= 0; i--) {
			for (int j = 0; j < m_w; j++) {
				col.r = (int)pixels[(i*m_w+j)*bpp+0]*inv;
				col.g = (int)pixels[(i*m_w+j)*bpp+1]*inv;
				col.b = (int)pixels[(i*m_w+j)*bpp+2]*inv;
				col.a = (int)pixels[(i*m_w+j)*bpp+3]*inv;
				pos.y = func(col);
				pos.x = -2*(j-0.5*(m_w-1))/(m_w-1);
				pos.z =  2*(i-0.5*(m_h-1))/(m_h-1);
				pos = Matrix::transformCoords(pos, mTransform);
				vP.push_back(pos);
			}
		}
		delete[] pixels;
		
		create(vP.data(), m_w, m_h);
		vP.clear();
	}
	void Terrain::create(const Vector3 *vertices, int w, int h) {
		m_w = w; m_h = h;
		if (!util::isPOT(m_w-1) || !util::isPOT(m_w-1))
			roe_except("texture width(" + util::toString(m_w) + ") or height(" + util::toString(m_h) + ") not POT+1", "create");
		Mesh mesh;
		mesh.build(vertices, m_w, m_h);
		mesh.setTexCoords(Vector2(0,0), 1.0/(m_w-1), -1.0/(m_h-1));
		mesh.calculateNormals();
		//mesh.simplify();
		m_pVertices = new Vertex[m_w*m_h];
		mesh.fillVertexArray(m_pVertices);
		mesh.destroy();
		createVBO();
		createRoot(32);
	}
	void Terrain::create(const Vertex *vertices, int w, int h) {
		m_w = w; m_h = h;
		m_pVertices = new Vertex[w*h];
		for(long i = 0; i < m_w*m_h; ++i) {
			m_pVertices[i] = vertices[i];
		}
		createVBO();
		createRoot(32);
	}
	void Terrain::createVBO() {
		auto& vboP = m_VBO.getPosition();
		auto& vboN = m_VBO.getNormal  ();
		auto& vboTC= m_VBO.getTexCoord();
		for (long i = 0; i < m_w*m_h; ++i) {
			vboP.push_back (m_pVertices[i].p);
			vboN.push_back (m_pVertices[i].n);
			vboTC.push_back(m_pVertices[i].tc);
		}
		m_VBO.create(GL_STATIC_DRAW);
	}
	void Terrain::createRoot(int min_edge_length) {
		m_pRoot = new Quadtree(this, 0, m_w, m_h);
		m_pRoot->buildToSize(min_edge_length); //building the quadtree
		//m_pRoot->build(5);
		m_pRoot->setNeighbors(); //setting the neighbors
		
		//saving pointers to the leaves in array
		Quadtree *leaf1 = m_pRoot->getTopLeftLeafNode(), *leaf2;
		long x, y;
		leaf2 = leaf1;
		for (m_iNumLeavesW = 0, leaf2 = leaf1; leaf2; ++m_iNumLeavesW, leaf2 = leaf2->rt) {}
		for (m_iNumLeavesH = 0, leaf2 = leaf1; leaf2; ++m_iNumLeavesH, leaf2 = leaf2->dn) {}
		m_pLeaves    = new Quadtree**[m_iNumLeavesW];
		m_pLeavesLOD = new long    **[m_iNumLeavesW];
		for (x = 0; x < m_iNumLeavesW; ++x) {
			m_pLeaves   [x] = new Quadtree*[m_iNumLeavesH];
			m_pLeavesLOD[x] = new long    *[m_iNumLeavesH];
		}
		for (x = 0; leaf1; ++x) { // copying into array
			leaf2 = leaf1;
			for (y = 0; leaf2; ++y) {
				m_pLeaves   [x][y] = leaf2;
				m_pLeavesLOD[x][y] = &(leaf2->m_iLOD);
				leaf2 = leaf2->rt;
			}
			leaf1 = leaf1->dn;
		}
		
		debug_cout("BigBox: "+m_pRoot->m_bbox.toString());
		debug_cout("MaxLOD: "+util::toString(m_pRoot->getTopLeftLeafNode()->m_iMaxLOD));
		debug_cout("LeavesW: "+util::toString(m_iNumLeavesW));
		debug_cout("LeavesH: "+util::toString(m_iNumLeavesH));
	}
	void Terrain::updateLODs() {
		//return;
		float minDistanceSq = -1.0, newDistanceSq;
		long minX = -1, minY = -1;
		for (long x = 0; x < m_iNumLeavesW; ++x) {
			for (long y = 0; y < m_iNumLeavesH; ++y) {
				newDistanceSq = m_pLeaves[x][y]->m_bbox.getCenter().distance2(m_Camera.pos);
				if (newDistanceSq < minDistanceSq || minX < 0) {
					minDistanceSq = newDistanceSq;
					minX = x;
					minY = y;
				}
			}
		}
		const long camIndexX = minX; //the x-index of the qt-leaf nearest to the camera
		const long camIndexY = minY; //the y-index of the qt-leaf nearest to the camera
		
		//circling arround camera-nearest qt-leaf, adjusting the LODs (but just to better, not worse quality)
		long x,y;
		for(long edge = 1;; ++edge) {//TODO: testing
			if(camIndexX-edge < 0 && camIndexY-edge < 0 && camIndexX+edge >= m_iNumLeavesW && camIndexY+edge >= m_iNumLeavesH)
				break; //done
			//up
			x = camIndexX; y = camIndexY-edge;
			if(camIndexY-edge >= 0) {
				adjustLOD(x,y, 2);
				for(long i = 1; i < edge; ++i) {
					adjustLOD(x-i,y, 2, 1); //going left
					adjustLOD(x+i,y, 2, 3); //right
				}
			}
			//down
			x = camIndexX; y = camIndexY+edge;
			if(camIndexY+edge < m_iNumLeavesH) {
				adjustLOD(x,y, 0);
				for(long i = 1; i < edge; ++i) {
					adjustLOD(x-i,y, 0, 1); //going left
					adjustLOD(x+i,y, 0, 3); //right
				}
			}
			//left
			x = camIndexX-edge; y = camIndexY;
			if(camIndexX-edge >= 0) {
				adjustLOD(x,y, 1);
				for(long i = 1; i < edge; ++i) {
					adjustLOD(x,y-i, 1, 2); //going up
					adjustLOD(x,y+i, 1, 0); //down
				}
			}
			//right
			x = camIndexX+edge; y = camIndexY;
			if(camIndexX+edge < m_iNumLeavesW) {
				adjustLOD(x,y, 3); 
				for(long i = 1; i < edge; ++i) {
					adjustLOD(x,y-i, 3, 2); //going up
					adjustLOD(x,y+i, 3, 0); //down
				}
			}
			//corners
			if(camIndexX-edge >=            0 && camIndexY-edge >=            0) adjustLOD(camIndexX-edge, camIndexY-edge, 1, 2);
			if(camIndexX+edge < m_iNumLeavesW && camIndexY-edge >=            0) adjustLOD(camIndexX+edge, camIndexY-edge, 3, 2);
			if(camIndexX+edge < m_iNumLeavesW && camIndexY+edge < m_iNumLeavesH) adjustLOD(camIndexX+edge, camIndexY+edge, 3, 0);
			if(camIndexX-edge >=            0 && camIndexY+edge < m_iNumLeavesH) adjustLOD(camIndexX-edge, camIndexY+edge, 1, 0);
			
		}
	}
	void Terrain::render(const Camera& cam, const Matrix& mCameraProjection) {
		std::vector<long> indices;
		Plane planes[6];
		util::computeClipPlanes(mCameraProjection, planes);
		update(); //updating matrices
		m_Camera = cam; //setting the static camera position
		m_Camera.pos = Matrix::transformCoords(m_Camera.pos, m_mInvMatrix);
		m_Camera.dir = Matrix::transformNormal(m_Camera.dir, m_mInvMatrix);
		m_Camera.up  = Matrix::transformNormal(m_Camera.up , m_mInvMatrix);
		Quadtree::s_Camera = m_Camera; //updating qt-cam
		
		util::timeDiff();
		m_pRoot->updateLODs(planes); //first draft for LODs
		debug_cout(util::toString(util::timeDiff())+"\t= updating LODs 1 time");
		updateLODs(); //second and final draft for LODs
		debug_cout(util::toString(util::timeDiff())+"\t= updating LODs 2 time");
		/*std::string models[7] = {"sphere", "cuboid", "cylinder", "cuboid","tetrahedron", "sphere", "teapot"};
		util::traverseArray2(m_iNumLeavesW,m_iNumLeavesH, [=](long x, long y){
			Model mod(models[m_pLeaves[x][y]->m_iLOD]);
			mod.setPosition(m_pLeaves[x][y]->m_bbox.getCenter());
			mod.setScaling(Vector3(1.2));
			mod.render();
		});//*/
		
		m_pRoot->fillIndexVector(indices);
		debug_cout(util::toString(util::timeDiff())+"\t= filling index vector (size: " + util::toString((long)(indices.size())) + ") time");
		const Matrix m = getMatrix();
#ifdef ROE_USE_SHADERS
		const Matrix n = getRotationMatrix();
		Program::setStdTransformationMatrix(m);
		Program::setStdNormalMatrix(n);
#else
		glPushMatrix();
			glMultMatrixf(m.n);
#endif
		m_VBO.enable();
			m_VBO.drawElements(GL_TRIANGLE_STRIP, (long)(indices.size()), indices.data());
			debug_cout(util::toString(util::timeDiff())+"\t= drawing elements time");
		m_VBO.disable();
#ifndef ROE_USE_SHADERS
		glPopMatrix();
#endif
		indices.clear();
	}
	std::string Terrain::quadtreeLODsToString() {
		std::string str;
		Quadtree *leaf1 = m_pRoot->getTopLeftLeafNode(), *leaf2;
		while(leaf1) {
			leaf2 = leaf1;
			while(leaf2) {
				if (leaf2->m_iLOD < 0)
					str += "_";
				else
					str += util::toString(leaf2->m_iLOD);
				leaf2 = leaf2->rt;
			}
			str += "\n";
			leaf1 = leaf1->dn;
		}
		str.erase((long)(str.length())-1);
		return str;
	}
	long Terrain::dirToLOD(long x, long y, int dir) {//0up, 1rt...
		switch(dir) {
			case 0: return *m_pLeavesLOD[x  ][y-1];
			case 1: return *m_pLeavesLOD[x+1][y  ];
			case 2: return *m_pLeavesLOD[x  ][y+1];
			case 3: return *m_pLeavesLOD[x-1][y  ];
			default: roe_except("impossible", "dirToLOD");
		}
	}
	void Terrain::adjustLOD(long x, long y, int dir1) {
		if(x < 0 || y < 0 || x >= m_iNumLeavesW || y >= m_iNumLeavesH) return; //outside
		//*m_pLeavesLOD[x][y] = 0; return;
		const long d  = *m_pLeavesLOD[x][y] - dirToLOD(x,y,dir1);
		     if(d >  1) *m_pLeavesLOD[x][y] -= d-1;
		else if(d < -1) *m_pLeavesLOD[x][y] -= d+1;
	}
	void Terrain::adjustLOD(long x, long y, int dir1, int dir2) { //dir1: main prio, dir2: sub-main prio
		if(x < 0 || y < 0 || x >= m_iNumLeavesW || y >= m_iNumLeavesH) return; //outside
		const long a = dirToLOD(x,y,dir1);
		const long b = dirToLOD(x,y,dir2);
		long &c = *m_pLeavesLOD[x][y];
		long d = std::abs(b - a);
		#ifdef ROE_DEBUG_TERRAIN
			if(d > 2) throw Exception("fatal", "adjustLOD", "RoeTerrain.cpp", ROE_LINE);
		#endif
		if (d == 2) {c = (a+b)/2; return;}
		if (d == 1) {
			if (c > std::max(a,b)) {c = std::max(a,b); return;}
			if (c < std::min(a,b)) {c = std::min(a,b); return;}
		}
		//d==0
		d = c-a;
		if (d >  1) {c = a+1; return;}
		if (d < -1) {c = a-1; return;}
	}
	
} // namespace roe
