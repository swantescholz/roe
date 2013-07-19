#ifndef ROEHACD_H_
#define ROEHACD_H_

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

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <set>
#include <vector>
#include <queue>
#include <map>

namespace HACD {
	
	//CIRCULAR=LIST=============================================================
	
	//!    CircularListElement class.
	template < typename T > class CircularListElement
	{
	public:
		T &                                     GetData() { return m_data; }
		const T &                               GetData() const { return m_data; }
		CircularListElement<T> * &                GetNext() { return m_next; }
		CircularListElement<T> * &                GetPrev() { return m_prev; }
		const CircularListElement<T> * &        GetNext() const { return m_next; }
		const CircularListElement<T> * &        GetPrev() const { return m_prev; }        
		//!    Constructor
		CircularListElement(const T & data) {m_data = data;}
		CircularListElement(void){}
		//! Destructor
		~CircularListElement(void){}
	private:
		T                                        m_data;
		CircularListElement<T> *                m_next; 
		CircularListElement<T> *                m_prev;
		
		CircularListElement(const CircularListElement & rhs);
	};
	
	
	//!    CircularList class.
	template < typename T > class CircularList
	{
	public:
		CircularListElement<T> *  &             GetHead() { return m_head;}        
		const CircularListElement<T> *          GetHead() const { return m_head;}
		bool                                    IsEmpty() const { return (m_size == 0);}
		size_t                                  GetSize() const { return m_size; }
		const T &                               GetData() const { return m_head->GetData(); }        
		T &                                     GetData() { return m_head->GetData();}
		bool                                    Delete() ;
		bool                                    Delete(CircularListElement<T> * element);
		CircularListElement<T> *                Add(const T * data = 0);
		CircularListElement<T> *                Add(const T & data);
		bool                                    Next();
		bool                                    Prev();
		void                                    Clear() { while(Delete());};
		const CircularList&                        operator=(const CircularList& rhs);
		//!    Constructor                                            
		CircularList()
		{ 
			m_head = 0; 
			m_size = 0;
		}
		CircularList(const CircularList& rhs);
		//! Destructor
		virtual                                    ~CircularList(void) {Clear();};
	private:
		CircularListElement<T> *                m_head;        //!< a pointer to the head of the circular list
		size_t                                    m_size;        //!< number of element in the circular list
		
	};
	
	//INL
	template < typename T > 
	inline bool CircularList<T>::Delete(CircularListElement<T> * element)
	{
		if (!element)
		{
			return false;
		}
		if (m_size > 1)
		{
			CircularListElement<T> * next = element->GetNext();
			CircularListElement<T> * prev = element->GetPrev();
			delete element;
			m_size--;
			if (element == m_head)
			{
				m_head = next;
			}
			next->GetPrev() = prev;
			prev->GetNext() = next;
			return true;
		}
		else if (m_size == 1)
		{
			delete m_head;
			m_size--;
			m_head = 0;
			return true;
		}
		else
		{
			return false;
		}
	}
	
	template < typename T > 
	inline bool CircularList<T>::Delete()
	{
		if (m_size > 1)
		{
			CircularListElement<T> * next = m_head->GetNext();
			CircularListElement<T> * prev = m_head->GetPrev();
			delete m_head;
			m_size--;
			m_head = next;
			next->GetPrev() = prev;
			prev->GetNext() = next;
			return true;
		}
		else if (m_size == 1)
		{
			delete m_head;
			m_size--;
			m_head = 0;
			return true;
		}
		else
		{
			return false;
		}
	}
	template < typename T > 
	inline CircularListElement<T> * CircularList<T>::Add(const T * data)
	{
		if (m_size == 0)
		{
			if (data)
			{
				m_head = new CircularListElement<T>(*data);
			}
			else
			{
				m_head = new CircularListElement<T>();
			}
			m_head->GetNext() = m_head->GetPrev() = m_head;
		}
		else
		{
			CircularListElement<T> * next = m_head->GetNext();
			CircularListElement<T> * element = m_head;
			if (data)
			{
				m_head = new CircularListElement<T>(*data);
			}
			else
			{
				m_head = new CircularListElement<T>;
			}
			m_head->GetNext() = next;
			m_head->GetPrev() = element;
			element->GetNext() = m_head;
			next->GetPrev() = m_head;
		}
		m_size++;
		return m_head;
	}
	template < typename T > 
	inline CircularListElement<T> * CircularList<T>::Add(const T & data)
	{
		const T * pData = &data;
		return Add(pData);
	}
	template < typename T > 
	inline bool CircularList<T>::Next()
	{
		if (m_size == 0)
		{
			return false;
		}
		m_head = m_head->GetNext();
		return true;
	}
	template < typename T > 
	inline bool CircularList<T>::Prev()
	{
		if (m_size == 0)
		{
			return false;
		}
		m_head = m_head->GetPrev();
		return true;
	}
	template < typename T > 
	inline CircularList<T>::CircularList(const CircularList& rhs)
	{
		if (rhs.m_size > 0)
		{
			CircularListElement<T> * current = rhs.m_head;
			do
			{
				current = current->GetNext();
				Add(current->GetData());
			}
			while ( current != rhs.m_head );
		}
	}
	template < typename T > 
	inline const CircularList<T>& CircularList<T>::operator=(const CircularList& rhs)
	{
		if (&rhs != this)
		{
			Clear();
			if (rhs.m_size > 0)
			{
				CircularListElement<T> * current = rhs.m_head;
				do
				{
					current = current->GetNext();
					Add(current->GetData());
				}
				while ( current != rhs.m_head );
			}
		}
		return (*this);
	}
	
