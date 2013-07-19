#ifndef ROETEXTURE_H_
#define ROETEXTURE_H_

#include <GL/gl.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <map>
#include <initializer_list>
#include "RoeCommon.h"
#include "RoeVector3.h"
#include "RoeMatrix.h"
#include "RoeColor.h"

namespace roe {
	
	class S_Texture {
	public:
		S_Texture() {data = nullptr; next_mipmap = nullptr; id = 0; stdMatrix = Matrix::IDENTITY;}
		S_Texture(const S_Texture& rhs) = default;
		S_Texture& operator=(const S_Texture& rhs) = default;
		~S_Texture() {} //does nothing; call destroy() instead!
		
		void destroy() {ROE_SAFE_DELETE_ARRAY(data); if(next_mipmap) {next_mipmap->destroy(); ROE_SAFE_DELETE(next_mipmap);}}
		GLuint id;   //texture-index
		Matrix stdMatrix; //standard-transformation-matrix
		int width;
		int height;
		int bpp; //3(rgb), or 4(rgba)
		unsigned char *data; //pixel-data (row-major)
		S_Texture *next_mipmap; //pointer to the next level of the mipmap
	};
	
	//alpha textures from back to front rendering!
	//not POT textures are allowerd now, but for
	//mipmapped textures POT w and h values are advised(but not strictly needed)
	class Texture
	{
	public:
		enum EFlipType {
			FT_HORIZONTAL = 1,
			FT_VERTICAL   = 2
		};
		Texture(const Matrix& m = Matrix::IDENTITY);
		Texture(const std::string& sPath, const Matrix& m = Matrix::IDENTITY);
		~Texture();
		
		void        load      (std::string sPath);
		std::string getName   ()               const {return m_sName;}
		Matrix      getMatrix ()               const {return m_mMatrix;}
		void        setMatrix (const Matrix &m)      {m_mMatrix = m;}
		
		//bind just after load()
		//does change the texture matrix, but returns to modelview (if bool true)
		//bottom left, bottom right, top right then top left is the right order
		//works now with lighting (dont forget the normal vector!)
		void bind(Color col = Color(1.0,1.0,1.0,1.0), bool bWithTransformation = true) const;
		int  getWidth () const {return s_mpTexture[m_sPath].width;}
		int  getHeight() const {return s_mpTexture[m_sPath].height;}
		unsigned char*  getData  () const {return s_mpTexture[m_sPath].data;} //returns the pixeldata
		int  getBPP   () const {return s_mpTexture[m_sPath].bpp;} //returns the number of bytes per pixel
		unsigned int getID() const {return s_mpTexture[m_sPath].id;} //returns the texture-id
		
		// *static*
		//loading and deleting
		static void  loadTexture         (std::string sPath, int numMipmaps = -1, const Matrix& mStdMatrix = Matrix::IDENTITY);
		static void  loadTexture         (std::initializer_list<std::string> list, int numMipmaps = -1, const Matrix& mStdMatrix = Matrix::IDENTITY);
		static void  loadTexture         (std::string sTmpStdPath, std::initializer_list<std::string> list, int numMipmaps = -1, const Matrix& mStdMatrix = Matrix::IDENTITY);
		static void  deleteTexture       (std::string sPath, bool bCompleteThePath = true);
		static void loadTextureIntoGL(S_Texture *tex); //loads the texture(and recursivly the mipmaps) into opengl
		static void buildMipmaps     (S_Texture *tex, int numMipmaps = -1);
		static S_Texture loadSTexture(std::string sPath, const Matrix& mStdMatrix = Matrix::IDENTITY);
		
		//scaling
		static void scaleImage2 (const byte *oldData, int w1, int h1, byte *newData, int w2, int h2, int bpp = 4); //scales an image 2D
		static void scaleImage1 (const byte *oldData, int w1, byte *newData, int w2, int bpp = 4); //scales an image 1D
		
		// for the static variables
		static void init(std::string sStart, std::string sEnd);
		static void setPathStart(std::string s) {s_sPathStart = s;}
		static void setPathEnd  (std::string s) {s_sPathEnd   = s;}
		static void quit();
		
		//other functions needed
		static Uint32 getPixel32( SDL_Surface *surface, int x, int y );
		static void   putPixel32( SDL_Surface *surface, int x, int y, Uint32 pixel );
		static SDL_Surface* flipSurface( SDL_Surface *surface, int flags );
		
	private:
		
		//scaling
		static void scaleImage2POT (const byte *oldData, int w1, int h1, byte *newData, int w2, int h2, int bpp = 4); //scales an image 2D
		static void scaleImage1POT (const byte *oldData, int w1, byte *newData, int w2, int bpp = 4); //scales an image 1D
		
		static std::map <std::string, S_Texture>           s_mpTexture;
		static std::map <std::string, S_Texture>::iterator s_mpi;
		static std::pair<std::string, S_Texture>           s_pair; //pair-object for the map
		static std::string s_sPathStart, s_sPathEnd; //standard path
		
		std::string m_sPath;
		std::string m_sName;
		Matrix      m_mMatrix;
	};
	
} // namespace roe

#endif /*ROETEXTURE_H_*/
