#ifndef ROEFORCEFIELD_H_
#define ROEFORCEFIELD_H_

#include <list>
#include <memory>
#include "RoeVector3.h"
#include "RoeVector2.h"
#include "RoePlane.h"
#include "RoeMatrix.h"
#include "RoeQuaternion.h"
#include "RoeObject.h"
#include "RoeUtil.h"

namespace roe {
	
	enum EForceFieldShape {
		FFS_SPHERE = 1,
		FFS_BOX,
		FFS_CYLINDER,
		FFS_TORUS
	};
	
	//base classes
	class dir_functor : public binary_function<const Vector3&, const Vector3&, Vector3>{ public:
		virtual Vector3 operator() (const Vector3& pos, const Vector3& vel) {return Vector3::ZERO;}
	};
	class strength_functor : public binary_function<const Vector3&, const Vector3&, double>{ public:
		virtual double operator() (const Vector3& pos, const Vector3& vel) {return 0.0;}
	};
	class collision_functor : public binary_function<const Vector3&, const Vector3&, bool>{ public:
		virtual bool operator() (const Vector3& pos) {return false;}
		virtual int getType() {return 0;} //returns the type of the shape
	};
	
	//direction behaviour functor-classes
	class ffbdConstant : public dir_functor{ public://constant
		ffbdConstant(const Vector3& v) : m_dir(Vector3::normalize(v)) {}
		Vector3 operator() (const Vector3& pos, const Vector3& vel) {return m_dir;}
	private: Vector3 m_dir;
	};
	class ffbdPointCentered : public dir_functor{ public://point-centered
		ffbdPointCentered(const Vector3& v = Vector3::ZERO) : center(v) {}
		Vector3 operator() (const Vector3& pos, const Vector3& vel) {return Vector3::normalize(center-pos);}
	private: Vector3 center;
	};
	class ffbdLineCentered : public dir_functor{ public://line-centered
		ffbdLineCentered(const Vector3& v1, const Vector3& v2) : linePos(v1), lineDir(v2) {}
		Vector3 operator() (const Vector3& pos, const Vector3& vel) {
			return Vector3::normalize(util::nearestPointOnRay(pos, linePos, lineDir) - pos);}
	private: Vector3 linePos, lineDir;
	};
	class ffbdPlaneCentered : public dir_functor{ public://plane-centered
		ffbdPlaneCentered(const Plane& p) : plane(p) {plane.normalize();}
		Vector3 operator() (const Vector3& pos, const Vector3& vel) {
			return ((plane.dotCoords(pos) > 0.0)?-1:1)*plane.n;}
	private: Plane plane;
	};
	class ffbdTorusCentered : public dir_functor{ public://torus-centered
		ffbdTorusCentered(const Vector3& v1, const Vector3& v2, double rBig)
		: center(v1), up(v2), plane(v1, v2), radius(rBig) {}
		Vector3 operator() (const Vector3& pos, const Vector3& vel) {
			return Vector3::normalize(center + radius*Vector3::normalize(plane.nearestPoint(pos) - center) - pos);
		}
	private: Vector3 center, up; Plane plane; double radius; 
	};
	class ffbdResistance : public dir_functor{ public://anti-velocity direction (resistance)
		Vector3 operator() (const Vector3& pos, const Vector3& vel) {
			//if (Vector3::isNearlyZero(vel)) return Vector3::ZERO;
			return Vector3::normalize(-vel);
		}
	};
	//lorentz forces ortho:
	class ffbdCross : public dir_functor{ public://orthogonal randomizer
		ffbdCross(const Vector3& v) : B_field(v) {B_field.normalize();}
		Vector3 operator() (const Vector3& pos, const Vector3& vel) {
			return Vector3::normalize(Vector3::cross(vel, B_field));
		}
	private: Vector3 B_field;
	};
	//randomizing accelerators:
	class ffbdRandomOrtho : public dir_functor{ public://orthogonal randomizer
		Vector3 operator() (const Vector3& pos, const Vector3& vel) {
			return Vector3::makePerpendicularTo(Vector3::createRandomUnit(), vel);
		}
	};
	