	//VECTOR===========================================================
			typedef double Real;
	//!    Vector dim 3.
	template < typename T > class Vec3
	{
	public:
		T &                    X();
		T &                    Y();
		T &                    Z();
		const T    &            X() const;
		const T    &            Y() const;
		const T    &            Z() const;
		void                Normalize();
		T                    GetNorm() const;
		void                operator= (const Vec3 & rhs);
		void                operator+=(const Vec3 & rhs);
		void                operator-=(const Vec3 & rhs);
		void                operator-=(T a);
		void                operator+=(T a);
		void                operator/=(T a);
		void                operator*=(T a);
		Vec3                operator^ (const Vec3 & rhs) const;
		T                    operator* (const Vec3 & rhs) const;
		Vec3                operator+ (const Vec3 & rhs) const;
		Vec3                operator- (const Vec3 & rhs) const;
		Vec3                operator- () const;
		Vec3                operator* (T rhs) const;
		Vec3                operator/ (T rhs) const;
		Vec3();
		Vec3(T a);
		Vec3(T x, T y, T z);
		Vec3(const Vec3 & rhs);
		/*virtual*/            ~Vec3(void);
		
	private:
		T                    m_data[3];
	};
	template<typename T>
	const bool Colinear(const Vec3<T> & a, const Vec3<T> & b, const Vec3<T> & c);
	template<typename T>
	const T Volume(const Vec3<T> & a, const Vec3<T> & b, const Vec3<T> & c, const Vec3<T> & d);
	//INLINE
	template <typename T> 
	inline Vec3<T> operator*(T lhs, const Vec3<T> & rhs)
	{
		return Vec3<T>(lhs * rhs.X(), lhs * rhs.Y(), lhs * rhs.Z());
	}
	template <typename T> 
	inline T & Vec3<T>::X() 
	{
		return m_data[0];
	}
	template <typename T>    
	inline  T &    Vec3<T>::Y() 
	{
		return m_data[1];
	}
	template <typename T>    
	inline  T &    Vec3<T>::Z() 
	{
		return m_data[2];
	}
	template <typename T>    
	inline  const T & Vec3<T>::X() const 
	{
		return m_data[0];
	}
	template <typename T>    
	inline  const T & Vec3<T>::Y() const 
	{
		return m_data[1];
	}
	template <typename T>    
	inline  const T & Vec3<T>::Z() const 
	{
		return m_data[2];
	}
	template <typename T>    
	inline  void Vec3<T>::Normalize()
	{
		T n = sqrt(m_data[0]*m_data[0]+m_data[1]*m_data[1]+m_data[2]*m_data[2]);
		if (n != 0.0) (*this) /= n;
	}
	template <typename T>    
	inline  T Vec3<T>::GetNorm() const 
	{ 
		return sqrt(m_data[0]*m_data[0]+m_data[1]*m_data[1]+m_data[2]*m_data[2]);
	}
	template <typename T>    
	inline  void Vec3<T>::operator= (const Vec3 & rhs)
	{ 
		this->m_data[0] = rhs.m_data[0]; 
		this->m_data[1] = rhs.m_data[1]; 
		this->m_data[2] = rhs.m_data[2]; 
	}
	template <typename T>    
	inline  void Vec3<T>::operator+=(const Vec3 & rhs)
	{ 
		this->m_data[0] += rhs.m_data[0]; 
		this->m_data[1] += rhs.m_data[1]; 
		this->m_data[2] += rhs.m_data[2]; 
	}     
	template <typename T>  
	inline void Vec3<T>::operator-=(const Vec3 & rhs)
	{ 
		this->m_data[0] -= rhs.m_data[0]; 
		this->m_data[1] -= rhs.m_data[1]; 
		this->m_data[2] -= rhs.m_data[2]; 
	}
	template <typename T>  
	inline void Vec3<T>::operator-=(T a)
	{ 
		this->m_data[0] -= a; 
		this->m_data[1] -= a; 
		this->m_data[2] -= a; 
	}
	template <typename T>  
	inline void Vec3<T>::operator+=(T a)
	{ 
		this->m_data[0] += a; 
		this->m_data[1] += a; 
		this->m_data[2] += a; 
	}
	template <typename T>  
	inline void Vec3<T>::operator/=(T a)
	{ 
		this->m_data[0] /= a; 
		this->m_data[1] /= a; 
		this->m_data[2] /= a; 
	}
	template <typename T>  
	inline void Vec3<T>::operator*=(T a)
	{ 
		this->m_data[0] *= a; 
		this->m_data[1] *= a; 
		this->m_data[2] *= a; 
	}  
	template <typename T>    
	inline Vec3<T> Vec3<T>::operator^ (const Vec3<T> & rhs) const
	{
		return Vec3<T>(m_data[1] * rhs.m_data[2] - m_data[2] * rhs.m_data[1],
				m_data[2] * rhs.m_data[0] - m_data[0] * rhs.m_data[2],
				m_data[0] * rhs.m_data[1] - m_data[1] * rhs.m_data[0]);
	}
	template <typename T>
	inline T Vec3<T>::operator*(const Vec3<T> & rhs) const
	{
		return (m_data[0] * rhs.m_data[0] + m_data[1] * rhs.m_data[1] + m_data[2] * rhs.m_data[2]);
	}        
	template <typename T>
	inline Vec3<T> Vec3<T>::operator+(const Vec3<T> & rhs) const
	{
		return Vec3<T>(m_data[0] + rhs.m_data[0],m_data[1] + rhs.m_data[1],m_data[2] + rhs.m_data[2]);
	}
	template <typename T> 
	inline  Vec3<T> Vec3<T>::operator-(const Vec3<T> & rhs) const
	{
		return Vec3<T>(m_data[0] - rhs.m_data[0],m_data[1] - rhs.m_data[1],m_data[2] - rhs.m_data[2]) ;
	}     
	template <typename T> 
	inline  Vec3<T> Vec3<T>::operator-() const
	{
		return Vec3<T>(-m_data[0],-m_data[1],-m_data[2]) ;
	}     
	
