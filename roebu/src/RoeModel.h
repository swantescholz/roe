#ifndef ROEMODEL_H_
#define ROEMODEL_H_

#include "RoeVector3.h"
#include "RoeMatrix.h"
#include "RoeBoundingBox.h"
#include "RoeCommon.h"
#include "RoeVertexBufferObject.h"
#include "RoeObject.h"
#include "RoeTexture.h"
#include "RoeMesh.h"
#include <GL/gl.h>
#include <vector>
#include <map>

namespace roe {
	
	struct Trimesh {
		std::vector<Vector3> vertices;
		std::vector<std::vector<long>> indices;
	};
	struct S_ModelIntern {      //only for the Model class
		VertexBufferObject *vbo;
		Trimesh *trimesh; //for trimesh physics
		//BoundingBox bb;
		//float radius;
	};
	//lighting + texture2d works now:
	//best automatic tex gen:
	/*
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	float currentCoeff1[4] = {1.0,0.0,1.0,0.0};
	float currentCoeff2[4] = {0.0,1.0,1.0,0.0};
	int currentGenMode = GL_OBJECT_LINEAR;
	int currentPlane = GL_OBJECT_PLANE;
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, currentGenMode);
	glTexGenfv(GL_S, currentPlane, currentCoeff1);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, currentGenMode);
	glTexGenfv(GL_T, currentPlane, currentCoeff2);
	*/
	class Model : public Object//a class for (OFF and OBJ) models
	{
		
	public:
		Model();
		Model(const std::string& s, bool bCompleteThePath = true); //calls load
		Model(const Model& rhs);
		virtual ~Model();
		
		Model& operator = (const Model& rhs);
		
		void load(const std::string& s, bool bCompleteThePath = true);   //load the right model
		void render();                     //renders the fitting display list from the map
		void renderAlpha();                //renders first the backside (front-face culled) then the front-side
		
		VertexBufferObject* getVBO() {return m_pVBO;}
		std::string getName  () const {return m_sName;}
		std::string getPath  () const {return m_sPath;}
		Trimesh*   getTrimesh() const {return m_pTrimesh;}
		std::vector<Vector3>& getDataPosition() const {return m_pVBO->getPosition();}
		std::vector<Vector3>& getDataNormal  () const {return m_pVBO->getNormal  ();}
		std::vector<Vector2>& getDataTexCoord() const {return m_pVBO->getTexCoord();}
		float     getRadius () const {return m_fRadius;}
		Vector3   getBBoxMin() const {return m_vBBoxMin;}
		Vector3   getBBoxMax() const {return m_vBBoxMax;}
		
		// for the static variables
		static void init(std::string sStart, std::string sEnd)  {s_sPathStart = sStart; s_sPathEnd = sEnd;}
		static void setPathStart(std::string s)                 {s_sPathStart = s;}
		static void setPathEnd  (std::string s)                 {s_sPathEnd   = s;}
		static std::string getPathStart()                       {return s_sPathStart;}
		static std::string getPathEnd  ()                       {return s_sPathEnd;}
		static void quit();
		
		//function(s) functions to load models from files
		//[all models should look in the positive z-axis; with center on zero; length on z-axis=2!]
		//[up vector should be positive y-axis, the surfaces must be CCW]
		static void loadOBJ(std::string sPath, bool bCompleteThePath = true, const Matrix& mTransform = Matrix::IDENTITY);
		static void loadOBJ(std::initializer_list<std::string> paths, bool bCompleteThePath = true, const Matrix& mTransform = Matrix::IDENTITY);
		static void loadOBJ(std::string sTmpStdPath, std::initializer_list<std::string> paths,
		                    bool bCompleteThePath = true, const Matrix& mTransform = Matrix::IDENTITY);
		static void loadHeightmap(const Texture& texture, std::string idName, bool bCompleteThePath = true, bool smoth = true, const Matrix& mTransform = Matrix::IDENTITY); //creates a heightfield-terrain from an image
		static void deleteModel(std::string sPath, bool bCompleteThePath = true); //deletes one model 
		
		//std. renderings:
		static void drawSphere(const Vector3& pos, float r);
		static void drawBox(const Vector3& pos, float size);
		static void drawBox(const Vector3& pos, const Vector3& size);
	protected:
		
		std::string m_sPath;   //complete path
		std::string m_sName;   //just the name
		VertexBufferObject *m_pVBO;
		Trimesh *m_pTrimesh; //pointer to the trimesh-data
		Vector3 m_vBBoxMin, m_vBBoxMax, m_v;
		float m_fRadius;
		
	private:
		static std::map <std::string, S_ModelIntern>           s_mpModel;   //map with all display lists
		static std::map <std::string, S_ModelIntern>::iterator s_mpi;       //map iterator
		static std::pair<std::string, S_ModelIntern>           s_pair;      //pair object for the map
		static std::string s_sPathStart, s_sPathEnd;                        //standard path
	};
	
} // namespace roe

#endif /*ROEMODEL_H_*/
