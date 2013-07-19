#ifndef GL_GLEXT_PROTOTYPES
	#define GL_GLEXT_PROTOTYPES
#endif
#include <GL/gl.h>
#include <GL/glext.h>
#include "RoeProgram.h"

#include "RoeException.h"
#include "RoeCommon.h"
#include "RoeUtil.h"

namespace roe {
	std::string Program::s_sPathStart = "", Program::s_sVertPathEnd = ".vert", Program::s_sFragPathEnd = ".frag";
	std::list<Program*> Program::s_lpPrograms;
	std::stack<int> Program::s_stUses;
	
	Program::Program() {
		s_lpPrograms.push_back(this);
		m_iVertID = m_iFragID = m_iProgID = 0;
		m_bInstalled = false;
	}
	Program::Program(const std::string& sourceFile, bool bInstall) {
		s_lpPrograms.push_back(this);
		
		m_iVertID = m_iFragID = m_iProgID = 0;
		m_bInstalled = false;
		setVertSourceFile(sourceFile);
		setFragSourceFile(sourceFile);
		if (bInstall) install();
	}
	Program::Program(const std::string& sourceFile, const std::string& headerFile, bool bInstall) {
		s_lpPrograms.push_back(this);
		
		m_iVertID = m_iFragID = m_iProgID = 0;
		m_bInstalled = false;
		setVertSourceFile(sourceFile);
		setFragSourceFile(sourceFile);
		setVertHeaderFile(headerFile);
		setFragHeaderFile(headerFile);
		
		if (bInstall) install();
	}
	Program::Program(const std::string& sourceFile, const std::string& preheader, const std::string& headerFile, bool bInstall) {
		s_lpPrograms.push_back(this);
		
		m_iVertID = m_iFragID = m_iProgID = 0;
		m_bInstalled = false;
		setVertSourceFile(sourceFile);
		setFragSourceFile(sourceFile);
		setVertHeader(preheader+Textfile::readFile(s_sPathStart+headerFile+s_sVertPathEnd));
		setFragHeader(preheader+Textfile::readFile(s_sPathStart+headerFile+s_sFragPathEnd));
		
		if (bInstall) install();
	}
	Program::Program(const std::string& vertSourceFile, const std::string& fragSourceFile,
		             const std::string& vertHeaderFile, const std::string& fragHeaderFile, bool bInstall) {
		s_lpPrograms.push_back(this);
		
		m_iVertID = m_iFragID = m_iProgID = 0;
		m_bInstalled = false;
		setVertSourceFile(vertSourceFile);
		setFragSourceFile(fragSourceFile);
		setVertHeaderFile(vertHeaderFile);
		setFragHeaderFile(fragHeaderFile);
		
		if (bInstall) install();
	}
	Program::Program(const std::string& vertSourceFile, const std::string& fragSourceFile, const std::string& preheader,
		             const std::string& vertHeaderFile, const std::string& fragHeaderFile, bool bInstall) {
		m_iVertID = m_iFragID = m_iProgID = 0;
		m_bInstalled = false;
		setVertSourceFile(vertSourceFile);
		setFragSourceFile(fragSourceFile);
		setVertHeader(preheader+Textfile::readFile(s_sPathStart+vertHeaderFile+s_sVertPathEnd));
		setFragHeader(preheader+Textfile::readFile(s_sPathStart+fragHeaderFile+s_sFragPathEnd));
		
		if (bInstall) install();
	}
	Program::~Program() {
		destroy(true, true);
		//erasing this instance from static list
		for (std::list<Program*>::iterator it = s_lpPrograms.begin(); it != s_lpPrograms.end(); it++) {
			if ( (*it) == this) { // its me!
				s_lpPrograms.erase(it);
				it = s_lpPrograms.begin();      //important!
				if (it == s_lpPrograms.end()) {
					break;
				}
			}
		}
	}
	
	Program::Program(const Program& rhs) {
		m_iVertID = m_iFragID = m_iProgID = 0;
		m_bInstalled = false;
		m_sVertSource = rhs.m_sVertSource;
		m_sFragSource = rhs.m_sFragSource;
		m_sVertHeader = rhs.m_sVertHeader;
		m_sFragHeader = rhs.m_sFragHeader;
	}
	Program& Program::operator=(const Program& rhs) {
		if(&rhs == this) return *this;
		destroy(true, true);
		m_sVertSource = rhs.m_sVertSource;
		m_sFragSource = rhs.m_sFragSource;
		m_sVertHeader = rhs.m_sVertHeader;
		m_sFragHeader = rhs.m_sFragHeader;
		return *this;
	}
	