	template <typename T> 
	inline Vec3<T> Vec3<T>::operator*(T rhs) const
	{
		return Vec3<T>(rhs * this->m_data[0], rhs * this->m_data[1], rhs * this->m_data[2]);
	}
	template <typename T>
	inline Vec3<T> Vec3<T>::operator/ (T rhs) const
	{
		return Vec3<T>(m_data[0] / rhs, m_data[1] / rhs, m_data[2] / rhs);
	}
	template <typename T>
	inline Vec3<T>::Vec3(T a) 
	{ 
		m_data[0] = m_data[1] = m_data[2] = a; 
	}
	template <typename T>
	inline Vec3<T>::Vec3(T x, T y, T z)
	{
		m_data[0] = x;
		m_data[1] = y;
		m_data[2] = z;
	}
	template <typename T>
	inline Vec3<T>::Vec3(const Vec3 & rhs)
	{        
		m_data[0] = rhs.m_data[0];
		m_data[1] = rhs.m_data[1];
		m_data[2] = rhs.m_data[2];
	}
	template <typename T>
	inline Vec3<T>::~Vec3(void){};
	
	template <typename T>
	inline Vec3<T>::Vec3() {}
	
	template<typename T>
	inline const bool Colinear(const Vec3<T> & a, const Vec3<T> & b, const Vec3<T> & c)
	{
		return  ((c.Z() - a.Z()) * (b.Y() - a.Y()) - (b.Z() - a.Z()) * (c.Y() - a.Y()) == 0.0 /*EPS*/) &&
				((b.Z() - a.Z()) * (c.X() - a.X()) - (b.X() - a.X()) * (c.Z() - a.Z()) == 0.0 /*EPS*/) &&
				((b.X() - a.X()) * (c.Y() - a.Y()) - (b.Y() - a.Y()) * (c.X() - a.X()) == 0.0 /*EPS*/);
	}
	
	template<typename T>
	inline const T Volume(const Vec3<T> & a, const Vec3<T> & b, const Vec3<T> & c, const Vec3<T> & d)
	{
		return (a-d) * ((b-d) ^ (c-d));
	}
	//MANIFOLD=MESH====================================================
	class TMMTriangle;
	class TMMEdge;
	class TMMesh;
	class ICHull;
	class HACD;
	
	class DPoint  
	{
	public:       
		DPoint(Real dist=0, bool computed=false, bool distOnly=false)
		:m_dist(dist),
		 m_computed(computed),
		 m_distOnly(distOnly){};
		~DPoint(){};      
	private:
		Real                                                    m_dist;
		bool                                                    m_computed;
		bool                                                    m_distOnly;
		friend class TMMTriangle;
		friend class TMMesh;
		friend class GraphVertex;
		friend class GraphEdge;
		friend class Graph;
		friend class ICHull;
		friend class HACD;
	};
	
	//! Vertex data structure used in a triangular manifold mesh (TMM).
	class TMMVertex {
	public:
		TMMVertex(void);
		~TMMVertex(void);
		
			private:
		Vec3<Real>                                        m_pos;
		long                                                m_name;
		size_t                                                m_id;
		CircularListElement<TMMEdge> *                        m_duplicate;        // pointer to incident cone edge (or NULL)
		bool                                                m_onHull;
		bool                                                m_tag;
		TMMVertex(const TMMVertex & rhs);
		
		friend class HACD;            
		friend class ICHull;
		friend class TMMesh;
		friend class TMMTriangle;
		friend class TMMEdge;
			};
			
			//!    Edge data structure used in a triangular manifold mesh (TMM).
			class TMMEdge
			{        
			public:
				TMMEdge(void);
				~TMMEdge(void);
			private:
				size_t                                                m_id;
				CircularListElement<TMMTriangle> *                    m_triangles[2];
				CircularListElement<TMMVertex> *                    m_vertices[2];
				CircularListElement<TMMTriangle> *                    m_newFace;
				
				
				TMMEdge(const TMMEdge & rhs);
				
				friend class HACD;
				friend class ICHull;
				friend class TMMTriangle;
				friend class TMMVertex;
				friend class TMMesh;
			};
			