	//strength behaviour functor-classes (THE MASS OF THE PARTICLE IS CONSIDERED TO BE 1kg!)
	class ffbsConstant : public strength_functor{ public://constant
		ffbsConstant(double d) : strength(d) {}
		double operator() (const Vector3& pos, const Vector3& vel) {return strength;}
	private: double strength;
	};
	//linears
	class ffbsDistPoint : public strength_functor{ public:// linear to point
		ffbsDistPoint(double d1, double d2, double r = 1.0, Vector3 v = Vector3::ZERO) : inner(d1), outer(d2), radius(r), center(v) {}
		double operator() (const Vector3& pos, const Vector3& vel) {return (pos-center).length()/radius * (outer-inner) + inner;}
	private: double inner, outer, radius; Vector3 center;
	};
	class ffbsDistLine : public strength_functor{ public:// linear to line
		ffbsDistLine(double d1, double d2, Vector3 vLinePos, Vector3 vLineDir, double r = 1.0)
		: inner(d1), outer(d2), radius(r), linePos(vLinePos), lineDir(vLineDir) {}
		double operator() (const Vector3& pos, const Vector3& vel) {
			return util::distancePointToRay(pos, linePos, lineDir)/radius * (outer-inner) + inner;}
	private: double inner, outer, radius; Vector3 linePos, lineDir;
	};
	class ffbsDistPlane : public strength_functor{ public:// linear to plane
		ffbsDistPlane(double d1, double d2, const Plane& p, double r = 1.0) : inner(d1), outer(d2), radius(r), plane(p) {plane.normalize();}
		double operator() (const Vector3& pos, const Vector3& vel) {return (plane.distance(pos))/radius * (outer-inner) + inner;}
	private: double inner, outer, radius; Plane plane;
	};
	class ffbsDistTorus : public strength_functor{ public:// linear to torus
		ffbsDistTorus(double d1, double d2, double rSmall, double rBig = 1.0, const Vector3& v1 = Vector3::ZERO, const Vector3& v2 = Vector3::Y)
		: inner(d1), outer(d2), smallRadius(rSmall), bigRadius(rBig), center(v1), up(v2), plane(v1,v2) {}
		double operator() (const Vector3& pos, const Vector3& vel) {
			//return (Vector3::normalize(Vector3(pos.x,0.0,pos.z)) - pos).length()/radius * (outer-inner) + inner;
			return ((center + bigRadius*Vector3::normalize(plane.nearestPoint(pos) - center) - pos).length()-smallRadius)
					/bigRadius*(outer-inner)+inner;
		}
	private: double inner, outer, smallRadius, bigRadius; Vector3 center, up; Plane plane;
	};
	//gravity (squared distance)
	class ffbsGravity : public strength_functor{ public://gravity (F = -G*m1*m2/r^2) (G=6.67*10^-11)
		ffbsGravity(double mass, const Vector3& v1 = Vector3::ONE, const Vector3& v2 = Vector3::ZERO) //scaling is important to calculate with absolute distances
		: factor(ROE_GRAVITY_CONSTANT*mass), scaling(v1), center(v2) {}
		double operator() (const Vector3& pos, const Vector3& vel) {
			return factor/((pos-center)*scaling).length2();}
	private: double factor; Vector3 scaling, center;
	};
	//drags
	class ffbsDragTurbulence : public strength_functor{ public://air drag (F = 0.5*p*Cd*A*vel^2)
		ffbsDragTurbulence(double p, double A = 1.0, double Cd = 2.0)
		: factor(0.5*p*A*Cd) {}
		double operator() (const Vector3& pos, const Vector3& vel) {
			return factor*vel.length2();}
	private: double factor;
	};
	class ffbsDragViscosity : public strength_functor{ public://fluid viscosity drag (F = 6*Pi*viscosity_n*radius_r*vel)
		ffbsDragViscosity(double fluid_viscosity_n, double radius_r = 0.053051647697295)
		: factor(6.0*ROE_PI*fluid_viscosity_n*radius_r) {}
		double operator() (const Vector3& pos, const Vector3& vel) {
			return factor*vel.length();}
	private: double factor;
	};
	//randoms
	class ffbsRandomUniform : public strength_functor{ public:
		ffbsRandomUniform(double d1, double d2)
		: min(d1), max(d2) {}
		double operator() (const Vector3& pos, const Vector3& vel) {return roe_random.uniformReal(min, max);}
	private: double min, max;
	};
	class ffbsRandomNormal : public strength_functor{ public:
		ffbsRandomNormal(double d1, double d2)
		: mean(d1), sigma(d2) {}
		double operator() (const Vector3& pos, const Vector3& vel) {return roe_random.normal(mean, sigma);}
	private: double mean, sigma;
	};
	
