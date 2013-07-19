#include "CApp.h"
#include <array>
#define NUM_PLANETS 3

CApp::CApp()
{
	m_fCamSpeed    = 12.0f;
	m_fCamRotSpeed = 60.0f;
	m_fFPS = 0.0f;
	m_fCoordSystemArrowSize = 1.0f;
	m_bShow2DText = true;
	m_vGravity = Vector3(0,-9.81,0);
	Program::init("shaders/", ".vert", ".frag");
	m_fFrameTime = 0.0;
	m_pEngine = nullptr;
	m_pPhyTmp = nullptr;
	m_pCamera = nullptr;
	m_pFont = nullptr;
	pgl = nullptr;
	m_pSplinter = nullptr;
	m_bWireframe = false;
}


CApp::~CApp() {
}

void CApp::run() {
	initRoe();
	initPhyScene();
	
	while (pgl->mouseQuit() == false && pgl->keyWasDown(KC_ESCAPE) == false && pgl->keyWasDown(KC_F4) == false) {
		
		if (pgl->mouseWasDown(MB_R)) {pgl->toggleFullscreen();}
		
		pgl->clearBuffers();
		pgl->resetModelViewMatrix();
		pgl->updateEvents();
		Timer::updateAll();
		if (m_ciFPS > 0)
			m_fElapsed = 1.0/60;
		else
			m_fElapsed = Timer::getElapsed();
		if (1.0f/m_fElapsed < m_fMinFPS && pgl->getCurrentTime() > 5.5f) break; //breaking program because of FPS
		
		if (m_fpsTimer.expired()) {m_fpsTimer.restart(); m_fFPS = 1.0f/m_fElapsed; Font::clearStringTextureMemory();}
		if      (pgl->keyIsDown(KC_ARR_LEFT )) {m_pCamera->yaw  (m_fElapsed * -ROE_DEG_TO_RAD(m_fCamRotSpeed));}
		else if (pgl->keyIsDown(KC_ARR_RIGHT)) {m_pCamera->yaw  (m_fElapsed *  ROE_DEG_TO_RAD(m_fCamRotSpeed));}
		if      (pgl->keyIsDown(KC_ARR_UP   )) {m_pCamera->pitch(m_fElapsed *  ROE_DEG_TO_RAD(m_fCamRotSpeed));}
		else if (pgl->keyIsDown(KC_ARR_DOWN )) {m_pCamera->pitch(m_fElapsed * -ROE_DEG_TO_RAD(m_fCamRotSpeed));}
		if      (pgl->keyIsDown(KC_C        )) {m_pCamera->pos+= Vector3::normalize(m_pCamera->dir)*(m_fElapsed *  m_fCamSpeed);}
		else if (pgl->keyIsDown(KC_X        )) {m_pCamera->pos+= Vector3::normalize(m_pCamera->dir)*(m_fElapsed * -m_fCamSpeed);}
		if (pgl->keyIsDown(KC_X) || pgl->keyIsDown(KC_C)) {m_pCamera->up = Vector3(0,1,0);}
		if (pgl->keyWasDown(KC_R)) loadInputFile("input.txt");           //reload
		if (pgl->keyWasDown(KC_P)) pgl->saveScreenshot("res/screenshots/screenshot"); //take a screenshot
		if (pgl->keyWasDown(KC_1)) {
			if(m_bWireframe) {glEnable( GL_CULL_FACE); glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );}
			else {glDisable(GL_CULL_FACE); glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );}
			m_bWireframe=!m_bWireframe;
		}
		
		const Vector3 halfSize = Vector3(1.0,1.0,1.0), impulse = m_fPhyImpulseStrength*Vector3::normalize(m_pCamera->dir);
		const float phyR      = 1.0f;
		const float phyH      = 1.0f;
		if(pgl->keyWasDown(KC_A)) {addPhySphere  (m_pCamera->pos, phyR        , m_fPhyDensity, impulse);}
		if(pgl->keyWasDown(KC_S)) {addPhyCuboid  (m_pCamera->pos, halfSize    , m_fPhyDensity, impulse);}
		if(pgl->keyWasDown(KC_D)) {addPhyCylinder(m_pCamera->pos, halfSize    , m_fPhyDensity, impulse);}
		if(pgl->keyWasDown(KC_F)) {addPhyCone    (m_pCamera->pos, phyR, 2*phyH, m_fPhyDensity, impulse);}
		if(pgl->keyWasDown(KC_G)) {addPhyCapsule (m_pCamera->pos, phyR, 2*phyH, m_fPhyDensity, impulse);}
		if(pgl->keyWasDown(KC_K)) {addPhyTrimesh ("arrow", m_pCamera->pos, 5.0 , impulse);}
		if(pgl->keyWasDown(KC_J)) {addCompoundSplinter("A", m_pCamera->pos, 0.4, impulse);}
		if(pgl->keyWasDown(KC_H)) {
			m_pSplinter = new Splinter(m_pEngine);
			for(int i = 0; i < (int)m_vSplinterPoints.size(); ++i) {
				m_pSplinter->addPoint(m_vSplinterPoints[i]);
			}
			m_pSplinter->finalize(m_pCamera->pos, 0.1);
			addSplinter(impulse);
		}
		if(pgl->keyWasDown(KC_W)) {
			if (m_lpPhyMod.size() > 0) {
				delete m_lpPhyMod.front();
				m_lpPhyMod.pop_front();
			}
		}
		if(pgl->keyWasDown(KC_E)) {
			while(m_lpPhyMod.size() > 0) {
				delete m_lpPhyMod.front();
				m_lpPhyMod.pop_front();
			}
		}
		if (pgl->keyWasDown(KC_T)) {
			Plane p(Vector3::createRandomBox(Vector3(-2),Vector3(2)), Vector3::createRandomUnit());
			for(auto i = m_lpSplinter.begin() ; i != m_lpSplinter.end() ; ++i) {(*i)->split(p);}
		}
		
		//DO PHYSICS:
		m_pEngine->doPhysics();
		
		//CAMERA
		Matrix cameraMatrix = pgl->applyCameraToScene(*m_pCamera);
		Matrix projectionMatrix = pgl->getProjectionMatrix();
		Matrix cameraProjectionMatrix = cameraMatrix*projectionMatrix;
		
		//LIGHT
		for(auto i = m_lpLights.begin(); i != m_lpLights.end(); ++i) {(*i)->light();}
		
		//setting global uniforms
		Program::setGlobalUniform("uCameraPosition", m_pCamera->pos);
		Program::setGlobalUniform("uCameraDirection", m_pCamera->dir);
		Program::setGlobalUniform("uCameraMatrix", cameraMatrix);
		Program::setGlobalUniform("uProjectionMatrix", projectionMatrix);
		Program::setGlobalUniform("uCameraProjectionMatrix", cameraProjectionMatrix);
		Program::setGlobalUniform("uFogColor", Color(.8,.8,.8,1));
		Program::setGlobalUniform("uFogDensity", 0.0006f);
		Program::setGlobalUniform("uElapsedTime", m_fElapsed);
		Program::setGlobalUniform("uTimeSinceInit", Timer::getTimeSinceInit());
		Program::setGlobalUniform("uSeparateSpecular", false);
		
		//SKYBOX
		Program::unuse();
		pgl->renderSkyBox(Vector3(25536.0), m_pCamera->pos, "cubemaps/redwinter", "_up","_dn","_lt","_rt","_ft","_bk",Color(1,1,1,1));
		m_ProgStd.use();
		//COORDINATE SYSTEM
		Material::WHITE.render();
		const Vector3 coordsCenter = Vector3(0,0,0);
		drawArrowVector(coordsCenter, Vector3(1,0,0), Color(1,0,0), Vector3(m_fCoordSystemArrowSize));
		drawArrowVector(coordsCenter, Vector3(0,1,0), Color(0,1,0), Vector3(m_fCoordSystemArrowSize));
		drawArrowVector(coordsCenter, Vector3(0,0,1), Color(0,0,1), Vector3(m_fCoordSystemArrowSize));
		
		m_ProgStd.use();
		Material::MELLOW.render(); for(auto i = m_lpPhyStatic.begin(); i != m_lpPhyStatic.end(); ++i) {(*i)->render();}
		Material::WHITE.render() ; for(auto i = m_lpPhyMod.begin()   ; i != m_lpPhyMod.end()   ; ++i) {(*i)->render();}
		Material::SKY.render()   ; for(auto i = m_lpSplinter.begin() ; i != m_lpSplinter.end() ; ++i) {(*i)->render();}
		m_ProgStdTex.use(); //TEXTURE STD PROGRAM
		
		Material::WHITE.render();
		/*setGradient("gradient5");
		setTexture("cubemaps/sand_dn");
		Model bunny("bunny");
		bunny.setScaling(Vector3(15));
		bunny.render();//*/
		//setTexture("cubemaps/sand_dn");
		//setTexture("heightmaps/colored2");
		//m_Terrain.render(camera, cameraProjectionMatrix);
		//*/
		Program::unuse(); //PROGRAM END
		if (m_bShow2DText) {
			std::string sText;
			sText += "FPS: " + util::toString(m_fFPS);
			//sText += "\n" + m_Terrain.quadtreeLODsToString();
			std::vector<std::string> vsText;
			util::stringToLines(sText, vsText);
			Font::begin2D();
				Vector2 wh(0,0), pos(0,pgl->getWindowH() - 20);
				Color col(.9,.7,.5,.8);
				for(auto it = vsText.begin(); it != vsText.end(); ++it) {
					m_pFont->renderText2(*it, col, pos, &wh, false); pos.y -= wh.y;
				}
			Font::end2D();
			vsText.clear();	
		}
		//screenmovie:
		if (m_ciFPS > 0) {
			m_fFrameTime += m_fElapsed;
			if(m_fFrameTime > 1.0/m_ciFPS) {
				m_fFrameTime -= 1.0/m_ciFPS;
				pgl->saveScreenshot("res/screenmovie/" + util::addZeros(util::toString(util::getNextNumber()), 4) + ".tga");
			}
		}
		
		pgl->flip();
	}
	
	quit();
}

