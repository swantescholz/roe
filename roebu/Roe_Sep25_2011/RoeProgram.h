#ifndef ROESHADER_H_
#define ROESHADER_H_

#include <string>
#include <stack>
#include "RoeTextfile.h"
#include "RoeColor.h"
#include "RoeVector2.h"
#include "RoeVector3.h"
#include "RoeMatrix.h"
#include "RoeTexture.h"
#include "RoeGradient.h"
#include "RoeLight.h"

namespace roe {
	
	//class for a shader program
	class Program {
	public:
		Program();
		Program(const std::string& sourceFile, bool bInstall = true);
		Program(const std::string& sourceFile, const std::string& headerFile, bool bInstall = true);
		Program(const std::string& sourceFile, const std::string& preheader, const std::string& headerFile, bool bInstall = true);
		Program(const std::string& vertSourceFile, const std::string& fragSourceFile,
		        const std::string& vertHeaderFile, const std::string& fragHeaderFile, bool bInstall = true);
		Program(const std::string& vertSourceFile, const std::string& fragSourceFile, const std::string& preheader,
		        const std::string& vertHeaderFile, const std::string& fragHeaderFile, bool bInstall = true);
		~Program();
		Program(const Program& rhs);              //this program will have to be reinstalled then
		Program& operator=(const Program& rhs);   //this program will have to be reinstalled then
		
		static std::string getShaderInfoLog(int shader);
		static std::string getProgramInfoLog(int shader);
		static void unuse();
		static void pushUses(); //contolling the program use stack
		static void popUses();
		static int  getActiveProgramID();
		
		void setVertSource    (const std::string& source);
		void setVertSourceFile(const std::string& sourceFile) {setVertSource(Textfile::readFile(s_sPathStart+sourceFile+s_sVertPathEnd));}
		void setFragSource    (const std::string& source);
		void setFragSourceFile(const std::string& sourceFile) {setFragSource(Textfile::readFile(s_sPathStart+sourceFile+s_sFragPathEnd));}
		void setVertHeader    (const std::string& header)     {m_sVertHeader = header;}
		void setVertHeaderFile(const std::string& headerFile) {setVertHeader(Textfile::readFile(s_sPathStart+headerFile+s_sVertPathEnd));}
		void setFragHeader    (const std::string& header)     {m_sFragHeader = header;}
		void setFragHeaderFile(const std::string& headerFile) {setFragHeader(Textfile::readFile(s_sPathStart+headerFile+s_sFragPathEnd));}
		void setHeader        (const std::string& header)     {setVertHeader(header); setFragHeader(header);}
		void setHeaderFile    (const std::string& headerFile) {setHeader(Textfile::readFile(s_sPathStart+headerFile));}
		std::string getVertSource() const {return m_sVertSource;}
		std::string getFragSource() const {return m_sFragSource;}
		std::string getVertHeader() const {return m_sVertHeader;}
		std::string getFragHeader() const {return m_sFragHeader;}
		int         getVertID    () const {return m_iVertID;}
		int         getFragID    () const {return m_iFragID;}
		int         getProgID    () const {return m_iProgID;}
		std::string getProgramInfoLog() const {return getProgramInfoLog(m_iProgID);}
		std::string getVertInfoLog   () const {return getShaderInfoLog(m_iVertID);}
		std::string getFragInfoLog   () const {return getShaderInfoLog(m_iFragID);}
		
		void install(); //compile and link
		void use();     //use this program
		
		void create();
		void destroy(bool clearSource = true, bool clearHeader = false);
		void renew(bool clearSource = true, bool clearHeader = false) {destroy(clearSource, clearHeader); create();}
		void detachShaders(); //invalidates program
		void reinstall() {detachShaders(); install();}
		bool isCreated()  const {return m_iProgID != 0 && m_iVertID != 0 && m_iFragID != 0;}
		bool isInstalled()const {return m_bInstalled;}
		
