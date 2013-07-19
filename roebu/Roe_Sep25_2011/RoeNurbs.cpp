#include "RoeNurbs.h"
#include "RoeException.h"
#include "RoeCommon.h"
#include "RoeRandom.h"
#include "RoeUtil.h"
#include "RoeModel.h"
#include "RoeMesh.h"
#ifdef ROE_USE_SHADERS
	#include "RoeProgram.h"
#endif
#include <vector>

//show box?
//#define ROE_NURBS_DEBUG 13.02
#define ROE_NURBS_RANDOM_HILLS 0.0

namespace roe {
	
	Nurbs::Nurbs()
	{
		vertices    = nullptr;
		tangents    = nullptr;
		m_texCoords = nullptr;
	}
	
	Nurbs::~Nurbs() {
		destroy();
	}
	
	void Nurbs::createControlPoints(int nu, int nv, Vector3 A, Vector3 B, Vector3 C, Vector3 D) {
		if (nv < 0) nv = nu;
		if (nu < 4 || nv < 4) roe_except("wrong size", "create");
		m_nu = nu; m_nv = nv;
		vertices = new Vector3*[m_nu];
		tangents = new Vector3**[m_nu];
		//initializing with unit grid
		const float denuInv =  1.0f/(m_nu - 1.0f);
		const float denvInv =  1.0f/(m_nv - 1.0f);
		const Vector3 east  = (B-A)/(m_nu - 1.0f);
		const Vector3 south = (C-A)/(m_nv - 1.0f);
		for (int x = 0; x < m_nu; x++) {
			vertices[x] = new Vector3 [m_nv];
			tangents[x] = new Vector3*[m_nv];
			for (int y = 0; y < m_nv; y++) {
				vertices[x][y] = util::interpolateBilinear(A,B,C,D, x*denuInv, y*denvInv);
				vertices[x][y].y += ROE_NURBS_RANDOM_HILLS * (roe_rand_unit-0.5)*2;
				tangents[x][y] = new Vector3[2];
				tangents[x][y][0] = east; //just two
				tangents[x][y][1] = south; //starting values
			}
		}
	}
	void Nurbs::createMesh(int usteps, int vsteps, bool excludeOuter, const Vector2& A, const Vector2& B, const Vector2& C) {
		if (usteps < 1) roe_except("bad ustep-size: "+util::toString(usteps), "createMesh");
		if (vsteps <= 0) vsteps = usteps;
		const float uinv = 1.0f/usteps, vinv = 1.0f/vsteps;
		float fu, fv;
		std::vector<Vector3> grid;
		const int stride = excludeOuter?1:0;
		for(int y = stride; y < m_nv-1-stride; ++y) {
			fv = 0.0f;
			for(int v = 0; v < vsteps; ++v) {
				for(int x = stride; x < m_nu-1-stride; ++x) {
					fu = 0.0;
					for(int u = 0; u < usteps; ++u) {
						grid.push_back(util::interpolateHermite(
							util::interpolateHermite(vertices[x][y  ]   ,tangents[x  ][y  ][0],vertices[x+1][y  ],tangents[x+1][y  ][0],fu),
							util::interpolateLinear (tangents[x][y  ][1],tangents[x+1][y  ][1],fu),
							util::interpolateHermite(vertices[x][y+1]   ,tangents[x  ][y+1][0],vertices[x+1][y+1],tangents[x+1][y+1][0],fu),
							util::interpolateLinear (tangents[x][y+1][1],tangents[x+1][y+1][1],fu),
							fv));//*/
						//grid.push_back(util::interpolateBilinear(vertices[x][y],vertices[x+1][y],vertices[x][y+1],vertices[x+1][y+1], fu, fv));
						fu += uinv;
					}
				}
				//last column extra
				grid.push_back(util::interpolateHermite(vertices[m_nu-1-stride][y  ],tangents[m_nu-1-stride][y  ][1],
					                                    vertices[m_nu-1-stride][y+1],tangents[m_nu-1-stride][y+1][1],fv));
				fv += vinv;
			}
		}
		//bottom line extra
		for (int x = stride; x < m_nu-1-stride; ++x) { 
			fu = 0.0f;
			for (int u = 0; u < usteps; ++u) {
				grid.push_back(util::interpolateHermite(vertices[x  ][m_nv-1-stride],tangents[x  ][m_nv-1-stride][0],
				                                        vertices[x+1][m_nv-1-stride],tangents[x+1][m_nv-1-stride][0],fu) );
				fu += uinv;
			}
		}
		grid.push_back(vertices[m_nu-1-stride][m_nv-1-stride]);
		
		long numusteps = (m_nu-1-2*stride)*usteps+1;
		long numvsteps = (m_nv-1-2*stride)*vsteps+1;
		/*cout << (int)grid.size() << endl;
		for (int i = 0; i < (int)grid.size(); ++i) {
			cout << grid[i].toString() << endl;
		}
		cout << numusteps << endl << numvsteps << endl;*/
		
		Mesh mesh;
		mesh.build(grid.data(), numusteps, numvsteps);
		grid.clear();
		mesh.setTexCoords(A, (B-A)/(numusteps-1), (C-A)/(numvsteps-1));
		mesh.calculateNormals();
		mesh.simplify();
		m_vbo.destroy();
		mesh.fillVBO(m_vbo);
		m_vbo.create();
		mesh.destroy();
	}
	void Nurbs::destroy() {
		if(!vertices) return;
		ROE_SAFE_DELETE_2ARRAY(vertices,m_nu);
		ROE_SAFE_DELETE_3ARRAY(tangents,m_nu,m_nv);
		m_vbo.destroy();
	}
	void Nurbs::render() {
		const Matrix m = getMatrix();
#ifdef ROE_USE_SHADERS
		const Matrix n = getRotationMatrix();
		Program::setStdTransformationMatrix(m);
		Program::setStdNormalMatrix(n);
#else
		glPushMatrix();
			glMultMatrixf(m.n);
#endif
		
#ifdef ROE_NURBS_DEBUG
		for (int x = 0; x < m_nu; x++) {
			for (int y = 0; y < m_nv; y++) {
				Model::drawBox(Matrix::tranformCoords(vertices[x][y],m), ROE_NURBS_DEBUG);
			}
		}
#endif
		m_vbo.enable();
			m_vbo.draw(GL_TRIANGLES);
		m_vbo.disable();
#ifndef ROE_USE_SHADERS
		glPopMatrix();
#endif
	}
	