void CApp::loadInputFile(string sFile) {
	//textfile rloading
	string sLine;
	Textfile tf;
	tf.readAt(sFile);
	tf.setText(util::deleteComments(tf.getText(), "//", "/*", "*/", true)); //deleting comments
	sLine = tf.getLine(0);
	string preheader = tf.getLine(0), uniPath = "universal_header";
	tf.popFront();
	preheader = util::replaceAnyWith(preheader, "\\", "\n");
	preheader += "\n";
	
	m_ProgStd        = Program("std"       , preheader, uniPath, false);
	m_ProgStdTex     = Program("std_tex"   , preheader, uniPath, false);
	//m_ProgFractal    = Program("fractal"   , preheader, uniPath, false);
	//m_ProgIsosurface = Program("isosurface", preheader, uniPath, false);
	m_ProgStd.install();
	m_ProgStdTex.install();
	Program::setStdNormalMatrix(Matrix::IDENTITY);
	Program::setStdTransformationMatrix(Matrix::IDENTITY);
	Program::setStdTextureMatrix(Matrix::IDENTITY);
	
	Tokenizer tok(tf.getText());
	m_fCamSpeed             = tok.readFloat(); tok.skipLine();
	m_fCamRotSpeed          = tok.readFloat(); tok.skipLine();
	m_fCoordSystemArrowSize = tok.readFloat(); tok.skipLine();
	m_vGravity.x            = tok.readFloat();
	m_vGravity.y            = tok.readFloat();
	m_vGravity.z            = tok.readFloat(); tok.skipLine();
	m_bShow2DText           = tok.readBool() ; tok.skipLine();
	m_fMinFPS               = tok.readFloat(); tok.skipLine();
	m_fPhyDensity           = tok.readFloat(true);
	m_fPhyImpulseStrength   = tok.readFloat(true);
	m_iNumSplits            = tok.readInt(true);
	m_ciFPS = tok.readInt(true);
	if(m_pEngine) initPhyScene(); //not the first time!
	Timer::updateAll();
}
void CApp::initRoe() {
	pgl = SDL_GL::getSingletonPtr();
	pgl->init(800,600,"bullet_test","bullet_test", "icon.tga", SDL_GL::F_LIGHTING | SDL_GL::F_ALPHA_BLENDING |
	                                               SDL_GL::F_TEXTURE2D | SDL_GL::F_CULL_SHOW_CCW);// | SDL_GL::F_FULLSCREEN); // 
	glEnable(GL_NORMALIZE);
	loadInputFile("input.txt");
	pgl->setClearColor(Color(0.5,0.7,0.9));
	Font::init("res/fonts/", ".ttf");
	Font::loadTTF("Ubuntu-R", 18,22);
	m_pFont = new Font("Ubuntu-R", 20);
	Gradient::init("res/gradients/", ".png");
	//Gradient::loadGradient("gradient", {"1","2","3","4","5","6","7","8","9","10","11","12"});
	Texture::init("res/textures/", ".png");
	Texture::loadTexture({"three", "ground", "ground2", "axiom", "player"});
	//Texture::loadTexture("cubemaps/env_map_"  , {"up","dn","lt","rt","ft","bk"});
	//Texture::loadTexture("cubemaps/irrlicht2_", {"up","dn","lt","rt","ft","bk"});
	//Texture::loadTexture("cubemaps/sand_"     , {"up","dn","lt","rt","ft","bk"});
	//Texture::loadTexture("cubemaps/chess_"    , {"up","dn","lt","rt","ft","bk"});
	//Texture::loadTexture("cubemaps/redboatpath_"  , {"up","dn","lt","rt","ft","bk"});
	//Texture::loadTexture("cubemaps/redislands_"   , {"up","dn","lt","rt","ft","bk"});
	//Texture::loadTexture("cubemaps/redrise_"      , {"up","dn","lt","rt","ft","bk"});
	//Texture::loadTexture("cubemaps/redsunset_"    , {"up","dn","lt","rt","ft","bk"});
	//Texture::loadTexture("cubemaps/redtropic_"    , {"up","dn","lt","rt","ft","bk"});
	Texture::loadTexture("cubemaps/redwinter_"    , {"up","dn","lt","rt","ft","bk"});
	Texture::loadTexture("particles/", {"fire", "sqfire", "snow"});
	//Texture::loadTexture("heightmaps/", {"big_hills4", "colored", "colored2", "colored3"});//,"craterlake","desert","desert2","eroded_mountains","eroded_mountains2","eroded_mountains3",
	                                     //"mountainlake","waterworld","waterworld2","grand_canyon", "colored"});
	Model::init("res/models/", ".obj");
	//Model::loadOBJ("dragon", true, Matrix::scaling(Vector3(10)));
	//Model::loadOBJ("elephant", true, Matrix::scaling(Vector3(10)));
	Model::loadOBJ("", {"plane","cylinder", "arrow", "sphere","cuboid","cone","tetrahedron"});//, "dragon", "elephant"});
	Model::loadOBJ("", {"A","B","E","I","N"});
	Model::loadOBJ("area/teapot", true, Matrix::scaling(40,10,40));
	Model::loadOBJ("area/area1", true, Matrix::scaling(20));
	//m_Terrain.create(Texture("heightmaps/colored2"), Matrix::scaling(Vector3(32,28,32)*18)*Matrix::translation(0,0,0));
	const Vector3 vCamStartPos = Vector3(1,1,1)*0.6*m_fCamSpeed;
	m_pCamera = new Camera(2,vCamStartPos, -vCamStartPos);
	m_fpsTimer.setLimit(2.0);
	Light *light = new Light(Vector3(2000,1500,0), true, true);
	light->setMaterial(Color(0.2,0.2,0.2,1),4*Color(.1,.1,.1,1),6*Color(.1,.1,.1,1));
	m_lpLights.push_back(light);
	light = new Light(Vector3(-2000,1500,0), true, true);
	light->setMaterial(Color(0.2,0.2,0.2,1),4*Color(.1,.1,.1,1),6*Color(.1,.1,.1,1));
	m_lpLights.push_back(light);
	Light::lightModel(Color(0.2,0.2,0.2,1), true, false);
	Timer::updateAll(); //->start m_fElapsed low
	if(m_bWireframe) glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	
	//PHYSICS:
	m_pEngine = new Engine();
	auto lsta = {"arrow","tetrahedron","A","B","E","I","N"}; //"elepahnt","dragon"
	auto ldyn = {"area/area1", "area/teapot"};
	for(auto i = lsta.begin(); i != lsta.end(); ++i) {m_pEngine->loadTrimesh(Model(*i),false);}
	for(auto i = ldyn.begin(); i != ldyn.end(); ++i) {m_pEngine->loadTrimesh(Model(*i),true);}//*/
}
void CApp::initPhyScene() {
	for(auto i = m_lpSplinter.begin() ; i != m_lpSplinter.end() ; ++i) {delete (*i);}
	for(auto i = m_lpPhyMod.begin()   ; i != m_lpPhyMod.end()   ; ++i) {delete (*i);}
	for(auto i = m_lpPhyStatic.begin(); i != m_lpPhyStatic.end(); ++i) {delete (*i);}
	m_lpSplinter.clear();
	m_lpPhyMod.clear();
	m_lpPhyStatic.clear();
	
	m_pPhyTmp = new Body(0, Vector3(0,-25,0));
	Model gfxPlane("plane"); gfxPlane.setScaling(1000.0*Vector3(1,0,1)+Vector3(0,1,0));
	m_pPhyTmp->setGfxModel(gfxPlane);
	m_pPhyTmp->create(m_pEngine->shapePlane(Vector3(0,1,0), 0.0));
	m_pEngine->addRigidBody(m_pPhyTmp);
	m_lpPhyStatic.push_back(m_pPhyTmp);
	m_pPhyTmp = new Body(0, Vector3(0,-10,0));
	Model gfxArea("area/teapot"); //gfxArea.setScaling(1.0*vec3::XZ+1.0*vec3::Y);
	m_pPhyTmp->setGfxModel(gfxArea);
	m_pPhyTmp->create(m_pEngine->shapeStaticTrimesh(gfxArea), false);
	m_pEngine->addRigidBody(m_pPhyTmp);
	m_lpPhyStatic.push_back(m_pPhyTmp);//*/
	
	int boxSize = 4;
	vec3 halfSize = Vector3(1,1,1)/2.05;
	for(int x = 0; x < boxSize; ++x) {
		for(int y = 0; y < boxSize; ++y) {
			for(int z = 0; z < boxSize; ++z) {
				//addPhyCylinder(1.0*vec3(x,y,z), vec3(1,1,0)*vec3(0.49)+0.49*vec3::Z, m_fPhyDensity, vec3::X*3);
				//addPhyCone    (1.0*vec3(x,2*y,z), 0.49, 2*0.49, m_fPhyDensity, vec3::X*0);
				//addPhyCapsule (1.0*vec3(x,2*y,z)*vec3(4,1,3), 0.49, 2*0.49, m_fPhyDensity, vec3::X*0);
				addPhySphere(1.0*vec3(x,y,z)*vec3(4,1,3),0.49,m_fPhyDensity,vec3::Y);
				//addPhyCuboid(1.0*vec3(x,y,z),halfSize,m_fPhyDensity,vec3::ZERO);
			}
		}
	}
	
	//splinter:
	m_vSplinterPoints.clear();
	for(int i = 0; i < 12; ++i) {
		const Vector3 v = Vector3::createRandomBox(Vector3(-1),Vector3(1))*2;
		m_vSplinterPoints.push_back(v);
	}//*/
	std::vector<std::string> a = {"N","E","I","B","A"};
	for(int i = 0; i < 5; ++i) {
		addCompoundSplinter(a[i], Vector3(0,8*(i+1),0), 0.4, vec3::Y*5);
	}
}
void CApp::quit() {
	m_vSplinterPoints.clear();
	ROE_SAFE_DELETE(m_pSplinter);
	for(auto i = m_lpPhyMod.begin()   ; i != m_lpPhyMod.end()   ; ++i) {ROE_SAFE_DELETE(*i);}
	for(auto i = m_lpPhyStatic.begin(); i != m_lpPhyStatic.end(); ++i) {ROE_SAFE_DELETE(*i);}
	for(auto i = m_lpSplinter.begin() ; i != m_lpSplinter.end() ; ++i) {ROE_SAFE_DELETE(*i);}
	for(auto i = m_lpLights.begin()   ; i != m_lpLights.end()   ; ++i) {ROE_SAFE_DELETE(*i);}
	m_lpPhyMod.clear();
	m_lpSplinter.clear();
	m_lpPhyStatic.clear();
	m_lpLights.clear();
	
	ROE_SAFE_DELETE(m_pFont);
	ROE_SAFE_DELETE(m_pCamera);
	ROE_SAFE_DELETE(m_pEngine);
	//m_Terrain.destroy();
	m_Nurb.destroy();
	Font::quit();
	Gradient::quit();
	Texture::quit();
	Model::quit();
	
	string tmp = "Time: " + util::toString(Timer::getTimeSinceInit()*1000);
	pgl->quit();
	cout << endl << tmp << endl;
}
void CApp::addSplinter(Vector3 impulse) {
	m_pSplinter->getRigidBody()->applyImpulse(impulse*m_pSplinter->getMass(), Vector3::ZERO);
	m_pSplinter->addToWorld(1<<14, ~(short)0^(1<<14));
	m_lpSplinter.push_back(m_pSplinter);
	m_pSplinter = nullptr;
}
void CApp::addCompoundSplinter(std::string path, Vector3 pos, float density, Vector3 impulse) {
	Model gfx(path);
	btCompoundShape *compoundShape = dynamic_cast<btCompoundShape*>(m_pEngine->shapeTrimesh(gfx));
	btConvexHullShape *hullShape = nullptr;
	btVector3 vertex;
	for(int i = 0; i < compoundShape->getNumChildShapes(); ++i) {
		hullShape = dynamic_cast<btConvexHullShape*>(compoundShape->getChildShape(i));
		if(hullShape->getNumVertices() <= 3) continue; //too few!
		const btTransform &transform = compoundShape->getChildTransform(i);
		m_pSplinter = new Splinter(m_pEngine);
		for(int j = 0; j < hullShape->getNumVertices(); ++j) {
			hullShape->getVertex(j, vertex);
			m_pSplinter->addPoint(Vector3(transform*vertex));
		}
		m_pSplinter->finalize(pos, density);
		m_pSplinter->getRigidBody()->applyImpulse(impulse*m_pSplinter->getMass(), Vector3::ZERO);
		for(int j = 0; j < m_iNumSplits; ++j) {
			m_pSplinter->split(Plane(Vector3::createRandomBox(pos+Vector3(-1),pos+Vector3(1)), Vector3::createRandomUnit()));	
		}
		m_pSplinter->addToWorld(((short)1)<<14, (~(short)0)^(1<<14));
		m_lpSplinter.push_back(m_pSplinter);
		m_pSplinter = nullptr;
	}
}
void CApp::addPhySphere(Vector3 pos, float r, float density, Vector3 impulse) {
	Model gfx("sphere"); gfx.setScaling(r*Vector3::ONE);
	const float mass = density*(ROE_PI*r*r*r*4.0/3.0);
	m_pPhyTmp = new Body(m_pEngine, gfx, mass, pos);
	m_pPhyTmp->create(m_pEngine->shapeSphere(r));
	addPhy(mass*impulse);
}
void CApp::addPhyCuboid(Vector3 pos, Vector3 halfSize, float density, Vector3 impulse) {
	Model gfx("cuboid"); gfx.setScaling(halfSize);
	const float mass = density*(8.0*halfSize.volume());
	m_pPhyTmp = new Body(m_pEngine, gfx, mass, pos);
	m_pPhyTmp->create(m_pEngine->shapeCuboid(halfSize));
	addPhy(mass*impulse);
}
void CApp::addPhyCylinder(Vector3 pos, Vector3 halfSize, float density, Vector3 impulse) {
	Model gfx("cylinder"); gfx.setScaling(halfSize);
	const float mass = density*(ROE_PI*2.0*halfSize.volume());
	m_pPhyTmp = new Body(m_pEngine, gfx, mass, pos);
	m_pPhyTmp->create(m_pEngine->shapeCylinder(halfSize));
	addPhy(mass*impulse);
}
void CApp::addPhyCone(Vector3 pos, float r, float h, float density, Vector3 impulse) {
	Model gfx("cone"); gfx.setScaling(r*Vector3(1,1,0)+0.5*h*Vector3(0,0,1));
	const float mass = density*(ROE_PI*r*r*0.5*h/3.0);
	m_pPhyTmp = new Body(m_pEngine, gfx, mass, pos);
	m_pPhyTmp->create(m_pEngine->shapeCone(r,h));
	addPhy(mass*impulse);
}
void CApp::addPhyCapsule(Vector3 pos, float r, float h, float density, Vector3 impulse) {
	Model gfx("sphere"); gfx.setScaling(r*Vector3(1,0,1)+h*Vector3(0,1,0));
	const float mass = density*(ROE_PI*r*r*h*4.0/3.0);
	m_pPhyTmp = new Body(m_pEngine, gfx, mass, pos);
	m_pPhyTmp->create(m_pEngine->shapeCapsule(r,h));
	addPhy(mass*impulse);
}
void CApp::addPhyTrimesh(std::string path, Vector3 pos, float mass, Vector3 impulse) {
	Model gfx(path); //gfx.setScaling(halfSize);
	m_pPhyTmp = new Body(m_pEngine, gfx, mass, pos);
	m_pPhyTmp->create(m_pEngine->shapeTrimesh(m_pPhyTmp->getGfxModel()), false);
	addPhy(mass*impulse);
}
void CApp::addPhyStaticTrimesh(std::string path, Vector3 pos, float mass, Vector3 impulse) {
	Model gfx(path); //gfx.setScaling(halfSize);
	m_pPhyTmp = new Body(m_pEngine, gfx, mass, pos);
	m_pPhyTmp->create(m_pEngine->shapeStaticTrimesh(m_pPhyTmp->getGfxModel()), false);
	addPhy(mass*impulse);
}
void CApp::addPhy(Vector3 impulse) {	
	m_pPhyTmp->getRigidBody()->applyImpulse(impulse, Vector3::ZERO);
	m_pEngine->addRigidBody(m_pPhyTmp->getRigidBody());
	m_lpPhyMod.push_back(m_pPhyTmp);
	m_pPhyTmp = nullptr;
}

