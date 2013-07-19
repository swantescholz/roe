#include "RoeObject.h"

namespace roe {
	
	Object::Object()
	{
		reset();
	}
	void Object::reset() {
		m_bUpdated = false;
		m_vPosition = Vector3(0.0f);
		m_vXAxis = Vector3(1.0f, 0.0f, 0.0f);
		m_vYAxis = Vector3(0.0f, 1.0f, 0.0f);
		m_vZAxis = Vector3(0.0f, 0.0f, 1.0f);
		m_vScaling = Vector3(1.0f);
		m_mScaling = Matrix::IDENTITY;
		m_mMatrix    = Matrix::IDENTITY;
		m_mInvMatrix = Matrix::IDENTITY;
		
		update();
	}
	void Object::update() {
		if (m_bUpdated) return;
		m_mMatrix = m_mScaling *
				Matrix::axes(m_vXAxis, m_vYAxis, m_vZAxis) *
				Matrix::translation(m_vPosition);
		
		m_mInvMatrix = Matrix::invert(m_mMatrix);
		m_bUpdated=true;
	}
	void Object::rotateAbs(const Vector3& vRotation) {
		Matrix mRotation(Matrix::rotationX(vRotation.x));
		m_vYAxis = Matrix::transformNormal(m_vYAxis, mRotation);
		m_vZAxis = Vector3::cross(m_vXAxis, m_vYAxis);
		
		mRotation = Matrix::rotationY(vRotation.y);
		m_vXAxis = Matrix::transformNormal(m_vXAxis, mRotation);
		m_vZAxis = Vector3::cross(m_vXAxis, m_vYAxis);
		
		mRotation = Matrix::rotationZ(vRotation.z);
		m_vXAxis = Matrix::transformNormal(m_vXAxis, mRotation);
		m_vYAxis = Matrix::transformNormal(m_vYAxis, mRotation);
		
		m_bUpdated=false;
	}
	void Object::rotateRel(const Vector3& vRotation) {
		Matrix mRotation(Matrix::rotationAxis(m_vXAxis, vRotation.x));
		m_vYAxis = Matrix::transformNormal(m_vYAxis, mRotation);
		m_vZAxis = Vector3::cross(m_vXAxis, m_vYAxis);
		
		mRotation = Matrix::rotationAxis(m_vYAxis, vRotation.y);
		m_vXAxis = Matrix::transformNormal(m_vXAxis, mRotation);
		m_vZAxis = Vector3::cross(m_vXAxis, m_vYAxis);
		
		mRotation = Matrix::rotationAxis(m_vZAxis, vRotation.z);
		m_vXAxis = Matrix::transformNormal(m_vXAxis, mRotation);
		m_vYAxis = Matrix::transformNormal(m_vYAxis, mRotation);
		
		m_bUpdated=false;
	}
	void Object::align(const Vector3& vZAxis,
	                   const Vector3& vUp) // = Vector3(0.0f, 1.0f, 0.0f)
	{
		m_vZAxis = Vector3::normalize(vZAxis);
		m_vXAxis = Vector3::normalize(Vector3::cross(vUp, m_vZAxis));
		m_vYAxis = Vector3::normalize(Vector3::cross(m_vZAxis, m_vXAxis));
		m_bUpdated=false;
	}
	void Object::lookAt(const Vector3& vLookAt,
	                    const Vector3& vUp) // = Vector3(0.0f, 1.0f, 0.0f)
	{
		align(vLookAt - m_vPosition, vUp);
	}
	/*void Object::setMatrix(const Matrix& m) {
		m_vXAxis=Matrix::transformNormal(Vector3::X,m);
		m_vYAxis=Matrix::transformNormal(Vector3::Y,m);
		m_vZAxis=Matrix::transformNormal(Vector3::Z,m);
		m_vPosition=Matrix::transformCoords(Vector3::ZERO,m);
		m_bUpdated=false;
	}
	void Object::transform(const Matrix& m) {
		m_vXAxis=Matrix::transformNormal(m_vXAxis,m);
		m_vYAxis=Matrix::transformNormal(m_vYAxis,m);
		m_vZAxis=Matrix::transformNormal(m_vZAxis,m);
		m_vPosition=Matrix::transformCoords(m_vPosition,m);
		m_bUpdated=false;
	}
	void Object::transformRel(const Matrix& m) {
		m_vXAxis=Matrix::transformNormal(m_vXAxis,m);
		m_vYAxis=Matrix::transformNormal(m_vYAxis,m);
		m_vZAxis=Matrix::transformNormal(m_vZAxis,m);
		m_vPosition+=Matrix::transformCoords(Vector3::ZERO,m);
		m_bUpdated=false;
	}//*/
	
} // namespace roe