			//!    Triangle data structure used in a triangular manifold mesh (TMM).
			class TMMTriangle
			{
			public:
				TMMTriangle(void);
				~TMMTriangle(void);
			private:
				size_t                          m_id;
				CircularListElement<TMMEdge>   *m_edges[3];
				CircularListElement<TMMVertex> *m_vertices[3];
				std::set<long>                  m_incidentPoints;
				bool                            m_visible;
				
				TMMTriangle(const TMMTriangle & rhs);
				
				friend class HACD;
				friend class ICHull;
				friend class TMMesh;
				friend class TMMVertex;
				friend class TMMEdge;
			};
			
			class Material
			{
			public:    
				Material(void);
				~Material(void){}
				//    private:
				Vec3<double> m_diffuseColor;
				double       m_ambientIntensity;
				Vec3<double> m_specularColor;
				Vec3<double> m_emissiveColor;
				double       m_shininess;
				double       m_transparency;
				
				friend class TMMesh;
				friend class HACD;
			};
			
			//!    triangular manifold mesh data structure.
			class TMMesh
			{
			public:
				
				//! Returns the number of vertices>
				inline size_t                                        GetNVertices() const { return m_vertices.GetSize();}
				//! Returns the number of edges
				inline size_t                                        GetNEdges() const { return m_edges.GetSize();}
				//! Returns the number of triangles
				inline size_t                                        GetNTriangles() const { return m_triangles.GetSize();}
				//! Returns the vertices circular list
				inline const CircularList<TMMVertex> &              GetVertices() const { return m_vertices;}
				//! Returns the edges circular list
				inline const CircularList<TMMEdge> &                GetEdges() const { return m_edges;}
				//! Returns the triangles circular list
				inline const CircularList<TMMTriangle> &            GetTriangles() const { return m_triangles;}
				//! Returns the vertices circular list
				inline CircularList<TMMVertex> &                    GetVertices() { return m_vertices;}
				//! Returns the edges circular list
				inline CircularList<TMMEdge> &                      GetEdges() { return m_edges;}
				//! Returns the triangles circular list
				inline CircularList<TMMTriangle> &                  GetTriangles() { return m_triangles;}               
				//! Add vertex to the mesh
				CircularListElement<TMMVertex> *                    AddVertex() {return m_vertices.Add();}
				//! Add vertex to the mesh
				CircularListElement<TMMEdge> *                        AddEdge() {return m_edges.Add();}
				//! Add vertex to the mesh
				CircularListElement<TMMTriangle> *                    AddTriangle() {return m_triangles.Add();}
				//! Print mesh information 
				void                                                Print();
				//!
				void                                                GetIFS(Vec3<Real> * const points, Vec3<long> * const triangles);
				//! Save mesh 
				bool                                                Save(const char *fileName);        
				//! Save mesh to VRML 2.0 format 
				bool                                                SaveVRML2(std::ofstream &fout);             
				//! Save mesh to VRML 2.0 format 
				bool                                                SaveVRML2(std::ofstream &fout, const Material & material);              
				//!  
				void                                                Clear();
				//!
				void                                                Copy(TMMesh & mesh);
				//!
				bool                                                CheckConsistancy();
				//!
				bool                                                Normalize();
				//!
				bool                                                Denormalize();
				//!    Constructor
				TMMesh(void);
				//! Destructor
				virtual                                                ~TMMesh(void);
				
			private:
				CircularList<TMMVertex>                                m_vertices;
				CircularList<TMMEdge>                                m_edges;
				CircularList<TMMTriangle>                            m_triangles;
				Real                                                m_diag;                        //>! length of the BB diagonal
				Vec3<Real>                                          m_barycenter;                //>! barycenter of the mesh
				
				// not defined
				TMMesh(const TMMesh & rhs);
				friend class ICHull;
				friend class HACD;
			};
			//! IntersectRayTriangle(): intersect a ray with a 3D triangle
			//!    Input:  a ray R, and a triangle T
			//!    Output: *I = intersection point (when it exists)
			//!             0 = disjoint (no intersect)
			//!             1 = intersect in unique point I1
			long                                                        IntersectRayTriangle( const Vec3<double> & P0, const Vec3<double> & dir, 
			                                                                                  const Vec3<double> & V0, const Vec3<double> & V1, 
			                                                                                  const Vec3<double> & V2, double &t);
			
			// intersect_RayTriangle(): intersect a ray with a 3D triangle
			//    Input:  a ray R, and a triangle T
			//    Output: *I = intersection point (when it exists)
			//    Return: -1 = triangle is degenerate (a segment or point)
			//             0 = disjoint (no intersect)
			//             1 = intersect in unique point I1
			//             2 = are in the same plane
			long                                                        IntersectRayTriangle2(const Vec3<double> & P0, const Vec3<double> & dir, 
			                                                                                  const Vec3<double> & V0, const Vec3<double> & V1, 
			                                                                                  const Vec3<double> & V2, double &r);
			