		//attributes and uniforms (just work, if program is in use!)
		unsigned int getAttribLoc (const char* var) const ;
		int          getUniformLoc(const char* var) const ;
		void         setAttrib (const char* var,   int f);
		void         setAttrib (const char* var, float f);
		void         setAttrib (const char* var,double f);
		void         setAttrib (const char* var, const Vector2& v);
		void         setAttrib (const char* var, const Vector3& v);
		void         setAttrib (const char* var, const Color  & c);
		void         setUniform(const char* var,  int  f);
		void         setUniform(const char* var,float  f);
		void         setUniform(const char* var,double f);
		void         setUniform(const char* var,int n,const   int* f);
		void         setUniform(const char* var,int n,const float* f);
		void         setUniform(const char* var, const Vector2& v);
		void         setUniform(const char* var, const Vector3& v);
		void         setUniform(const char* var, const Color& c);
		void         setUniform(const char* var, int n, const Vector2* v);
		void         setUniform(const char* var, int n, const Vector3* v);
		void         setUniform(const char* var, int n, const Color* c);
		void         setUniform(const char* var, const Matrix& m, bool transpose = false);
		void         setUniform(const char* var, const Gradient& g, int activeTexture = GL_TEXTURE0);
		void         setUniform(const char* var, const Texture & t, int activeTexture = GL_TEXTURE1);
		
		//specify global uniforms for all active programs
		static void  setGlobalUniform(const char* var,  int  f);
		static void  setGlobalUniform(const char* var,float  f);
		static void  setGlobalUniform(const char* var,double f);
		static void  setGlobalUniform(const char* var,int n,const   int* f);
		static void  setGlobalUniform(const char* var,int n,const float* f);
		static void  setGlobalUniform(const char* var, const Vector2& v);
		static void  setGlobalUniform(const char* var, const Vector3& v);
		static void  setGlobalUniform(const char* var, const Color& c);
		static void  setGlobalUniform(const char* var, int n, const Vector2* v);
		static void  setGlobalUniform(const char* var, int n, const Vector3* v);
		static void  setGlobalUniform(const char* var, int n, const Color* c);
		static void  setGlobalUniform(const char* var, const Matrix& m, bool transpose = false);
		static void  setGlobalUniformSampler1(const char* var, const GLuint id, int activeTexture = GL_TEXTURE1);
		static void  setGlobalUniformSampler2(const char* var, const GLuint id, int activeTexture = GL_TEXTURE0);
		static void  setGlobalUniform(const char* var, const Light& light);
		static void  setStdTransformationMatrix(const Matrix& m) {setGlobalUniform("uTransformationMatrix",m,false);}
		static void  setStdNormalMatrix        (const Matrix& m) {setGlobalUniform("uNormalMatrix"        ,m,false);}
		static void  setStdTextureMatrix       (const Matrix& m) {setGlobalUniform("uTextureMatrix"       ,m,false);}
		static void  setStdGradient(const GLuint id, int activeTexture = GL_TEXTURE1) {setGlobalUniformSampler1("uGradient",id,activeTexture);}
		static void  setStdTexture (const GLuint id, int activeTexture = GL_TEXTURE0) {setGlobalUniformSampler2("uTexture" ,id,activeTexture);}
		static void  setStdLightSource(const Light& light)   {setGlobalUniform("uLightSource", light);}
		static void  setStdLightModelAmbient(const Color& c) {setGlobalUniform("uLightModelAmbient", c);}
		
		
		// for the static variables
		static void init(std::string sStart, std::string sVertEnd, std::string sFragEnd)
			{s_sPathStart = sStart; s_sVertPathEnd = sVertEnd; s_sFragPathEnd = sFragEnd;}
		static void setPathStart(std::string s)                 {s_sPathStart = s;}
		static void setVertPathEnd(std::string s)               {s_sVertPathEnd = s;}
		static void setFragPathEnd(std::string s)               {s_sFragPathEnd = s;}
		static std::string getPathStart()                       {return s_sPathStart;}
		static std::string getVertPathEnd()                     {return s_sVertPathEnd;}
		static std::string getFragPathEnd()                     {return s_sFragPathEnd;}
	private:
		
		unsigned int m_iProgID;
		int m_iVertID;
		int m_iFragID;
		std::string m_sVertSource, m_sVertHeader;
		std::string m_sFragSource, m_sFragHeader;
		bool m_bInstalled;
	
		static std::string s_sPathStart, s_sVertPathEnd, s_sFragPathEnd; //standard path
		static std::list<Program*> s_lpPrograms;     //pointers to all timer instances
		static std::stack<int> s_stUses;             //program use stack
	};
	
} // namespace roe

#endif /* ROESHADER_H_ */