	void Program::unuse() {glUseProgram(0);}
	void Program::pushUses() {int old=0;glGetIntegerv(GL_CURRENT_PROGRAM, &old); s_stUses.push(old);}
	void Program::popUses () {glUseProgram(s_stUses.top()); s_stUses.pop();/*int old=0;glGetIntegerv(GL_CURRENT_PROGRAM, &old);cout << "o: " << old << endl;*/}
	int  Program::getActiveProgramID() {int active=0;glGetIntegerv(GL_CURRENT_PROGRAM, &active); return active;}
	
	void Program::create() {
		m_bInstalled = false;
		m_iVertID = glCreateShader(GL_VERTEX_SHADER);
		m_iFragID = glCreateShader(GL_FRAGMENT_SHADER);
		if (m_iVertID == 0 || m_iFragID == 0)
			roe_except("Vertex or Fragment Shader object cannot be created; have you correctly set up OpenGL?", "create");
		m_iProgID = glCreateProgram();
		if (m_iProgID == 0)
			roe_except("Program object cannot be created; have you correctly set up OpenGL?", "create");
	}
	void Program::destroy(bool clearSource, bool clearHeader) {
		m_bInstalled = false;
		if (m_iVertID != 0) {glDeleteShader(m_iVertID) ; m_iVertID = 0;}
		if (m_iFragID != 0) {glDeleteShader(m_iFragID) ; m_iFragID = 0;}
		if (m_iProgID != 0) {glDeleteProgram(m_iProgID); m_iProgID = 0;}
		if (clearSource) {
			m_sVertSource.clear();
			m_sFragSource.clear();
		}
		if (clearHeader) {
			m_sVertHeader.clear();
			m_sFragHeader.clear();
		}
	}
	void Program::setVertSource(const std::string& source) {
		m_sVertSource = source;
	}
	void Program::setFragSource(const std::string& source) {
		m_sFragSource = source;
	}
	void Program::install() {
		//maybe creating first:
		if (!isCreated()) {create();}
		
		//setting source:
		std::string sTmp = m_sVertHeader+m_sVertSource;
		const char *ch1 = sTmp.c_str();
		glShaderSource(m_iVertID, 1, &ch1, NULL);
		sTmp = m_sFragHeader+m_sFragSource;
		const char *ch2 = sTmp.c_str();
		glShaderSource(m_iFragID, 1, &ch2, NULL);
		
		int vertCompiled, fragCompiled, linked;
		
		//compiling source:
		glCompileShader(m_iVertID);
		glGetShaderiv(m_iVertID, GL_COMPILE_STATUS, &vertCompiled);
		if (!vertCompiled) {
			roe_except("Vertex Shader '\n" + util::addLineNumbersToString(std::string(ch1), "  ") + "\n' cannot be compiled!\n"+getVertInfoLog(), "install");
		}
		glCompileShader(m_iFragID);
		glGetShaderiv(m_iFragID, GL_COMPILE_STATUS, &fragCompiled);
		if (!fragCompiled) {
			roe_except("Fragment Shader '\n" + util::addLineNumbersToString(std::string(ch2), "  ") + "\n' cannot be compiled!\n"+getFragInfoLog(), "install");
		}
		
		//attaching and linking
		glAttachShader(m_iProgID, m_iVertID);
		glAttachShader(m_iProgID, m_iFragID);
		glLinkProgram(m_iProgID);
		glGetProgramiv(m_iProgID, GL_LINK_STATUS, &linked);
		if (!linked) {
			roe_except("Shader-Program cannot be compiled!\n"+getProgramInfoLog(), "install");
		}
		m_bInstalled = true;
	}
	void Program::use() {
		glUseProgram(m_iProgID);
	}
	void Program::detachShaders() {
		glDetachShader(m_iProgID, m_iVertID);
		glDetachShader(m_iProgID, m_iVertID);
	}
	
