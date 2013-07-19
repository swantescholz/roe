#ifndef ROE_H_
#define ROE_H_

//Roe (relevant Ogre extensions) or (reusable O[Ogre/Ode/Ois] engine)
//static_assert(sizeof(void*)>=8, "64-bit code generation required for this library.");
static_assert(sizeof(unsigned int)>=4, "int-size too small.");

/*Roe TODO:
-terrain with LOD
//*/

//IMPORTANT REMARKS:
//Roe works counter clock wise(CCW)!
//Roe Matrix-system works ROW-MAJOR!

//everything is in namespace Roe

//general Roe application base class
#include "RoeApplication.h"
//graphics with SDL, SDL_image, SDL_ttf and OpenGL
#include "RoeSDL_GL.h"  //+input with SDL
#include "RoeTexture.h" //SDL_image is used
#include "RoeFractalTexture.h" //with fractals generated textures
#include "RoeGradient.h" //1D-Texture
#include "RoeFont.h"    //SDL_ttf is used
#include "RoeLight.h"
#include "RoeModel.h"   //OFF-models
#include "RoeParticle.h"
#include "RoeProgram.h" //shader-programs
#include "RoeNurbs.h"   //OpenGL-2D-evaluators
#include "RoeVertexBufferObject.h"     //OpenGL(possibly ARB)-vertex(and index)-buffer-object
#include "RoeMesh.h"
#include "RoeTerrain.h"
#include "RoeBillboard.h"
//physics with Bullet Physics (namespace 'Physics')
#include "RoeEngine.h"
#include "RoeBody.h"
#include "RoeMotionState.h"
#include "RoeSplinter.h"
#include "RoeHACD.h" //for convex decomposition
#include "RoeHACDCompoundShape.h" //for convex decomposition
//audio with SDL_mixer
#include "RoeMusic.h"
#include "RoeSound.h"
#include "RoeAudioManager.h"
//file i/o
#include "RoeTextfile.h"
#include "RoeBinFile.h"
#include "RoeLogfile.h"
//math
#include "RoeKeyframeInterpolator.h"
#include "RoeRandom.h"
#include "RoeQuaternion.h"
#include "RoeMatrix.h"
#include "RoeVector2.h"
#include "RoeVector3.h"
#include "RoeColor.h"
#include "RoeMaterial.h"
#include "RoePlane.h"
#include "RoeBoundingBox.h"
#include "RoeBoundingSphere.h"
#include "RoeCamera.h"
#include "RoeObject.h"
#include "RoeForceField.h"
//system specific things (clipboard, directory parsing...)
#include "RoeSystem.h"
//other
#include "RoeTokenizer.h"
#include "RoeTimer.h"
#include "RoeSingleton.h"
#include "RoeException.h"
//common and util
#include "RoeUtil.h"
#include "RoeCommon.h"

//libraries:
#include <GL/gl.h>
#include <SDL/SDL.h>

#endif /*ROE_H_*/
