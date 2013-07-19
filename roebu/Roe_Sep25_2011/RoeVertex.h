#ifndef ROEVERTEX_H_
#define ROEVERTEX_H_

#include "RoeVector2.h"
#include "RoeVector3.h"

namespace roe {
	
	class Vertex {
	public:
		Vertex(Vector3 pp = Vector3::ZERO, Vector3 pn = Vector3::Y, Vector2 ptc = Vector2::ZERO, long pindex = 0)
		: p(pp), n(pn), tc(ptc), index(pindex) {}
		Vertex(const Vertex& rhs) : p(rhs.p), n(rhs.n), tc(rhs.tc), index(rhs.index) {}
		Vertex& operator=(const Vertex& rhs) {p = rhs.p; n = rhs.n; tc = rhs.tc; index = rhs.index; return *this;}
		~Vertex() {}
		Vector3 p,n;
		Vector2 tc;
		long index;
	};
	
} // namespace roe

#endif /* ROEVERTEX_H_ */
