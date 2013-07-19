#include "RoeFractalTexture.h"
#include "RoeException.h"
#include "RoeUtil.h"
#include "RoeTextfile.h"
#include <GL/gl.h>
#ifdef ROE_USE_SHADERS
	#include "RoeProgram.h"
#endif

#define ROE_FRACTAL_GMP_PREC 400

namespace roe {
	
	FractalTexture::FractalTexture()
#ifndef ROE_FRACTAL_GMP_USE_C
	: m_xUpLt(ROE_FRACTAL_GMP_PREC)
	, m_yUpLt(ROE_FRACTAL_GMP_PREC)
	, m_xCenter(ROE_FRACTAL_GMP_PREC)
	, m_yCenter(ROE_FRACTAL_GMP_PREC)
	, m_width  (ROE_FRACTAL_GMP_PREC)
	, m_height (ROE_FRACTAL_GMP_PREC)
	, m_xPos   (ROE_FRACTAL_GMP_PREC)
	, m_yPos   (ROE_FRACTAL_GMP_PREC)
	, xPos     (ROE_FRACTAL_GMP_PREC)
	, yPos     (ROE_FRACTAL_GMP_PREC)
	, xPos2    (ROE_FRACTAL_GMP_PREC)
	, yPos2    (ROE_FRACTAL_GMP_PREC)
	, tmp      (ROE_FRACTAL_GMP_PREC)
#endif
	{
		mpf_set_default_prec(ROE_FRACTAL_GMP_PREC); //setting the default precision
#ifdef ROE_FRACTAL_GMP_USE_C
		gmpcInit({&m_xUpLt,&m_yUpLt,&m_xCenter,&m_yCenter,&m_width,&m_height});
		gmpcInit({&m_xPos,&m_yPos,&xPos,&yPos,&xPos2,&yPos2,&tmp});
		setPrecision(ROE_FRACTAL_GMP_PREC);
		mpf_set_d(m_xCenter, 0.0);
		mpf_set_d(m_yCenter, 0.0);
		mpf_set_d(m_width  , 4.0);
		mpf_set_d(m_height , 4.0);
#else
		setPrecision(ROE_FRACTAL_GMP_PREC);
		m_width = 4;
		m_height = 4;
		m_xCenter = 0.0;
		m_yCenter = 0.0;
#endif
		
		updateCorners();
		m_innerR = m_innerG = m_innerB = 0; //black inner color
		texture.data = nullptr;
	}
	
	FractalTexture::~FractalTexture() {
#ifdef ROE_FRACTAL_GMP_USE_C
		gmpcClear({&m_xUpLt,&m_yUpLt,&m_xCenter,&m_yCenter,&m_width,&m_height});
		gmpcClear({&m_xPos,&m_yPos,&xPos,&yPos,&xPos2,&yPos2,&tmp});
#endif
		if(!texture.data) return;
		if(glIsTexture(texture.id)) glDeleteTextures(1, &texture.id);
		texture.destroy();
	}
	void FractalTexture::init(int w, long numIterations) {
		if(texture.data) { //if already initialized, then first destroy
			if(glIsTexture(texture.id)) glDeleteTextures(1, &texture.id); //cleaning gl memory up
			texture.destroy();
		}
		m_iStdMaxIterations = numIterations;
		texture.width = texture.height = w;
		texture.bpp = 3;
		texture.stdMatrix = Matrix::IDENTITY;
		texture.data = new unsigned char[texture.width*texture.height*texture.bpp];
		for (long i = 0; i < texture.width*texture.height*texture.bpp; ++i) {
			texture.data[i] = 0;
		}
		Texture::buildMipmaps(&texture); //first mipmap creating (for memory acquisition)
	}
	void FractalTexture::bind() {
		glColor4fv(Color::WHITE.rg);
#ifdef ROE_USE_SHADERS
		Program::setStdTexture(texture.id);
#else
		glBindTexture(GL_TEXTURE_2D, texture.id);
#endif
	}
	
