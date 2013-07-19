#ifndef ROEGRADIENT_H_
#define ROEGRADIENT_H_

#include <GL/gl.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <map>
#include <initializer_list>
#include "RoeCommon.h"
#include "RoeVector3.h"
#include "RoeMatrix.h"
#include "RoeColor.h"
//#include "RoeKeyframeInterpolator.h"

namespace roe {
	
	class S_Gradient {
	public:
		S_Gradient() {data = nullptr; next_mipmap = nullptr; id = 0;}
		void destroy() {ROE_SAFE_DELETE_ARRAY(data); if(next_mipmap) {next_mipmap->destroy(); ROE_SAFE_DELETE(next_mipmap);}}
		GLuint id;   //texture-1D-index
		int width;
		int bpp; //3(rgb), or 4(rgba)
		unsigned char *data; //pixel-data (row-major)
		S_Gradient *next_mipmap; //pointer to the next level of the mipmap
	};
	
	//alpha textures from back to front rendering!
	//not POT textures are allowerd now, but for
	//mipmapped textures POT w and h values are advised(but not strictly needed)
	class Gradient
	{
	public:
		
		Gradient();
		Gradient(const std::string& sPath);
		~Gradient();
		
		void        load      (std::string sPath);
		std::string getName   ()               const {return m_sName;}
		
		//bind just after load()
		//does change the texture matrix, but returns to modelview (if bool true)
		//bottom left, bottom right, top right then top left is the right order
		//works now with lighting (dont forget the normal vector!)
		void bind() const;
		int  getWidth () const {return s_mpGradient[m_sPath].width;}
		unsigned char*  getData  () const {return s_mpGradient[m_sPath].data;} //returns the pixeldata
		int  getBPP   () const {return s_mpGradient[m_sPath].bpp;} //returns the number of bytes per pixel
		unsigned int getID () const {return s_mpGradient[m_sPath].id;} //returns the gradient-id
		
		// *static*
		//loading and deleting
		static void  loadGradient  (std::string sPath);
		static void  loadGradient  (std::initializer_list<std::string> list);
		static void  loadGradient  (std::string sTmpStdPath, std::initializer_list<std::string> list);
	//	static void loadGradient(BasicColorInterpolator& colInt, int w, std::string name);
		static void  deleteGradient(std::string sPath, bool bCompleteThePath = true);
		
		// for the static variables
		static void init(std::string sStart, std::string sEnd);
		static void setPathStart(std::string s) {s_sPathStart = s;}
		static void setPathEnd  (std::string s) {s_sPathEnd   = s;}
		static void quit();
		
	private:
		
		static void loadGradientIntoGL(S_Gradient *tex); //loads the texture(and recursivly the mipmaps) into opengl
		static void buildMipmaps      (S_Gradient *tex, int numMipmaps = -1);
		
		static std::map <std::string, S_Gradient>           s_mpGradient;
		static std::map <std::string, S_Gradient>::iterator s_mpi;
		static std::pair<std::string, S_Gradient>           s_pair; //pair-object for the map
		static std::string s_sPathStart, s_sPathEnd; //standard path
		
		std::string m_sPath;
		std::string m_sName;
	};
	
} // namespace roe

#endif /* ROEGRADIENT_H_ */
