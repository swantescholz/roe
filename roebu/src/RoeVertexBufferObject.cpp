#include "RoeVertexBufferObject.h"
#include "RoeException.h"
#include <cstring>

namespace roe {
	
	VertexBufferObject::VertexBufferObject() {
		m_vboP = m_vboN = m_vboTC = m_vboI = 0;
		m_bSpecP = m_bSpecN = m_bSpecTC = m_bSpecI = false;
		m_bCreated = m_bEnabled = false;
	}
	
	void VertexBufferObject::fill(Vertex *pv, long num) {
		for (long i = 0; i < num; i++) {
			m_vDataPosition.push_back(pv[i].p);
			m_vDataNormal.push_back  (pv[i].n);
			m_vDataTexCoord.push_back(pv[i].tc);
			m_vDataIndex.push_back   (pv[i].index);
		}
	}
	void VertexBufferObject::create(int usage) {
		m_iUsage = usage;
		if(m_vDataPosition.empty()) roe_except("No PositionData specified", "create");
		GLsizeiptr	sizePosition = m_vDataPosition.size()*sizeof(Vector3);
		GLsizeiptr	sizeNormal   = m_vDataNormal.size  ()*sizeof(Vector3);
		GLsizeiptr	sizeTexCoord = m_vDataTexCoord.size()*sizeof(Vector2);
		GLsizeiptr	sizeIndex    = m_vDataIndex.size   ()*sizeof(long);
		
		if(m_vboP == 0) glGenBuffers(1, &m_vboP);
		if(m_vboN == 0) glGenBuffers(1, &m_vboN);
		if(m_vboTC== 0) glGenBuffers(1, &m_vboTC);
		if(m_vboI == 0) glGenBuffers(1, &m_vboI);
		if(m_vboP == 0) roe_except("Vertex-Buffer could not be generated"           , "create");
		if(m_vboN == 0) roe_except("(Normal-)Vertex-Buffer could not be generated"  , "create");
		if(m_vboTC== 0) roe_except("(TexCoord-)Vertex-Buffer could not be generated", "create");
		if(m_vboI == 0) roe_except("(Index-)Vertex-Buffer could not be generated"   , "create");
		
		if (!m_vDataIndex.empty()) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboI);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeIndex, 0, m_iUsage);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeIndex, (const GLvoid*)(&m_vDataIndex[0]));
			m_bSpecI = true;
		}
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); //unbind
		
		if(!m_vDataPosition.empty  ()) {
			glBindBuffer(GL_ARRAY_BUFFER, m_vboP);
			glBufferData(GL_ARRAY_BUFFER, sizePosition, 0, m_iUsage);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizePosition, (const GLvoid*)(&m_vDataPosition[0]));
			m_bSpecP = true;
		}
		if(!m_vDataNormal.empty  ()) {
			glBindBuffer(GL_ARRAY_BUFFER, m_vboN);
			glBufferData(GL_ARRAY_BUFFER, sizeNormal, 0, m_iUsage);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeNormal  , (const GLvoid*)(&m_vDataNormal  [0]));
			m_bSpecN = true;
		}
		if(!m_vDataTexCoord.empty()) {
			glBindBuffer(GL_ARRAY_BUFFER, m_vboTC);
			glBufferData(GL_ARRAY_BUFFER, sizeTexCoord, 0, m_iUsage);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeTexCoord, (const GLvoid*)(&m_vDataTexCoord[0]));
			m_bSpecTC = true;
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0); //unbind
		m_bCreated = true;
		if (usage == GL_STATIC_DRAW) { //static geometry?
			//clearLocalData(); //no local data for changes needed if static
		}
	}
	
	void VertexBufferObject::destroy() {
		if (m_vboP != 0) {glDeleteBuffers(1, &m_vboP) ; m_vboP = 0;}
		if (m_vboN != 0) {glDeleteBuffers(1, &m_vboN) ; m_vboN = 0;}
		if (m_vboTC!= 0) {glDeleteBuffers(1, &m_vboTC); m_vboTC= 0;}
		if (m_vboI != 0) {glDeleteBuffers(1, &m_vboI) ; m_vboI = 0;}
		clearLocalData();
		m_bCreated = m_bEnabled = false;
		m_bSpecP = m_bSpecN = m_bSpecTC = m_bSpecI = false;
	}
	
	void VertexBufferObject::enable() {
		if(m_bSpecI) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboI);
			glIndexPointer(GL_INT, 0, 0);
			glEnableClientState(GL_INDEX_ARRAY);
		}
		if(m_bSpecN) {
			glBindBuffer(GL_ARRAY_BUFFER, m_vboN);
			glNormalPointer(GL_FLOAT, 0, 0);
			glEnableClientState(GL_NORMAL_ARRAY);
		}
		if(m_bSpecTC) {
			glBindBuffer(GL_ARRAY_BUFFER, m_vboTC);
			//glClientActiveTexture(GL_TEXTURE0);
			glTexCoordPointer(2, GL_FLOAT, 0, 0);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		if(m_bSpecP) {
			glBindBuffer(GL_ARRAY_BUFFER, m_vboP);
			glVertexPointer(3, GL_FLOAT, 0, 0);
			glEnableClientState(GL_VERTEX_ARRAY);
		}
		m_bEnabled = true;
	}
	void VertexBufferObject::disable() {
		unbindBoth();
		if(m_bSpecI) {
			glDisableClientState(GL_INDEX_ARRAY);
		}
		if(m_bSpecN) {
			glDisableClientState(GL_NORMAL_ARRAY);
		}
		if(m_bSpecTC) {
			//glClientActiveTexture(GL_TEXTURE0);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		if(m_bSpecP) {
			glDisableClientState(GL_VERTEX_ARRAY);
		}
		m_bEnabled = false;
	}
	void VertexBufferObject::clearLocalData() {
		m_vDataPosition.clear();
		m_vDataNormal.clear();
		m_vDataTexCoord.clear();
		m_vDataIndex.clear();
	}
	void VertexBufferObject::updateData(bool updateP, bool updateN, bool updateTC, bool updateI) {
		GLsizeiptr	sizePosition = m_vDataPosition.size()*sizeof(Vector3);
		GLsizeiptr	sizeNormal   = m_vDataNormal.size  ()*sizeof(Vector3);
		GLsizeiptr	sizeTexCoord = m_vDataTexCoord.size()*sizeof(Vector2);
		GLsizeiptr	sizeIndex    = m_vDataIndex.size   ()*sizeof(long);
		GLsizeiptr *dataVBO = nullptr;
		
		if (updateP && !m_vDataPosition.empty  ()) {
			glBindBuffer(GL_ARRAY_BUFFER, m_vboP);
			glBufferData(GL_ARRAY_BUFFER, sizePosition, 0, m_iUsage);
			dataVBO = (GLsizeiptr*)mapVBO(false,true);
			memcpy(dataVBO, m_vDataPosition.data(), sizePosition);
			unmapVBO();
			m_bSpecP = true;
		} else {m_bSpecP = false;}
		if(updateN && !m_vDataNormal.empty  ()) {
			glBindBuffer(GL_ARRAY_BUFFER, m_vboN);
			glBufferData(GL_ARRAY_BUFFER, sizeNormal, 0, m_iUsage);
			dataVBO = (GLsizeiptr*)mapVBO(false,true);
			memcpy(dataVBO, m_vDataNormal.data(), sizeNormal);
			unmapVBO();
			m_bSpecN = true;
		} else {m_bSpecN = false;}
		if(updateTC && !m_vDataTexCoord.empty()) {
			glBindBuffer(GL_ARRAY_BUFFER, m_vboTC);
			glBufferData(GL_ARRAY_BUFFER, sizeTexCoord, 0, m_iUsage);
			dataVBO = (GLsizeiptr*)mapVBO(false,true);
			memcpy(dataVBO, m_vDataTexCoord.data(), sizeTexCoord);
			unmapVBO();
			m_bSpecTC = true;
		} else {m_bSpecTC = false;}
		
		if(updateI && !m_vDataIndex.empty ()) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboI);
			glBufferData(GL_ARRAY_BUFFER, sizeIndex, 0, m_iUsage);
			GLsizeiptr *dataIBO = (GLsizeiptr*)mapIBO(false,true);
			memcpy(dataIBO, m_vDataIndex.data(), sizeIndex);
			unmapIBO();
			m_bSpecI = true;
		} else {m_bSpecI = false;}
	}
	
	void VertexBufferObject::draw(int mode) {
		if(m_bSpecI) {
			drawElements(mode);
		}
		else {
			drawArrays(mode);
		}
	}
	void VertexBufferObject::drawElements(int mode, long count) {
		if (count < 0) count = (long)(m_vDataIndex.size());
		glDrawElements(mode, count, GL_UNSIGNED_INT, 0);
	}
	void VertexBufferObject::drawElements(int mode, long count, long *indices) {
		glDrawElements(mode, count, GL_UNSIGNED_INT, (void*)indices);
	}
	void VertexBufferObject::drawRangeElements(int mode, long count, long min, long max) {
		glDrawRangeElements(mode, min, max, count, GL_UNSIGNED_INT, 0);
	}
	void VertexBufferObject::drawRangeElements(int mode, long count, long *indices, long min, long max) {
		glDrawRangeElements(mode, min, max, count, GL_UNSIGNED_INT, (void*)indices);
	}
	void VertexBufferObject::drawArrays(int mode, long first, long count) {
		if (count < 0) count = (long)(m_vDataPosition.size());
		glDrawArrays(mode, first, count);
	}
	
	void* VertexBufferObject::mapVBO(bool read, bool write) {
		GLenum type = GL_WRITE_ONLY; if (read && write) type = GL_READ_WRITE; else if (read) type = GL_READ_ONLY;
		return glMapBuffer(GL_ARRAY_BUFFER, type);
	}
	void* VertexBufferObject::mapIBO(bool read, bool write) {
		GLenum type = GL_WRITE_ONLY; if (read && write) type = GL_READ_WRITE; else if (read) type = GL_READ_ONLY;
		return glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, type);
	}
	void VertexBufferObject::unmapVBO() {
		if (glUnmapBuffer(GL_ARRAY_BUFFER) == GL_FALSE)
			roe_except("VBO corrupted", "unmap");
	}
	void VertexBufferObject::unmapIBO() {
		if (glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER) == GL_FALSE)
			roe_except("IBO corrupted", "unmap");
	}
	
	void VertexBufferObject::unbindVBO () {glBindBuffer(GL_ARRAY_BUFFER        ,0);}
	void VertexBufferObject::unbindIBO () {glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);}
	void VertexBufferObject::unbindBoth() {glBindBuffer(GL_ARRAY_BUFFER,0); glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);}
	//*/
	
} // namespace roe
