#include "RoeForceField.h"
#include "RoeException.h"
#include "RoeRandom.h"

namespace roe {
	
	ForceField::ForceField() {
		
	}
	ForceField::ForceField(const ForceField& rhs)
	: m_pDirFunc      (rhs.m_pDirFunc)
	, m_pStrengthFunc (rhs.m_pStrengthFunc)
	, m_pCollisionFunc(rhs.m_pCollisionFunc)
	{}
	ForceField& ForceField::operator=(const ForceField& rhs) {
		m_pDirFunc        = rhs.m_pDirFunc;
		m_pStrengthFunc   = rhs.m_pStrengthFunc;
		m_pCollisionFunc  = rhs.m_pCollisionFunc;
		return (*this);
	}
	ForceField::~ForceField() {}
	
	Vector3 ForceField::calculateInfluence(const Vector3& pos, const Vector3& vel) {
		/*if (m_pDirFunc)
			throw Exception("no direction function found", "ForceField::calculateInfluence", "RoeForceField.cpp", ROE_LINE);
		if (m_pStrengthFunc)
			throw Exception("no strength function found", "ForceField::calculateInfluence", "RoeForceField.cpp", ROE_LINE);
		if (m_pCollisionFunc)
			throw Exception("no collision function found", "ForceField::calculateInfluence", "RoeForceField.cpp", ROE_LINE);
		//*/
		if (!(*m_pCollisionFunc)(pos)) return Vector3::ZERO; //no collision
		
		return (*m_pDirFunc)(pos,vel) * (*m_pStrengthFunc)(pos,vel);
	}
	
	//------------------------------------
	ForceFieldGroup::ForceFieldGroup() {}
	ForceFieldGroup::ForceFieldGroup(const ForceFieldGroup& rhs)
	: ForceField(rhs)
	{
		clear();
		for (auto it = rhs.begin(); it != rhs.end(); ++it) {
			addForceField(*(*it));
		}
	}
	ForceFieldGroup& ForceFieldGroup::operator=(const ForceFieldGroup& rhs) {
		ForceField::operator=(rhs);
		for (auto it = rhs.begin(); it != rhs.end(); ++it) {
			addForceField(*(*it));
		}
		return (*this);
	}
	ForceFieldGroup::~ForceFieldGroup() {
		clear();
	}
	void ForceFieldGroup::clear() {
		for (auto it = begin(); it != end(); ++it) {
			delete (*it);
		}
		std::list<ForceField*>::clear();
	}
	Vector3 ForceFieldGroup::calculateInfluence(const Vector3& pos, const Vector3& vel) {
		Vector3 acc(0,0,0);
		for (auto it = begin(); it != end(); ++it) {
			acc += (*it)->influence(pos, vel);
		}
		return acc;
	}
	
	
	
} //namespace roe