	unsigned int Program::getAttribLoc (const char* var) const {return glGetAttribLocation (m_iProgID, var);}
	int          Program::getUniformLoc(const char* var) const {return glGetUniformLocation(m_iProgID, var);}
	void Program::setAttrib (const char* var,   int f) {glVertexAttrib1s(glGetAttribLocation(m_iProgID,var),f);}
	void Program::setAttrib (const char* var, float f) {glVertexAttrib1f(glGetAttribLocation(m_iProgID,var),f);}
	void Program::setAttrib (const char* var,double f) {glVertexAttrib1d(glGetAttribLocation(m_iProgID,var),f);}
	void Program::setAttrib (const char* var, const Vector2& v) {glVertexAttrib2fv(glGetAttribLocation(m_iProgID,var),v.rg);}
	void Program::setAttrib (const char* var, const Vector3& v) {glVertexAttrib3fv(glGetAttribLocation(m_iProgID,var),v.rg);}
	void Program::setAttrib (const char* var, const Color  & c) {glVertexAttrib4fv(glGetAttribLocation(m_iProgID,var),c.rg);}
	void Program::setUniform(const char* var,   int f) {glUniform1i(glGetUniformLocation(m_iProgID,var),f);}
	void Program::setUniform(const char* var, float f) {glUniform1f(glGetUniformLocation(m_iProgID,var),f);}
	void Program::setUniform(const char* var,double f) {glUniform1f(glGetUniformLocation(m_iProgID,var),(float)f);}
	void Program::setUniform(const char* var,int n,const   int* f) {glUniform1iv(glGetUniformLocation(m_iProgID,var),n,f);}
	void Program::setUniform(const char* var,int n,const float* f) {glUniform1fv(glGetUniformLocation(m_iProgID,var),n,f);}
	void Program::setUniform(const char* var, const Vector2& v)    {glUniform2fv(glGetUniformLocation(m_iProgID,var),1,v.rg);}
	void Program::setUniform(const char* var, const Vector3& v)    {glUniform3fv(glGetUniformLocation(m_iProgID,var),1,v.rg);}
	void Program::setUniform(const char* var, const Color& c)      {glUniform4fv(glGetUniformLocation(m_iProgID,var),1,c.rg);}
	void Program::setUniform(const char* var, int n, const Vector2* v) {glUniform2fv(glGetUniformLocation(m_iProgID,var),n,v[0].rg);}
	void Program::setUniform(const char* var, int n, const Vector3* v) {glUniform3fv(glGetUniformLocation(m_iProgID,var),n,v[0].rg);}
	void Program::setUniform(const char* var, int n, const Color  * c) {glUniform4fv(glGetUniformLocation(m_iProgID,var),n,c[0].rg);}
	void Program::setUniform(const char* var, const Matrix& m, bool transpose)
	{glUniformMatrix4fv(glGetUniformLocation(m_iProgID,var),1,transpose,m.n);}
	void Program::setUniform(const char* var, const Gradient& g, int activeTexture) {
		glActiveTexture(activeTexture); g.bind();
		glUniform1i(glGetUniformLocation(m_iProgID,var),(activeTexture-GL_TEXTURE0));} //-0x84C0
	void Program::setUniform(const char* var, const Texture& t, int activeTexture) {
		glActiveTexture(activeTexture); t.bind();
		glUniform1i(glGetUniformLocation(m_iProgID,var),(activeTexture-GL_TEXTURE0));} //-0x84C0
	
