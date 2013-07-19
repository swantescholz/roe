#ifndef ROEEXCEPTION_H_
#define ROEEXCEPTION_H_

#include <string>
#include <cstdio>

#define roe_error(x) throw roe::Exception(x,"-",__FILE__,__LINE__)
#define roe_except(x,y) throw roe::Exception(x,y,__FILE__,__LINE__)

namespace roe {

	class Exception
	{
	public:
		Exception(const std::string& psError, const std::string& sFunction = "-", const std::string sFile = "-", int line = 0)
			: m_sErrorMessage(psError), m_sFunction(sFunction), m_sFile(sFile), m_iLine(line)
		{
		}
		virtual ~Exception() {}
		virtual std::string toString() const {
			std::string tmp("Exception thrown in function ");
			if (m_sFunction != "-") tmp += m_sFunction + "() ";
			if (m_sFile     != "-") tmp += "of " + m_sFile;
			char *ch = new char[15000];
			if (m_iLine      >   0) {
				tmp += " in line ";
				sprintf(ch,"%s%i: \"%s\"", tmp.c_str(), m_iLine, m_sErrorMessage.c_str());
			}
			else {
				sprintf(ch,"%s: \"%s\""  , tmp.c_str(), m_sErrorMessage.c_str());
			}
			std::string str(ch);
			delete[] ch;
			return str;
		}
		
	private:
		std::string m_sErrorMessage;
		std::string m_sFunction;
		std::string m_sFile;
		int         m_iLine;
	};
	
} // namespace roe

#endif /* ROEEXCEPTION_H_ */
