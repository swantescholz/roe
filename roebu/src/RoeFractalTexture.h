#ifndef ROEFRACTALTEXTURE_H_
#define ROEFRACTALTEXTURE_H_

#include "RoeCommon.h"
#ifdef ROE_COMPILE_GMP
	#include <gmp.h>
#endif
#include <GL/gl.h>
#include <map>
#include <initializer_list>

#include "RoeVector3.h"
#include "RoeMatrix.h"
#include "RoeColor.h"
#include "RoeTexture.h"
#include "RoeKeyframeInterpolator.h"

#ifdef ROE_COMPILE_GMP
	//use the gnu multiple precision library?
	#define ROE_FRACTAL_USE_GMP
	//using the simpler (and about 25% faster) pure c implementation?
	#define ROE_FRACTAL_GMP_USE_C
#endif

namespace roe {
	class FractalTexture {
#ifdef ROE_FRACTAL_USE_GMP
#ifdef ROE_FRACTAL_GMP_USE_C
		typedef mpf_t type;
#else
		typedef mpf_class type;
#endif
#else
		typedef double type;
#endif
	public:
		
		FractalTexture();
		FractalTexture(const FractalTexture& rhs) = delete;
		FractalTexture& operator=(const FractalTexture& rhs) = delete;
		~FractalTexture();
		
		void init(int w = 256, long numIterations = 250);
		void setInnerColor(const Color& c) {m_innerR=c.byter();m_innerG=c.byteg();m_innerB=c.byteb();}
		void bind();   //binds the created texture
		void create(); //creates the pixel data from the given view rectangle
		void setPrecision(long numBits); //sets the precision
		void setPosition(double x, double y); //sets ablsolute position
		void setPosition(const std::string& x, const std::string& y, int base = 10);
		void setZoom (double w); //sets the width
		void setZoom (const std::string& w, int base = 10);
		void move(double x, double y); //moves within relative view (uplt(-1,1),dnrt(1,-1))
		void zoom(double zoom); //updates view rectangle accordingly to zoom factor
		std::string getCenterX(int base = 10) {return gmpNumToString(m_xCenter, base);}
		std::string getCenterY(int base = 10) {return gmpNumToString(m_yCenter, base);}
		std::string getZoomW  (int base = 10) {return gmpNumToString(m_width  , base);}
		std::string getZoomH  (int base = 10) {return gmpNumToString(m_height , base);}
		
		S_Texture texture;
		LinearColorInterpolator colorInterpolation;
		
		static std::string gmpNumToString(type num, int base = 10);
	private:
#ifdef ROE_COMPILE_GMP
		void gmpcInit (std::initializer_list<mpf_t*> list) {for(auto i=list.begin();i!=list.end();++i){mpf_init(**i);}}
		void gmpcClear(std::initializer_list<mpf_t*> list) {for(auto i=list.begin();i!=list.end();++i){mpf_init(**i);}}
#endif
		void updateWH(double zoom); //updates width and height according to the zoom factor
		void updateCorners(); //updates the corners according to the new width and heith and center
		void computeColor(unsigned char *pixel);
		void updateMipmaps();
		
		unsigned char m_innerR, m_innerG, m_innerB; //std. color when inside of fractal
		long m_iStdMaxIterations; //number of iterations when zoom = 1
		long m_iMaxIterations; //actual number of iterations
		double m_dInvMaxIterations; //1/iterations
		
		//gmp
		type m_xUpLt, m_yUpLt; //position of the upper left corner of the view rectangle
		type m_xCenter, m_yCenter; //position of the center of the view rectangle
		type m_width, m_height;   //w|h of view rectangle
		type m_xPos, m_yPos;
		type xPos, yPos, xPos2, yPos2, tmp;
	};
	
} // namespace roe

#endif /* ROEFRACTALTEXTURE_H_ */
