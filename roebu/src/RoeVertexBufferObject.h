#ifndef ROEVBO_H_
#define ROEVBO_H_

#ifndef GL_GLEXT_PROTOTYPES
	#define GL_GLEXT_PROTOTYPES
#endif
#include <GL/gl.h>
#include <GL/glext.h>
#include <vector>

#include "RoeCommon.h"
#include "RoeVector2.h"
#include "RoeVector3.h"
#include "RoeColor.h"
#include "RoeMatrix.h"
#include "RoeVertex.h"

namespace roe {
	
	//a class for OpenGL vertex buffer objects
	class VertexBufferObject {
	public:
		VertexBufferObject();
		~VertexBufferObject() {destroy();}
		
		void fill(Vertex *pv, long num);
		void create(int usage = GL_STATIC_DRAW);
		void destroy();
		void enable();
		void disable();
		void clearLocalData(); //clears the local arrays
		bool isEnabled() {return m_bEnabled;}
		bool isCreated() {return m_bCreated;}
		bool isPositionDataSpecified() {return m_bSpecP;}
		bool isNormalDataSpecified  () {return m_bSpecN;}
		bool isTexCoordDataSpecified() {return m_bSpecTC;}
		bool isIndexDataSpecified   () {return m_bSpecI;}
		
		void draw(int mode);
		void drawElements(int mode, long count=-1);               //rendering using m_vDataIndex indexdata
		void drawElements(int mode, long count, long *indices); //rendering using extra specified indexdata
		void drawRangeElements(int mode, long count, long min=0, long max=2000000000); //rendering using indexdata (just indexes [min to max])
		void drawRangeElements(int mode, long count, long *indices, long min=0, long max=2000000000); //rendering using extra specified indexdata (just indexes [min to max])
		void drawArrays  (int mode, long first=0, long count=-1); //rendering ignoring any indexdata
		void* mapVBO(bool read = false, bool write = true);        //returns a pointer to the mapped VBO
		void* mapIBO(bool read = false, bool write = true);        //returns a pointer to the mapped IBO
		void  unmapVBO();                           //unmapping VBO
		void  unmapIBO();                           //unmapping IBO
		
		std::vector<Vector3>& getPosition() {return m_vDataPosition;}
		std::vector<Vector3>& getNormal  () {return m_vDataNormal;}
		std::vector<Vector2>& getTexCoord() {return m_vDataTexCoord;}
		std::vector<long   >& getIndex   () {return m_vDataIndex;}
		void updateData(bool updateP = true, bool updateN = true, bool updateTC = true, bool updateI = true); //call this, after you've changed any of the above queried vectors! create() have been called beforehand
		
		//int getVBOID() {return m_iVBO_ID;}
		//int getIBOID () {return m_iIBO_ID;}
	private:
		static void unbindVBO();
		static void unbindIBO();
		static void unbindBoth();
		
		std::vector<Vector3> m_vDataPosition;
		std::vector<Vector3> m_vDataNormal;
		std::vector<Vector2> m_vDataTexCoord;
		std::vector<long>    m_vDataIndex;
		
		unsigned int m_vboP, m_vboN, m_vboTC, m_vboTA, m_vboI; //vertex buffer object ids
		bool m_bCreated, m_bEnabled;
		bool m_bSpecP, m_bSpecN, m_bSpecTC, m_bSpecI;
		int m_iUsage;
	};
	typedef VertexBufferObject VBO;//*/
	
} // namespace roe

#endif /* ROEVBO_H_ */
