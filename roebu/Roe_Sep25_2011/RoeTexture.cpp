#include "RoeTexture.h"
#include "RoeException.h"
#include "RoeUtil.h"
#include "RoeTextfile.h"
#ifdef ROE_USE_SHADERS
	#include "RoeProgram.h"
#endif

namespace roe {

	std::map <std::string, S_Texture>           Texture::s_mpTexture;
	std::map <std::string, S_Texture>::iterator Texture::s_mpi;
	std::pair<std::string, S_Texture>           Texture::s_pair;
	std::string Texture::s_sPathStart, Texture::s_sPathEnd = ".png";
	
	Texture::Texture(const Matrix &m)
	{
		m_mMatrix = m;
	}
	
	Texture::Texture(const std::string &sPath, const Matrix &m) {
		m_mMatrix = m;
		load(sPath);
	}
	Texture::~Texture()
	{
	}
	void Texture::bind(Color col, bool bWithTransformation) const {
		glColor4fv(col.rg);
#ifdef ROE_USE_SHADERS
		if (bWithTransformation) {
			Program::setStdTextureMatrix(s_mpTexture[m_sPath].stdMatrix * m_mMatrix);
		}
		Program::setStdTexture(s_mpTexture[m_sPath].id);
#else
		if (bWithTransformation) {
			glMatrixMode(GL_TEXTURE);
			glLoadMatrixf((s_mpTexture[m_sPath].stdMatrix * m_mMatrix).n);
			glMatrixMode(GL_MODELVIEW);
		}
		glBindTexture(GL_TEXTURE_2D, s_mpTexture[m_sPath].id);
#endif
	}
	void Texture::load(std::string sPath) {
		m_sName = sPath;
		sPath = s_sPathStart + sPath + s_sPathEnd;
		
		s_mpi = s_mpTexture.find(sPath);
		if (s_mpi == s_mpTexture.end()) {
			roe_except("angefragte Textur noch nicht geladen: " + sPath, "load");
		}
		
		m_sPath = sPath;
	}
	S_Texture Texture::loadSTexture(std::string sPath, const Matrix& mStdMatrix) {
		sPath = s_sPathStart + sPath + s_sPathEnd;
		S_Texture tex;
		tex.stdMatrix = mStdMatrix;
		
		SDL_Surface* surface = IMG_Load(sPath.c_str());
		if (surface == nullptr) {
			roe_except("texture does not exist: " + sPath, "loadSTexture");
		}
		tex.width  = surface->w;
		tex.height = surface->h;
		tex.bpp    = surface->format->BytesPerPixel;
		if (tex.bpp < 1 || tex.bpp == 2 || tex.bpp > 4)
			roe_except(util::toString(tex.bpp) + " bytes per pixel not supported: " + sPath, "loadSTexture");
		long imageSize = tex.width * tex.height * std::max(tex.bpp, 3);
		tex.data   = new GLubyte[imageSize];
		long pos = 0;
		byte r=0, g=0, b=0, a = 255;
		//cout << tex.bpp << " " << tex.width << " " << tex.height << " " << surface->pitch << endl;
		SDL_LockSurface(surface);
		for (int y = tex.height-1; y >= 0; --y) {
			for (int x = 0; x < tex.width; ++x) {
				//new (very complicated), controls also non-aplha textures
				//byte4 color = *((byte4*)&(((byte*)surface->pixels)[(y*tex.width + x)*tex.bpp]));
				byte *color = &(((byte*)surface->pixels)[y*surface->pitch + x*tex.bpp]); //never forget about the pitch!!
				
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
		}
		SDL_UnlockSurface(surface);
		SDL_FreeSurface(surface); //not needed anymore
		util::clamp(tex.bpp, 3,4);
		return tex;
	}
	void Texture::loadTexture(std::string sPath, int numMipmaps, const Matrix& mStdMatrix) {
		S_Texture tex = loadSTexture(sPath, mStdMatrix);
		sPath = s_sPathStart + sPath + s_sPathEnd;
		/*util::saveImageTGA(std::string("res/textures/mipmaps/map") + util::getNextNumber() + "_" + util::toString(tex.width)+"_"+
				util::toString(tex.height)+".tga", tex.data, tex.width, tex.height, tex.bpp==4);//*/
		buildMipmaps(&tex, numMipmaps);
		loadTextureIntoGL(&tex);
		
		//expand map
		s_pair = std::make_pair(sPath, tex);
		s_mpTexture.insert(s_pair);
	}
	void Texture::loadTexture(std::initializer_list<std::string> list, int numMipmaps, const Matrix& mStdMatrix) {
		for (auto it = list.begin(); it != list.end(); ++it)
			loadTexture(*it, numMipmaps, mStdMatrix);
	}
	void Texture::loadTexture(std::string sTmpStdPath, std::initializer_list<std::string> list, int numMipmaps, const Matrix& mStdMatrix) {
		for (auto it = list.begin(); it != list.end(); ++it)
			loadTexture(sTmpStdPath + *it, numMipmaps, mStdMatrix);
	}
	void Texture::loadTextureIntoGL(S_Texture *tex) {
		GLuint id;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (!tex->next_mipmap) ? GL_LINEAR : GL_LINEAR_MIPMAP_LINEAR);
		const int type = (tex->bpp==4) ? GL_RGBA : GL_RGB;
		bool oldWasPOT = util::isPOT(tex->width) && util::isPOT(tex->height); //was image before power of two?
		int mipmap = 0;
		if (!tex->next_mipmap) {
			//we've just one texture to go with; bad idea if NPOT! ;(
			glTexImage2D(GL_TEXTURE_2D, mipmap++, type, tex->width, tex->height, 0, type, GL_UNSIGNED_BYTE, tex->data);
			tex->id = id;
			return;
		}
		else if (!oldWasPOT) {
			tex->id = id;
			tex = tex->next_mipmap; //first NPOT texture not needed, so skip it
		}
		
		while(tex) {
			/*util::saveImageTGA(std::string("res/textures/mipmaps/mip") + util::getNextNumber() + "_" + util::toString(tex->width)+"_"+
				util::toString(tex->height)+".tga", tex->data, tex->width, tex->height, tex->bpp==4);//*/
			glTexImage2D(GL_TEXTURE_2D, mipmap++, type, tex->width, tex->height, 0, type, GL_UNSIGNED_BYTE, tex->data);  //create mipmap in GL
			tex->id = id; //setting the id
			tex = tex->next_mipmap;
		}
	}
	void Texture::buildMipmaps(S_Texture *tex, int numMipmaps) {
		//MIPMAPPING
		if (numMipmaps < 0) { //auto calculate num mipmaps
			for (numMipmaps = 0; 1<<numMipmaps < tex->width || 1<<numMipmaps < tex->height; numMipmaps++) {}
		}
		S_Texture *subtex1 = tex, *subtex2 = nullptr;
		for (int i = 1; i <= numMipmaps; i++) { //scaling for each mipmap
			subtex2 = new S_Texture;
			subtex2->bpp = subtex1->bpp;
			subtex2->stdMatrix = subtex1->stdMatrix;
			subtex2->width  = util::previousPOT(subtex1->width);
			subtex2->height = util::previousPOT(subtex1->height);
			subtex2->data = new byte[subtex2->width*subtex2->height*subtex2->bpp];
			scaleImage2(subtex1->data, subtex1->width, subtex1->height, subtex2->data, subtex2->width, subtex2->height, subtex1->bpp); //scaling image to new size
			subtex1->next_mipmap = subtex2; //linking
			subtex1 = subtex2; //one level deeper
		}
	}
	void Texture::scaleImage2POT(const byte *oldData, int w1, int h1, byte *newData, int w2, int h2, int bpp) {
		static const float sinv = 1.0f/255.0f;
		float a[4] = {1.0f,1.0f,1.0f,1.0f};
		for (int x = 0; x < w2; ++x) {
			for (int y = 0; y < h2; ++y) {
				if(bpp == 4)  {
					a[0] = sinv*oldData[((2*y+0)*w1+2*x+0)*bpp+3];
					a[1] = sinv*oldData[((2*y+0)*w1+2*x+1)*bpp+3];
					a[2] = sinv*oldData[((2*y+1)*w1+2*x+0)*bpp+3];
					a[3] = sinv*oldData[((2*y+1)*w1+2*x+1)*bpp+3];
					newData[(y*w2+x)*bpp+3] = 255*0.25f*(a[0]+a[1]+a[2]+a[3]);
				}
				for (int i = 0; i < 3; ++i) {
					newData[(y*w2+x)*bpp+i] =
						(a[0]*oldData[((2*y+0)*w1+2*x+0)*bpp+i]+a[1]*oldData[((2*y+0)*w1+2*x+1)*bpp+i]+
						 a[2]*oldData[((2*y+1)*w1+2*x+0)*bpp+i]+a[3]*oldData[((2*y+1)*w1+2*x+1)*bpp+i])/4;
				}
			}
		}
		
		//util::saveImageTGA(std::string("res/mip") + util::getNextNumber() + "_" + util::toString(w2)+"_"+util::toString(h2)+".tga",
		//                   newData, w2, h2, bpp==4?true:false);
	}
	void Texture::scaleImage1POT(const byte *oldData, int w1, byte *newData, int w2, int bpp) {
		static const float sinv = 1.0f/255.0f;
		float a[2] = {1.0f,1.0f};
		for (int x = 0; x < w2; ++x) {
			if(bpp == 4)  {
				a[0] = sinv*oldData[(2*x+0)*bpp+3];
				a[1] = sinv*oldData[(2*x+1)*bpp+3];
				newData[x*bpp+3] = (oldData[(2*x+0)*bpp+3]+oldData[(2*x+1)*bpp+3])/2;
			}
			for (int i = 0; i < 3; ++i) {
				newData[x*bpp+i] = (a[0]*oldData[(2*x+0)*bpp+i]+a[1]*oldData[(2*x+1)*bpp+i])/2;
			}
		}
	}
	void Texture::scaleImage2(const byte *oldData, int w1, int h1, byte *newData, int w2, int h2, int bpp) {
		if(w2*2==w1 && h2*2==h1 && util::isPOT(w2) && util::isPOT(h2)) { //catch simple pot scaling
			scaleImage2POT(oldData, w1, h1, newData, w2, h2, bpp);
			return;
		}
		
		byte *tmp = new byte[w2*h1*bpp];
		byte *columnOld = new byte[h1*bpp];
		byte *columnNew = new byte[h2*bpp];
		
		//scale first W
		for (int i = 0; i < h1; i++) {
			scaleImage1(oldData + (i*w1*bpp), w1, tmp + (i*w2*bpp), w2, bpp); //resize row
		}
		
		//then scale H
		for (int i = 0; i < w2; i++) {
			for (int j = 0; j < h1; j++) { //copy data from image into column array
				for (int h = 0; h < bpp; h++) columnOld[j*bpp+h] = tmp[(j*w2+i)*bpp+h];
			}
			scaleImage1(columnOld, h1, columnNew, h2, bpp); //resize column
			for (int j = 0; j < h2; j++) { //copy data from new column array into new image
				for (int h = 0; h < bpp; h++) {
					newData[(j*w2+i)*bpp+h] = columnNew[j*bpp+h];
				}
			}
		}
		
		//util::saveImageTGA(std::string("res/mip") + util::getNextNumber() + "_" + util::toString(w2)+"_"+util::toString(h2)+".tga",
		//                   newData, w2, h2, bpp==4?true:false);
		
		delete[] columnNew;
		delete[] columnOld;
		delete[] tmp;
	}
	void Texture::scaleImage1(const byte *oldData, int w1, byte *newData, int w2, int bpp) {
		if(w2*2==w1 && util::isPOT(w2)) { //catch simple pot scaling
			scaleImage1POT(oldData, w1, newData, w2, bpp);
			return;
		}
		if (w2==w1) { //catch equality
			for (int i = 0; i < w1*bpp; i++) {
				newData[i] = oldData[i];
			}
			return;
		}
		if (w2>=w1) { //catch upscaling
			byte *tmp = new byte[w1*bpp*2]; //upscaled oldData by 2
			for (int i = 0; i < w1; i++) {
				tmp[(2*i+0)*bpp+0] = oldData[i*bpp+0];
				tmp[(2*i+0)*bpp+1] = oldData[i*bpp+1];
				tmp[(2*i+0)*bpp+2] = oldData[i*bpp+2];
				tmp[(2*i+1)*bpp+0] = oldData[i*bpp+0];
				tmp[(2*i+1)*bpp+1] = oldData[i*bpp+1];
				tmp[(2*i+1)*bpp+2] = oldData[i*bpp+2];
			}
			scaleImage1(tmp, w1*2, newData, w2, bpp);
			delete[] tmp;
			return;
		}
		
		//normal DOWNSCALING
		const float factorW = static_cast<float>(w2-1) / (w1-1);
		
		//*TEST for more sharp look
		int index = 0;
		for (int i = 0; i < w2; i++) {
			for (int j = 0; j < bpp; j++) {
				index = static_cast<int>(i*(w1*1.0f/w2))*bpp+j;
				newData[i*bpp+j] = oldData[index];
			}
		}
		return;
		//TEST END*/
		
		float *leftBorders   = new float[w1];
		float *intensityLeft = new float[w1];
		int   *leftPixel     = new int  [w1];
		float *newIntensity  = new float[w2*bpp];
		float *intensitySum  = new float[w2];
		float *opacity       = new float[w1];
		for (int i = 0; i < w2    ; i++) intensitySum[i] = 0.0f;
		for (int i = 0; i < w2*bpp; i++) newIntensity[i] = 0.0f;
		for (int i = 0; i < w1    ; i++) {
			if (bpp!=4) opacity[i] = 1.0f;
			else {
				opacity[i] = 1.0f;
				opacity[i] = oldData[i*bpp+3] / 256.0f;
			}
		}
		
		for (int i = 0; i < w1; i++) { //calculating all important informations
			leftBorders   [i] = factorW * i;
			leftPixel     [i] = static_cast<int>(leftBorders[i]);
			intensityLeft [i] = ((leftPixel[i]+1.0f) - leftBorders[i]) * opacity[i];
			intensitySum[leftPixel[i] ] += intensityLeft[i];
			if (leftPixel[i]+1 < w2) {
				intensitySum[leftPixel[i]+1] += opacity[i] - intensityLeft[i];
			}
		}
		
		for (int i = 0; i < w1; i++) {
			for (int j = 0; j < bpp; j++) {
				newIntensity[bpp*leftPixel[i]+j] += oldData[bpp*i+j] * intensityLeft[i]; 
				if (leftPixel[i]+1 < w2) {
					newIntensity[bpp*leftPixel[i]+bpp+j] += oldData[bpp*i+j] * (opacity[i] - intensityLeft[i]);
				}
			}
		}
		for (int i = 0; i < w2; i++) {
			for (int j = 0; j < bpp; j++) {
				newData[i*bpp+j] = (int)util::round(newIntensity[i*bpp+j] / intensitySum[i]); //adjust intensity
			}
		}
		
		delete[] leftBorders;
		delete[] intensityLeft;
		delete[] leftPixel;
		delete[] newIntensity;
		delete[] intensitySum;
		delete[] opacity;
	}
	void Texture::deleteTexture (std::string sPath, bool bCompleteThePath) {
		if (bCompleteThePath)
			sPath = s_sPathStart + sPath + s_sPathEnd;
		
		s_mpi = s_mpTexture.find(sPath);
		if (s_mpi == s_mpTexture.end()) {
			roe_except("texture to be deleted does not exist: " + sPath, "deleteTexture");
		} else {
			glDeleteTextures(1, &s_mpi->second.id);
			s_mpi->second.destroy();
			s_mpTexture.erase(s_mpi);
		}
	}
	void Texture::init(std::string sStart, std::string sEnd) {
		s_sPathStart  = sStart;
		s_sPathEnd    = sEnd;
	}
	void Texture::quit() {
		for(s_mpi=s_mpTexture.begin(); s_mpi!=s_mpTexture.end(); ++s_mpi) {
			glDeleteTextures(1, &s_mpi->second.id);
			s_mpi->second.destroy();
		}
		s_mpTexture.clear();
	}
	Uint32 Texture::getPixel32( SDL_Surface *surface, int x, int y )
	{
		//Convert the pixels to 32 bit
		Uint32 *pixels = (Uint32 *)surface->pixels;
		
		//Get the requested pixel
		return pixels[ ( y * surface->w ) + x ];
	}
	void Texture::putPixel32( SDL_Surface *surface, int x, int y, Uint32 pixel )
	{
		//Convert the pixels to 32 bit
		Uint32 *pixels = (Uint32 *)surface->pixels;
		
		//Set the pixel
		pixels[ ( y * surface->w ) + x ] = pixel;
	}
	SDL_Surface* Texture::flipSurface( SDL_Surface *surface, int flags )
	{
		//Pointer to the soon to be flipped surface
		SDL_Surface *flipped = NULL;
		
		//If the image is color keyed
		if( surface->flags & SDL_SRCCOLORKEY )
		{
			flipped = SDL_CreateRGBSurface( SDL_SWSURFACE, surface->w, surface->h, surface->format->BitsPerPixel,
			                                surface->format->Rmask, surface->format->Gmask, surface->format->Bmask, 0 );
		}
		//Otherwise
		else
		{
			flipped = SDL_CreateRGBSurface( SDL_SWSURFACE, surface->w, surface->h, surface->format->BitsPerPixel,
			                                surface->format->Rmask, surface->format->Gmask, surface->format->Bmask, surface->format->Amask );
		}
		
		//If the surface must be locked
		if( SDL_MUSTLOCK( surface ) )
		{
			//Lock the surface
			SDL_LockSurface( surface );
		}
		
		//Go through columns
		for( int x = 0, rx = flipped->w - 1; x < flipped->w; x++, rx-- )
		{
			//Go through rows
			for( int y = 0, ry = flipped->h - 1; y < flipped->h; y++, ry-- )
			{
				//Get pixel
				Uint32 pixel = getPixel32( surface, x, y );
				
				//Copy pixel
				if( ( flags & FT_VERTICAL ) && ( flags & FT_HORIZONTAL ) )
				{
					putPixel32( flipped, rx, ry, pixel );
				}
				else if( flags & FT_HORIZONTAL )
				{
					putPixel32( flipped, rx, y, pixel );
				}
				else if( flags & FT_VERTICAL )
				{
					putPixel32( flipped, x, ry, pixel );
				}
			}
		}
		
		//Unlock surface
		if( SDL_MUSTLOCK( surface ) )
		{
			SDL_UnlockSurface( surface );
		}
		
		//Copy color key
		if( surface->flags & SDL_SRCCOLORKEY )
		{
			SDL_SetColorKey( flipped, SDL_RLEACCEL | SDL_SRCCOLORKEY, surface->format->colorkey );
		}
		
		//Return flipped surface
		return flipped;
	}
	
} // namespace roe


