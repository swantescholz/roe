#ifndef CAPP_H_
#define CAPP_H_


#include "Roe.h"
#include <iostream>
#include <deque>
#include <list>
#include <array>

using namespace std;
using namespace roe;
using namespace physics;

class CApp : public Application
{
public:
	CApp();
	~CApp();
	
	virtual void run();
	void loadInputFile(string sFilename);
private:
	void initRoe();
	void initPhyScene();
	void quit();
	
	void drawArrowVector(Vector3 pos, Vector3 dir, Color col, Vector3 scale = Vector3(1,1,1));
	void drawBox(Vector3 pos, float size);
	void drawBox(Vector3 pos, Vector3 size);
	void drawSphere(Vector3 pos, float r);
	void drawForceField(ForceField& s);
	void setGradient(string name);
	void setTexture(string name);
	void setColor(const Color& c);
	
	void addPhySphere  (Vector3 pos, float r         , float density = 1.0, Vector3 impulse = Vector3::ZERO);
	void addPhyCuboid  (Vector3 pos, Vector3 halfSize, float density = 1.0, Vector3 impulse = Vector3::ZERO);
	void addPhyCylinder(Vector3 pos, Vector3 halfSize, float density = 1.0, Vector3 impulse = Vector3::ZERO);
	void addPhyCone    (Vector3 pos, float r, float h, float density = 1.0, Vector3 impulse = Vector3::ZERO);
	void addPhyCapsule (Vector3 pos, float r, float h, float density = 1.0, Vector3 impulse = Vector3::ZERO);
	void addPhyTrimesh       (std::string path, Vector3 pos, float mass = 1.0, Vector3 impulse = Vector3::ZERO);
	void addPhyStaticTrimesh (std::string path, Vector3 pos, float mass = 1.0, Vector3 impulse = Vector3::ZERO);
	void addPhy(Vector3 impulse);
	void addSplinter(Vector3 impulse);
	void addCompoundSplinter(std::string path, Vector3 pos, float density, Vector3 impulse);
	
	float m_fCamSpeed, m_fCamRotSpeed;
	bool  m_bShow2DText;
	float m_fCoordSystemArrowSize;
	float m_fFPS;
	Vector3 m_vGravity;
	float m_fMinFPS;
	float m_fPhyDensity, m_fPhyImpulseStrength;
	int m_ciFPS; //frames per second for screenmovie
	float m_fElapsed, m_fFrameTime;
	int m_iNumSplits;
	std::list<Light*> m_lpLights;
	bool m_bWireframe;
	SDL_GL *pgl;
	Camera *m_pCamera;
	Timer m_fpsTimer;
	Font *m_pFont;
	
	Program m_ProgStd, m_ProgStdTex;
	//Program m_ProgIsosurface, m_ProgFractal;
	Nurbs   m_Nurb;
	Terrain m_Terrain;
	Engine *m_pEngine;
	Splinter *m_pSplinter; //tmp splinter ptr
	std::list<Body*> m_lpPhyMod, m_lpPhyStatic; //all physical models
	std::list<Splinter*> m_lpSplinter;
	std::vector<Vector3> m_vSplinterPoints;
	Body *m_pPhyTmp;
};



#endif /*CAPP_H_*/
