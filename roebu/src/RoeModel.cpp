#include "RoeModel.h"
#include "RoeException.h"
#include "RoeUtil.h"
#include "RoeTextfile.h"
#ifdef ROE_USE_SHADERS
	#include "RoeProgram.h"
#endif
#include <iostream>
#include <fstream>
#include <sstream>

namespace roe {
	
	std::map <std::string, S_ModelIntern> Model::s_mpModel;
	std::map <std::string, S_ModelIntern>::iterator Model::s_mpi;
	std::pair<std::string, S_ModelIntern> Model::s_pair;
	std::string Model::s_sPathStart, Model::s_sPathEnd = ".obj";
	
	Model::Model() {
		
	}
	Model::Model(const Model& rhs) {
		(*this) = rhs;
	}
	Model& Model::operator= (const Model& rhs) {
		Object::operator=(rhs);
		m_sPath = rhs.m_sPath;
		m_sName = rhs.m_sName;
		m_pVBO     = rhs.m_pVBO;
		m_pTrimesh = rhs.m_pTrimesh;
		update();
		return (*this);
	}
	Model::~Model() {}
	
	Model::Model(const std::string& s, bool bCompleteThePath) {
		load(s, bCompleteThePath);
	}
	
	void Model::load(const std::string& s, bool bCompleteThePath) {
		std::string sPath;
		if (bCompleteThePath)
			sPath = s_sPathStart + s + s_sPathEnd;
		else
			sPath = s;
		
		s_mpi = s_mpModel.find(sPath);
		if (s_mpi == s_mpModel.end()) {
			roe_except("model requested to be loaded does not exist: " + sPath, "load");
		}
		
		m_sPath = sPath;
		m_sName = s;
		m_pVBO      = s_mpModel[m_sPath].vbo;
		m_pTrimesh  = s_mpModel[m_sPath].trimesh;
		
		update();
	}
	
	void Model::render() {
		const Matrix m = getMatrix();
#ifdef ROE_USE_SHADERS
		const Matrix n = getRotationMatrix();
		Program::setStdTransformationMatrix(m);
		Program::setStdNormalMatrix(n);
#else
		glPushMatrix();
			glMultMatrixf(m.n);
#endif
			m_pVBO->enable();
				m_pVBO->draw(GL_TRIANGLES);
			m_pVBO->disable();
#ifndef ROE_USE_SHADERS
		glPopMatrix();
#endif
	}
	void Model::renderAlpha() {
		cout << "renderA" << endl;
		glPushMatrix();
		glPushAttrib(GL_POLYGON_BIT);
			glMultMatrixf(m_mMatrix.n);
			//const int dispList = s_mpModel[m_sPath].iDispList;
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
			//glCallList(dispList); //rendering the back
			glCullFace(GL_BACK);
			//glCallList(dispList); //rendering the front
		glPopAttrib();
		glPopMatrix();
	}
	
	void Model::quit() {
		for(s_mpi=s_mpModel.begin(); s_mpi!=s_mpModel.end(); ++s_mpi) {
			s_mpi->second.trimesh->vertices.clear();
			s_mpi->second.trimesh->indices.clear();
			delete s_mpi->second.trimesh;
			s_mpi->second.vbo->destroy();
			delete s_mpi->second.vbo;
		}
		s_mpModel.clear();
	}
	void Model::deleteModel(std::string sPath, bool bCompleteThePath){
		if (bCompleteThePath)
			sPath = s_sPathStart + sPath + s_sPathEnd;
		
		s_mpi = s_mpModel.find(sPath);
		if (s_mpi == s_mpModel.end()) {
			roe_except("model to be deleted does not exist: " + sPath, "deleteOFF");
		} else {
			s_mpi->second.trimesh->vertices.clear();
			s_mpi->second.trimesh->indices.clear();
			delete s_mpi->second.trimesh;
			s_mpi->second.vbo->destroy();
			delete s_mpi->second.vbo;
			s_mpModel.erase(s_mpi);
		}
	}
		
