#ifndef CSURFACE_H_
#define CSURFACE_H_

#include "RoeVector2.h"
#include "RoeVector3.h"

namespace roe {

	class Billboard
	{
	public:
		Billboard();
		~Billboard();
		
		void render();
		
		Vector3 getPos   () {return pos;}
		Vector3 getNormal() {return normal;}
		Vector3 getDir   () {return dir;}
		Vector2 getSize  () {return size;}
		void setPos   (Vector3 v) {pos = v; update();}
		void setNormal(Vector3 v) {normal = v; /*normal.normalize();*/ update();}
		void setDir   (Vector3 v) {dir    = v; dir.normalize()   ; update();}
		void setSize  (Vector2 v) {size = v; update();}
		
	private:
		void update();
		
		Vector3 pos;
		Vector3 normal;
		Vector3 dir;
		Vector3 avVertices[4];
		Vector2 size; //edge length
	};

} // namespace roe
#endif /* CSURFACE_H_ */