			/*
             Calculate the line segment PaPb that is the shortest route between
             two lines P1P2 and P3P4. Calculate also the values of mua and mub where
             Pa = P1 + mua (P2 - P1)
             Pb = P3 + mub (P4 - P3)
             Return FALSE if no solution exists.
			 */
			bool                                                        IntersectLineLine(const Vec3<double> & p1, const Vec3<double> & p2, 
			                                                                              const Vec3<double> & p3, const Vec3<double> & p4,
			                                                                              Vec3<double> & pa, Vec3<double> & pb, 
			                                                                              double & mua, double &mub);
			//HULL=============================================================
			class DPoint;
			class HACD;
			//!    Incremental Convex Hull algorithm (cf. http://maven.smith.edu/~orourke/books/ftp.html ).
			enum ICHullError
			{
				ICHullErrorOK = 0,
						ICHullErrorCoplanarPoints,
						ICHullErrorNoVolume,
						ICHullErrorInconsistent,
						ICHullErrorNotEnoughPoints
			};
			class ICHull
			{
			public:
				//!
				bool                                                IsFlat() { return m_isFlat;}
				//! 
				std::map<long, DPoint> *                            GetDistPoints() const { return m_distPoints;}
				//!
				void                                                SetDistPoints(std::map<long, DPoint> * distPoints) { m_distPoints = distPoints;}
				//! Returns the computed mesh
				TMMesh &                                            GetMesh() { return m_mesh;}
				//!    Add one point to the convex-hull    
				bool                                                AddPoint(const Vec3<Real> & point) {return AddPoints(&point, 1);}
				//!    Add one point to the convex-hull    
				bool                                                AddPoint(const Vec3<Real> & point, long id);
				//!    Add points to the convex-hull
				bool                                                AddPoints(const Vec3<Real> * points, size_t nPoints);    
				bool                                                AddPoints(std::vector< Vec3<Real> > points);
				//!    
				ICHullError                                         Process();
				//! 
				ICHullError                                         Process(unsigned long nPointsCH);
				//!
				double                                              ComputeVolume();
				//!
				bool                                                IsInside(const Vec3<Real> & pt0);
				//!
				double                                                ComputeDistance(long name, const Vec3<Real> & pt, const Vec3<Real> & normal, bool & insideHull, bool updateIncidentPoints);
				//!
				const ICHull &                                      operator=(ICHull & rhs);        
				
				//!    Constructor
				ICHull(void);
				//! Destructor
				virtual                                             ~ICHull(void) {};
				
			private:
				//!    DoubleTriangle builds the initial double triangle.  It first finds 3 noncollinear points and makes two faces out of them, in opposite order. It then finds a fourth point that is not coplanar with that face.  The vertices are stored in the face structure in counterclockwise order so that the volume between the face and the point is negative. Lastly, the 3 newfaces to the fourth point are constructed and the data structures are cleaned up. 
				ICHullError                                         DoubleTriangle();
				//!    MakeFace creates a new face structure from three vertices (in ccw order).  It returns a pointer to the face.
				CircularListElement<TMMTriangle> *                  MakeFace(CircularListElement<TMMVertex> * v0,  
				                                                             CircularListElement<TMMVertex> * v1,
				                                                             CircularListElement<TMMVertex> * v2,
				                                                             CircularListElement<TMMTriangle> * fold);            
				//!    
				CircularListElement<TMMTriangle> *                  MakeConeFace(CircularListElement<TMMEdge> * e, CircularListElement<TMMVertex> * v);
				//!    
				bool                                                ProcessPoint();
				//!
				bool                                                ComputePointVolume(double &totalVolume, bool markVisibleFaces);
				//!
				bool                                                FindMaxVolumePoint();
				//!    
				bool                                                CleanEdges();
				//!    
				bool                                                CleanVertices(unsigned long & addedPoints);
				//!    
				bool                                                CleanTriangles();
				//!    
				bool                                                CleanUp(unsigned long & addedPoints);
				//!
				bool                                                MakeCCW(CircularListElement<TMMTriangle> * f,
				                                                            CircularListElement<TMMEdge> * e, 
				                                                            CircularListElement<TMMVertex> * v);
				void                                                Clear(); 
			private:
				static const long                                   sc_dummyIndex;
				static const double                                 sc_distMin;
				TMMesh                                              m_mesh;
				std::vector<CircularListElement<TMMEdge> *>         m_edgesToDelete;
				std::vector<CircularListElement<TMMEdge> *>         m_edgesToUpdate;
				std::vector<CircularListElement<TMMTriangle> *>     m_trianglesToDelete; 
				std::map<long, DPoint> *                            m_distPoints;            
				CircularListElement<TMMVertex> *                    m_dummyVertex;
				Vec3<Real>                                          m_normal;
				bool                                                m_isFlat;
				
				
				ICHull(const ICHull & rhs);
				
				friend class HACD;
			};
			//GRAPH============================================================
			class GraphVertex;
			class GraphEdge;
			class Graph;
			class HACD;
			
			class GraphVertex  
			{
			public:
				bool                                    AddEdge(long name) 
				{ 
					m_edges.insert(name); 
					return true; 
				}
				bool                                    DeleteEdge(long name);        
				GraphVertex();
				~GraphVertex(){ delete m_convexHull;};      
			private:
				long                                    m_name;
				long                                    m_cc;
				std::set<long>                          m_edges;
				bool                                    m_deleted;
				std::vector<long>                        m_ancestors;
				std::map<long, DPoint>                    m_distPoints;
				
				Real                                    m_error;
				double                                  m_surf;
				double                                  m_volume;
				double                                  m_perimeter;
				double                                  m_concavity;
				ICHull *                                m_convexHull;
				std::set<unsigned long long>            m_boudaryEdges;
				
				
				friend class GraphEdge;
				friend class Graph;
				friend class HACD;
			};
			