	void FractalTexture::create() {
		if(glIsTexture(texture.id)) glDeleteTextures(1, &texture.id); //cleaning gl memory up
		m_iMaxIterations = (long)((double)m_iStdMaxIterations * 1.0); //todo?
		m_dInvMaxIterations = 1.0 / (m_iMaxIterations+0.0001);
		const double winv = 1.0/(texture.width -1);
		const double hinv = 1.0/(texture.height-1);
		util::timeDiff();
		for (int x = 0; x < texture.width; ++x) {
			for (int y = 0; y < texture.height; ++y) {
#ifdef ROE_FRACTAL_GMP_USE_C
				mpf_set_d(m_xPos, x*winv);
				mpf_set_d(m_yPos, y*hinv);
				mpf_mul(m_xPos, m_xPos, m_width);
				mpf_mul(m_yPos, m_yPos, m_height);
				mpf_add(m_xPos, m_xUpLt, m_xPos);
				mpf_sub(m_yPos, m_yUpLt, m_yPos);
#else
				m_xPos = m_xUpLt + (x*winv)*m_width;
				m_yPos = m_yUpLt - (y*hinv)*m_height;
#endif
				computeColor(&(texture.data[((texture.height-1-y)*texture.width+x)*texture.bpp]));
			}
		}
		cout << "dt: " << util::timeDiff() << endl;
		cout << getCenterX() << endl;
		cout << getCenterY() << endl;
		cout << getZoomW() << endl;
		updateMipmaps();
		Texture::loadTextureIntoGL(&texture);
	}
	void FractalTexture::setPrecision(long numBits) {
#ifdef ROE_FRACTAL_GMP_USE_C
		mpf_set_prec(m_xUpLt  , numBits);
		mpf_set_prec(m_yUpLt  , numBits);
		mpf_set_prec(m_xCenter, numBits);
		mpf_set_prec(m_yCenter, numBits);
		mpf_set_prec(m_width  , numBits);
		mpf_set_prec(m_height , numBits);
		mpf_set_prec(m_xPos   , numBits);
		mpf_set_prec(m_yPos   , numBits);
		mpf_set_prec(xPos     , numBits);
		mpf_set_prec(yPos     , numBits);
		mpf_set_prec(xPos2    , numBits);
		mpf_set_prec(yPos2    , numBits);
		mpf_set_prec(tmp      , numBits);
#else
		m_xUpLt.set_prec(numBits);
		m_yUpLt.set_prec(numBits);
		m_xCenter.set_prec(numBits);
		m_yCenter.set_prec(numBits);
		m_width.set_prec(numBits);
		m_height.set_prec(numBits);
		m_xPos.set_prec(numBits);
		m_yPos.set_prec(numBits);
		xPos.set_prec(numBits);
		yPos.set_prec(numBits);
		xPos2.set_prec(numBits);
		yPos2.set_prec(numBits);
		tmp.set_prec(numBits);
#endif
	}
	void FractalTexture::computeColor(unsigned char *pixel) {
#ifdef ROE_FRACTAL_GMP_USE_C
		mpf_set(xPos, m_xPos);
		mpf_set(yPos, m_yPos);
#else
		xPos = m_xPos;
		yPos = m_yPos;
#endif
		for(long iteration = 0; iteration < m_iMaxIterations; ++iteration) {
#ifdef ROE_FRACTAL_GMP_USE_C
			mpf_mul(xPos2, xPos, xPos);
			mpf_mul(yPos2, yPos, yPos);
			mpf_add(tmp, xPos2, yPos2); //save sum temporarily
			if(mpf_cmp_ui(tmp,4) >= 0) {
#else
			xPos2 = xPos*xPos;
			yPos2 = yPos*yPos;
			if(xPos2 + yPos2 > 4.0) {
#endif
				//(coloured) outer
				const double ratio = iteration*m_dInvMaxIterations;
				//const Color c = Color::WHITE;
				const Color c = colorInterpolation.interpolate(ratio);
				pixel[0] = c.byter();
				pixel[1] = c.byteg();
				pixel[2] = c.byteb();
				return;
			}
#ifdef ROE_FRACTAL_GMP_USE_C
			mpf_mul(yPos, yPos, xPos);
			mpf_mul_ui(yPos, yPos, 2);
			mpf_add(yPos, yPos, m_yPos);
			mpf_sub(xPos, xPos2, yPos2);
			mpf_add(xPos, xPos, m_xPos);
#else
			yPos *= xPos;
			yPos *= 2;
			yPos += m_yPos;
			xPos = xPos2;
			xPos -= yPos2;
			xPos += m_xPos;
#endif
		}
		//inner
		pixel[0] = m_innerR;
		pixel[1] = m_innerG;
		pixel[2] = m_innerB;
	}
	void FractalTexture::updateMipmaps() {
		S_Texture *subtex1 = &texture, *subtex2 = nullptr;
		while (subtex1->next_mipmap) {
			subtex2 = subtex1->next_mipmap;
			Texture::scaleImage2(subtex1->data, subtex1->width, subtex1->height, subtex2->data, subtex2->width, subtex2->height, subtex1->bpp); //scaling image to new size
			subtex1 = subtex2; //one level deeper
		}
	}
	
	//GMP-------------------
	void FractalTexture::setPosition(double x, double y) {
#ifdef ROE_FRACTAL_GMP_USE_C
		mpf_set_d(m_xCenter, x);
		mpf_set_d(m_yCenter, y);
#else
		m_xCenter = x;
		m_yCenter = y;
#endif
		updateCorners();
	}
	void FractalTexture::setPosition(const std::string& x, const std::string& y, int base) {
#ifdef ROE_FRACTAL_USE_GMP
#ifdef ROE_FRACTAL_GMP_USE_C
		int xreturn = mpf_set_str(m_xCenter, x.c_str(), base);
		int yreturn = mpf_set_str(m_yCenter, y.c_str(), base);
#else
		int xreturn = m_xCenter.set_str(x,base);
		int yreturn = m_yCenter.set_str(y,base);
#endif
		if(xreturn==-1) roe_except("bad x-string("+x+") or bad base("+util::toString(base)+")", "setPosition");
		if(yreturn==-1) roe_except("bad y-string("+y+") or bad base("+util::toString(base)+")", "setPosition");
#else
		if (base != 10) roe_except("bad base("+util::toString(base)+"), only base 10 allowed", "setPosition");
		m_xCenter = (type)(util::parseDouble(x));
		m_yCenter = (type)(util::parseDouble(y));
#endif
		updateCorners();
	}
	void FractalTexture::setZoom(double w) {
#ifdef ROE_FRACTAL_GMP_USE_C
		mpf_set_d(m_width , w);
		mpf_set_d(m_height, w);
#else
		m_width = m_height = w;
#endif
		updateCorners();
	}
	void FractalTexture::setZoom(const std::string& w, int base) {
#ifdef ROE_FRACTAL_USE_GMP
#ifdef ROE_FRACTAL_GMP_USE_C
		int wreturn = mpf_set_str(m_width , w.c_str(), base);
		int hreturn = mpf_set_str(m_height, w.c_str(), base);
#else
		int wreturn = m_width.set_str (w,base);
		int hreturn = m_height.set_str(w,base);
#endif
		if(wreturn==-1) roe_except("bad w-string("+w+") or bad base("+util::toString(base)+")", "setZoom");
		if(hreturn==-1) roe_except("bad w-string("+w+") or bad base("+util::toString(base)+")", "setZoom");
#else
		if (base != 10) roe_except("bad base("+util::toString(base)+"), only base 10 allowed", "setZoom");
		m_width  = (type)(util::parseDouble(w));
		m_height = (type)(util::parseDouble(w));
#endif
		updateCorners();
	}
	void FractalTexture::move(double x, double y) {
		x *= 0.5;
		y *= 0.5;
#ifdef ROE_FRACTAL_GMP_USE_C
		mpf_set_d(xPos, x);
		mpf_set_d(yPos, y);
		mpf_mul(xPos, m_width, xPos);
		mpf_mul(yPos, m_width, yPos);
		mpf_add(m_xCenter, m_xCenter, xPos);
		mpf_add(m_yCenter, m_yCenter, yPos);
#else
		m_xCenter += x*m_width;
		m_yCenter += y*m_height;
#endif
		updateCorners();
	}
	void FractalTexture::zoom(double zoom) {
		updateWH(zoom);
		updateCorners();
	}
	void FractalTexture::updateWH(double zoom) {
#ifdef ROE_FRACTAL_GMP_USE_C
		mpf_set_d(xPos, zoom);
		mpf_mul(m_width , m_width , xPos);
		mpf_mul(m_height, m_height, xPos);
#else
		m_width  *= zoom;
		m_height *= zoom;
#endif
	}
	void FractalTexture::updateCorners() {
#ifdef ROE_FRACTAL_GMP_USE_C
		mpf_div_ui(xPos, m_width , 2);
		mpf_div_ui(yPos, m_height, 2);
		mpf_sub(m_xUpLt, m_xCenter, xPos);
		mpf_add(m_yUpLt, m_yCenter, yPos);
#else
		m_xUpLt = m_xCenter-0.5*m_width;
		m_yUpLt = m_yCenter+0.5*m_height;
#endif
	}
	std::string FractalTexture::gmpNumToString(type num, int base) {
		std::string s;
#ifdef ROE_FRACTAL_USE_GMP
		long exp;
#ifdef ROE_FRACTAL_GMP_USE_C
		s = mpf_get_str(nullptr, &exp, base, 0, num);
#else
		s = num.get_str(exp, base, 0);
#endif
		std::string sign = "";
		if(s.length() == 0) {s="00"; ++exp;}
		if(s[0] == '-') {sign = "-"; s = s.substr(1);}
		while (exp <= 0) {
			s = "0"+s;
			++exp;
		}
		while (exp >= (long)s.length()) {
			s += "0";
		}
		s.insert(exp, ".");//*/
		s = sign + s;
#else
		s = util::toString((double)num);
#endif
		return s;
	}
	
} //namespace roe
