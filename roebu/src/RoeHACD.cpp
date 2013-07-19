#include "RoeHACD.h"

/* Copyright (c) 2011 Khaled Mamou (kmamou at gmail dot com)
 All rights reserved.
 
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 
 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 
 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 
 3. The names of the contributors may not be used to endorse or promote products derived from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
//I(swash93) used Khaled Mamou's great HACD-code for my project, thanks!
//I put it all for simplicity in one(or rather two) file(s) and modified it slightly(mainly the indention)

#include <limits>
#include <sstream>
#include <cstring>
#include <algorithm>
#include <iterator>

namespace HACD {
	
	//MANIFOLD=MESH====================================================
	Material::Material(void)
	{
		m_diffuseColor.X()  = 0.5;
		m_diffuseColor.Y()  = 0.5;
		m_diffuseColor.Z()  = 0.5;
		m_specularColor.X() = 0.5;
		m_specularColor.Y() = 0.5;
		m_specularColor.Z() = 0.5;
		m_ambientIntensity  = 0.4;
		m_emissiveColor.X() = 0.0;
		m_emissiveColor.Y() = 0.0;
		m_emissiveColor.Z() = 0.0;
		m_shininess         = 0.4;
		m_transparency      = 0.0;
	}
	
	TMMVertex::TMMVertex(void)
	{
		m_name = 0;
		m_id = 0;
		m_duplicate = 0;
		m_onHull = false;
		m_tag = false;
	}
	TMMVertex::~TMMVertex(void)
	{
	}
	TMMEdge::TMMEdge(void)
	{
		m_id = 0;
		m_triangles[0] = m_triangles[1] = m_newFace = 0;
		m_vertices[0] = m_vertices[1] = 0;
	}
	TMMEdge::~TMMEdge(void)
	{
	}
	TMMTriangle::TMMTriangle(void)
	{
		m_id = 0;
		for(int i = 0; i < 3; i++)
		{
			m_edges[i] = 0;
			m_vertices[0] = 0;
		}
		m_visible = false;
	}
	TMMTriangle::~TMMTriangle(void)
	{
	}
	TMMesh::TMMesh(void)
	{
		m_barycenter = Vec3<Real>(0,0,0);
		m_diag = 1;
	}
	TMMesh::~TMMesh(void)
	{
	}
	
	void TMMesh::Print() 
	{
		size_t nV = m_vertices.GetSize();
		std::cout << "-----------------------------" << std::endl;
		std::cout << "vertices (" << nV << ")" << std::endl;
		for(size_t v = 0; v < nV; v++)
		{
			const TMMVertex & currentVertex = m_vertices.GetData();
			std::cout  << currentVertex.m_id      << ", " 
					<< currentVertex.m_pos.X() << ", " 
					<< currentVertex.m_pos.Y() << ", "
					<< currentVertex.m_pos.Z() << std::endl;
			m_vertices.Next();
		}
		
		
		size_t nE = m_edges.GetSize();
		std::cout << "edges (" << nE << ")" << std::endl;
		for(size_t e = 0; e < nE; e++)
		{
			const TMMEdge & currentEdge = m_edges.GetData();
			const CircularListElement<TMMVertex> * v0 = currentEdge.m_vertices[0];
			const CircularListElement<TMMVertex> * v1 = currentEdge.m_vertices[1];
			const CircularListElement<TMMTriangle> * f0 = currentEdge.m_triangles[0];
			const CircularListElement<TMMTriangle> * f1 = currentEdge.m_triangles[1];
			
			std::cout << "-> (" << v0->GetData().m_name << ", " << v1->GetData().m_name << ")" << std::endl; 
			std::cout << "-> F0 (" << f0->GetData().m_vertices[0]->GetData().m_name << ", " 
					<< f0->GetData().m_vertices[1]->GetData().m_name << ", "
					<< f0->GetData().m_vertices[2]->GetData().m_name <<")" << std::endl; 
			std::cout << "-> F1 (" << f1->GetData().m_vertices[0]->GetData().m_name << ", " 
					<< f1->GetData().m_vertices[1]->GetData().m_name << ", "
					<< f1->GetData().m_vertices[2]->GetData().m_name << ")" << std::endl;                         
			m_edges.Next();
		}
		size_t nT = m_triangles.GetSize();
		std::cout << "triangles (" << nT << ")" << std::endl;
		for(size_t t = 0; t < nT; t++)
		{
			const TMMTriangle & currentTriangle = m_triangles.GetData();
			const CircularListElement<TMMVertex> * v0 = currentTriangle.m_vertices[0];
			const CircularListElement<TMMVertex> * v1 = currentTriangle.m_vertices[1];
			const CircularListElement<TMMVertex> * v2 = currentTriangle.m_vertices[2];
			const CircularListElement<TMMEdge> * e0 = currentTriangle.m_edges[0];
			const CircularListElement<TMMEdge> * e1 = currentTriangle.m_edges[1];
			const CircularListElement<TMMEdge> * e2 = currentTriangle.m_edges[2];
			
			std::cout << "-> (" << v0->GetData().m_name << ", " << v1->GetData().m_name << ", "<< v2->GetData().m_name << ")" << std::endl; 
			
			std::cout << "-> E0 (" << e0->GetData().m_vertices[0]->GetData().m_name << ", " 
					<< e0->GetData().m_vertices[1]->GetData().m_name << ")" << std::endl; 
			std::cout << "-> E1 (" << e1->GetData().m_vertices[0]->GetData().m_name << ", " 
					<< e1->GetData().m_vertices[1]->GetData().m_name << ")" << std::endl;
			std::cout << "-> E2 (" << e2->GetData().m_vertices[0]->GetData().m_name << ", " 
					<< e2->GetData().m_vertices[1]->GetData().m_name << ")" << std::endl;   
			m_triangles.Next();
		}   
	}
	bool TMMesh::Save(const char *fileName)
	{
		std::ofstream fout(fileName);
		std::cout << "Saving " <<  fileName << std::endl;
		if (SaveVRML2(fout))
		{
			fout.close();
			return true;
		}
		return false;
	}
	bool TMMesh::SaveVRML2(std::ofstream &fout)
	{
		return SaveVRML2(fout, Material());
	}
	bool TMMesh::SaveVRML2(std::ofstream &fout, const Material & material)
	{
		if (fout.is_open()) 
		{
			size_t nV = m_vertices.GetSize();
			size_t nT = m_triangles.GetSize();            
			fout <<"#VRML V2.0 utf8" << std::endl;            
			fout <<"" << std::endl;
			fout <<"# Vertices: " << nV << std::endl;        
			fout <<"# Triangles: " << nT << std::endl;        
			fout <<"" << std::endl;
			fout <<"Group {" << std::endl;
			fout <<"    children [" << std::endl;
			fout <<"        Shape {" << std::endl;
			fout <<"            appearance Appearance {" << std::endl;
			fout <<"                material Material {" << std::endl;
			fout <<"                    diffuseColor "      << material.m_diffuseColor.X()      << " " 
					<< material.m_diffuseColor.Y()      << " "
					<< material.m_diffuseColor.Z()      << std::endl;  
			fout <<"                    ambientIntensity "  << material.m_ambientIntensity      << std::endl;
			fout <<"                    specularColor "     << material.m_specularColor.X()     << " " 
					<< material.m_specularColor.Y()     << " "
					<< material.m_specularColor.Z()     << std::endl; 
			fout <<"                    emissiveColor "     << material.m_emissiveColor.X()     << " " 
					<< material.m_emissiveColor.Y()     << " "
					<< material.m_emissiveColor.Z()     << std::endl; 
			fout <<"                    shininess "         << material.m_shininess             << std::endl;
			fout <<"                    transparency "      << material.m_transparency          << std::endl;
			fout <<"                }" << std::endl;
			fout <<"            }" << std::endl;
			fout <<"            geometry IndexedFaceSet {" << std::endl;
			fout <<"                ccw TRUE" << std::endl;
			fout <<"                solid TRUE" << std::endl;
			fout <<"                convex TRUE" << std::endl;
			if (GetNVertices() > 0) {
				fout <<"                coord DEF co Coordinate {" << std::endl;
				fout <<"                    point [" << std::endl;
				for(size_t v = 0; v < nV; v++)
				{
					TMMVertex & currentVertex = m_vertices.GetData();
					fout <<"                        " << currentVertex.m_pos.X() << " " 
							<< currentVertex.m_pos.Y() << " " 
							<< currentVertex.m_pos.Z() << "," << std::endl;
					currentVertex.m_id = v;
					m_vertices.Next();
				}
				fout <<"                    ]" << std::endl;
				fout <<"                }" << std::endl;
			}
			if (GetNTriangles() > 0) {
				fout <<"                coordIndex [ " << std::endl;
				for(size_t f = 0; f < nT; f++)
				{
					TMMTriangle & currentTriangle = m_triangles.GetData();
					fout <<"                        " << currentTriangle.m_vertices[0]->GetData().m_id << ", " 
							<< currentTriangle.m_vertices[1]->GetData().m_id << ", "                                                  
							<< currentTriangle.m_vertices[2]->GetData().m_id << ", -1," << std::endl;
					m_triangles.Next();
				}
				fout <<"                ]" << std::endl;
			}
			fout <<"            }" << std::endl;
			fout <<"        }" << std::endl;
			fout <<"    ]" << std::endl;
			fout <<"}" << std::endl;    
		}
		return true;
	}
	void TMMesh::GetIFS(Vec3<Real> * const points, Vec3<long> * const triangles)
	{
		size_t nV = m_vertices.GetSize();
		size_t nT = m_triangles.GetSize(); 
		
		for(size_t v = 0; v < nV; v++)
		{
			points[v] = m_vertices.GetData().m_pos;
			m_vertices.GetData().m_id = v;
			m_vertices.Next();
		}
		for(size_t f = 0; f < nT; f++)
		{
			TMMTriangle & currentTriangle = m_triangles.GetData();
			triangles[f].X() = static_cast<long>(currentTriangle.m_vertices[0]->GetData().m_id);
			triangles[f].Y() = static_cast<long>(currentTriangle.m_vertices[1]->GetData().m_id);
			triangles[f].Z() = static_cast<long>(currentTriangle.m_vertices[2]->GetData().m_id);
			m_triangles.Next();
		}
	}
	void TMMesh::Clear()
	{
		m_vertices.Clear();
		m_edges.Clear();
		m_triangles.Clear();
	}
	void TMMesh::Copy(TMMesh & mesh)
	{
		Clear();
		// updating the id's
		size_t nV = mesh.m_vertices.GetSize();
		size_t nE = mesh. m_edges.GetSize();
		size_t nT = mesh.m_triangles.GetSize();
		for(size_t v = 0; v < nV; v++)
		{
			mesh.m_vertices.GetData().m_id = v;
			mesh.m_vertices.Next();            
		}
		for(size_t e = 0; e < nE; e++)
		{
			mesh.m_edges.GetData().m_id = e;
			mesh.m_edges.Next();
			
		}        
		for(size_t f = 0; f < nT; f++)
		{
			mesh.m_triangles.GetData().m_id = f;
			mesh.m_triangles.Next();
		}
		// copying data
		m_vertices  = mesh.m_vertices;
		m_edges     = mesh.m_edges;
		m_triangles = mesh.m_triangles;
		
		// generating mapping
		CircularListElement<TMMVertex> ** vertexMap     = new CircularListElement<TMMVertex> * [nV];
		CircularListElement<TMMEdge> ** edgeMap         = new CircularListElement<TMMEdge> * [nE];
		CircularListElement<TMMTriangle> ** triangleMap = new CircularListElement<TMMTriangle> * [nT];
		for(size_t v = 0; v < nV; v++)
		{
			vertexMap[v] = m_vertices.GetHead();
			m_vertices.Next();            
		}
		for(size_t e = 0; e < nE; e++)
		{
			edgeMap[e] = m_edges.GetHead();
			m_edges.Next();            
		}        
		for(size_t f = 0; f < nT; f++)
		{
			triangleMap[f] = m_triangles.GetHead();
			m_triangles.Next();
		}
		
		// updating pointers
		for(size_t v = 0; v < nV; v++)
		{
			if (vertexMap[v]->GetData().m_duplicate)
			{
				vertexMap[v]->GetData().m_duplicate = edgeMap[vertexMap[v]->GetData().m_duplicate->GetData().m_id];
			}
		}
		for(size_t e = 0; e < nE; e++)
		{
			if (edgeMap[e]->GetData().m_newFace)
			{
				edgeMap[e]->GetData().m_newFace = triangleMap[edgeMap[e]->GetData().m_newFace->GetData().m_id];
			}
			if (nT > 0)
			{
				for(int f = 0; f < 2; f++)
				{
					if (edgeMap[e]->GetData().m_triangles[f])
					{
						edgeMap[e]->GetData().m_triangles[f] = triangleMap[edgeMap[e]->GetData().m_triangles[f]->GetData().m_id];
					}
				}            
			}
			for(int v = 0; v < 2; v++)
			{
				if (edgeMap[e]->GetData().m_vertices[v])
				{
					edgeMap[e]->GetData().m_vertices[v] = vertexMap[edgeMap[e]->GetData().m_vertices[v]->GetData().m_id];
				}
			}
		}        
		for(size_t f = 0; f < nT; f++)
		{
			if (nE > 0)
			{
				for(int e = 0; e < 3; e++)
				{
					if (triangleMap[f]->GetData().m_edges[e])
					{
						triangleMap[f]->GetData().m_edges[e] = edgeMap[triangleMap[f]->GetData().m_edges[e]->GetData().m_id];
					}
				}            
			}
			for(int v = 0; v < 3; v++)
			{
				if (triangleMap[f]->GetData().m_vertices[v])
				{
					triangleMap[f]->GetData().m_vertices[v] = vertexMap[triangleMap[f]->GetData().m_vertices[v]->GetData().m_id];
				}
			}
		}
		delete [] vertexMap;
		delete [] edgeMap;
		delete [] triangleMap;
		
	}
	long  IntersectRayTriangle(const Vec3<double> & P0, const Vec3<double> & dir, 
	                           const Vec3<double> & V0, const Vec3<double> & V1, 
	                           const Vec3<double> & V2, double &t)
	{
		Vec3<double> edge1, edge2, edge3;
		double det, invDet;
		edge1 = V1 - V2;
		edge2 = V2 - V0;
		Vec3<double> pvec = dir ^ edge2;
		det = edge1 * pvec;
		if (det == 0.0)
			return 0;
		invDet = 1.0/det;
		Vec3<double> tvec = P0 - V0;
		Vec3<double> qvec = tvec ^ edge1;
		t = (edge2 * qvec) * invDet;
		if (t < 0.0)
		{
			return 0;
		}
		edge3 = V0 - V1;
		Vec3<double> I(P0 + t * dir);
		Vec3<double> s0 = (I-V0) ^ edge3;
		Vec3<double> s1 = (I-V1) ^ edge1;
		Vec3<double> s2 = (I-V2) ^ edge2;
		if (s0*s1 > -1e-9 && s2*s1 > -1e-9)
		{
			return 1;
		}
		return 0;
	}
	
	bool IntersectLineLine(const Vec3<double> & p1, const Vec3<double> & p2, 
	                       const Vec3<double> & p3, const Vec3<double> & p4,
	                       Vec3<double> & pa, Vec3<double> & pb, 
	                       double & mua, double & mub)
	{
		Vec3<double> p13,p43,p21;
		double d1343,d4321,d1321,d4343,d2121;
		double numer,denom;
		
		p13.X() = p1.X() - p3.X();
		p13.Y() = p1.Y() - p3.Y();
		p13.Z() = p1.Z() - p3.Z();
		p43.X() = p4.X() - p3.X();
		p43.Y() = p4.Y() - p3.Y();
		p43.Z() = p4.Z() - p3.Z();
		if (p43.X()==0.0 && p43.Y()==0.0 && p43.Z()==0.0)
			return false;
		p21.X() = p2.X() - p1.X();
		p21.Y() = p2.Y() - p1.Y();
		p21.Z() = p2.Z() - p1.Z();
		if (p21.X()==0.0 && p21.Y()==0.0 && p21.Z()==0.0)
			return false;
		
		d1343 = p13.X() * p43.X() + p13.Y() * p43.Y() + p13.Z() * p43.Z();
		d4321 = p43.X() * p21.X() + p43.Y() * p21.Y() + p43.Z() * p21.Z();
		d1321 = p13.X() * p21.X() + p13.Y() * p21.Y() + p13.Z() * p21.Z();
		d4343 = p43.X() * p43.X() + p43.Y() * p43.Y() + p43.Z() * p43.Z();
		d2121 = p21.X() * p21.X() + p21.Y() * p21.Y() + p21.Z() * p21.Z();
		
		denom = d2121 * d4343 - d4321 * d4321;
		if (denom==0.0)
			return false;
		numer = d1343 * d4321 - d1321 * d4343;
		
		mua = numer / denom;
		mub = (d1343 + d4321 * (mua)) / d4343;
		
		pa.X() = p1.X() + mua * p21.X();
		pa.Y() = p1.Y() + mua * p21.Y();
		pa.Z() = p1.Z() + mua * p21.Z();
		pb.X() = p3.X() + mub * p43.X();
		pb.Y() = p3.Y() + mub * p43.Y();
		pb.Z() = p3.Z() + mub * p43.Z();
		
		return true;
	}
	
	long  IntersectRayTriangle2(const Vec3<double> & P0, const Vec3<double> & dir, 
	                            const Vec3<double> & V0, const Vec3<double> & V1, 
	                            const Vec3<double> & V2, double &r)
	{
		Vec3<double> u, v, n;          // triangle vectors
		Vec3<double> w0, w;          // ray vectors
		double     a, b;             // params to calc ray-plane intersect
		
		// get triangle edge vectors and plane normal
		u = V1 - V0;
		v = V2 - V0;
		n = u ^ v;             // cross product
		if (n.GetNorm() == 0.0)            // triangle is degenerate
			return -1;                 // do not deal with this case
		
		w0 = P0 - V0;
		a = - n * w0;
		b =   n * dir;
		if (fabs(b) <= 0.0) {     // ray is parallel to triangle plane
			if (a == 0.0)                // ray lies in triangle plane
				return 2;
			else return 0;             // ray disjoint from plane
		}
		
		// get intersect point of ray with triangle plane
		r = a / b;
		if (r < 0.0)                   // ray goes away from triangle
			return 0;                  // => no intersect
		// for a segment, also test if (r > 1.0) => no intersect
		
		Vec3<double> I = P0 + r * dir;           // intersect point of ray and plane
		
		// is I inside T?
		double uu, uv, vv, wu, wv, D;
		uu = u * u;
		uv = u * v;
		vv = v * v;
		w  = I - V0;
		wu = w * u;
		wv = w * v;
		D = uv * uv - uu * vv;
		
		// get and test parametric coords
		double s, t;
		s = (uv * wv - vv * wu) / D;
		if (s < 0.0 || s > 1.0)        // I is outside T
			return 0;
		t = (uv * wu - uu * wv) / D;
		if (t < 0.0 || (s + t) > 1.0)  // I is outside T
			return 0;
		return 1;                      // I is in T
	}
	
	
	bool TMMesh::CheckConsistancy()
	{
		size_t nE = m_edges.GetSize();
		size_t nT = m_triangles.GetSize();
		for(size_t e = 0; e < nE; e++)
		{
			for(int f = 0; f < 2; f++)
			{
				if (!m_edges.GetHead()->GetData().m_triangles[f])
				{
					return false;
				}
			}            
			m_edges.Next();
		}        
		
		for(size_t f = 0; f < nT; f++)
		{
			for(int e = 0; e < 3; e++)
			{
				int found = 0;
				for(int k = 0; k < 2; k++)
				{
					if (m_triangles.GetHead()->GetData().m_edges[e]->GetData().m_triangles[k] == m_triangles.GetHead())
					{
						found++;
					}
				}
				if (found != 1)
				{
					return false;
				}
			}            
			m_triangles.Next();
		}
		
		return true;
	}
	bool TMMesh::Normalize()
	{
		size_t nV = m_vertices.GetSize();
		if (nV == 0)
		{
			return false;
		}
		m_barycenter = m_vertices.GetHead()->GetData().m_pos;
		Vec3<Real> min = m_barycenter;
		Vec3<Real> max = m_barycenter;
		Real x, y, z;
		for(size_t v = 1; v < nV; v++)
		{
			m_barycenter +=  m_vertices.GetHead()->GetData().m_pos;
			x = m_vertices.GetHead()->GetData().m_pos.X();
			y = m_vertices.GetHead()->GetData().m_pos.Y();
			z = m_vertices.GetHead()->GetData().m_pos.Z();
			if ( x < min.X()) min.X() = x;
			else if ( x > max.X()) max.X() = x;
			if ( y < min.Y()) min.Y() = y;
			else if ( y > max.Y()) max.Y() = y;
			if ( z < min.Z()) min.Z() = z;
			else if ( z > max.Z()) max.Z() = z;
			m_vertices.Next();
		}
		m_barycenter /= static_cast<Real>(nV);
		m_diag = static_cast<Real>(0.001 * (max-min).GetNorm());
		const Real invDiag = static_cast<Real>(1.0 / m_diag);
		if (m_diag != 0.0)
		{
			for(size_t v = 0; v < nV; v++)
			{
				m_vertices.GetHead()->GetData().m_pos = (m_vertices.GetHead()->GetData().m_pos - m_barycenter) * invDiag;
				m_vertices.Next();
			}
		}
		return true;
	}
	bool TMMesh::Denormalize()
	{
		size_t nV = m_vertices.GetSize();
		if (nV == 0)
		{
			return false;
		}
		if (m_diag != 0.0)
		{
			for(size_t v = 0; v < nV; v++)
			{
				m_vertices.GetHead()->GetData().m_pos = m_vertices.GetHead()->GetData().m_pos * m_diag + m_barycenter;
				m_vertices.Next();
			}
		}
		return false;
	}
	//HULL=============================================================
			const long ICHull::sc_dummyIndex = std::numeric_limits<long>::max();
	ICHull::ICHull(void)
	{
		m_distPoints = 0;
		m_isFlat = false;
		m_dummyVertex = 0;
	}
	bool ICHull::AddPoints(const Vec3<Real> * points, size_t nPoints)
	{
		if (!points)
		{
			return false;
		}
		CircularListElement<TMMVertex> * vertex = NULL;
		for (size_t i = 0; i < nPoints; i++)
		{
			vertex = m_mesh.AddVertex();
			vertex->GetData().m_pos.X() = points[i].X();
			vertex->GetData().m_pos.Y() = points[i].Y();
			vertex->GetData().m_pos.Z() = points[i].Z();
			vertex->GetData().m_name = static_cast<long>(i);
		}     
		return true;
	}
	bool ICHull::AddPoints(std::vector< Vec3<Real> > points)
	{
		CircularListElement<TMMVertex> * vertex = NULL;
		for (size_t i = 0; i < points.size(); i++)
		{
			vertex = m_mesh.AddVertex();
			vertex->GetData().m_pos.X() = points[i].X();
			vertex->GetData().m_pos.Y() = points[i].Y();
			vertex->GetData().m_pos.Z() = points[i].Z();
		}     
		return true;
	}
	
	bool ICHull::AddPoint(const Vec3<Real> & point, long id)
	{
		if (AddPoints(&point, 1))
		{
			m_mesh.m_vertices.GetData().m_name = id;
			return true;
		}
		return false;
	}
	
	ICHullError ICHull::Process()
	{
		unsigned long addedPoints = 0;    
		if (m_mesh.GetNVertices() < 3)
		{
			return ICHullErrorNotEnoughPoints;
		}
		if (m_mesh.GetNVertices() == 3)
		{
			m_isFlat = true;
			CircularListElement<TMMTriangle> * t1 = m_mesh.AddTriangle();
			CircularListElement<TMMTriangle> * t2 = m_mesh.AddTriangle();
			CircularListElement<TMMVertex> * v0 = m_mesh.m_vertices.GetHead();
			CircularListElement<TMMVertex> * v1 = v0->GetNext();
			CircularListElement<TMMVertex> * v2 = v1->GetNext();
			// Compute the normal to the plane
			Vec3<Real> p0 = v0->GetData().m_pos;
			Vec3<Real> p1 = v1->GetData().m_pos;            
			Vec3<Real> p2 = v2->GetData().m_pos;            
			m_normal = (p1-p0) ^ (p2-p0);
			m_normal.Normalize();
			t1->GetData().m_vertices[0] = v0;
			t1->GetData().m_vertices[1] = v1;
			t1->GetData().m_vertices[2] = v2;
			t2->GetData().m_vertices[0] = v1;
			t2->GetData().m_vertices[1] = v2;
			t2->GetData().m_vertices[2] = v2;
			return ICHullErrorOK;
		}
		if (m_isFlat)
		{
			m_mesh.m_edges.Clear();
			m_mesh.m_triangles.Clear();
			m_isFlat = false;
		}
		if (m_mesh.GetNTriangles() == 0) // we have to create the first polyhedron
		{
			ICHullError res = DoubleTriangle();
			if (res != ICHullErrorOK)
			{
				return res;
			}
			else
			{
				addedPoints += 3;
			}
		}
		CircularList<TMMVertex> & vertices = m_mesh.GetVertices();
		// go to the first added and not processed vertex
		while (!(vertices.GetHead()->GetPrev()->GetData().m_tag))
		{
			vertices.Prev();
		}
		while (!vertices.GetData().m_tag) // not processed
		{
			vertices.GetData().m_tag = true;
			if (ProcessPoint())
			{
				addedPoints++;
				CleanUp(addedPoints);
				vertices.Next();
				if (!GetMesh().CheckConsistancy())
				{
					return ICHullErrorInconsistent;
				}
			}
		}
		if (m_isFlat)
		{
			std::vector< CircularListElement<TMMTriangle> * > trianglesToDuplicate;
			size_t nT = m_mesh.GetNTriangles();
			for(size_t f = 0; f < nT; f++)
			{
				TMMTriangle & currentTriangle = m_mesh.m_triangles.GetHead()->GetData();
				if( currentTriangle.m_vertices[0]->GetData().m_name == sc_dummyIndex ||
						currentTriangle.m_vertices[1]->GetData().m_name == sc_dummyIndex ||
						currentTriangle.m_vertices[2]->GetData().m_name == sc_dummyIndex )
				{
					m_trianglesToDelete.push_back(m_mesh.m_triangles.GetHead());
					for(int k = 0; k < 3; k++)
					{
						for(int h = 0; h < 2; h++)
						{
							if (currentTriangle.m_edges[k]->GetData().m_triangles[h] == m_mesh.m_triangles.GetHead())
							{
								currentTriangle.m_edges[k]->GetData().m_triangles[h] = 0;
								break;
							}
						}
					}
				}
				else
				{
					trianglesToDuplicate.push_back(m_mesh.m_triangles.GetHead());
				}
				m_mesh.m_triangles.Next();
			}
			size_t nE = m_mesh.GetNEdges();
			for(size_t e = 0; e < nE; e++)
			{
				TMMEdge & currentEdge = m_mesh.m_edges.GetHead()->GetData();
				if( currentEdge.m_triangles[0] == 0 && currentEdge.m_triangles[1] == 0) 
				{
					m_edgesToDelete.push_back(m_mesh.m_edges.GetHead());
				}
				m_mesh.m_edges.Next();
			}
			m_mesh.m_vertices.Delete(m_dummyVertex);
			m_dummyVertex = 0;
			size_t nV = m_mesh.GetNVertices();
			CircularList<TMMVertex> & vertices = m_mesh.GetVertices();
			for(size_t v = 0; v < nV; ++v)
			{
				vertices.GetData().m_tag = false;
				vertices.Next();
			}
			CleanEdges();
			CleanTriangles();
			CircularListElement<TMMTriangle> * newTriangle;
			for(size_t t = 0; t < trianglesToDuplicate.size(); t++)
			{
				newTriangle = m_mesh.AddTriangle();
				newTriangle->GetData().m_vertices[0] = trianglesToDuplicate[t]->GetData().m_vertices[1];
				newTriangle->GetData().m_vertices[1] = trianglesToDuplicate[t]->GetData().m_vertices[0];
				newTriangle->GetData().m_vertices[2] = trianglesToDuplicate[t]->GetData().m_vertices[2];
			}
		}
		return ICHullErrorOK;
	}
	ICHullError ICHull::Process(unsigned long nPointsCH)
	{
		unsigned long addedPoints = 0;  
		if (nPointsCH < 3 || m_mesh.GetNVertices() < 3)
		{
			return ICHullErrorNotEnoughPoints;
		}
		if (m_mesh.GetNVertices() == 3)
		{
			m_isFlat = true;
			CircularListElement<TMMTriangle> * t1 = m_mesh.AddTriangle();
			CircularListElement<TMMTriangle> * t2 = m_mesh.AddTriangle();
			CircularListElement<TMMVertex> * v0 = m_mesh.m_vertices.GetHead();
			CircularListElement<TMMVertex> * v1 = v0->GetNext();
			CircularListElement<TMMVertex> * v2 = v1->GetNext();
			// Compute the normal to the plane
			Vec3<Real> p0 = v0->GetData().m_pos;
			Vec3<Real> p1 = v1->GetData().m_pos;            
			Vec3<Real> p2 = v2->GetData().m_pos;            
			m_normal = (p1-p0) ^ (p2-p0);
			m_normal.Normalize();
			t1->GetData().m_vertices[0] = v0;
			t1->GetData().m_vertices[1] = v1;
			t1->GetData().m_vertices[2] = v2;
			t2->GetData().m_vertices[0] = v1;
			t2->GetData().m_vertices[1] = v0;
			t2->GetData().m_vertices[2] = v2;
			return ICHullErrorOK;
		}
		
		if (m_isFlat)
		{
			m_mesh.m_triangles.Clear();
			m_mesh.m_edges.Clear();
			m_isFlat = false;
		}
		
		if (m_mesh.GetNTriangles() == 0) // we have to create the first polyhedron
		{
			ICHullError res = DoubleTriangle();
			if (res != ICHullErrorOK)
			{
				return res;
			}
			else
			{
				addedPoints += 3;
			}
		}
		CircularList<TMMVertex> & vertices = m_mesh.GetVertices();
		while (!vertices.GetData().m_tag && addedPoints < nPointsCH) // not processed
		{
			if (!FindMaxVolumePoint())
			{
				break;
			}                  
			vertices.GetData().m_tag = true;                      
			if (ProcessPoint())
			{
				addedPoints++;
				CleanUp(addedPoints);
				if (!GetMesh().CheckConsistancy())
				{
					return ICHullErrorInconsistent;
				}
				vertices.Next();
			}
		}
		// delete remaining points
		while (!vertices.GetData().m_tag)
		{
			vertices.Delete();
		}
		if (m_isFlat)
		{
			std::vector< CircularListElement<TMMTriangle> * > trianglesToDuplicate;
			size_t nT = m_mesh.GetNTriangles();
			for(size_t f = 0; f < nT; f++)
			{
				TMMTriangle & currentTriangle = m_mesh.m_triangles.GetHead()->GetData();
				if( currentTriangle.m_vertices[0]->GetData().m_name == sc_dummyIndex ||
						currentTriangle.m_vertices[1]->GetData().m_name == sc_dummyIndex ||
						currentTriangle.m_vertices[2]->GetData().m_name == sc_dummyIndex )
				{
					m_trianglesToDelete.push_back(m_mesh.m_triangles.GetHead());
					for(int k = 0; k < 3; k++)
					{
						for(int h = 0; h < 2; h++)
						{
							if (currentTriangle.m_edges[k]->GetData().m_triangles[h] == m_mesh.m_triangles.GetHead())
							{
								currentTriangle.m_edges[k]->GetData().m_triangles[h] = 0;
								break;
							}
						}
					}
				}
				else
				{
					trianglesToDuplicate.push_back(m_mesh.m_triangles.GetHead());
				}
				m_mesh.m_triangles.Next();
			}
			size_t nE = m_mesh.GetNEdges();
			for(size_t e = 0; e < nE; e++)
			{
				TMMEdge & currentEdge = m_mesh.m_edges.GetHead()->GetData();
				if( currentEdge.m_triangles[0] == 0 && currentEdge.m_triangles[1] == 0) 
				{
					m_edgesToDelete.push_back(m_mesh.m_edges.GetHead());
				}
				m_mesh.m_edges.Next();
			}
			m_mesh.m_vertices.Delete(m_dummyVertex);
			m_dummyVertex = 0;
			size_t nV = m_mesh.GetNVertices();
			CircularList<TMMVertex> & vertices = m_mesh.GetVertices();
			for(size_t v = 0; v < nV; ++v)
			{
				vertices.GetData().m_tag = false;
				vertices.Next();
			}
			CleanEdges();
			CleanTriangles();
			CircularListElement<TMMTriangle> * newTriangle;
			for(size_t t = 0; t < trianglesToDuplicate.size(); t++)
			{
				newTriangle = m_mesh.AddTriangle();
				newTriangle->GetData().m_vertices[0] = trianglesToDuplicate[t]->GetData().m_vertices[1];
				newTriangle->GetData().m_vertices[1] = trianglesToDuplicate[t]->GetData().m_vertices[0];
				newTriangle->GetData().m_vertices[2] = trianglesToDuplicate[t]->GetData().m_vertices[2];
			}
		}
		return ICHullErrorOK;
	}
	bool ICHull::FindMaxVolumePoint()
	{
		CircularList<TMMVertex> & vertices = m_mesh.GetVertices();
		CircularListElement<TMMVertex> * vMaxVolume = 0;
		CircularListElement<TMMVertex> * vHeadPrev = vertices.GetHead()->GetPrev();
		
		double maxVolume = 0.0;
		double volume = 0.0;
		
		while (!vertices.GetData().m_tag) // not processed
				{
			if (ComputePointVolume(volume, false))
			{
				if ( maxVolume < volume)
				{
					maxVolume = volume;
					vMaxVolume = vertices.GetHead();
				}
				vertices.Next();
			}
				}
		CircularListElement<TMMVertex> * vHead = vHeadPrev->GetNext();
		vertices.GetHead() = vHead;  
		
		if (!vMaxVolume)
		{
			return false;
		}
		
		if (vMaxVolume != vHead)
		{
			Vec3<Real> pos = vHead->GetData().m_pos;
			long id = vHead->GetData().m_name;
			vHead->GetData().m_pos = vMaxVolume->GetData().m_pos;
			vHead->GetData().m_name = vMaxVolume->GetData().m_name;
			vMaxVolume->GetData().m_pos = pos;
			vHead->GetData().m_name = id;
		}
		
		
		return true;
	}
	ICHullError ICHull::DoubleTriangle()
	{
		// find three non colinear points
		m_isFlat = false;
		CircularList<TMMVertex> & vertices = m_mesh.GetVertices();
		CircularListElement<TMMVertex> * v0 = vertices.GetHead();
		while( Colinear(v0->GetData().m_pos, 
		                v0->GetNext()->GetData().m_pos, 
		                v0->GetNext()->GetNext()->GetData().m_pos))
		{
			if ( (v0 = v0->GetNext()) == vertices.GetHead())
			{
				return ICHullErrorCoplanarPoints;
			}
		}
		CircularListElement<TMMVertex> * v1 = v0->GetNext();
		CircularListElement<TMMVertex> * v2 = v1->GetNext();
		// mark points as processed
		v0->GetData().m_tag = v1->GetData().m_tag = v2->GetData().m_tag = true;
		
		// create two triangles
		CircularListElement<TMMTriangle> * f0 = MakeFace(v0, v1, v2, 0);
		MakeFace(v2, v1, v0, f0);
		
		// find a fourth non-coplanar point to form tetrahedron
		CircularListElement<TMMVertex> * v3 = v2->GetNext();
		vertices.GetHead() = v3;
		
		double vol = Volume(v0->GetData().m_pos, v1->GetData().m_pos, v2->GetData().m_pos, v3->GetData().m_pos);
		while (vol == 0.0 && !v3->GetNext()->GetData().m_tag)
		{
			v3 = v3->GetNext();
			vol = Volume(v0->GetData().m_pos, v1->GetData().m_pos, v2->GetData().m_pos, v3->GetData().m_pos);
		}            
		if (vol == 0.0)
		{
			// compute the barycenter
			Vec3<Real> bary(0.0,0.0,0.0);
			CircularListElement<TMMVertex> * vBary = v0;
			do
			{
				bary += vBary->GetData().m_pos;
			}
			while ( (vBary = vBary->GetNext()) != v0);
			bary /= static_cast<Real>(vertices.GetSize());
			
			// Compute the normal to the plane
			Vec3<Real> p0 = v0->GetData().m_pos;
			Vec3<Real> p1 = v1->GetData().m_pos;            
			Vec3<Real> p2 = v2->GetData().m_pos;            
			m_normal = (p1-p0) ^ (p2-p0);
			m_normal.Normalize();
			// add dummy vertex placed at (bary + normal)
			vertices.GetHead() = v2;
			Vec3<Real> newPt = bary + m_normal;
			AddPoint(newPt, sc_dummyIndex); 
			m_dummyVertex = vertices.GetHead();
			m_isFlat = true;
			v3 = v2->GetNext();
			vol = Volume(v0->GetData().m_pos, v1->GetData().m_pos, v2->GetData().m_pos, v3->GetData().m_pos);
			return ICHullErrorOK;
		}
		else if (v3 != vertices.GetHead())
		{
			TMMVertex temp;
			temp.m_name = v3->GetData().m_name;
			temp.m_pos = v3->GetData().m_pos;
			v3->GetData().m_name = vertices.GetHead()->GetData().m_name;
			v3->GetData().m_pos = vertices.GetHead()->GetData().m_pos;
			vertices.GetHead()->GetData().m_name = temp.m_name;
			vertices.GetHead()->GetData().m_pos = temp.m_pos;
		}
		return ICHullErrorOK;
	}
	CircularListElement<TMMTriangle> *    ICHull::MakeFace(CircularListElement<TMMVertex> * v0,  
	                                                       CircularListElement<TMMVertex> * v1,
	                                                       CircularListElement<TMMVertex> * v2,
	                                                       CircularListElement<TMMTriangle> * fold)
	{        
		CircularListElement<TMMEdge> * e0;
		CircularListElement<TMMEdge> * e1;
		CircularListElement<TMMEdge> * e2;
		long index = 0;
		if (!fold) // if first face to be created
		{
			e0 = m_mesh.AddEdge(); // create the three edges
			e1 = m_mesh.AddEdge();
			e2 = m_mesh.AddEdge();            
		}
		else // otherwise re-use existing edges (in reverse order)
		{
			e0 = fold->GetData().m_edges[2];
			e1 = fold->GetData().m_edges[1];
			e2 = fold->GetData().m_edges[0];
			index = 1;
		}
		e0->GetData().m_vertices[0] = v0; e0->GetData().m_vertices[1] = v1;
		e1->GetData().m_vertices[0] = v1; e1->GetData().m_vertices[1] = v2;
		e2->GetData().m_vertices[0] = v2; e2->GetData().m_vertices[1] = v0;
		// create the new face
		CircularListElement<TMMTriangle> * f = m_mesh.AddTriangle();   
		f->GetData().m_edges[0]    = e0; f->GetData().m_edges[1]    = e1; f->GetData().m_edges[2]    = e2;
		f->GetData().m_vertices[0] = v0; f->GetData().m_vertices[1] = v1; f->GetData().m_vertices[2] = v2;     
		// link edges to face f
		e0->GetData().m_triangles[index] = e1->GetData().m_triangles[index] = e2->GetData().m_triangles[index] = f;
		return f;
	}
	CircularListElement<TMMTriangle> * ICHull::MakeConeFace(CircularListElement<TMMEdge> * e, CircularListElement<TMMVertex> * p)
	{
		// create two new edges if they don't already exist
		CircularListElement<TMMEdge> * newEdges[2];
		for(int i = 0; i < 2; ++i)
		{
			if ( !( newEdges[i] = e->GetData().m_vertices[i]->GetData().m_duplicate ) )  
			{ // if the edge doesn't exits add it and mark the vertex as duplicated
				newEdges[i] = m_mesh.AddEdge();
				newEdges[i]->GetData().m_vertices[0] = e->GetData().m_vertices[i];
				newEdges[i]->GetData().m_vertices[1] = p;
				e->GetData().m_vertices[i]->GetData().m_duplicate = newEdges[i];
			}
		}
		// make the new face
		CircularListElement<TMMTriangle> * newFace = m_mesh.AddTriangle();
		newFace->GetData().m_edges[0] = e;
		newFace->GetData().m_edges[1] = newEdges[0];
		newFace->GetData().m_edges[2] = newEdges[1];
		MakeCCW(newFace, e, p);
		for(int i=0; i < 2; ++i)
		{
			for(int j=0; j < 2; ++j)
			{
				if ( ! newEdges[i]->GetData().m_triangles[j] )
				{
					newEdges[i]->GetData().m_triangles[j] = newFace;
					break;
				}
			}
		}
		return newFace;
	}
	bool ICHull::ComputePointVolume(double &totalVolume, bool markVisibleFaces)
	{
		// mark visible faces
		CircularListElement<TMMTriangle> * fHead = m_mesh.GetTriangles().GetHead();
		CircularListElement<TMMTriangle> * f = fHead;
		CircularList<TMMVertex> & vertices = m_mesh.GetVertices();
		CircularListElement<TMMVertex> * vertex0 = vertices.GetHead();
		bool visible = false;
		Vec3<double> pos0 = Vec3<double>(vertex0->GetData().m_pos.X(),
				vertex0->GetData().m_pos.Y(),
				vertex0->GetData().m_pos.Z());
		double vol = 0.0;
		totalVolume = 0.0;
		Vec3<double> ver0, ver1, ver2;
		do 
		{
			ver0.X() = f->GetData().m_vertices[0]->GetData().m_pos.X();
			ver0.Y() = f->GetData().m_vertices[0]->GetData().m_pos.Y();
			ver0.Z() = f->GetData().m_vertices[0]->GetData().m_pos.Z();
			ver1.X() = f->GetData().m_vertices[1]->GetData().m_pos.X();
			ver1.Y() = f->GetData().m_vertices[1]->GetData().m_pos.Y();
			ver1.Z() = f->GetData().m_vertices[1]->GetData().m_pos.Z();
			ver2.X() = f->GetData().m_vertices[2]->GetData().m_pos.X();
			ver2.Y() = f->GetData().m_vertices[2]->GetData().m_pos.Y();
			ver2.Z() = f->GetData().m_vertices[2]->GetData().m_pos.Z();
			vol = Volume(ver0, ver1, ver2, pos0);
			if ( vol < 0.0 )
			{
				vol = fabs(vol);
				totalVolume += vol;
				if (markVisibleFaces)
				{
					f->GetData().m_visible = true;
					m_trianglesToDelete.push_back(f);
				}
				visible = true;
			}
			f = f->GetNext();
		} 
		while (f != fHead);
		
		if (m_trianglesToDelete.size() == m_mesh.m_triangles.GetSize())
		{
			for(size_t i = 0; i < m_trianglesToDelete.size(); i++)
			{
				m_trianglesToDelete[i]->GetData().m_visible = false;
			}
			visible = false;
		}
		// if no faces visible from p then p is inside the hull
		if (!visible && markVisibleFaces)
		{
			vertices.Delete();
			m_trianglesToDelete.clear();
			return false;
		}
		return true;
	}
	bool ICHull::ProcessPoint()
	{
		double totalVolume = 0.0;
		if (!ComputePointVolume(totalVolume, true))
		{
			return false;
		}
		// Mark edges in interior of visible region for deletion.
		// Create a new face based on each border edge
		CircularListElement<TMMVertex> * v0 = m_mesh.GetVertices().GetHead();
		CircularListElement<TMMEdge> * eHead = m_mesh.GetEdges().GetHead();
		CircularListElement<TMMEdge> * e = eHead;    
		CircularListElement<TMMEdge> * tmp = 0;
		long nvisible = 0;
		m_edgesToDelete.clear();
		m_edgesToUpdate.clear();
		do 
		{
			tmp = e->GetNext();
			nvisible = 0;
			for(int k = 0; k < 2; k++)
			{
				if ( e->GetData().m_triangles[k]->GetData().m_visible )
				{
					nvisible++;
				}
			}
			if ( nvisible == 2)
			{
				m_edgesToDelete.push_back(e);
			}
			else if ( nvisible == 1)
			{
				e->GetData().m_newFace = MakeConeFace(e, v0);
				m_edgesToUpdate.push_back(e);
			}
			e = tmp;
		}
		while (e != eHead);        
		return true;
	}
	bool ICHull::MakeCCW(CircularListElement<TMMTriangle> * f,
	                     CircularListElement<TMMEdge> * e, 
	                     CircularListElement<TMMVertex> * v)
	{
		// the visible face adjacent to e
		CircularListElement<TMMTriangle> * fv; 
		if (e->GetData().m_triangles[0]->GetData().m_visible)
		{
			fv = e->GetData().m_triangles[0];
		}
		else
		{
			fv = e->GetData().m_triangles[1];
		}
		
		//  set vertex[0] and vertex[1] to have the same orientation as the corresponding vertices of fv.
		long i;                                 // index of e->m_vertices[0] in fv
		CircularListElement<TMMVertex> * v0 = e->GetData().m_vertices[0];
		CircularListElement<TMMVertex> * v1 = e->GetData().m_vertices[1];
		for(i = 0; fv->GetData().m_vertices[i] !=  v0; i++);
		
		if ( fv->GetData().m_vertices[(i+1) % 3] != e->GetData().m_vertices[1] )
		{
			f->GetData().m_vertices[0] = v1;
			f->GetData().m_vertices[1] = v0;
		}
		else
		{
			f->GetData().m_vertices[0] = v0;
			f->GetData().m_vertices[1] = v1;  
			// swap edges
			CircularListElement<TMMEdge> * tmp = f->GetData().m_edges[0];
			f->GetData().m_edges[0] = f->GetData().m_edges[1];
			f->GetData().m_edges[1] = tmp;
		}
		f->GetData().m_vertices[2] = v;
		return true;
	}
	bool ICHull::CleanUp(unsigned long & addedPoints)
	{
		bool r0 = CleanEdges();
		bool r1 = CleanTriangles();
		bool r2 = CleanVertices(addedPoints);
		return  r0 && r1 && r2;
	}
	bool ICHull::CleanEdges()
	{
		// integrate the new faces into the data structure
		CircularListElement<TMMEdge> * e;
		const std::vector<CircularListElement<TMMEdge> *>::iterator itEndUpdate = m_edgesToUpdate.end();
		for(std::vector<CircularListElement<TMMEdge> *>::iterator it = m_edgesToUpdate.begin(); it != itEndUpdate; ++it)
		{
			e = *it;
			if ( e->GetData().m_newFace )
			{
				if ( e->GetData().m_triangles[0]->GetData().m_visible)
				{
					e->GetData().m_triangles[0] = e->GetData().m_newFace;
				}
				else
				{
					e->GetData().m_triangles[1] = e->GetData().m_newFace;
				}
				e->GetData().m_newFace = 0;
			}           
		}
		// delete edges maked for deletion
		CircularList<TMMEdge> & edges = m_mesh.GetEdges();
		const std::vector<CircularListElement<TMMEdge> *>::iterator itEndDelete = m_edgesToDelete.end();
		for(std::vector<CircularListElement<TMMEdge> *>::iterator it = m_edgesToDelete.begin(); it != itEndDelete; ++it)
		{
			edges.Delete(*it);         
		}
		m_edgesToDelete.clear();
		m_edgesToUpdate.clear();
		return true;
	}
	bool ICHull::CleanTriangles()
	{
		CircularList<TMMTriangle> & triangles = m_mesh.GetTriangles();
		const std::vector<CircularListElement<TMMTriangle> *>::iterator itEndDelete = m_trianglesToDelete.end();
		for(std::vector<CircularListElement<TMMTriangle> *>::iterator it = m_trianglesToDelete.begin(); it != itEndDelete; ++it)
		{
			if (m_distPoints)
			{
				if (m_isFlat)
				{
					// to be updated
				}
				else
				{
					std::set<long>::const_iterator itPEnd((*it)->GetData().m_incidentPoints.end());
					std::set<long>::const_iterator itP((*it)->GetData().m_incidentPoints.begin());
					std::map<long, DPoint>::iterator itPoint;
					for(; itP != itPEnd; ++itP) 
					{
						itPoint = m_distPoints->find(*itP);
						if (itPoint != m_distPoints->end())
						{
							itPoint->second.m_computed = false;
						}
					}
				}
			}
			triangles.Delete(*it);         
		}
		m_trianglesToDelete.clear();
		return true;
	}
	bool ICHull::CleanVertices(unsigned long & addedPoints)
	{
		// mark all vertices incident to some undeleted edge as on the hull
		CircularList<TMMEdge> & edges = m_mesh.GetEdges();
		CircularListElement<TMMEdge> * e = edges.GetHead();
		size_t nE = edges.GetSize();
		for(size_t i = 0; i < nE; i++)
		{
			e->GetData().m_vertices[0]->GetData().m_onHull = true;
			e->GetData().m_vertices[1]->GetData().m_onHull = true;
			e = e->GetNext();
		}
		// delete all the vertices that have been processed but are not on the hull
		CircularList<TMMVertex> & vertices = m_mesh.GetVertices();
		CircularListElement<TMMVertex> * vHead = vertices.GetHead();
		CircularListElement<TMMVertex> * v = vHead;
		v = v->GetPrev();
		do 
		{
			if (v->GetData().m_tag && !v->GetData().m_onHull)
			{
				CircularListElement<TMMVertex> * tmp = v->GetPrev();
				vertices.Delete(v);
				v = tmp;
				addedPoints--;
			}
			else
			{
				v->GetData().m_duplicate = 0;
				v->GetData().m_onHull = false;
				v = v->GetPrev();
			}
		} 
		while (v->GetData().m_tag && v != vHead);
		return true;
	}
	void ICHull::Clear()
	{    
		m_mesh.Clear();
		m_edgesToDelete = std::vector<CircularListElement<TMMEdge> *>();
		m_edgesToUpdate = std::vector<CircularListElement<TMMEdge> *>();
		m_trianglesToDelete= std::vector<CircularListElement<TMMTriangle> *>();
		m_isFlat = false;
	}
	const ICHull & ICHull::operator=(ICHull & rhs)
	{
		if (&rhs != this)
		{
			m_mesh.Copy(rhs.m_mesh);
			m_edgesToDelete = rhs.m_edgesToDelete;
			m_edgesToUpdate = rhs.m_edgesToUpdate;
			m_trianglesToDelete = rhs.m_trianglesToDelete;
			m_isFlat = rhs.m_isFlat;
		}
		return (*this);
	}   
	double ICHull::ComputeVolume()
	{
		size_t nV = m_mesh.m_vertices.GetSize();
		if (nV == 0 || m_isFlat)
		{
			return 0.0;
		}       
		Vec3<double> bary(0.0, 0.0, 0.0);
		for(size_t v = 0; v < nV; v++)
		{
			bary.X() +=  m_mesh.m_vertices.GetHead()->GetData().m_pos.X();
			bary.Y() +=  m_mesh.m_vertices.GetHead()->GetData().m_pos.Y();
			bary.Z() +=  m_mesh.m_vertices.GetHead()->GetData().m_pos.Z();
			m_mesh.m_vertices.Next();
		}
		bary /= static_cast<double>(nV);
		
		size_t nT = m_mesh.m_triangles.GetSize();
		Vec3<double> ver0, ver1, ver2;
		double totalVolume = 0.0;
		for(size_t t = 0; t < nT; t++)
		{
			ver0.X() = m_mesh.m_triangles.GetHead()->GetData().m_vertices[0]->GetData().m_pos.X();
			ver0.Y() = m_mesh.m_triangles.GetHead()->GetData().m_vertices[0]->GetData().m_pos.Y();
			ver0.Z() = m_mesh.m_triangles.GetHead()->GetData().m_vertices[0]->GetData().m_pos.Z();
			ver1.X() = m_mesh.m_triangles.GetHead()->GetData().m_vertices[1]->GetData().m_pos.X();
			ver1.Y() = m_mesh.m_triangles.GetHead()->GetData().m_vertices[1]->GetData().m_pos.Y();
			ver1.Z() = m_mesh.m_triangles.GetHead()->GetData().m_vertices[1]->GetData().m_pos.Z();
			ver2.X() = m_mesh.m_triangles.GetHead()->GetData().m_vertices[2]->GetData().m_pos.X();
			ver2.Y() = m_mesh.m_triangles.GetHead()->GetData().m_vertices[2]->GetData().m_pos.Y();
			ver2.Z() = m_mesh.m_triangles.GetHead()->GetData().m_vertices[2]->GetData().m_pos.Z();
			totalVolume += Volume(ver0, ver1, ver2, bary);
			m_mesh.m_triangles.Next();
		}
		return totalVolume;
	}
	bool ICHull::IsInside(const Vec3<Real> & pt0)
	{
		const Vec3<double> pt(pt0.X(), pt0.Y(), pt0.Z());
		if (m_isFlat)
		{
			size_t nT = m_mesh.m_triangles.GetSize();
			Vec3<double> ver0, ver1, ver2, a, b, c;
			double u,v;
			for(size_t t = 0; t < nT; t++)
			{
				ver0.X() = m_mesh.m_triangles.GetHead()->GetData().m_vertices[0]->GetData().m_pos.X();
				ver0.Y() = m_mesh.m_triangles.GetHead()->GetData().m_vertices[0]->GetData().m_pos.Y();
				ver0.Z() = m_mesh.m_triangles.GetHead()->GetData().m_vertices[0]->GetData().m_pos.Z();
				ver1.X() = m_mesh.m_triangles.GetHead()->GetData().m_vertices[1]->GetData().m_pos.X();
				ver1.Y() = m_mesh.m_triangles.GetHead()->GetData().m_vertices[1]->GetData().m_pos.Y();
				ver1.Z() = m_mesh.m_triangles.GetHead()->GetData().m_vertices[1]->GetData().m_pos.Z();
				ver2.X() = m_mesh.m_triangles.GetHead()->GetData().m_vertices[2]->GetData().m_pos.X();
				ver2.Y() = m_mesh.m_triangles.GetHead()->GetData().m_vertices[2]->GetData().m_pos.Y();
				ver2.Z() = m_mesh.m_triangles.GetHead()->GetData().m_vertices[2]->GetData().m_pos.Z();
				a = ver1 - ver0;
				b = ver2 - ver0;
				c = pt - ver0;
				u = c * a;
				v = c * b;
				if ( u >= 0.0 && u <= 1.0 && v >= 0.0 && u+v <= 1.0)
				{
					return true;
				}
				m_mesh.m_triangles.Next();
			}
			return false;
		}
		else
		{
			size_t nT = m_mesh.m_triangles.GetSize();
			Vec3<double> ver0, ver1, ver2;
			for(size_t t = 0; t < nT; t++)
			{
				ver0.X() = m_mesh.m_triangles.GetHead()->GetData().m_vertices[0]->GetData().m_pos.X();
				ver0.Y() = m_mesh.m_triangles.GetHead()->GetData().m_vertices[0]->GetData().m_pos.Y();
				ver0.Z() = m_mesh.m_triangles.GetHead()->GetData().m_vertices[0]->GetData().m_pos.Z();
				ver1.X() = m_mesh.m_triangles.GetHead()->GetData().m_vertices[1]->GetData().m_pos.X();
				ver1.Y() = m_mesh.m_triangles.GetHead()->GetData().m_vertices[1]->GetData().m_pos.Y();
				ver1.Z() = m_mesh.m_triangles.GetHead()->GetData().m_vertices[1]->GetData().m_pos.Z();
				ver2.X() = m_mesh.m_triangles.GetHead()->GetData().m_vertices[2]->GetData().m_pos.X();
				ver2.Y() = m_mesh.m_triangles.GetHead()->GetData().m_vertices[2]->GetData().m_pos.Y();
				ver2.Z() = m_mesh.m_triangles.GetHead()->GetData().m_vertices[2]->GetData().m_pos.Z();
				if (Volume(ver0, ver1, ver2, pt) < 0.0)
				{
					return false;
				}
				m_mesh.m_triangles.Next();
			}
			return true;
		}
	}
	double ICHull::ComputeDistance(long name, const Vec3<Real> & pt, const Vec3<Real> & normal, bool & insideHull, bool updateIncidentPoints)
	{
		Vec3<double> ptNormal(static_cast<double>(normal.X()), 
		                      static_cast<double>(normal.Y()), 
		                      static_cast<double>(normal.Z()));
		Vec3<double> p0( static_cast<double>(pt.X()), 
		                 static_cast<double>(pt.Y()), 
		                 static_cast<double>(pt.Z()));
		
		if (m_isFlat)
		{
			double distance = 0.0;
			Vec3<double> chNormal(static_cast<double>(m_normal.X()), 
			                      static_cast<double>(m_normal.Y()), 
			                      static_cast<double>(m_normal.Z()));
			ptNormal -= (ptNormal * chNormal) * chNormal;
			if (ptNormal.GetNorm() > 0.0)
			{
				ptNormal.Normalize();
				long nameVE1;
				long nameVE2;
				Vec3<double> pa, pb, d0, d1, d2, d3;                
				Vec3<double> p1 = p0 + ptNormal;
				Vec3<double> p2, p3;
				double mua, mub, s;
				const double EPS = 0.00000000001;
				size_t nE = m_mesh.GetNEdges();
				for(size_t e = 0; e < nE; e++)
				{
					TMMEdge & currentEdge = m_mesh.m_edges.GetHead()->GetData();
					nameVE1 = currentEdge.m_vertices[0]->GetData().m_name;
					nameVE2 = currentEdge.m_vertices[1]->GetData().m_name;
					if (currentEdge.m_triangles[0] == 0 || currentEdge.m_triangles[1] == 0)
					{
						if ( nameVE1==name || nameVE2==name )
						{
							return 0.0;
						}
						/*
                        if (debug) std::cout << "V" << name 
                                             << " E "  << nameVE1 << " " << nameVE2 << std::endl;
						 */
						 
						 p2.X() = currentEdge.m_vertices[0]->GetData().m_pos.X();
						 p2.Y() = currentEdge.m_vertices[0]->GetData().m_pos.Y();
						 p2.Z() = currentEdge.m_vertices[0]->GetData().m_pos.Z();
						 p3.X() = currentEdge.m_vertices[1]->GetData().m_pos.X();
						 p3.Y() = currentEdge.m_vertices[1]->GetData().m_pos.Y();
						 p3.Z() = currentEdge.m_vertices[1]->GetData().m_pos.Z();
						 d0 = p3 - p2;
						 if (d0.GetNorm() > 0.0)
						 {
							 if (IntersectLineLine(p0, p1, p2, p3, pa, pb, mua, mub))
							 {
								 d1 = pa - p2;
								 d2 = pa - pb;
								 d3 = pa - p0;
								 mua = d1.GetNorm()/d0.GetNorm();
								 mub = d1*d0;
								 s = d3*ptNormal;
								 if (d2.GetNorm() < EPS &&  mua <= 1.0 && mub>=0.0 && s>0.0)
								 {
									 distance = std::max<double>(distance, d3.GetNorm());
								 }
							 }
						 }
					}
					m_mesh.m_edges.Next();
				}
			}
			return distance;
		}
		else
		{
			Vec3<double> ptNormal(static_cast<double>(normal.X()), 
			                      static_cast<double>(normal.Y()), 
			                      static_cast<double>(normal.Z()));
			
			Vec3<double> impact;
			long nhit;
			double dist;
			double distance = 0.0; 
			size_t nT = m_mesh.GetNTriangles();
			insideHull = false;
			CircularListElement<TMMTriangle> * face = 0;
			Vec3<double> ver0, ver1, ver2;
			for(size_t f = 0; f < nT; f++)
			{
				TMMTriangle & currentTriangle = m_mesh.m_triangles.GetHead()->GetData();
				/*
                if (debug) std::cout << "T " << currentTriangle.m_vertices[0]->GetData().m_name << " "
                                                          << currentTriangle.m_vertices[1]->GetData().m_name << " "
                                                          << currentTriangle.m_vertices[2]->GetData().m_name << std::endl;
				 */
				if (currentTriangle.m_vertices[0]->GetData().m_name == name ||
						currentTriangle.m_vertices[1]->GetData().m_name == name ||
						currentTriangle.m_vertices[2]->GetData().m_name == name)
				{
					nhit = 1;
					dist = 0.0;
				}
				else
				{
					ver0.X() = currentTriangle.m_vertices[0]->GetData().m_pos.X();
					ver0.Y() = currentTriangle.m_vertices[0]->GetData().m_pos.Y();
					ver0.Z() = currentTriangle.m_vertices[0]->GetData().m_pos.Z();
					ver1.X() = currentTriangle.m_vertices[1]->GetData().m_pos.X();
					ver1.Y() = currentTriangle.m_vertices[1]->GetData().m_pos.Y();
					ver1.Z() = currentTriangle.m_vertices[1]->GetData().m_pos.Z();
					ver2.X() = currentTriangle.m_vertices[2]->GetData().m_pos.X();
					ver2.Y() = currentTriangle.m_vertices[2]->GetData().m_pos.Y();
					ver2.Z() = currentTriangle.m_vertices[2]->GetData().m_pos.Z();
					nhit = IntersectRayTriangle(p0, ptNormal, ver0, ver1, ver2, dist);
				}
				
				if (nhit == 1 && distance <= dist)
				{
					distance = dist;
					insideHull = true;
					face = m_mesh.m_triangles.GetHead();    
					/*
                    std::cout << name << " -> T " << currentTriangle.m_vertices[0]->GetData().m_name << " "
                                                  << currentTriangle.m_vertices[1]->GetData().m_name << " "
                                                  << currentTriangle.m_vertices[2]->GetData().m_name << " Dist "
                                                  << dist << " P " << currentTriangle.m_normal * normal << std::endl;
					 */
					if (dist > 0.1)
					{
						break;
					}
				}
				m_mesh.m_triangles.Next();
			}
			if (updateIncidentPoints && face && m_distPoints)
			{
				(*m_distPoints)[name].m_dist = static_cast<Real>(distance);
				face->GetData().m_incidentPoints.insert(name);
			}
			return distance;
		}
	}
	//GRAPH============================================================
	GraphEdge::GraphEdge()
	{
		m_convexHull = 0;
		m_v1 = -1;
		m_v2 = -1;
		m_name = -1;
		m_error = 0;
		m_surf = 0;
		m_perimeter = 0;
		m_concavity = 0;
		m_volume = 0;
		m_deleted = false;
	}
	
	GraphVertex::GraphVertex()
	{
		m_convexHull = 0;
		m_name = -1;
		m_cc = -1;
		m_error = 0;
		m_surf = 0;
		m_perimeter = 0;
		m_concavity = 0;
		m_volume = 0;
		m_deleted = false;
	}
	
	bool GraphVertex::DeleteEdge(long name)
	{
		std::set<long>::iterator it = m_edges.find(name);
		if (it != m_edges.end() )
		{
			m_edges.erase(it);
			return true;
		}
		return false;
	}
	
	Graph::Graph()
	{
		m_nV = 0;
		m_nE = 0;
		m_nCCs = 0;
	}
	
	Graph::~Graph()
	{
	}
	
	void Graph::Allocate(size_t nV, size_t nE)
	{ 
		m_nV = nV;
		m_edges.reserve(nE);
		m_vertices.resize(nV);
		for(size_t i = 0; i < nV; i++)
		{
			m_vertices[i].m_name = static_cast<long>(i);
		}
	}
	
	long Graph::AddVertex()
	{
		size_t name = m_vertices.size();
		m_vertices.resize(name+1);
		m_vertices[name].m_name = static_cast<long>(name);
		m_nV++;
		return static_cast<long>(name);
	}
	
	long Graph::AddEdge(long v1, long v2)
	{
		size_t name = m_edges.size();
		m_edges.push_back(GraphEdge());
		m_edges[name].m_name = static_cast<long>(name);
		m_edges[name].m_v1 = v1;
		m_edges[name].m_v2 = v2;
		m_vertices[v1].AddEdge(static_cast<long>(name));
		m_vertices[v2].AddEdge(static_cast<long>(name));
		m_nE++;
		return static_cast<long>(name);
	}
	
	bool Graph::DeleteEdge(long name)
	{
		if (name < static_cast<long>(m_edges.size()))
		{
			long v1 = m_edges[name].m_v1;
			long v2 = m_edges[name].m_v2;
			m_edges[name].m_deleted = true;
			m_vertices[v1].DeleteEdge(name);
			m_vertices[v2].DeleteEdge(name);
			delete m_edges[name].m_convexHull;
			m_edges[name].m_distPoints.clear();
			m_edges[name].m_boudaryEdges.clear();
			m_edges[name].m_convexHull = 0;
			m_nE--;
			return true;
		}
		return false;
	}
	bool Graph::DeleteVertex(long name)
	{
		if (name < static_cast<long>(m_vertices.size()))
		{
			m_vertices[name].m_deleted = true;
			m_vertices[name].m_edges.clear();
			m_vertices[name].m_ancestors = std::vector<long>();
			delete m_vertices[name].m_convexHull;
			m_vertices[name].m_distPoints.clear();
			m_vertices[name].m_boudaryEdges.clear();
			m_vertices[name].m_convexHull = 0;
			m_nV--;
			return true;
		}
		return false;
	}    
	bool Graph::EdgeCollapse(long v1, long v2)
	{
		long edgeToDelete = GetEdgeID(v1, v2);
		if (edgeToDelete >= 0) 
		{
			// delete the edge (v1, v2)
			DeleteEdge(edgeToDelete);
			// add v2 to v1 ancestors
			m_vertices[v1].m_ancestors.push_back(v2);
			// add v2's ancestors to v1's ancestors
			m_vertices[v1].m_ancestors.insert(m_vertices[v1].m_ancestors.begin(),
			                                  m_vertices[v2].m_ancestors.begin(), 
			                                  m_vertices[v2].m_ancestors.end());
			// update adjacency information
			std::set<long> & v1Edges =  m_vertices[v1].m_edges;
			std::set<long>::const_iterator ed(m_vertices[v2].m_edges.begin());
			std::set<long>::const_iterator itEnd(m_vertices[v2].m_edges.end());
			long b = -1;
			for(; ed != itEnd; ++ed) 
			{
				if (m_edges[*ed].m_v1 == v2)
				{
					b = m_edges[*ed].m_v2;
				}
				else
				{
					b = m_edges[*ed].m_v1;
				}
				if (GetEdgeID(v1, b) >= 0)
				{
					m_edges[*ed].m_deleted = true;
					m_vertices[b].DeleteEdge(*ed);
					m_nE--;
				}
				else
				{
					m_edges[*ed].m_v1 = v1;
					m_edges[*ed].m_v2 = b;
					v1Edges.insert(*ed);
				}
			}
			// delete the vertex v2
			DeleteVertex(v2);            
			return true;
		}
		return false;
	}
	
	long Graph::GetEdgeID(long v1, long v2) const
	{
		if (v1 < static_cast<long>(m_vertices.size()) && !m_vertices[v1].m_deleted)
		{
			std::set<long>::const_iterator ed(m_vertices[v1].m_edges.begin());
			std::set<long>::const_iterator itEnd(m_vertices[v1].m_edges.end());
			for(; ed != itEnd; ++ed) 
			{
				if ( (m_edges[*ed].m_v1 == v2) || 
						(m_edges[*ed].m_v2 == v2)   ) 
				{
					return m_edges[*ed].m_name;
				}
			}
		}
		return -1;
	}
	
	void Graph::Print() const
	{
		std::cout << "-----------------------------" << std::endl;
		std::cout << "vertices (" << m_nV << ")" << std::endl;
		for (size_t v = 0; v < m_vertices.size(); ++v) 
		{
			const GraphVertex & currentVertex = m_vertices[v];
			if (!m_vertices[v].m_deleted)
			{
				
				std::cout  << currentVertex.m_name      << "\t";
				std::set<long>::const_iterator ed(currentVertex.m_edges.begin());
				std::set<long>::const_iterator itEnd(currentVertex.m_edges.end());
				for(; ed != itEnd; ++ed) 
				{
					std::cout  << "(" << m_edges[*ed].m_v1 << "," << m_edges[*ed].m_v2 << ") ";       
				}
				std::cout << std::endl;
			}            
		}
		
		std::cout << "vertices (" << m_nE << ")" << std::endl;
		for (size_t e = 0; e < m_edges.size(); ++e) 
		{
			const GraphEdge & currentEdge = m_edges[e];
			if (!m_edges[e].m_deleted)
			{
				std::cout  << currentEdge.m_name      << "\t(" 
				                                             << m_edges[e].m_v1          << "," 
				                                             << m_edges[e].m_v2          << ") "<< std::endl;
			}            
		}
	}
	void Graph::Clear()
	{
		m_vertices.clear();
		m_edges.clear();
		m_nV = 0;
		m_nE = 0;
	}
	
	long Graph::ExtractCCs()
	{
		// all CCs to -1
		for (size_t v = 0; v < m_vertices.size(); ++v) 
		{
			if (!m_vertices[v].m_deleted)
			{
				m_vertices[v].m_cc = -1;
			}
		}
		
		// we get the CCs
		m_nCCs = 0;
		long v2 = -1;
		std::vector<long> temp;
		for (size_t v = 0; v < m_vertices.size(); ++v) 
		{
			if (!m_vertices[v].m_deleted && m_vertices[v].m_cc == -1) 
			{
				m_vertices[v].m_cc = static_cast<long>(m_nCCs);
				temp.clear();
				temp.push_back(m_vertices[v].m_name);
				while (temp.size()) 
				{
					long vertex = temp[temp.size()-1];
					temp.pop_back();                    
					std::set<long>::const_iterator ed(m_vertices[vertex].m_edges.begin());
					std::set<long>::const_iterator itEnd(m_vertices[vertex].m_edges.end());
					for(; ed != itEnd; ++ed) 
					{
						if (m_edges[*ed].m_v1 == vertex) 
						{
							v2 = m_edges[*ed].m_v2;
						}
						else 
						{
							v2 = m_edges[*ed].m_v1;
						}
						if ( !m_vertices[v2].m_deleted && m_vertices[v2].m_cc == -1) 
						{
							m_vertices[v2].m_cc = static_cast<long>(m_nCCs);
							temp.push_back(v2);
						}
					}
				}
				m_nCCs++;
			}
		}        
		return static_cast<long>(m_nCCs);
	}
	//MAIN=HACD========================================================
	double  HACD::Concavity(ICHull & ch, std::map<long, DPoint> & distPoints)
	{
		double concavity = 0.0;
		double distance = 0.0;       
		std::map<long, DPoint>::iterator itDP(distPoints.begin());
		std::map<long, DPoint>::iterator itDPEnd(distPoints.end());
		for(; itDP != itDPEnd; ++itDP) 
		{
			if (!(itDP->second).m_computed)
			{
				if (itDP->first >= 0)
				{
					distance = ch.ComputeDistance(itDP->first, m_points[itDP->first], m_normals[itDP->first], (itDP->second).m_computed, true);
				}
				else
				{
					distance = ch.ComputeDistance(itDP->first, m_facePoints[-itDP->first-1], m_faceNormals[-itDP->first-1], (itDP->second).m_computed, true);
				}
			}
			else
			{
				distance = (itDP->second).m_dist;
			}
			if (concavity < distance) 
			{
				concavity = distance;
			}
		}
		return concavity;
	}
	
	void HACD::CreateGraph()
	{
		// vertex to triangle adjacency information
		std::vector< std::set<long> >  vertexToTriangles;        
		vertexToTriangles.resize(m_nPoints);
		for(size_t t = 0; t < m_nTriangles; ++t)
		{
			vertexToTriangles[m_triangles[t].X()].insert(static_cast<long>(t));
			vertexToTriangles[m_triangles[t].Y()].insert(static_cast<long>(t));
			vertexToTriangles[m_triangles[t].Z()].insert(static_cast<long>(t));
		}
		
		m_graph.Clear();
		m_graph.Allocate(m_nTriangles, 5 * m_nTriangles);
		unsigned long long tr1[3];
		unsigned long long tr2[3];
		long i1, j1, k1, i2, j2, k2;
		long t1, t2;
		for (size_t v = 0; v < m_nPoints; v++) 
		{
			std::set<long>::const_iterator it1(vertexToTriangles[v].begin()), itEnd(vertexToTriangles[v].end()); 
			for(; it1 != itEnd; ++it1)
			{
				t1 = *it1;
				i1 = m_triangles[t1].X();
				j1 = m_triangles[t1].Y();
				k1 = m_triangles[t1].Z();
				tr1[0] = GetEdgeIndex(i1, j1);
				tr1[1] = GetEdgeIndex(j1, k1);
				tr1[2] = GetEdgeIndex(k1, i1);
				std::set<long>::const_iterator it2(it1);
				for(++it2; it2 != itEnd; ++it2)
				{
					t2 = *it2;
					i2 = m_triangles[t2].X();
					j2 = m_triangles[t2].Y();
					k2 = m_triangles[t2].Z();
					tr2[0] = GetEdgeIndex(i2, j2);
					tr2[1] = GetEdgeIndex(j2, k2);
					tr2[2] = GetEdgeIndex(k2, i2);
					int shared = 0;
					for(int i = 0; i < 3; ++i)
					{
						for(int j = 0; j < 3; ++j)
						{
							if (tr1[i] == tr2[j])    
							{
								shared++;
							}
						}
					}
					if (shared == 1) // two triangles are connected if they share exactly one edge
							{
						m_graph.AddEdge(t1, t2);
							}
				}
			}
		}
		if (m_ccConnectDist >= 0.0)
		{
			m_graph.ExtractCCs();
			if (m_graph.m_nCCs > 1) 
			{
				std::vector< std::set<long> > cc2V;
				cc2V.resize(m_graph.m_nCCs);
				long cc;
				for(size_t t = 0; t < m_nTriangles; ++t)
				{
					cc = m_graph.m_vertices[t].m_cc;
					cc2V[cc].insert(m_triangles[t].X());
					cc2V[cc].insert(m_triangles[t].Y());
					cc2V[cc].insert(m_triangles[t].Z());
				}
				
				for(size_t cc1 = 0; cc1 < m_graph.m_nCCs; ++cc1)
				{
					for(size_t cc2 = cc1+1; cc2 < m_graph.m_nCCs; ++cc2)
					{
						std::set<long>::const_iterator itV1(cc2V[cc1].begin()), itVEnd1(cc2V[cc1].end()); 
						for(; itV1 != itVEnd1; ++itV1)
						{
							double distC1C2 = std::numeric_limits<double>::max();
							double dist;
							t1 = -1;
							t2 = -1;
							std::set<long>::const_iterator itV2(cc2V[cc2].begin()), itVEnd2(cc2V[cc2].end()); 
							for(; itV2 != itVEnd2; ++itV2)
							{
								dist = (m_points[*itV1] - m_points[*itV2]).GetNorm();
								if (dist < distC1C2)
								{
									distC1C2 = dist;
									t1 = *vertexToTriangles[*itV1].begin();
									
									std::set<long>::const_iterator it2(vertexToTriangles[*itV2].begin()), 
											it2End(vertexToTriangles[*itV2].end()); 
									t2 = -1;
									for(; it2 != it2End; ++it2)
									{
										if (*it2 != t1)
										{
											t2 = *it2;
											break;
										}
									}
								}
							}
							if (distC1C2 <= m_ccConnectDist && t1 > 0 && t2 > 0)
							{
								
								m_graph.AddEdge(t1, t2);                    
							}
						}
					}
				}
			}
		}
	}
	void HACD::InitializeDualGraph()
	{
		long i, j, k;
		Vec3<Real> u, v, w, normal;
		delete [] m_normals;
		m_normals = new Vec3<Real>[m_nPoints];
		if (m_addFacesPoints)
		{
			delete [] m_facePoints;
			delete [] m_faceNormals;
			m_facePoints = new Vec3<Real>[m_nTriangles];
			m_faceNormals = new Vec3<Real>[m_nTriangles];
		}
		memset(m_normals, 0, sizeof(Vec3<Real>) * m_nPoints);
		for(unsigned long f = 0; f < m_nTriangles; f++)
		{
			i = m_triangles[f].X();
			j = m_triangles[f].Y();
			k = m_triangles[f].Z();
			
			m_graph.m_vertices[f].m_distPoints[i].m_distOnly = false;
			m_graph.m_vertices[f].m_distPoints[j].m_distOnly = false;
			m_graph.m_vertices[f].m_distPoints[k].m_distOnly = false;
			
			ICHull  * ch = new ICHull;
			m_graph.m_vertices[f].m_convexHull = ch;
			ch->AddPoint(m_points[i], i);
			ch->AddPoint(m_points[j], j);
			ch->AddPoint(m_points[k], k);
			ch->SetDistPoints(&m_graph.m_vertices[f].m_distPoints);
			
			u = m_points[j] - m_points[i];
			v = m_points[k] - m_points[i];
			w = m_points[k] - m_points[j];
			normal = u ^ v;
			
			m_normals[i] += normal;
			m_normals[j] += normal;
			m_normals[k] += normal;
			
			m_graph.m_vertices[f].m_surf = normal.GetNorm();
			m_graph.m_vertices[f].m_perimeter = u.GetNorm() + v.GetNorm() + w.GetNorm();
			
			normal.Normalize();
			
			m_graph.m_vertices[f].m_boudaryEdges.insert(GetEdgeIndex(i,j));
			m_graph.m_vertices[f].m_boudaryEdges.insert(GetEdgeIndex(j,k));
			m_graph.m_vertices[f].m_boudaryEdges.insert(GetEdgeIndex(k,i));
			if(m_addFacesPoints)
			{
				m_faceNormals[f] = normal;
				m_facePoints[f] = (m_points[i] + m_points[j] + m_points[k]) / 3.0;
				m_graph.m_vertices[f].m_distPoints[-static_cast<long>(f)-1].m_distOnly = true;
			}
			if (m_addExtraDistPoints)    
			{// we need a kd-tree structure to accelerate this part!
				long i1, j1, k1;
			Vec3<Real> u1, v1, normal1;
			normal = -normal;
			double distance = 0.0;
			double distMin = 0.0;
			size_t faceIndex = m_nTriangles;
			Vec3<Real> seedPoint((m_points[i] + m_points[j] + m_points[k]) / 3.0);
			long nhit = 0;
			for(size_t f1 = 0; f1 < m_nTriangles; f1++)
			{
				i1 = m_triangles[f1].X();
				j1 = m_triangles[f1].Y();
				k1 = m_triangles[f1].Z();
				u1 = m_points[j1] - m_points[i1];
				v1 = m_points[k1] - m_points[i1];
				normal1 = (u1 ^ v1);
				if (normal * normal1 > 0.0)
				{
					nhit = IntersectRayTriangle(Vec3<double>(seedPoint.X(), seedPoint.Y(), seedPoint.Z()),
					                            Vec3<double>(normal.X(), normal.Y(), normal.Z()),
					                            Vec3<double>(m_points[i1].X(), m_points[i1].Y(), m_points[i1].Z()),
					                            Vec3<double>(m_points[j1].X(), m_points[j1].Y(), m_points[j1].Z()),
					                            Vec3<double>(m_points[k1].X(), m_points[k1].Y(), m_points[k1].Z()),
					                            distance);
					if ((nhit==1) && ((distMin > distance) || (faceIndex == m_nTriangles)))
					{
						distMin = distance;
						faceIndex = f1;
					}
					
				}
			}
			if (faceIndex < m_nTriangles )
			{
				i1 = m_triangles[faceIndex].X();
				j1 = m_triangles[faceIndex].Y();
				k1 = m_triangles[faceIndex].Z();
				m_graph.m_vertices[f].m_distPoints[i1].m_distOnly = true;
				m_graph.m_vertices[f].m_distPoints[j1].m_distOnly = true;
				m_graph.m_vertices[f].m_distPoints[k1].m_distOnly = true;
				if (m_addFacesPoints)
				{
					m_graph.m_vertices[f].m_distPoints[-static_cast<long>(faceIndex)-1].m_distOnly = true;
				}
			}
			}
		}
		for (size_t v = 0; v < m_nPoints; v++) 
		{
			m_normals[v].Normalize();
		}
	}
	
	void HACD::NormalizeData()
	{
		if (m_nPoints == 0)
		{
			return;
		}
		m_barycenter = m_points[0];
		Vec3<Real> min = m_points[0];
		Vec3<Real> max = m_points[0];
		Real x, y, z;
		for (size_t v = 1; v < m_nPoints ; v++) 
		{
			m_barycenter += m_points[v];
			x = m_points[v].X();
			y = m_points[v].Y();
			z = m_points[v].Z();
			if ( x < min.X()) min.X() = x;
			else if ( x > max.X()) max.X() = x;
			if ( y < min.Y()) min.Y() = y;
			else if ( y > max.Y()) max.Y() = y;
			if ( z < min.Z()) min.Z() = z;
			else if ( z > max.Z()) max.Z() = z;
		}
		m_barycenter /= static_cast<Real>(m_nPoints);
		m_diag = (max-min).GetNorm();
		const Real invDiag = static_cast<Real>(2.0 * m_scale / m_diag);
		if (m_diag != 0.0)
		{
			for (size_t v = 0; v < m_nPoints ; v++) 
			{
				m_points[v] = (m_points[v] - m_barycenter) * invDiag;
			}
		}
	}
	void HACD::DenormalizeData()
	{
		if (m_nPoints == 0)
		{
			return;
		}
		if (m_diag != 0.0)
		{
			const Real diag = static_cast<Real>(m_diag / (2.0 * m_scale));
			for (size_t v = 0; v < m_nPoints ; v++) 
			{
				m_points[v] = m_points[v] * diag + m_barycenter;
			}
		}
	}
	HACD::HACD(void)
	{
		m_convexHulls = 0;
		m_triangles = 0;
		m_points = 0;
		m_normals = 0;
		m_nTriangles = 0;
		m_nPoints = 0;
		m_nClusters = 0;
		m_concavity = 0.0;
		m_diag = 1.0;
		m_barycenter = Vec3<Real>(0.0, 0.0,0.0);
		m_alpha = 0.1;
		m_beta = 0.1;
		m_nVerticesPerCH = 30;
		m_callBack = 0;
		m_addExtraDistPoints = false;
		m_addNeighboursDistPoints = false;
		m_scale = 1000.0;
		m_partition = 0;
		m_nMinClusters = 3;
		m_facePoints = 0;
		m_faceNormals = 0;
		m_ccConnectDist = 30;
	}                                                                
	HACD::~HACD(void)
	{
		delete [] m_normals;
		delete [] m_convexHulls;
		delete [] m_partition;
		delete [] m_facePoints;
		delete [] m_faceNormals;
	}
	int iteration = 0;
	void HACD::ComputeEdgeCost(size_t e)
	{
		GraphEdge & gE = m_graph.m_edges[e];
		long v1 = gE.m_v1;
		long v2 = gE.m_v2;
		
		if (m_graph.m_vertices[v2].m_distPoints.size()>m_graph.m_vertices[v1].m_distPoints.size())
		{
			gE.m_v1 = v2;
			gE.m_v2 = v1;
			//std::swap<long>(v1, v2);
			std::swap(v1, v2);
		}
		GraphVertex & gV1 = m_graph.m_vertices[v1];
		GraphVertex & gV2 = m_graph.m_vertices[v2];
		
		// delete old convex-hull
		delete gE.m_convexHull;
		// create the edge's convex-hull
		ICHull  * ch = new ICHull;
		gE.m_convexHull = ch;
		(*ch) = (*gV1.m_convexHull);
		
		// update distPoints
		gE.m_distPoints = gV1.m_distPoints;
		std::map<long, DPoint>::iterator itDP(gV2.m_distPoints.begin());
		std::map<long, DPoint>::iterator itDPEnd(gV2.m_distPoints.end());
		std::map<long, DPoint>::iterator itDP1;
		
		for(; itDP != itDPEnd; ++itDP) 
		{
			itDP1 = gE.m_distPoints.find(itDP->first);
			if (itDP1 == gE.m_distPoints.end())
			{
				gE.m_distPoints[itDP->first].m_distOnly = (itDP->second).m_distOnly;
				if ( !(itDP->second).m_distOnly )
				{
					ch->AddPoint(m_points[itDP->first], itDP->first);
				}
			}
			else
			{
				if ( (itDP1->second).m_distOnly && !(itDP->second).m_distOnly)
				{
					gE.m_distPoints[itDP->first].m_distOnly = false;
					ch->AddPoint(m_points[itDP->first], itDP->first);
				}
			}
		}
		
		ch->SetDistPoints(&gE.m_distPoints);
		// create the convex-hull
		while (ch->Process() == ICHullErrorInconsistent)        // if we face problems when constructing the visual-hull. really ugly!!!!
		{
			//            if (m_callBack) (*m_callBack)("\t Problem with convex-hull construction [HACD::ComputeEdgeCost]\n", 0.0, 0.0, 0);
			ch = new ICHull;
			CircularList<TMMVertex> & verticesCH = (gE.m_convexHull)->GetMesh().m_vertices;
			size_t nV = verticesCH.GetSize();
			long ptIndex = 0;
			verticesCH.Next();
			for(size_t v = 1; v < nV; ++v)
			{
				ptIndex = verticesCH.GetHead()->GetData().m_name;
				ch->AddPoint(m_points[ptIndex], ptIndex);
				verticesCH.Next();
			}
			delete gE.m_convexHull;
			gE.m_convexHull = ch;
		}
		double volume = 0.0; 
		double concavity = 0.0;
		if (ch->IsFlat())
		{
			bool insideHull;
			std::map<long, DPoint>::iterator itDP(gE.m_distPoints.begin());
			std::map<long, DPoint>::iterator itDPEnd(gE.m_distPoints.end());
			for(; itDP != itDPEnd; ++itDP) 
			{    
				if (itDP->first >= 0)
				{
					concavity = std::max<double>(concavity, ch->ComputeDistance(itDP->first, m_points[itDP->first], m_normals[itDP->first], insideHull, false));
				}
			}
		}
		else
		{
			if (m_addNeighboursDistPoints)
			{  // add distance points from adjacent clusters
				std::set<long> eEdges;
			std::set_union(gV1.m_edges.begin(), 
			               gV1.m_edges.end(), 
			               gV2.m_edges.begin(), 
			               gV2.m_edges.end(),
			               std::inserter( eEdges, eEdges.begin() ) );
			
			std::set<long>::const_iterator ed(eEdges.begin());
			std::set<long>::const_iterator itEnd(eEdges.end());
			long a, b, c;
			for(; ed != itEnd; ++ed) 
			{
				a = m_graph.m_edges[*ed].m_v1;
				b = m_graph.m_edges[*ed].m_v2;
				if ( a != v2 && a != v1)
				{
					c = a;
				}
				else if ( b != v2 && b != v1)
				{
					c = b;
				}
				else
				{
					c = -1;
				}
				if ( c > 0)
				{
					GraphVertex & gVC = m_graph.m_vertices[c];
					std::map<long, DPoint>::iterator itDP(gVC.m_distPoints.begin());
					std::map<long, DPoint>::iterator itDPEnd(gVC.m_distPoints.end());
					std::map<long, DPoint>::iterator itDP1;
					for(; itDP != itDPEnd; ++itDP) 
					{
						itDP1 = gE.m_distPoints.find(itDP->first);
						if (itDP1 == gE.m_distPoints.end())
						{
							if (itDP->first >= 0 && itDP1 == gE.m_distPoints.end() && ch->IsInside(m_points[itDP->first]))
							{
								gE.m_distPoints[itDP->first].m_distOnly = true;
							}
							else if (itDP->first < 0 && ch->IsInside(m_facePoints[-itDP->first-1]))
							{
								gE.m_distPoints[itDP->first].m_distOnly = true;
							}
						}
					}
				}
			}
			}
			concavity = Concavity(*ch, gE.m_distPoints);
		}
		
		// compute boudary edges
		double perimeter = 0.0;
		double surf    = 1.0;
		if (m_alpha > 0.0)
		{
			gE.m_boudaryEdges.clear();
			std::set_symmetric_difference (gV1.m_boudaryEdges.begin(), 
			                               gV1.m_boudaryEdges.end(), 
			                               gV2.m_boudaryEdges.begin(), 
			                               gV2.m_boudaryEdges.end(),
			                               std::inserter( gE.m_boudaryEdges, 
			                                              gE.m_boudaryEdges.begin() ) );
			
			std::set<unsigned long long>::const_iterator itBE(gE.m_boudaryEdges.begin());
			std::set<unsigned long long>::const_iterator itBEEnd(gE.m_boudaryEdges.end());
			for(; itBE != itBEEnd; ++itBE)
			{
				perimeter += (m_points[static_cast<long>((*itBE) >> 32)] - 
						m_points[static_cast<long>((*itBE) & 0xFFFFFFFFULL)]).GetNorm();
			}
			surf    = gV1.m_surf + gV2.m_surf;
		}
		double ratio   = perimeter * perimeter / (4.0 * sc_pi * surf);
		gE.m_volume       = (m_beta == 0.0)?0.0:ch->ComputeVolume()/pow(m_scale, 3.0);                        // cluster's volume
		gE.m_surf      = surf;                          // cluster's area  
		gE.m_perimeter = perimeter;                     // cluster's perimeter
		gE.m_concavity = concavity;                     // cluster's concavity
		gE.m_error     = static_cast<Real>(concavity +  m_alpha * ratio + m_beta * volume);    // cluster's priority
	}
	bool HACD::InitializePriorityQueue()
	{
		m_pqueue.reserve(m_graph.m_nE + 100);
		for (size_t e=0; e < m_graph.m_nE; ++e) 
		{
			ComputeEdgeCost(static_cast<long>(e));
			m_pqueue.push(GraphEdgePriorityQueue(static_cast<long>(e), m_graph.m_edges[e].m_error));
		}
		return true;
	}
	void HACD::Simplify()
	{
		long v1 = -1;
		long v2 = -1;        
		double progressOld = -1.0;
		double progress = 0.0;
		double globalConcavity  = 0.0;     
		char msg[1024];
		double ptgStep = 1.0;
		while ( (globalConcavity < m_concavity) && 
				(m_graph.GetNVertices() > m_nMinClusters) && 
				(m_graph.GetNEdges() > 0)) 
		{
			progress = 100.0-m_graph.GetNVertices() * 100.0 / m_nTriangles;
			if (fabs(progress-progressOld) > ptgStep && m_callBack)
			{
				sprintf(msg, "%3.2f %% V = %lu \t C = %f \t \t \r", progress, static_cast<unsigned long>(m_graph.GetNVertices()), globalConcavity);
				(*m_callBack)(msg, progress, globalConcavity,  m_graph.GetNVertices());
				progressOld = progress;
				if (progress > 99.0)
				{
					ptgStep = 0.01;
				}
				else if (progress > 90.0)
				{
					ptgStep = 0.1;
				}
			}
			
			GraphEdgePriorityQueue currentEdge(0,0.0);
			bool done = false;
			do
			{
				done = false;
				if (m_pqueue.size() == 0)
				{
					done = true;
					break;
				}
				currentEdge = m_pqueue.top();
				m_pqueue.pop();
			}
			while (  m_graph.m_edges[currentEdge.m_name].m_deleted || 
					m_graph.m_edges[currentEdge.m_name].m_error != currentEdge.m_priority);
			
			
			if (m_graph.m_edges[currentEdge.m_name].m_concavity < m_concavity && !done)
			{
				globalConcavity = std::max<double>(globalConcavity ,m_graph.m_edges[currentEdge.m_name].m_concavity);
				v1 = m_graph.m_edges[currentEdge.m_name].m_v1;
				v2 = m_graph.m_edges[currentEdge.m_name].m_v2;    
				// update vertex info
				m_graph.m_vertices[v1].m_error     = m_graph.m_edges[currentEdge.m_name].m_error;
				m_graph.m_vertices[v1].m_surf       = m_graph.m_edges[currentEdge.m_name].m_surf;
				m_graph.m_vertices[v1].m_volume       = m_graph.m_edges[currentEdge.m_name].m_volume;
				m_graph.m_vertices[v1].m_concavity = m_graph.m_edges[currentEdge.m_name].m_concavity;
				m_graph.m_vertices[v1].m_perimeter = m_graph.m_edges[currentEdge.m_name].m_perimeter;
				m_graph.m_vertices[v1].m_distPoints   = m_graph.m_edges[currentEdge.m_name].m_distPoints;
				(*m_graph.m_vertices[v1].m_convexHull) = (*m_graph.m_edges[currentEdge.m_name].m_convexHull);
				(m_graph.m_vertices[v1].m_convexHull)->SetDistPoints(&(m_graph.m_vertices[v1].m_distPoints));
				m_graph.m_vertices[v1].m_boudaryEdges   = m_graph.m_edges[currentEdge.m_name].m_boudaryEdges;
				
				// We apply the optimal ecol
				//                std::cout << "v1 " << v1 << " v2 " << v2 << std::endl;
				m_graph.EdgeCollapse(v1, v2);
				// recompute the adjacent edges costs
				std::set<long>::const_iterator itE(m_graph.m_vertices[v1].m_edges.begin()), 
						itEEnd(m_graph.m_vertices[v1].m_edges.end());
				for(; itE != itEEnd; ++itE)
				{
					size_t e = *itE;
					ComputeEdgeCost(static_cast<long>(e));
					m_pqueue.push(GraphEdgePriorityQueue(static_cast<long>(e), m_graph.m_edges[e].m_error));
				}
			}
			else
			{
				break;
			}
		}
		while (!m_pqueue.empty())
		{
			m_pqueue.pop();
		}
		
		m_cVertices.clear();
		m_nClusters = m_graph.GetNVertices();
		m_cVertices.reserve(m_nClusters);
		for (size_t p=0, v = 0; v != m_graph.m_vertices.size(); ++v) 
		{
			if (!m_graph.m_vertices[v].m_deleted)
			{
				if (m_callBack) 
				{
					char msg[1024];
					sprintf(msg, "\t CH \t %lu \t %lf \t %lf\n", static_cast<unsigned long>(p), m_graph.m_vertices[v].m_concavity, m_graph.m_vertices[v].m_error);
					(*m_callBack)(msg, 0.0, 0.0, m_nClusters);
					p++;
				}
				m_cVertices.push_back(static_cast<long>(v));            
			}
		}
		if (m_callBack)
		{
			sprintf(msg, "# clusters =  %lu \t C = %f\n", static_cast<unsigned long>(m_nClusters), globalConcavity);
			(*m_callBack)(msg, progress, globalConcavity,  m_graph.GetNVertices());
		}
		
	}
	
	bool HACD::Compute(bool fullCH, bool exportDistPoints)
	{
		if ( !m_points || !m_triangles || !m_nPoints || !m_nTriangles)
		{
			return false;
		}
		size_t nV = m_nTriangles;
		if (m_callBack)
		{
			std::ostringstream msg;
			msg << "+ Mesh" << std::endl;
			msg << "\t # vertices                     \t" << m_nPoints << std::endl;
			msg << "\t # triangles                    \t" << m_nTriangles << std::endl;
			msg << "+ Parameters" << std::endl;
			msg << "\t min # of clusters              \t" << m_nMinClusters << std::endl;
			msg << "\t max concavity                  \t" << m_concavity << std::endl;
			msg << "\t compacity weigth               \t" << m_alpha << std::endl;
			msg << "\t volume weigth                  \t" << m_beta << std::endl;
			msg << "\t # vertices per convex-hull     \t" << m_nVerticesPerCH << std::endl;
			msg << "\t scale                          \t" << m_scale << std::endl;
			msg << "\t add extra distance points      \t" << m_addExtraDistPoints << std::endl;
			msg << "\t add neighbours distance points \t" << m_addNeighboursDistPoints << std::endl;
			msg << "\t add face distance points       \t" << m_addFacesPoints << std::endl;
			msg << "\t produce full convex-hulls      \t" << fullCH << std::endl;    
			msg << "\t max. distance to connect CCs   \t" << m_ccConnectDist << std::endl;
			(*m_callBack)(msg.str().c_str(), 0.0, 0.0, nV);
		}
		if (m_callBack) (*m_callBack)("+ Normalizing Data\n", 0.0, 0.0, nV);
		NormalizeData();
		if (m_callBack) (*m_callBack)("+ Creating Graph\n", 0.0, 0.0, nV);
		CreateGraph();
		// Compute the surfaces and perimeters of all the faces
		if (m_callBack) (*m_callBack)("+ Initializing Dual Graph\n", 0.0, 0.0, nV);
		InitializeDualGraph();
		if (m_callBack) (*m_callBack)("+ Initializing Priority Queue\n", 0.0, 0.0, nV);
		InitializePriorityQueue();
		// we simplify the graph        
		if (m_callBack) (*m_callBack)("+ Simplification ...\n", 0.0, 0.0, m_nTriangles);
		Simplify();
		if (m_callBack) (*m_callBack)("+ Denormalizing Data\n", 0.0, 0.0, m_nClusters);
		DenormalizeData();
		if (m_callBack) (*m_callBack)("+ Computing final convex-hulls\n", 0.0, 0.0, m_nClusters);
		delete [] m_convexHulls;
		m_convexHulls = new ICHull[m_nClusters];
		delete [] m_partition;
		m_partition = new long [m_nTriangles];
		for (size_t p = 0; p != m_cVertices.size(); ++p) 
		{
			size_t v = m_cVertices[p];
			m_partition[v] = static_cast<long>(p);
			for(size_t a = 0; a < m_graph.m_vertices[v].m_ancestors.size(); a++)
			{
				m_partition[m_graph.m_vertices[v].m_ancestors[a]] = static_cast<long>(p);
			}
			// compute the convex-hull
			const std::map<long, DPoint> & pointsCH =  m_graph.m_vertices[v].m_distPoints;
			std::map<long, DPoint>::const_iterator itCH(pointsCH.begin());
			std::map<long, DPoint>::const_iterator itCHEnd(pointsCH.end());
			for(; itCH != itCHEnd; ++itCH) 
			{
				if (!(itCH->second).m_distOnly)
				{
					m_convexHulls[p].AddPoint(m_points[itCH->first], itCH->first);
				}
			}
			m_convexHulls[p].SetDistPoints(&m_graph.m_vertices[v].m_distPoints);
			if (fullCH)
			{
				m_convexHulls[p].Process();
			}
			else
			{
				m_convexHulls[p].Process(static_cast<unsigned long>(m_nVerticesPerCH));
			}
			if (exportDistPoints)
			{
				itCH = pointsCH.begin();
				for(; itCH != itCHEnd; ++itCH) 
				{
					if ((itCH->second).m_distOnly)
					{
						if (itCH->first >= 0)
						{
							m_convexHulls[p].AddPoint(m_points[itCH->first], itCH->first);
						}
						else
						{
							m_convexHulls[p].AddPoint(m_facePoints[-itCH->first-1], itCH->first);
						}
					}
				}
			}
		}       
		return true;
	}
	
	size_t HACD::GetNTrianglesCH(size_t numCH) const
	{
		if (numCH >= m_nClusters)
		{
			return 0;
		}
		return m_convexHulls[numCH].GetMesh().GetNTriangles();
	}
	size_t HACD::GetNPointsCH(size_t numCH) const
	{
		if (numCH >= m_nClusters)
		{
			return 0;
		}
		return m_convexHulls[numCH].GetMesh().GetNVertices();
	}
	
	bool HACD::GetCH(size_t numCH, Vec3<Real> * const points, Vec3<long> * const triangles)
	{
		if (numCH >= m_nClusters)
		{
			return false;
		}
		m_convexHulls[numCH].GetMesh().GetIFS(points, triangles);
		return true;
	}
	
	bool HACD::Save(const char * fileName, bool uniColor, long numCluster) const
	{
		std::ofstream fout(fileName);
		if (fout.is_open())
		{
			if (m_callBack)
			{
				char msg[1024];
				sprintf(msg, "Saving %s\n", fileName);
				(*m_callBack)(msg, 0.0, 0.0, m_graph.GetNVertices());
			}
			Material mat;
			if (numCluster < 0)
			{
				for (size_t p = 0; p != m_nClusters; ++p) 
				{
					if (!uniColor)
					{
						mat.m_diffuseColor.X() = mat.m_diffuseColor.Y() = mat.m_diffuseColor.Z() = 0.0;
						while (mat.m_diffuseColor.X() == mat.m_diffuseColor.Y() ||
								mat.m_diffuseColor.Z() == mat.m_diffuseColor.Y() ||
								mat.m_diffuseColor.Z() == mat.m_diffuseColor.X()  )
						{
							mat.m_diffuseColor.X() = (rand()%100) / 100.0;
							mat.m_diffuseColor.Y() = (rand()%100) / 100.0;
							mat.m_diffuseColor.Z() = (rand()%100) / 100.0;
						}
					}
					m_convexHulls[p].GetMesh().SaveVRML2(fout, mat);
				}
			}
			else if (numCluster < static_cast<long>(m_cVertices.size()))
			{
				m_convexHulls[numCluster].GetMesh().SaveVRML2(fout, mat);
			}
			fout.close();
			return true;
		}
		else
		{
			if (m_callBack)
			{
				char msg[1024];
				sprintf(msg, "Error saving %s\n", fileName);
				(*m_callBack)(msg, 0.0, 0.0, m_graph.GetNVertices());
			}
			return false;
		}
	}
} // namespace HACD