	void Model::loadOBJ(std::initializer_list<std::string> paths, bool bCompleteThePath, const Matrix& mTransform) {
		for (auto it = paths.begin(); it != paths.end(); ++it) {
			loadOBJ(*it, bCompleteThePath, mTransform);
		}
	}
	void Model::loadOBJ(std::string sTmpStdPath, std::initializer_list<std::string> paths, bool bCompleteThePath, const Matrix& mTransform) {
		std::string s;
		for (auto it = paths.begin(); it != paths.end(); ++it) {
			s = sTmpStdPath+(*it);
			loadOBJ(s, bCompleteThePath, mTransform); 
		}
	}
	void Model::loadOBJ(std::string sPath, bool bCompleteThePath, const Matrix& mTransform) {
		if (bCompleteThePath)
			sPath = s_sPathStart + sPath + s_sPathEnd;
		
		s_mpi = s_mpModel.find(sPath);
		if (s_mpi != s_mpModel.end())
			roe_except("tried to load a model again: " + sPath, "loadOBJ");
			
		Textfile tf;
		tf.readAt(sPath);
		stringstream ss;
		const long ciNumLines = tf.length();
		std::vector<std::string> vsP, vsN, vsTC, vsF;
		std::string *asLines = new std::string[ciNumLines];
		tf.getTextAsArray(asLines);
		tf.clear();
		
		//loading vertex and face strings:
		for (long i = 0; i < ciNumLines; ++i) {
			     if (util::stringBeginsWith(asLines[i], "vt")) vsTC.push_back(asLines[i].substr(3));
			else if (util::stringBeginsWith(asLines[i], "vn")) vsN.push_back (asLines[i].substr(3));
			else if (util::stringBeginsWith(asLines[i], "v" )) vsP.push_back (asLines[i].substr(2));
			else if (util::stringBeginsWith(asLines[i], "f" )) vsF.push_back (asLines[i].substr(2));
		}
		delete []asLines;
		long ciNumVertices = (long)vsP.size(), ciNumNormals = (long)vsN.size(), ciNumTexCoords = (long)vsTC.size(), ciNumFaces = (long)vsF.size();
		//determine face format:
		int vertexOrder = 0; //0(v), 1(vn), 2(vt), 3(vtn)
		int vertexLength = 1;
		std::string sFirstFaceLine = vsF.at(0);
		if (sFirstFaceLine.find("//") != std::string::npos) {
			vertexOrder = 1;
			vertexLength = 2;
		}
		else if (sFirstFaceLine.find("/") != std::string::npos) {
			vertexLength = util::getNumberOfNumbersInString(sFirstFaceLine.substr(0,sFirstFaceLine.find(" ")));
			if (vertexLength == 2) vertexOrder = 2;
			else if (vertexLength == 3) vertexOrder = 3;
			else roe_except("OBJ model parse error " + sFirstFaceLine, "loadOBJ");
		}
		const bool hasTC = (vertexOrder==2 || vertexOrder==3);
		const bool hasN  = (vertexOrder==1 || vertexOrder==3);
		
		S_ModelIntern modTmp;     //tmp structure for the map
		modTmp.vbo = new VertexBufferObject();
		modTmp.trimesh = new Trimesh();
		modTmp.trimesh->vertices.reserve(ciNumVertices);
		modTmp.trimesh->indices.reserve(ciNumFaces);
		//modTmp.vFarthest =  Vector3(0.0f,0.0f,0.001f);
		//modTmp.vBoxMin   = -Vector3(0.001f,0.001f,0.001f);
		//modTmp.vBoxMax   =  Vector3(0.001f,0.001f,0.001f);
		
		//getting the vertex-vectors
		auto& vboP  = modTmp.vbo->getPosition();
		auto& vboTC = modTmp.vbo->getTexCoord();
		auto& vboN  = modTmp.vbo->getNormal();
		//auto& vboI  = modTmp.vbo->getIndex();
		
		Vector3 *avP = new Vector3[ciNumVertices];
		Vector3 *avN = new Vector3[ciNumNormals];
		Vector2 *avTC= new Vector2[ciNumTexCoords];
		std::vector<std::vector<long>> *avF = new std::vector<std::vector<long>>[ciNumFaces];
		std::vector<long> vlTmp(vertexLength);
		
		Vector3 vTmp;
		Vector2 vTmp2;
		std::string sLine;
		
		//loading vertices:
		for (long i = 0; i < ciNumVertices; i++) {
			ss.clear();
			ss.str(vsP[i]);
			ss >> vTmp.x >> vTmp.y >> vTmp.z;
			vTmp = Matrix::transformCoords(vTmp, mTransform);
			/*if(vTmp.squaredLength() >= modTmp.vFarthest.squaredLength()) { //serch longest vector
				modTmp.vFarthest = vTmp;
			}
			modTmp.vBoxMin = Vector3::floor(modTmp.vBoxMin, vTmp);
			modTmp.vBoxMax = Vector3::ceil (modTmp.vBoxMax, vTmp);//*/
			avP[i] = vTmp;
			modTmp.trimesh->vertices.push_back(avP[i]);//PYHSICS
		}
		for (long i = 0; i < ciNumNormals; i++) {
			ss.clear();
			ss.str(vsN[i]);
			ss >> vTmp.x >> vTmp.y >> vTmp.z;
			vTmp = Matrix::transformNormal(vTmp, mTransform);
			avN[i] = vTmp;
		}
		for (long i = 0; i < ciNumTexCoords; i++) {
			ss.clear();
			ss.str(vsTC[i]);
			ss >> vTmp2.x >> vTmp2.y;
			vTmp2 = Matrix::transformCoords(vTmp2, mTransform);
			avTC[i] = vTmp2;
		}
		
		for (long i = 0; i < ciNumFaces; i++) {
			sLine = vsF[i];
			sLine = util::replaceAnyWith(sLine, "//", " ");
			sLine = util::replaceAnyWith(sLine, "/", " ");
			
			ss.clear();
			ss.str(sLine);
			switch (vertexOrder) {
				case 0:
					while (ss.good()) {
						ss >> vlTmp[0];
						avF[i].push_back(vlTmp);
					}
					break;
				case 1: case 2:
					while (ss.good()) {
						ss >> vlTmp[0] >> vlTmp[1];
						avF[i].push_back(vlTmp);
					}
					break;
				case 3:
					while (ss.good()) {
						ss >> vlTmp[0] >> vlTmp[1] >> vlTmp[2];
						avF[i].push_back(vlTmp);
					}
					break;
				default: break;
			}
		}
		Vector3 *triangleP, *triangleN, *oldN;
		Vector2 *triangleTC;
		std::vector<Vector3*> triangles;
		std::vector<Vector3*> normals;
		std::vector<Vector2*> texcoords;
		
		for (int i = 0; i < ciNumFaces; i++) {
			if((int)(avF[i].size()) >= 5)
				roe_except("(loading model '" + sPath + "') faces with more than 4 vertices are not supported", "loadOBJ");
			if((int)(avF[i].size()) <= 2) {
				roe_except("(loading model '" + sPath + "') faces with less than 3 vertices are not supported", "loadOBJ");
			}
			if((int)(avF[i].size()) >= 3) {
				triangleP = new Vector3[3];
				triangleN = new Vector3[3];
				if(hasTC) triangleTC= new Vector2[3];
				for(int j=0;j<3;j++) {
					triangleP [j] = avP[avF[i][j][0]-1];
					if (hasN ) triangleN [j] = avN [avF[i][j][hasTC?2:1]-1];
					/*triangleN[0] = Vector3::normalize(Vector3::cross(triangleP[1]-triangleP[0], triangleP[2]-triangleP[0]));
					triangleN[1] = triangleN[0];
					triangleN[2] = triangleN[0];//*/
					if (hasTC) triangleTC[j] = avTC[avF[i][j][1]-1];
				}
				if (!hasN) { //if normal not in file specified, calculate it:
					triangleN[0] = Vector3::normalize(Vector3::cross(triangleP[1]-triangleP[0], triangleP[2]-triangleP[0]));
					triangleN[1] = triangleN[0];
					triangleN[2] = triangleN[0];
				}
				modTmp.trimesh->indices.push_back(std::vector<long>({avF[i][0][0]-1,avF[i][1][0]-1,avF[i][2][0]-1}));//PYHSICS
				triangles.push_back(triangleP);
				normals.push_back(triangleN);
				if (hasTC) texcoords.push_back(triangleTC);
				if((int)(avF[i].size()) == 4) {
					oldN = triangleN;
					triangleP = new Vector3[3];
					triangleN = new Vector3[3];
					if(hasTC) triangleTC= new Vector2[3];
					triangleP[0] = avP[avF[i][0][0]-1];
					triangleP[1] = avP[avF[i][2][0]-1];
					triangleP[2] = avP[avF[i][3][0]-1];
					modTmp.trimesh->indices.push_back(std::vector<long>({avF[i][0][0]-1,avF[i][2][0]-1,avF[i][3][0]-1}));//PYHSICS
					if (hasN) {
						triangleN [0] = avN [avF[i][0][hasTC?2:1]-1];
						triangleN [1] = avN [avF[i][2][hasTC?2:1]-1];
						triangleN [2] = avN [avF[i][3][hasTC?2:1]-1];
					} else {
						for (int j = 0; j < 3; j++) triangleN[j] = oldN[j]; 
					}
					if (hasTC) {
						triangleTC[0] = avTC[avF[i][0][1]-1];
						triangleTC[1] = avTC[avF[i][2][1]-1];
						triangleTC[2] = avTC[avF[i][3][1]-1];
					}
					triangles.push_back(triangleP);
					normals.push_back(triangleN);
					if (hasTC) texcoords.push_back(triangleTC);
				}
			}
		}
		
		//updating sizes
		ciNumFaces = (long)(triangles.size());
		ciNumVertices = 3*ciNumFaces;
		
		//filling the vbo:
		for (int i = 0; i < ciNumFaces; i++) {
			for (int j = 0; j < 3; j++) {
				//vboI.push_back((long)(vboP.size()));
				vboP.push_back(triangles[i][j]);
				vboN.push_back(normals[i][j]);
				if (hasTC) vboTC.push_back(texcoords[i][j]);
			}
		}
		modTmp.vbo->create(GL_STATIC_DRAW);
		
		//add to map
		s_pair = make_pair(sPath, modTmp);
		s_mpModel.insert(s_pair);
		
		//cleaning up
		for(int i = 0; i < ciNumFaces; i++) {
			delete[] triangles[i];
			delete[] normals[i];
			if (hasTC) delete[] texcoords[i];
		}
		triangles.clear();
		normals.clear();
		texcoords.clear();
		
		delete[] avP;
		delete[] avN;
		delete[] avTC;
		delete[] avF;
	}
	
