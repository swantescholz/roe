#ifndef ROELIGHT_H_
#define ROELIGHT_H_

#define ROE_MAX_LIGHTS 8

#include "RoeColor.h"
#include "RoeMatrix.h"
#include <GL/gl.h>

namespace roe {

	class Light
	{
	public:
		Light(Vector3 vPos, bool bActive = true, bool bPos = true,
			  Color c1 = Color(0.0f,0.0f,0.0f,1.0f), Color c2 = Color(1.0f,1.0f,1.0f,1.0f), Color c3 = Color(1.0f,1.0f,1.0f,1.0f),
			  float kc = 1.0f, float kl = 0.0f, float kq = 0.0f, Vector3 vDir = Vector3(0.0f,0.0f,1.0f), float fEx = 0.0f, float fCut = 180.0f);
		
		Light(const Light &rhs) = delete;
		Light& operator=(const Light &rhs) = delete;
		~Light();
		
		void transform(const Matrix &m);     //transforms the position vector and the spot direction
		void light();                        //the light shines(before objects!, after camera!)
		Vector3 getPosition() const {return pos;}
		
		//activate / deactivate
		void enable   ()       {glEnable (getLightName(m_iLight)); m_bActive = true;}
		void disable  ()       {glDisable(getLightName(m_iLight)); m_bActive = false;}
		bool isEnabled() const {return glIsEnabled(getLightName(m_iLight));}
		int  getLightIndex() const {return m_iLight;}
		
		//global lightmodel
		static void lightModel   (const Color &col = Color(0.2f,0.2f,0.2f,1.0f), bool local_viewer = false, bool two_sides = false);
		static void lightModelAmb(const Color &col = Color(0.2f,0.2f,0.2f,1.0f)) {s_LightModelAmbient=col;} //ambient world color
		static void lightModelLoc(bool b = false) {glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, b);}   //local viewer?
		static void lightModelTwo(bool b = false) {glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, b);}       //two-sided lighting?
		static Color getLightModelAmb() {return s_LightModelAmbient;}
		
		void setPositional(bool b)   {m_bPositional = b;}  //positional light or infinite distance=
		void setActive(bool b)       {m_bActive = b;}
		bool getPositional  () const {return m_bPositional;}
		bool getActive      () const {return m_bActive;}
		
		
		//material
		void setMaterial(Color amb, Color diff, Color spec) {m_amb = amb; m_diff = diff; m_spec = spec;}
		void setMaterialAmb (Color amb)  {m_amb  = amb ;}
		void setMaterialDiff(Color diff) {m_diff = diff;}
		void setMaterialSpec(Color spec) {m_spec = spec;} 
		Color getMaterialAmb () const {return m_amb ;}
		Color getMaterialDiff() const {return m_diff;}
		Color getMaterialSpec() const {return m_spec;}
		
		//attentuation
		void  setAttentuation (float kc = 1.0f, float kl = 0.0f, float kq = 0.0f) {m_fattc = kc; m_fattl = kl; m_fattq = kq;}
		void  setAttentuationC(float kc = 1.0f) {m_fattc = kc;}
		void  setAttentuationL(float kl = 0.0f) {m_fattl = kl;}
		void  setAttentuationQ(float kq = 0.0f) {m_fattq = kq;}
		float getAttentuationC() const {return m_fattc;}
		float getAttentuationL() const {return m_fattl;}
		float getAttentuationQ() const {return m_fattq;}
		
		//spot
		void setSpot(Vector3 vDir = Vector3(0.0f,0.0f,-1.0f), float fEx = 0.0f, float fCut = 180.0f) {dir = vDir; m_fSpotExponent = fEx; m_fSpotCutoff = fCut;}
		void setSpotDirection   (Vector3 vDir = Vector3(0.0f,0.0f,-1.0f))      {dir              = vDir;}
		void setSpotExponent    (float fEx  = 0.0f)                            {m_fSpotExponent  = fEx ;}
		void setSpotCutoff      (float fCut = 180.0f)                          {m_fSpotCutoff    = fCut;}
		Vector3 getSpotDirection () const {return dir;}
		float   getSpotExponent  () const {return m_fSpotExponent;}
		float   getSpotCutoff    () const {return m_fSpotCutoff;}
		
		Vector3 pos, dir;
		static GLenum getLightName(int lightIndex);
	private:
		static int  s_iNumLights;    //number of lights
		static Color s_LightModelAmbient;
		
		int m_iLight;              //light index
		bool m_bPositional;        //true: positional, false: directional
		bool m_bActive;            //active?
		Color m_amb, m_diff, m_spec;      //material
		
		//spot
		float m_fSpotExponent, m_fSpotCutoff;   //exponent, alngle part
		
		//attentuation
		float m_fattc, m_fattl, m_fattq; // constant, linear, quadratic
	};
	
} // namespace roe

#endif /*GLCLIGHT_H_*/
