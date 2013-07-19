#ifndef ROE_H_
#define ROE_H_

#ifdef ROE_DONT_DEFINE_THIS
CUSE = MINGW#		G++ or MINGW (linux/windows)
NAME = roeTest1#	name of the programm

CMINGW = i586-mingw32msvc-g++
CG++ = g++

CXX = $(C$(CUSE))

ARG_MINGW = -I/usr/i586-mingw32msvc/include -I/usr/i586-mingw32msvc/include/SDL -I/usr/i586-mingw32msvc/include/ode -u _WinMain@16 -L/usr/i586-mingw32msvc/lib
ARG_G++ = -I/usr/include/SDL -I/usr/include/GL -I/usr/include/ode

ARG = $(ARG_$(CUSE))

#LIB_MINGW = -lSDL_ttf -lSDL_mixer -lSDL_image -lSDLmain -lSDL -lopengl32
LIB_MINGW = /usr/i586-mingw32msvc/lib/SDL_ttf.lib /usr/i586-mingw32msvc/lib/SDL_mixer.lib /usr/i586-mingw32msvc/lib/SDL_image.lib /usr/i586-mingw32msvc/lib/libSDLmain.a /usr/i586-mingw32msvc/lib/libSDL.dll.a ode_opensim.dll -lopengl32
LIB_G++ = -lSDL -lSDL_ttf -lSDL_image -lSDL_mixer -lGL -lode -lgmpxx

#/usr/i586-mingw32msvc/lib/libode_single.a
#-lode

LIB = $(LIB_$(CUSE))

END_G++ =
END_MINGW = .exe

END = $(END_$(CUSE))

#enter classes here
OBJ = main.o CApp.o RoeApplication.o RoeAudioManager.o RoeBillboard.o RoeBinFile.o RoeCamera.o RoeColor.o RoeFont.o RoeForceField.o RoeFractalTexture.o RoeGradient.o RoeLight.o RoeLogfile.o RoeMaterial.o RoeMatrix.o RoeMesh.o RoeModel.o RoeMusic.o RoeNurbs.o RoeObject.o RoeParticle.o RoePhysicsEngine.o RoePhysicsModel.o RoePlane.o RoeProgram.o RoeSDL_GL.o RoeSound.o RoeSystem.o RoeTerrain.o RoeTextfile.o RoeTexture.o RoeTimer.o RoeTokenizer.o RoeUtil.o RoeVector2.o RoeVector3.o RoeVertexBufferObject.o# ode_obj/amotor.o ode_obj/array.o ode_obj/ball.o ode_obj/box.o ode_obj/capsule.o ode_obj/collision_cylinder_box.o ode_obj/collision_cylinder_plane.o ode_obj/collision_cylinder_sphere.o ode_obj/collision_cylinder_trimesh.o ode_obj/collision_kernel.o ode_obj/collision_quadtreespace.o ode_obj/collision_sapspace.o ode_obj/collision_space.o ode_obj/collision_transform.o ode_obj/collision_trimesh_box.o ode_obj/collision_trimesh_ccylinder.o ode_obj/collision_trimesh_disabled.o ode_obj/collision_trimesh_distance.o ode_obj/collision_trimesh_gimpact.o ode_obj/collision_trimesh_opcode.o ode_obj/collision_trimesh_plane.o ode_obj/collision_trimesh_ray.o ode_obj/collision_trimesh_sphere.o ode_obj/collision_trimesh_trimesh.o ode_obj/collision_trimesh_trimesh_new.o ode_obj/collision_util.o ode_obj/contact.o ode_obj/convex.o ode_obj/cylinder.o ode_obj/error.o ode_obj/export-dif.o ode_obj/fastdot.o ode_obj/fastldlt.o ode_obj/fastlsolve.o ode_obj/fastltsolve.o ode_obj/fixed.o ode_obj/heightfield.o ode_obj/hinge.o ode_obj/hinge2.o ode_obj/IceAABB.o ode_obj/IceContainer.o ode_obj/IceHPoint.o ode_obj/IceIndexedTriangle.o ode_obj/IceMatrix3x3.o ode_obj/IceMatrix4x4.o ode_obj/IceOBB.o ode_obj/IcePlane.o ode_obj/IcePoint.o ode_obj/IceRandom.o ode_obj/IceRandom.o ode_obj/IceRay.o ode_obj/IceRevisitedRadix.o ode_obj/IceSegment.o ode_obj/IceTriangle.o ode_obj/IceUtils.o ode_obj/joint.o ode_obj/lcp.o ode_obj/lmotor.o ode_obj/mass.o ode_obj/mat.o ode_obj/matrix.o ode_obj/memory.o ode_obj/misc.o ode_obj/null.o ode_obj/obstack.o ode_obj/ode.o ode_obj/odeinit.o ode_obj/odemath.o ode_obj/odeou.o ode_obj/odetls.o ode_obj/OPC_AABBCollider.o ode_obj/OPC_AABBTree.o ode_obj/OPC_BaseModel.o ode_obj/OPC_Collider.o ode_obj/OPC_Common.o ode_obj/OPC_HybridModel.o ode_obj/OPC_LSSCollider.o ode_obj/OPC_MeshInterface.o ode_obj/OPC_Model.o ode_obj/OPC_OBBCollider.o ode_obj/Opcode.o ode_obj/OPC_OptimizedTree.o ode_obj/OPC_Picking.o ode_obj/OPC_PlanesCollider.o ode_obj/OPC_RayCollider.o ode_obj/OPC_SphereCollider.o ode_obj/OPC_TreeBuilders.o ode_obj/OPC_TreeCollider.o ode_obj/OPC_VolumeCollider.o ode_obj/piston.o ode_obj/plane.o ode_obj/plane2d.o ode_obj/pr.o ode_obj/pu.o ode_obj/quickstep.o ode_obj/ray.o ode_obj/rotation.o ode_obj/slider.o ode_obj/sphere.o ode_obj/step.o ode_obj/stepfast.o ode_obj/testing.o ode_obj/timer.o ode_obj/universal.o ode_obj/util.o

all: $(NAME)$(END)# delete

$(NAME)$(END): $(OBJ)
	$(CXX) -o $@ $(ARG) $(OBJ) $(LIB) 

%.o: %.cpp
	$(CXX) -o $@ $(ARG) -c $<

delete:
	rm -f *.o
exit
#endif

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