	void Model::loadHeightmap(const Texture& texture, std::string idName, bool bCompleteThePath, bool smooth, const Matrix& mTransform) {
		static const float inv = 1.0f/255.0f;
		if (bCompleteThePath) idName = s_sPathStart + idName + s_sPathEnd;
		
		const int w = texture.getWidth();
		const int h = texture.getHeight();
		
		S_ModelIntern modTmp;     //tmp structure for the map
		modTmp.vbo = new VertexBufferObject();
		//modTmp.vFarthest =  Vector3(0.0f,0.0f,0.001f);
		//modTmp.vBoxMin   = -Vector3(0.001f,0.001f,0.001f);
		//modTmp.vBoxMax   =  Vector3(0.001f,0.001f,0.001f);
		
		Vector3 pos;
		const int bpp = texture.getBPP();
		byte* pixels = texture.getData();
		
		std::vector<Vector3> vP;
		Color col;
		
		//positions
		for (int i = h-1; i >= 0; i--) {
			for (int j = 0; j < w; j++) {
				col.r = (int)pixels[(i*w+j)*bpp+0]*inv;
				col.g = (int)pixels[(i*w+j)*bpp+1]*inv;
				col.b = (int)pixels[(i*w+j)*bpp+2]*inv;
				col.a = (int)pixels[(i*w+j)*bpp+3]*inv;
				pos.y = (col.brightness()) * col.a;
				pos.x = -2*(j-0.5*(w-1))/(w-1);
				pos.z =  2*(i-0.5*(h-1))/(h-1);
				pos = Matrix::transformCoords(pos, mTransform);
				vP.push_back(pos);
			}
		}
		
		delete[] pixels;
		
		Mesh mesh;
		mesh.build(vP.data(), w, h);
		mesh.setTexCoords(Vector2(0,0), 1.0/(w-1), -1.0/(h-1));
		mesh.calculateNormals();
		mesh.simplify();
		mesh.fillVBO(*(modTmp.vbo));
		mesh.destroy();
		
		vP.clear();
		
		//physics:
		auto &vecPos = modTmp.vbo->getPosition();
		auto &vecInd = modTmp.vbo->getIndex();
		modTmp.trimesh = new Trimesh;
		modTmp.trimesh->vertices.resize(vecPos.size());
		modTmp.trimesh->indices.reserve(vecInd.size()/3);
		unsigned long index = 0;
		for (auto i = vecPos.begin(); i!=vecPos.end(); ++i) {
			modTmp.trimesh->vertices[index++] = *i;
		}
		for (unsigned long i = 0; i < vecInd.size(); i+=3) {
			modTmp.trimesh->indices.push_back(std::vector<long>({vecInd[i+0],vecInd[i+1],vecInd[i+2]}));
		}
		
		modTmp.vbo->create(GL_STATIC_DRAW);
		
		//add to map
		s_pair = make_pair(idName, modTmp);
		s_mpModel.insert(s_pair);
	}
	
	void Model::drawSphere(const Vector3& pos, float r) {
		Model mod("sphere",true);
		mod.setScaling(Vector3(r));
		mod.setPosition(pos);;
		mod.render();
	}
	void Model::drawBox(const Vector3& pos, float size) {
		Model mod("cuboid",true);
		mod.setScaling(Vector3(size,size,size));
		mod.setPosition(pos);
		mod.render();
	}
	void Model::drawBox(const Vector3& pos, const Vector3& size) {
		Model mod("cuboid",true);
		mod.setScaling(size);
		mod.setPosition(pos);
		mod.render();
	}
	
} // namespace roe


