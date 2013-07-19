#include "RoeGradient.h"
#include "RoeException.h"
#include "RoeUtil.h"
#include "RoeTextfile.h"
#include "RoeTexture.h"
#ifdef ROE_USE_SHADERS
#include "RoeProgram.h"
#endif
#include <GL/gl.h>

namespace roe {

	std::map <std::string, S_Gradient>           Gradient::s_mpGradient;
	std::map <std::string, S_Gradient>::iterator Gradient::s_mpi;
	std::pair<std::string, S_Gradient>           Gradient::s_pair;
	std::string Gradient::s_sPathStart, Gradient::s_sPathEnd = ".png";
	
	Gradient::Gradient(){}
	Gradient::Gradient(const std::string &sPath) {
		load(sPath);
	}
	Gradient::~Gradient()
	{}
	void Gradient::bind() const {
#ifdef ROE_USE_SHADERS
		Program::setStdGradient(s_mpGradient[m_sPath].id);	
#else
		glBindTexture(GL_TEXTURE_1D, s_mpGradient[m_sPath].id);
#endif
	}
	void Gradient::load(std::string sPath) {
		m_sName = sPath;
		sPath = s_sPathStart + sPath + s_sPathEnd;
		s_mpi = s_mpGradient.find(sPath);
		if (s_mpi == s_mpGradient.end()) {
			roe_except("angefragte Textur noch nicht geladen: " + sPath, "load");
		}
		m_sPath = sPath;
	}
	void Gradient::loadGradient(std::string sPath) {
		sPath = s_sPathStart + sPath + s_sPathEnd;
		S_Gradient tex;
		SDL_Surface* surface = IMG_Load(sPath.c_str());
		if (surface == nullptr) {
			roe_except("texture does not exist: " + sPath, "loadGradientIntoGL");
		}
		int direction = (surface->w >= surface->h) ? 0 : 1; //0->horizontal, 1->vertical
		tex.width  = (direction==0) ? surface->w : surface->h;
		tex.bpp    = surface->format->BytesPerPixel;
		if (tex.bpp < 1 || tex.bpp == 2 || tex.bpp > 4)
			roe_except(util::toString(tex.bpp) + " bytes per pixel not supported: " + sPath, "loadGradientIntoGL");
		long imageSize = tex.width * std::max(tex.bpp, 3);
		tex.data   = new GLubyte[imageSize];
		long pos = 0;
		byte r=0, g=0, b=0, a = 255;
		//cout << tex.bpp << " " << tex.width << " " << tex.height << " " << surface->pitch << endl;
		SDL_LockSurface(surface);
		byte *color = nullptr;
		for (int x = 0; x < tex.width; ++x) {
			if (direction == 0)
				color = &(((byte*)surface->pixels)[x*tex.bpp]);
			else
				color = &(((byte*)surface->pixels)[x*surface->pitch]);
			
			if (tex.bpp == 4) {
				//SDL_GetRGBA(color, surface->format, &r, &g, &b, &a);
				r = color[0];
				g = color[1];
				b = color[2];
				a = color[3];//*/
			}
			else if (tex.bpp == 3) {
				//SDL_GetRGB(color, surface->format, &r, &g, &b);
				r = color[0];
				g = color[1];
				b = color[2];//*/
			}
			else {
				//r = ((byte*)(&color))[0];
				r = color[0];
				g = r;
				b = r;
			}
			tex.data[pos++] = r;
			tex.data[pos++] = g;
			tex.data[pos++] = b;
			if (tex.bpp==4) tex.data[pos++] = a;
		}
		SDL_UnlockSurface(surface);
		SDL_FreeSurface(surface); //not needed anymore
		util::clamp(tex.bpp, 3,4);
		/*util::saveImageTGA(std::string("res/textures/mipmaps/map") + util::getNextNumber() + "_" + util::toString(tex.width)+"_"+
				util::toString(tex.height)+".tga", tex.data, tex.width, tex.height, tex.bpp==4);//*/
		buildMipmaps(&tex);
		loadGradientIntoGL(&tex);
		
		//expand map
		s_pair = make_pair(sPath, tex);
		s_mpGradient.insert(s_pair);
	}
	void Gradient::loadGradient(std::initializer_list<std::string> list) {
		for (auto it = list.begin(); it != list.end(); ++it)
			loadGradient(*it);
	}
	void Gradient::loadGradient(std::string sTmpStdPath, std::initializer_list<std::string> list) {
		for (auto it = list.begin(); it != list.end(); ++it)
			loadGradient(sTmpStdPath + *it);
	}
	/*void Gradient::loadGradient(BasicColorInterpolator& colInt, int w, std::string name) {
		name = s_sPathStart + name + s_sPathEnd;
		S_Gradient tex;
		tex.bpp = 4;
		tex.width = w;
		tex.data = new unsigned char[w*tex.bpp];
		const float inv = 1.0/(w-1);
		Color col;
		for (int x = 0; x < w; ++x) {
			col = colInt.interpolate(util::interpolateLinear(colInt.getMinTime(),colInt.getMaxTime(),x*inv));
			tex.data[x*tex.bpp+0] = (unsigned char)(col.r*256-0.001);
			tex.data[x*tex.bpp+1] = (unsigned char)(col.g*256-0.001);
			tex.data[x*tex.bpp+2] = (unsigned char)(col.b*256-0.001);
			tex.data[x*tex.bpp+3] = (unsigned char)(col.a*256-0.001);
		}
		
		buildMipmaps(&tex);
		loadGradientIntoGL(&tex);
		//expand map
		s_pair = std::make_pair(name, tex);
		s_mpGradient.insert(s_pair);
	}//*/
	void Gradient::loadGradientIntoGL(S_Gradient *tex) {
		GLuint id;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_1D, id);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, (!tex->next_mipmap) ? GL_LINEAR : GL_LINEAR_MIPMAP_LINEAR);
		const int type = (tex->bpp==4) ? GL_RGBA : GL_RGB;
		bool oldWasPOT = util::isPOT(tex->width); //was gradient before power of two?
		if (!tex->next_mipmap) {
			//we've just one texture to go with; bad idea if NPOT! ;(
			glTexImage1D(GL_TEXTURE_1D, 0, type, tex->width, 0, type, GL_UNSIGNED_BYTE, tex->data);
			tex->id = id;
			return;
		}
		else if (!oldWasPOT) {
			tex->id = id;
			tex = tex->next_mipmap; //first NPOT texture not needed, so skip it
		}
		
		for (int i = 0; tex; ++i) {
			/*util::saveImageTGA(std::string("res/textures/mipmaps/mip") + util::getNextNumber() + "_" + util::toString(tex->width)+"_"+
				util::toString(tex->height)+".tga", tex->data, tex->width, tex->height, tex->bpp==4);//*/
			glTexImage1D(GL_TEXTURE_1D, i, type, tex->width, 0, type, GL_UNSIGNED_BYTE, tex->data);  //create mipmap in GL
			tex->id = id; //setting the id
			tex = tex->next_mipmap;
		}
	}
	void Gradient::buildMipmaps(S_Gradient *tex, int numMipmaps) {
		//MIPMAPPING
		if (numMipmaps < 0) { //auto calculate num mipmaps
			for (numMipmaps = 0; 1<<numMipmaps < tex->width; numMipmaps++) {}
		}
		S_Gradient *subtex1 = tex, *subtex2 = nullptr;
		for (int i = 1; i <= numMipmaps; i++) { //scaling for each mipmap
			subtex2 = new S_Gradient;
			subtex2->bpp = subtex1->bpp;
			subtex2->width  = util::previousPOT(subtex1->width);
			subtex2->data = new byte[subtex2->width*subtex2->bpp];
			Texture::scaleImage1(subtex1->data, subtex1->width, subtex2->data, subtex2->width, subtex1->bpp); //scaling image to new size
			subtex1->next_mipmap = subtex2; //linking
			subtex1 = subtex2; //one level deeper
		}
	}
	void Gradient::deleteGradient (std::string sPath, bool bCompleteThePath) {
		if (bCompleteThePath)
			sPath = s_sPathStart + sPath + s_sPathEnd;
		
		s_mpi = s_mpGradient.find(sPath);
		if (s_mpi == s_mpGradient.end()) {
			roe_except("texture to be deleted does not exist: " + sPath, "deleteGradient");
		} else {
			glDeleteTextures(1, &s_mpi->second.id);
			s_mpi->second.destroy();
			s_mpGradient.erase(s_mpi);
		}
	}
	void Gradient::init(std::string sStart, std::string sEnd) {
		s_sPathStart  = sStart;
		s_sPathEnd    = sEnd;
	}
	void Gradient::quit() {
		for(s_mpi=s_mpGradient.begin(); s_mpi!=s_mpGradient.end(); ++s_mpi) {
			glDeleteTextures(1, &s_mpi->second.id);
			s_mpi->second.destroy();
		}
		s_mpGradient.clear();
	}
	
} // namespace roe