			class GraphEdge 
			{
			public:
				GraphEdge();
				~GraphEdge(){delete m_convexHull;};
			private:
				long                                    m_name;
				long                                    m_v1;
				long                                    m_v2;
				std::map<long, DPoint>                    m_distPoints;
				Real                                    m_error;
				double                                  m_surf;
				double                                  m_volume;
				double                                  m_perimeter;
				double                                  m_concavity;
				ICHull *                                m_convexHull;
				std::set<unsigned long long>            m_boudaryEdges;
				bool                                    m_deleted;
				
				
				
				friend class GraphVertex;
				friend class Graph;
				friend class HACD;
			};
			
			class Graph  
			{
			public:
				size_t                                    GetNEdges() const { return m_nE;}
				size_t                                    GetNVertices() const { return m_nV;}
				bool                                    EdgeCollapse(long v1, long v2);
				long                                    AddVertex();
				long                                    AddEdge(long v1, long v2);
				bool                                    DeleteEdge(long name);    
				bool                                    DeleteVertex(long name);
				long                                    GetEdgeID(long v1, long v2) const;
				void                                    Clear();
				void                                    Print() const;
				long                                    ExtractCCs();
				
				Graph();
				virtual                                 ~Graph();      
				void                                    Allocate(size_t nV, size_t nE);
				
			private:
				size_t                                  m_nCCs;
				size_t                                  m_nV;
				size_t                                  m_nE;
				std::vector<GraphEdge>                  m_edges;
				std::vector<GraphVertex>                m_vertices;
				
				friend class HACD;
			};
			//MAIN=HACD========================================================
					const double                                    sc_pi = 3.14159265;
			class HACD;
			
			// just to be able to set the capcity of the container
			
			template<class _Ty, class _Container = std::vector<_Ty>, class _Pr = std::less<typename _Container::value_type> >
			class reservable_priority_queue: public std::priority_queue<_Ty, _Container, _Pr> 
			{
				typedef typename std::priority_queue<_Ty, _Container, _Pr>::size_type size_type;
			public:
				reservable_priority_queue(size_type capacity = 0) { reserve(capacity); };
				void                                        reserve(size_type capacity) { this->c.reserve(capacity); } 
				size_type                                    capacity() const { return this->c.capacity(); } 
			};
			
			//! priority queque element
			class GraphEdgePriorityQueue
			{
			public:
				//! Constructor
				//! @param name edge's id
				//! @param priority edge's priority
				GraphEdgePriorityQueue(long name, Real priority)
				{
					m_name = name;
					m_priority = priority;
				}
				//! Destructor
				~GraphEdgePriorityQueue(void){}
			private:
				long                                    m_name;                        //!< edge name
				Real                                    m_priority;                    //!< priority
				//! Operator < for GraphEdgePQ
				friend bool                                 operator<(const GraphEdgePriorityQueue & lhs, const GraphEdgePriorityQueue & rhs);
				//! Operator > for GraphEdgePQ
				friend bool                                 operator>(const GraphEdgePriorityQueue & lhs, const GraphEdgePriorityQueue & rhs);
				friend class HACD;
			};
			inline bool                                        operator<(const GraphEdgePriorityQueue & lhs, const GraphEdgePriorityQueue & rhs)
			{
				return lhs.m_priority<rhs.m_priority;
			}
			inline bool                                        operator>(const GraphEdgePriorityQueue & lhs, const GraphEdgePriorityQueue & rhs)
			{
				return lhs.m_priority>rhs.m_priority;
			}
			typedef void (*CallBackFunction)(const char *, double, double, size_t);
			
			//! Provides an implementation of the Hierarchical Approximate Convex Decomposition (HACD) technique described in "A Simple and Efficient Approach for 3D Mesh Approximate Convex Decomposition" Game Programming Gems 8 - Chapter 2.8, p.202. A short version of the chapter was published in ICIP09 and is available at ftp://ftp.elet.polimi.it/users/Stefano.Tubaro/ICIP_USB_Proceedings_v2/pdfs/0003501.pdf
			class HACD
			{            
			public:
				
				//! Gives the triangles partitionas an array of size m_nTriangles where the i-th element specifies the cluster to which belong the i-th triangle
				//! @return triangles partition
				const long * const                            GetPartition() const { return m_partition;}
				//! Sets the scale factor
				//! @param scale scale factor
				void                                        SetScaleFactor(double  scale) { m_scale = scale;}
				//! Gives the scale factor
				//! @return scale factor
				const double                                GetScaleFactor() const { return m_scale;}
				//! Sets the call-back function
				//! @param callBack pointer to the call-back function
				void                                        SetCallBack(CallBackFunction  callBack) { m_callBack = callBack;}
				//! Gives the call-back function
				//! @return pointer to the call-back function
				const CallBackFunction                      GetCallBack() const { return m_callBack;}
				
