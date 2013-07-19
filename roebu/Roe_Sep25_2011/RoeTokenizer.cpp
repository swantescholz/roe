#include "RoeTokenizer.h"
#include "RoeUtil.h"
#include "RoeException.h"

namespace roe {

	Tokenizer::Tokenizer(std::string text, char stringDelim) {	
		m_sText = text; 
		m_cStringDelim = stringDelim;
	}
	
	Tokenizer::~Tokenizer() {}
	
	bool Tokenizer::readBool(bool skipline) {
		if (util::getNumberOfBoolsInString(m_sText) == 0)
			roe_except("no bool found in '" + m_sText + "'", "readBool");
		bool b = util::getFirstBoolOfStringAndErase(m_sText);
		if (skipline) skipLine(1);
		return b;
	}
	int Tokenizer::readInt(bool skipline) {
		if (util::getNumberOfNumbersInString(m_sText) == 0)
			roe_except("no ints found in '" + m_sText + "'", "readInt");
		int i = util::parseInt(util::getFirstNumberOfStringAndErase(m_sText));
		if (skipline) skipLine(1);
		return i;
	}
	float Tokenizer::readFloat(bool skipline) {
		if (util::getNumberOfNumbersInString(m_sText) == 0)
			roe_except("no doubles found in '" + m_sText + "'", "readDouble");
		float f = util::parseFloat(util::getFirstNumberOfStringAndErase(m_sText));
		if (skipline) skipLine(1);
		return f;
	}
	double Tokenizer::readDouble(bool skipline) {
		if (util::getNumberOfNumbersInString(m_sText) == 0)
			roe_except("no doubles found in '" + m_sText + "'", "readDouble");
		double d = util::parseDouble(util::getFirstNumberOfStringAndErase(m_sText));
		if (skipline) skipLine(1);
		return d;
	}
	std::string Tokenizer::readString(bool skipline) {
		if (util::getNumberOfStringsInString(m_sText, m_cStringDelim) == 0)
			roe_except("no strings found in '" + m_sText + "'", "readString");
		std::string s = util::getFirstStringOfStringAndErase(m_sText, m_cStringDelim);
		if (skipline) skipLine(1);
		return s;
	}
	std::string Tokenizer::readLine(bool skipline) {
		std::string::size_type pos = m_sText.find("\n");
		std::string s;
		if (pos == std::string::npos) {
			s = m_sText;
		}
		else {
			s = m_sText.substr(0,pos);
		}
		if (skipline) skipLine(1);
		return s;
	}
	
	void Tokenizer::readBool(bool& b, bool skipline) {
		if (util::getNumberOfBoolsInString(m_sText) != 0)
			b = util::getFirstBoolOfStringAndErase(m_sText);
		if (skipline) skipLine(1);
	}
	void Tokenizer::readInt(int& i, bool skipline) {
		if (util::getNumberOfNumbersInString(m_sText) != 0)
			i = util::parseInt(util::getFirstNumberOfStringAndErase(m_sText));
		if (skipline) skipLine(1);
	}
	void Tokenizer::readFloat(float& f, bool skipline) {
		if (util::getNumberOfNumbersInString(m_sText) != 0)
			f = util::parseFloat(util::getFirstNumberOfStringAndErase(m_sText));
		if (skipline) skipLine(1);
	}
	void Tokenizer::readDouble(double& d, bool skipline) {
		if (util::getNumberOfNumbersInString(m_sText) != 0)
			d = util::parseDouble(util::getFirstNumberOfStringAndErase(m_sText));
		if (skipline) skipLine(1);
	}
	void Tokenizer::readString(std::string& s, bool skipline) {
		if (util::getNumberOfStringsInString(m_sText, m_cStringDelim) != 0)
			s = util::getFirstStringOfStringAndErase(m_sText, m_cStringDelim);
		if (skipline) skipLine(1);
	}
	void Tokenizer::readLine(std::string& s, bool skipline) {
		std::string::size_type pos = m_sText.find("\n");
		if (pos == std::string::npos) {
			s = m_sText;
		}
		else {
			s = m_sText.substr(0,pos);
		}
		if (skipline) skipLine(1);
	}
	
	void Tokenizer::skipLine(int num) {
		int r = 0;
		for (int i = 0; i < num; i++) {
			r = m_sText.find("\n");
			if (r < 0) {
				m_sText.clear();
				return;
			}
			m_sText.erase(0, r+1 );
		}
	}
}
