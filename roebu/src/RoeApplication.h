#ifndef ROEAPPLICATION_H_
#define ROEAPPLICATION_H_

#include "RoeSDL_GL.h"
#include "RoeAudioManager.h"
#include "RoeRandom.h"
#include "RoeTexture.h"
#include "RoeLogfile.h"
#include "RoeCommon.h"


namespace roe {
	
	//class Application, it combines Ogre + Ode + OIS + SDL_Mixer
	class Application
	{
	public:
		Application();
		virtual ~Application();
		
	protected:
		
	private:
		
	};
	
} // namespace roe

#endif /*ROEAPPLICATION_H_*/