				//! Specifies whether faces points should be added when computing the concavity
				//! @param addFacesPoints true = faces points should be added
				void                                        SetAddFacesPoints(bool  addFacesPoints) { m_addFacesPoints = addFacesPoints;}
				//! Specifies wheter faces points should be added when computing the concavity
				//! @return true = faces points should be added
				const bool                                    GetAddFacesPoints() const { return m_addFacesPoints;}
				//! Specifies whether extra points should be added when computing the concavity
				//! @param addExteraDistPoints true = extra points should be added
				void                                        SetAddExtraDistPoints(bool  addExtraDistPoints) { m_addExtraDistPoints = addExtraDistPoints;}
				//! Specifies wheter extra points should be added when computing the concavity
				//! @return true = extra points should be added
				const bool                                    GetAddExtraDistPoints() const { return m_addExtraDistPoints;}
				//! Specifies whether extra points should be added when computing the concavity
				//! @param addExteraDistPoints true = extra points should be added
				void                                        SetAddNeighboursDistPoints(bool  addNeighboursDistPoints) { m_addNeighboursDistPoints = addNeighboursDistPoints;}
				//! Specifies wheter extra points should be added when computing the concavity
				//! @return true = extra points should be added
				const bool                                    GetAddNeighboursDistPoints() const { return m_addNeighboursDistPoints;}
				//! Sets the points of the input mesh (Remark: the input points will be scaled and shifted. Use DenormalizeData() to invert those operations)
				//! @param points pointer to the input points
				void                                        SetPoints(Vec3<Real>  * points) { m_points = points;}
				//! Gives the points of the input mesh (Remark: the input points will be scaled and shifted. Use DenormalizeData() to invert those operations)
				//! @return pointer to the input points 
				const Vec3<Real> *                          GetPoints() const { return m_points;}
				//! Sets the triangles of the input mesh.
				//! @param triangles points pointer to the input points
				void                                        SetTriangles(Vec3<long>  * triangles) { m_triangles = triangles;}
				//! Gives the triangles in the input mesh 
				//! @return pointer to the input triangles 
				const Vec3<long>   *                        GetTriangles() const { return m_triangles;}
				//! Sets the number of points in the input mesh.
				//! @param nPoints number of points the input mesh
				void                                        SetNPoints(size_t nPoints) { m_nPoints = nPoints;}
				//! Gives the number of points in the input mesh.
				//! @return number of points the input mesh
				const size_t                                GetNPoints() const { return m_nPoints;}
				//! Sets the number of triangles in the input mesh.
				//! @param nTriangles number of triangles in the input mesh
				void                                        SetNTriangles(size_t nTriangles) { m_nTriangles = nTriangles;}
				//! Gives the number of triangles in the input mesh.
				//! @return number of triangles the input mesh
				const size_t                                GetNTriangles() const { return m_nTriangles;}
				//! Sets the minimum number of clusters to be generated.
				//! @param nClusters minimum number of clusters
				void                                        SetNClusters(size_t nClusters) { m_nMinClusters = nClusters;}
				//! Gives the number of generated clusters.
				//! @return number of generated clusters
				const size_t                                GetNClusters() const { return m_nClusters;}
				//! Sets the maximum allowed concavity.
				//! @param concavity maximum concavity
				void                                        SetConcavity(double concavity) { m_concavity = concavity;}
				//! Gives the maximum allowed concavity.
				//! @return maximum concavity
				double                                      GetConcavity() const { return m_concavity;}
				//! Sets the maximum allowed distance to get CCs connected.
				//! @param concavity maximum distance to get CCs connected
				void                                        SetConnectDist(double ccConnectDist) { m_ccConnectDist = ccConnectDist;}
				//! Gives the maximum allowed distance to get CCs connected.
				//! @return maximum distance to get CCs connected
				double                                      GetConnectDist() const { return m_ccConnectDist;}        
				//! Sets the volume weight.
				//! @param beta volume weight
				void                                        SetVolumeWeight(double beta) { m_beta = beta;}
				//! Gives the volume weight.
				//! @return volume weight
				double                                      GetVolumeWeight() const { return m_beta;}    
				//! Sets the compacity weight (i.e. parameter alpha in ftp://ftp.elet.polimi.it/users/Stefano.Tubaro/ICIP_USB_Proceedings_v2/pdfs/0003501.pdf).
				//! @param alpha compacity weight
				void                                        SetCompacityWeight(double alpha) { m_alpha = alpha;}
				//! Gives the compacity weight (i.e. parameter alpha in ftp://ftp.elet.polimi.it/users/Stefano.Tubaro/ICIP_USB_Proceedings_v2/pdfs/0003501.pdf).
				//! @return compacity weight
				double                                      GetCompacityWeight() const { return m_alpha;}    
				//! Sets the maximum number of vertices for each generated convex-hull.
				//! @param nVerticesPerCH maximum # vertices per CH
				void                                        SetNVerticesPerCH(size_t nVerticesPerCH) { m_nVerticesPerCH = nVerticesPerCH;}
				//! Gives the maximum number of vertices for each generated convex-hull.
				//! @return maximum # vertices per CH
				const size_t                                GetNVerticesPerCH() const { return m_nVerticesPerCH;}
				//! Gives the number of vertices for the cluster number numCH.
				//! @return number of vertices
				size_t                                      GetNPointsCH(size_t numCH) const;
				//! Gives the number of triangles for the cluster number numCH.
				//! @param numCH cluster's number
				//! @return number of triangles
				size_t                                      GetNTrianglesCH(size_t numCH) const;
				//! Gives the vertices and the triangles of the cluster number numCH.
				//! @param numCH cluster's number
				//! @param points pointer to the vector of points to be filled
				//! @param triangles pointer to the vector of triangles to be filled
				//! @return true if sucess
				bool                                        GetCH(size_t numCH, Vec3<Real> * const points, Vec3<long> * const triangles);     
				//! Computes the HACD decomposition.
				//! @param fullCH specifies whether to generate convex-hulls with a full or limited (i.e. < m_nVerticesPerCH) number of vertices
				//! @param exportDistPoints specifies wheter distance points should ne exported or not (used only for debugging).
				//! @return true if sucess
				bool                                        Compute(bool fullCH=false, bool exportDistPoints=false);
				//! Saves the generated convex-hulls in a VRML 2.0 file.
				//! @param fileName the output file name
				//! @param uniColor specifies whether the different convex-hulls should have the same color or not
				//! @param numCluster specifies the cluster to be saved, if numCluster < 0 export all clusters
				//! @return true if sucess
				bool                                        Save(const char * fileName, bool uniColor, long numCluster=-1) const;
				//! Shifts and scales to the data to have all the coordinates between 0.0 and 1000.0.
				void                                        NormalizeData();
				//! Inverse the operations applied by NormalizeData().
				void                                        DenormalizeData();
				//! Constructor.
				HACD(void);
				//! Destructor.
				~HACD(void);
				