	Vector3 Nurbs::interpolate(float u, float v) {
		const int x = 1+(int)(u*(m_nu-3));
		const int y = 1+(int)(v*(m_nv-3));
		u *= ((m_nu-3.0)/x);
		v *= ((m_nv-3.0)/y);
		const Vector3 a = util::interpolateHermite(vertices[x][y  ],tangents[x][y  ][0],vertices[x+1][y  ],tangents[x+1][y  ][0],u);
		const Vector3 b = util::interpolateHermite(vertices[x][y+1],tangents[x][y+1][0],vertices[x+1][y+1],tangents[x+1][y+1][0],u);
		return util::interpolateHermite(a,2.0*(tangents[x][y][1]+tangents[x+1][y][1]),b,0.5*(tangents[x][y+1][1]+tangents[x+1][y+1][1]),v);
	}
	
	void Nurbs::calculateTangentsCatmull(float factor) {
		tangents[0][0][0] = 2.0*factor*(vertices[1][0]-vertices[0][0]);
		tangents[0][0][1] = 2.0*factor*(vertices[0][1]-vertices[0][0]);
		tangents[m_nu-1][0][0] = 2.0*factor*(vertices[m_nu-1][0]-vertices[m_nu-2][0]);
		tangents[m_nu-1][0][1] = 2.0*factor*(vertices[m_nu-1][1]-vertices[m_nu-1][0]);
		tangents[0][m_nv-1][0] = 2.0*factor*(vertices[1][m_nv-1]-vertices[0][m_nv-1]);
		tangents[0][m_nv-1][1] = 2.0*factor*(vertices[0][m_nv-1]-vertices[0][m_nv-2]);
		tangents[m_nu-1][m_nv-1][0] = 2.0*factor*(vertices[m_nu-1][m_nv-1]-vertices[m_nu-2][m_nv-1]);
		tangents[m_nu-1][m_nv-1][1] = 2.0*factor*(vertices[m_nu-1][m_nv-1]-vertices[m_nu-1][m_nv-2]);
		for (int y = 1; y < m_nv-1; y++) {
			tangents[0][y][0] = 2.0*factor*(vertices[1][y  ]-vertices[0][y  ]);
			tangents[0][y][1] = factor*(vertices[0][y+1]-vertices[0][y-1]);
			tangents[m_nu-1][y][0] = 2.0*factor*(vertices[m_nu-1][y  ]-vertices[m_nu-2][y  ]);
			tangents[m_nu-1][y][1] = factor*(vertices[m_nu-1][y+1]-vertices[m_nu-1][y-1]);
		}
		for (int x = 1; x < m_nu-1; x++) {
			tangents[x][0][0] = factor*(vertices[x+1][0]-vertices[x-1][0]);
			tangents[x][0][1] = 2.0*factor*(vertices[x  ][1]-vertices[x  ][0]);
			tangents[x][m_nv-1][0] = factor*(vertices[x+1][m_nv-1]-vertices[x-1][m_nv-1]);
			tangents[x][m_nv-1][1] = 2.0*factor*(vertices[x  ][m_nv-1]-vertices[x  ][m_nv-2]);
		}
		for (int y = 1; y < m_nv-1; y++) {
			for (int x = 1; x < m_nu-1; x++) {
				tangents[x][y][0] = factor*(vertices[x+1][y  ]-vertices[x-1][y  ]);
				tangents[x][y][1] = factor*(vertices[x  ][y+1]-vertices[x  ][y-1]);
			}
		}
	}
	
} // namespace roe
