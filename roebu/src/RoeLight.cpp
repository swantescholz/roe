#include "RoeLight.h"
#include "RoeException.h"
#include "RoeCommon.h"
#include "RoeProgram.h"

namespace roe {
	
	int  Light::s_iNumLights = 0;
	Color Light::s_LightModelAmbient = Color(0.0,0.0,0.0,1.0);
	
	Light::Light(Vector3 vPos, bool bActive, bool bPos, Color c1, Color c2, Color c3, float kc, float kl, float kq, Vector3 vDir, float fEx, float fCut)
		: pos(vPos)
	{
		setActive      (bActive);
		setPositional  (bPos);
		setMaterial    (c1,c2,c3);
		setAttentuation(kc,kl,kq);
		setSpot        (vDir,fEx,fCut);
		
		if (s_iNumLights >= ROE_MAX_LIGHTS)
			roe_except("too many lights", "Light");
		m_iLight = s_iNumLights;
		
		if (!glIsEnabled(getLightName(m_iLight))) glEnable(getLightName(m_iLight));
		else roe_except("warning: new light already enabled", "Light");
		
		++s_iNumLights;
	}
	Light::~Light() {
		glDisable(getLightName(m_iLight));
		--s_iNumLights;
	}
	void Light::lightModel(const Color &col, bool local_viewer, bool two_sides) {
		s_LightModelAmbient = col;
		glLightModeli (GL_LIGHT_MODEL_LOCAL_VIEWER, local_viewer);
		glLightModeli (GL_LIGHT_MODEL_TWO_SIDE    , two_sides);
	}
	void Light::transform(const Matrix &m) {
		Matrix::transformCoords(pos, m);
		Matrix::transformNormal(dir, m);
	}
	void Light::light() {
		if (m_bActive && glIsEnabled(GL_LIGHTING)) {
#ifndef ROE_USE_SHADERS
			//global ambient color
			glLightModelfv(GL_LIGHT_MODEL_AMBIENT,s_LightModelAmbient.rg);
			
			//light character
			glLightfv(m_eLight, GL_AMBIENT , m_amb.rg);
			glLightfv(m_eLight, GL_DIFFUSE , m_diff.rg);
			glLightfv(m_eLight, GL_SPECULAR, m_spec.rg);
			
			//position
			//c_vLightCenter[m_iLight] = m_Sphere.GetCenter(); // die statischen positionen updaten
			GLfloat light_position[] = { pos.x, pos.y, pos.z, m_bPositional?1.0f:0.0f };
			glLightfv(m_eLight, GL_POSITION, light_position);
			
			//attentuation
			glLightf(m_eLight, GL_CONSTANT_ATTENUATION, m_fattc);
			glLightf(m_eLight, GL_LINEAR_ATTENUATION, m_fattl);
			glLightf(m_eLight, GL_QUADRATIC_ATTENUATION, m_fattq);
			
			//spot
			glLightf (m_eLight, GL_SPOT_EXPONENT, m_fSpotExponent);
			glLightf (m_eLight, GL_SPOT_CUTOFF  , m_fSpotCutoff);
			glLightfv(m_eLight,GL_SPOT_DIRECTION, dir.rg);
#else
			Program::setStdLightSource(*this);
#endif
			
		}
	}
	
	GLenum Light::getLightName(int lightIndex) {
		if (lightIndex == 0) return GL_LIGHT0;
		else if (lightIndex == 1) return GL_LIGHT1;
		else if (lightIndex == 2) return GL_LIGHT2;
		else if (lightIndex == 3) return GL_LIGHT3;
		else if (lightIndex == 4) return GL_LIGHT4;
		else if (lightIndex == 5) return GL_LIGHT5;
		else if (lightIndex == 6) return GL_LIGHT6;
		else if (lightIndex == 7) return GL_LIGHT7;
		else roe_except("too many lights", "getLightName");
	}

} // namespace roe