			private:
				//! Gives the edge index.
				//! @param a first vertex id
				//! @param b second vertex id
				//! @return edge's index
				static unsigned long long                    GetEdgeIndex(unsigned long long a, unsigned long long b) 
				{ 
					if (a > b) return (a << 32) + b;
					else       return (b << 32) + a;
				}
				//! Computes the concavity of a cluster.
				//! @param ch the cluster's convex-hull
				//! @param distPoints the cluster's points 
				//! @return cluster's concavity
				double                                        Concavity(ICHull & ch, std::map<long, DPoint> & distPoints);
				//! Computes the perimeter of a cluster.
				//! @param triIndices the cluster's triangles
				//! @param distPoints the cluster's points 
				//! @return cluster's perimeter
				double                                        ComputePerimeter(const std::vector<long> & triIndices) const;
				//! Creates the Graph by associating to each mesh triangle a vertex in the graph and to each couple of adjacent triangles an edge in the graph.
				void                                        CreateGraph();    
				//! Initializes the graph costs and computes the vertices normals
				void                                        InitializeDualGraph();
				//! Computes the cost of an edge
				//! @param e edge's id
				void                                        ComputeEdgeCost(size_t e);
				//! Initializes the priority queue
				//! @param fast specifies whether fast mode is used
				//! @return true if success
				bool                                        InitializePriorityQueue();
				//! Cleans the intersection between convex-hulls
				void                                        CleanClusters();
				//! Computes convex-hulls from partition information
				//! @param fullCH specifies whether to generate convex-hulls with a full or limited (i.e. < m_nVerticesPerCH) number of vertices
				void                                        ComputeConvexHulls(bool fullCH);
				//! Simplifies the graph
				//! @param fast specifies whether fast mode is used
				void                                        Simplify();
				
			private:
				double                                        m_scale;                    //>! scale factor used for NormalizeData() and DenormalizeData()
				Vec3<long> *                                m_triangles;                //>! pointer the triangles array
				Vec3<Real> *                                m_points;                    //>! pointer the points array
				Vec3<Real> *                                m_facePoints;               //>! pointer to the faces points array
				Vec3<Real> *                                m_faceNormals;              //>! pointer to the faces normals array
				Vec3<Real> *                                m_normals;                    //>! pointer the normals array
				size_t                                        m_nTriangles;                //>! number of triangles in the original mesh
				size_t                                        m_nPoints;                    //>! number of vertices in the original mesh
				size_t                                        m_nClusters;                //>! number of clusters
				size_t                                        m_nMinClusters;                //>! minimum number of clusters
				double                                        m_ccConnectDist;            //>! maximum allowed distance to connect CCs
				double                                        m_concavity;                //>! maximum concavity
				double                                        m_alpha;                    //>! compacity weigth
				double                                      m_beta;                     //>! volume weigth
				double                                        m_diag;                        //>! length of the BB diagonal
				Vec3<Real>                                  m_barycenter;                //>! barycenter of the mesh
				std::vector< long >                         m_cVertices;                //>! array of vertices each belonging to a different cluster
				ICHull *                                    m_convexHulls;                //>! convex-hulls associated with the final HACD clusters
				Graph                                        m_graph;                    //>! simplification graph
				size_t                                      m_nVerticesPerCH;            //>! maximum number of vertices per convex-hull
				reservable_priority_queue<GraphEdgePriorityQueue, 
				std::vector<GraphEdgePriorityQueue>,
				std::greater<std::vector<GraphEdgePriorityQueue>::value_type> > m_pqueue;        //!> priority queue
				HACD(const HACD & rhs);
				CallBackFunction                            m_callBack;                    //>! call-back function
				long *                                        m_partition;                //>! array of size m_nTriangles where the i-th element specifies the cluster to which belong the i-th triangle
				bool                                        m_addFacesPoints;           //>! specifies whether to add faces points or not
				bool                                        m_addExtraDistPoints;       //>! specifies whether to add extra points for concave shapes or not
				bool                                        m_addNeighboursDistPoints;  //>! specifies whether to add extra points from adjacent clusters or not
				
			};
} // namespace HACD

#endif /* ROEHACD_H_ */
