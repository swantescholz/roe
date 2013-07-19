#include "RoeBillboard.h"
#include "RoePlane.h"
#include "RoeMatrix.h"

#include <GL/gl.h>

namespace roe {

	Billboard::Billboard() {
		avVertices[0] = Vector3(-0.5,0.0, 0.5);
		avVertices[1] = Vector3( 0.5,0.0, 0.5);
		avVertices[2] = Vector3( 0.5,0.0,-0.5);
		avVertices[3] = Vector3(-0.5,0.0,-0.5);
		size = Vector2(1,1);
		dir  = Vector3(0,0,1);
		normal  = Vector3(0,1,0);
		pos     = Vector3(0,0,0);
	}
	
	Billboard::~Billboard() {
		
	}
	
	void Billboard::render() {
		glBegin(GL_TRIANGLE_FAN);
		glNormal3fv(normal.rg);
		glTexCoord2i(0,0); glVertex3fv(avVertices[0].rg);
		glTexCoord2i(1,0); glVertex3fv(avVertices[1].rg);
		glTexCoord2i(1,1); glVertex3fv(avVertices[2].rg);
		glTexCoord2i(0,1); glVertex3fv(avVertices[3].rg);
		glEnd();
	}
	
	void Billboard::update() {
		static Vector3 av[4] = {
			Vector3( 0.5,0.0,-0.5),
			Vector3(-0.5,0.0,-0.5),
			Vector3(-0.5,0.0, 0.5),
			Vector3( 0.5,0.0, 0.5)};
		
		normal = Vector3::makePerpendicularTo(normal, dir);
		Matrix mTransform = Matrix::rotationFromTo(Vector3::Z, normal);
		
		for(int i = 0; i < 4; i++) {
			avVertices[i] = Matrix::transformCoords(av[i]*Vector3(size.x,1.0,size.y), mTransform);
		}
	}

} // namespace roe