	//collision behaviour functor-classes
	class ffbcSphere : public collision_functor{ public:
		ffbcSphere() {}
		bool operator() (const Vector3& pos) {
			return pos.length2() <= 1.0;}
		int getType() {return 1;}
	};
	class ffbcBox : public collision_functor{ public:
		ffbcBox() {}
		bool operator() (const Vector3& pos) {
			if (pos.x < -1.0 || pos.x > 1.0) return false;
			if (pos.y < -1.0 || pos.y > 1.0) return false;
			if (pos.z < -1.0 || pos.z > 1.0) return false;
			return true;
		}
		int getType() {return 2;}
	};
	class ffbcCylinder : public collision_functor{ public:
		ffbcCylinder() {}
		bool operator() (const Vector3& pos) {
			if (pos.y < -1.0 || pos.y > 1.0) return false;
			if (Vector2(pos.x, pos.z).length2() > 1.0) return false;
			return true;
		}
		int getType() {return 3;}
	};
	class ffbcTorus : public collision_functor{ public:
		ffbcTorus(double r) : sqradius(r*r) {}
		bool operator() (const Vector3& pos) {
			return (Vector3::normalize(Vector3(pos.x,0.0,pos.z)) - pos).length2() <= sqradius;}
		int getType() {return 4;}
	private: double sqradius; //small radius squared
	};
	
	//base class for force fields
	class ForceField : public Object{
	public:
		static Matrix createMatSphere(const Vector3& pos, const Vector3& size) {return Matrix::scaling(size)*Matrix::translation(pos);}
		static Matrix createMatSphere(const Vector3& pos, double r) {return createMatSphere(pos, Vector3(r,r,r));}
		static Matrix createMatBox(const Vector3& pos, const Vector3& size) {return Matrix::scaling(0.5*size)*Matrix::translation(pos);}
		static Matrix createMatBox(const Vector3& pos, double x, double y, double z) {return createMatSphere(pos, Vector3(x,y,z));}
		static Matrix createMatBox(const Vector3& pos, double a) {return createMatSphere(pos, Vector3(a,a,a));}
		static Matrix createMatBox(const Vector3& pos, const Vector3& dir, double x, double z)
		{return Matrix::scaling(Vector3(0.5*x,dir.length(),0.5*z))*Matrix::rotationFromTo(Vector3::Y, dir)*Matrix::translation(pos);}
		static Matrix createMatBox(const Vector3& pos, const Vector3& dir, double a) {return createMatBox(pos, dir, a, a);}
		static Matrix createMatCylinder(const Vector3& pos, const Vector3& dir, double r)
		{return Matrix::scaling(Vector3(r,dir.length(),r))*Matrix::rotationFromTo(Vector3::Y, dir)*Matrix::translation(pos);}
		static Matrix createMatTorus(const Vector3& pos, const Vector3& up, double rBig)
		{return Matrix::scaling(Vector3(rBig,1.0,rBig))*Matrix::rotationFromTo(Vector3::Y, up)*Matrix::translation(pos);}
		
		ForceField();
		ForceField(const ForceField& rhs);
		ForceField& operator=(const ForceField& rhs);
		virtual ~ForceField();
		
		template<typename T> void setBehaviourDir      (T func) {m_pDirFunc.reset      (new T(func));}
		template<typename T> void setBehaviourStrength (T func) {m_pStrengthFunc.reset (new T(func));}
		template<typename T> void setBehaviourCollision(T func) {m_pCollisionFunc.reset(new T(func));}
		dir_functor*       getDirFunctor      () const {return m_pDirFunc.get();}
		strength_functor*  getStrengthFunctor () const {return m_pStrengthFunc.get();}
		collision_functor* getCollisionFunctor() const {return m_pCollisionFunc.get();}
		int getCollisionType() const {if(!m_pCollisionFunc) return 0; return m_pCollisionFunc->getType();}
		
		Vector3 influence(const Vector3& pos, const Vector3& vel) //first transforms pos and vel, and then calls overridden method to return force
		{return Matrix::transformNormal(
				calculateInfluence(Matrix::transformCoords(pos, getInvMatrix()), Matrix::transformNormal(vel, getInvMatrix())),
				getMatrix());}
	protected:
		virtual Vector3 calculateInfluence(const Vector3& pos, const Vector3& vel); //to be overridden: returns the fitting force
		
		std::shared_ptr<dir_functor>       m_pDirFunc;
		std::shared_ptr<strength_functor>  m_pStrengthFunc;
		std::shared_ptr<collision_functor> m_pCollisionFunc;
	};
	
	//a group of force field tied together as a list (dont write using std::list!)
	class ForceFieldGroup : public ForceField, public std::list<ForceField*> {
	public:
		ForceFieldGroup();
		ForceFieldGroup(const ForceFieldGroup& rhs);
		ForceFieldGroup& operator=(const ForceFieldGroup& rhs);
		virtual ~ForceFieldGroup();
		
		template<typename T> void addForceField(const T& ff) {
			ForceField *pff = new T(ff);
			push_back(pff);
		}
		void clear();
	protected:
		virtual Vector3 calculateInfluence(const Vector3& pos, const Vector3& vel);
	};
	
	
} //namespace roe

#endif /* ROEFORCEFIELD_H_ */