//------
void CApp::drawArrowVector(Vector3 pos, Vector3 dir, Color col, Vector3 scale) {
	bool wasEnabled = glIsEnabled(GL_TEXTURE_2D);
	if (wasEnabled) glDisable(GL_TEXTURE_2D);
	Model arrow("arrow");
	arrow.multScaling(Vector3(.25,.25,1)*scale);
	setColor(col);
	arrow.align(dir, Vector3::ONE);
	arrow.translateAbs(dir*scale);
	arrow.translateAbs(pos);
	//Program::setGlobalUniform("uTransformationMatrix", arrow.getMatrix());
	arrow.render();//z
	if (wasEnabled) glEnable(GL_TEXTURE_2D);
}
void CApp::drawSphere(Vector3 pos, float r) {
	Model mod("sphere",true);
	mod.setScaling(Vector3(r));
	mod.setPosition(pos);
	//Program::setGlobalUniform("uTransformationMatrix", mod.getMatrix());
	mod.render();
}
void CApp::drawBox(Vector3 pos, float size) {
	Model mod("cuboid",true);
	mod.setScaling(Vector3(size));
	mod.setPosition(pos);
	mod.render();
}
void CApp::drawBox(Vector3 pos, Vector3 size) {
	Model mod("cuboid",true);
	mod.setScaling(size);
	mod.setPosition(pos);
	mod.render();
}
void CApp::drawForceField(ForceField& s) {
	string name = "";
	switch(s.getCollisionType()) {
		case 1:  name = "sphere";   break;
		case 2:  name = "cuboid";   break;
		case 3:  name = "cylinder"; break;
		case 4:  name = "torus";    break;
		default: name = "sphere";   break;
	}
	Model mod(name,true);
	mod.copyTransformation(&s);
	mod.render();
}
void CApp::setGradient(string name) {
	//Program::setGlobalUniform("uGradient", Gradient(name));
	Gradient(name).bind();
}
void CApp::setTexture(string name) {
	//Program::setGlobalUniform("uTexture", Texture(name));
	Texture(name).bind();
}
void CApp::setColor(const Color& c) {
	if (glIsEnabled(GL_LIGHTING))
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, c.rg);
	else
		glColor4fv(c.rg);
}