	//static global:
	void Program::setGlobalUniform(const char* var,   int f) {pushUses();for(auto it = s_lpPrograms.begin();it!=s_lpPrograms.end();++it) {(*it)->use();glUniform1i(glGetUniformLocation((*it)->getProgID(),var),f);}popUses();}
	void Program::setGlobalUniform(const char* var, float f) {pushUses();for(auto it = s_lpPrograms.begin();it!=s_lpPrograms.end();++it) {(*it)->use();glUniform1f(glGetUniformLocation((*it)->getProgID(),var),f);}popUses();}
	void Program::setGlobalUniform(const char* var,double f) {pushUses();for(auto it = s_lpPrograms.begin();it!=s_lpPrograms.end();++it) {(*it)->use();glUniform1f(glGetUniformLocation((*it)->getProgID(),var),(float)f);}popUses();}
	void Program::setGlobalUniform(const char* var,int n,const   int* f) {pushUses();for(auto it = s_lpPrograms.begin();it!=s_lpPrograms.end();++it) {(*it)->use();glUniform1iv(glGetUniformLocation((*it)->getProgID(),var),n,f);}popUses();}
	void Program::setGlobalUniform(const char* var,int n,const float* f) {pushUses();for(auto it = s_lpPrograms.begin();it!=s_lpPrograms.end();++it) {(*it)->use();glUniform1fv(glGetUniformLocation((*it)->getProgID(),var),n,f);}popUses();}
	void Program::setGlobalUniform(const char* var, const Vector2& v)    {pushUses();for(auto it = s_lpPrograms.begin();it!=s_lpPrograms.end();++it) {(*it)->use();glUniform2fv(glGetUniformLocation((*it)->getProgID(),var),1,v.rg);}popUses();}
	void Program::setGlobalUniform(const char* var, const Vector3& v)    {pushUses();for(auto it = s_lpPrograms.begin();it!=s_lpPrograms.end();++it) {(*it)->use();glUniform3fv(glGetUniformLocation((*it)->getProgID(),var),1,v.rg);}popUses();}
	void Program::setGlobalUniform(const char* var, const Color& c)      {pushUses();for(auto it = s_lpPrograms.begin();it!=s_lpPrograms.end();++it) {(*it)->use();glUniform4fv(glGetUniformLocation((*it)->getProgID(),var),1,c.rg);}popUses();}
	void Program::setGlobalUniform(const char* var, int n, const Vector2* v) {pushUses();for(auto it = s_lpPrograms.begin();it!=s_lpPrograms.end();++it) {(*it)->use();glUniform2fv(glGetUniformLocation((*it)->getProgID(),var),n,v[0].rg);}popUses();}
	void Program::setGlobalUniform(const char* var, int n, const Vector3* v) {pushUses();for(auto it = s_lpPrograms.begin();it!=s_lpPrograms.end();++it) {(*it)->use();glUniform3fv(glGetUniformLocation((*it)->getProgID(),var),n,v[0].rg);}popUses();}
	void Program::setGlobalUniform(const char* var, int n, const Color  * c) {pushUses();for(auto it = s_lpPrograms.begin();it!=s_lpPrograms.end();++it) {(*it)->use();glUniform4fv(glGetUniformLocation((*it)->getProgID(),var),n,c[0].rg);}popUses();}
	void Program::setGlobalUniform(const char* var, const Matrix& m, bool transpose)
	{pushUses();for(auto it = s_lpPrograms.begin();it!=s_lpPrograms.end();++it) {
		(*it)->use();glUniformMatrix4fv(glGetUniformLocation((*it)->getProgID(),var),1,transpose,m.n);}popUses();}
	void Program::setGlobalUniformSampler1(const char* var, const GLuint id, int activeTexture) {
		pushUses(); glActiveTexture(activeTexture); glBindTexture(GL_TEXTURE_1D, id);
		for(auto it = s_lpPrograms.begin();it!=s_lpPrograms.end();++it) {
			(*it)->use();glUniform1i(glGetUniformLocation((*it)->getProgID(),var),(activeTexture-GL_TEXTURE0));}popUses();} //-0x84C0
	void Program::setGlobalUniformSampler2(const char* var, const  GLuint id, int activeTexture) {
		pushUses(); glActiveTexture(activeTexture); glBindTexture(GL_TEXTURE_2D, id);
		for(auto it = s_lpPrograms.begin();it!=s_lpPrograms.end();++it) {
			(*it)->use();glUniform1i(glGetUniformLocation((*it)->getProgID(),var),(activeTexture-GL_TEXTURE0));}popUses();} //-0x84C0
	void Program::setGlobalUniform(const char* var, const Light& light) {
		setStdLightModelAmbient(Light::getLightModelAmb());
		std::string name = std::string(var) + "[" + util::toString(light.getLightIndex()) + "].";
		setGlobalUniform((name+"ambient" ).c_str(), light.getMaterialAmb ());
		setGlobalUniform((name+"diffuse" ).c_str(), light.getMaterialDiff());
		setGlobalUniform((name+"specular").c_str(), light.getMaterialSpec());
		setGlobalUniform((name+"position").c_str(), light.getPosition    ());
		setGlobalUniform((name+"spotDirection").c_str(), light.getSpotDirection());
		setGlobalUniform((name+"spotExponent" ).c_str(), light.getSpotExponent ());
		setGlobalUniform((name+"spotCutoff"   ).c_str(), light.getSpotCutoff   ());
		setGlobalUniform((name+"constantAttenuation" ).c_str(), light.getAttentuationC());
		setGlobalUniform((name+"linearAttenuation"   ).c_str(), light.getAttentuationL());
		setGlobalUniform((name+"quadraticAttenuation").c_str(), light.getAttentuationQ());
	}
	
	std::string Program::getShaderInfoLog(int shader) {
		int infologLen = 0;
		int charsWritten = 0;
		char *infoLog = nullptr;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLen);
		if (infologLen > 0) {
			infoLog = new char[infologLen];
			if (!infoLog) {
				roe_except("shader Info-Log could not be allocated!", "getShaderInfoLog");
			}
			glGetShaderInfoLog(shader, infologLen, &charsWritten, infoLog);
			std::string str(infoLog);
			delete[] infoLog;
			return str;
		}
		return "";
	}
	std::string Program::getProgramInfoLog(int program) {
		int infologLen = 0;
		int charsWritten = 0;
		char *infoLog = nullptr;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infologLen);
		if (infologLen > 0) {
			infoLog = new char[infologLen];
			if (!infoLog) {
				roe_except("program Info-Log could not be allocated!", "getProgramInfoLog");
			}
			glGetProgramInfoLog(program, infologLen, &charsWritten, infoLog);
			std::string str(infoLog);
			delete[] infoLog;
			return str;
		}
		return "";
	}
	
} // namespace roe
//*/
