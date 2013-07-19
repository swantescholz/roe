#ifndef ROENURBS_H_
#define ROENURBS_H_

#include <GL/gl.h>
#include "RoeColor.h"
#include "RoeVector2.h"
#include "RoeVector3.h"
#include "RoeMatrix.h"
#include "RoeObject.h"
#include "RoeVertexBufferObject.h"

namespace roe {

	class Nurbs : public Object{
	public:
		Vector3  **vertices; //read-write; pointer to 2-dimensional array [w][h]
		Vector3 ***tangents; //read-maybe-write; pointer to 3D tangent-array [w][h][right 0 or down 1], boundaries excluded
		
		Nurbs();
		~Nurbs();
		
		void createControlPoints(int nu, int nv = -1,
		            Vector3 A = Vector3( 1,0, 1), Vector3 B = Vector3(-1,0, 1),
		            Vector3 C = Vector3( 1,0,-1), Vector3 D = Vector3(-1,0,-1)); //initialisation
		void calculateTangentsCatmull(float factor = 0.5); //use this or set ***tangents yourself!
		void createMesh(int usteps, int vsteps = -1, bool excludeOuter = true,
		                const Vector2& A = Vector2(0,0), const Vector2& B = Vector2(1,0), const Vector2& C = Vector2(0,-1));
		void destroy();//deletes vectors
		
		void setTextureData(Vector2 top_left    = Vector2(0,1), Vector2 top_right    = Vector2(1,1),
		                    Vector2 bottom_left = Vector2(0,0), Vector2 bottom_right = Vector2(1,0));
		
		void render(); //boundary is discarded
		Vector3 interpolate(float u, float v);
		
	private:
		
		VertexBufferObject m_vbo; //for mesh
		Vector2 *m_texCoords; //all texture coordinates, top-left, top-right, bottom-left, bottom-right
		
		int m_nu, m_nv;
	};
	
} //namespace roe

#endif /* ROENURBS_H_ */
