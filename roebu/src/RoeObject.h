#ifndef ROEOBJECT_H_
#define ROEOBJECT_H_

#include "RoeVector2.h"
#include "RoeVector3.h"
#include "RoeMatrix.h"
#include "RoeQuaternion.h"

namespace roe {

	class Object {
	protected:
		bool    m_bUpdated;
		Vector3 m_vXAxis;
		Vector3 m_vYAxis;
		Vector3 m_vZAxis;
		Vector3 m_vPosition;
		Vector3 m_vScaling;
		Matrix  m_mScaling;
		Matrix  m_mMatrix;
		Matrix  m_mInvMatrix;
	public:
		Object();
		virtual ~Object() {}
		
		// methods
		void reset();
		void update();
		void rotateAbs(const Vector3& vRotation);
		void rotateRel(const Vector3& vRotation);
		void align    (const Vector3& vZAxis , const Vector3& vUp = Vector3(0.0f, 1.0f, 0.0f));
		void lookAt   (const Vector3& vLookAt, const Vector3& vUp = Vector3(0.0f, 1.0f, 0.0f));
		void setRotation(const Quaternion& q) {align(q.zAxis(), q.yAxis());}
		void setRotation(const Matrix    & m) {align(Matrix::transformNormal(Vector3::Z,m),Matrix::transformNormal(Vector3::Y,m));}
		void copyTransformation(Object *obj) {setScaling(obj->getScaling()); setRotation(obj->getQuaternion()); setPosition(obj->getPosition()); update();}
		
		// Inline-methods
		Vector3 absToRelPos(const Vector3& v) {return Matrix::transformCoords(v, m_mInvMatrix);}
		Vector3 relToAbsPos(const Vector3& v) {return Matrix::transformCoords(v, m_mMatrix);}
		Vector3 absToRelDir(const Vector3& v) {return Matrix::transformNormal(v, m_mInvMatrix);}
		Vector3 relToAbsDir(const Vector3& v) {return Matrix::transformNormal(v, m_mMatrix);}
		void    translateAbs(const Vector3& vAdd) {m_vPosition += vAdd; m_bUpdated=false;}
		void    translateRel(const Vector3& vAdd) {m_vPosition += Matrix::transformNormal(vAdd, m_mMatrix); m_bUpdated=false;}
		void    setPosition(const Vector3& vPosition){m_vPosition = vPosition; m_bUpdated=false;}
		Vector3 getPosition() {return m_vPosition;}
		float          getDistance(const Vector3& vPoint) {return Vector3::length(vPoint - m_vPosition);}
		Vector3 getXAxis() {return m_vXAxis;}
		Vector3 getYAxis() {return m_vYAxis;}
		Vector3 getZAxis() {return m_vZAxis;}
		void    setScaling (const Vector3& vScaling) {m_vScaling  = vScaling; m_mScaling = Matrix::scaling(m_vScaling); m_bUpdated=false;}
		void    multScaling(const Vector3& vScaling) {m_vScaling *= vScaling; m_mScaling = Matrix::scaling(m_vScaling); m_bUpdated=false;}
		Vector3  getScaling       (){return m_vScaling;}
		Matrix   getScalingMatrix (){return m_mScaling;}
		Matrix   getRotationMatrix(){if(!m_bUpdated) update(); return Matrix::axes(m_vXAxis, m_vYAxis, m_vZAxis);}
		//no scaling here!:
		//void setMatrix(const Matrix& m);
		//void transform(const Matrix& m);
		//void transformRel(const Matrix& m);
		Matrix     getMatrix    () {if (!m_bUpdated) update(); return m_mMatrix;}
		Matrix     getInvMatrix () {if (!m_bUpdated) update(); return m_mInvMatrix;}
		Quaternion getQuaternion() {if (!m_bUpdated) update(); return Quaternion(m_mMatrix);}
	};
	
} // namespace roe

#endif /* ROEOBJECT_H_ */
