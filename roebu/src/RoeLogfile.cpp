#include "RoeLogfile.h"
#include "RoeUtil.h"

namespace roe {

	Logfile::Logfile ()
		: m_bCreated(false)
		, m_colLast(0,0,0,1)
	{
	}
	
	Logfile::~Logfile ()
	{
		if (m_bCreated) {
			addLine();
			write("CORRECT QUITTING", Color::BLUE);
			m_tf.pushBack("<br><br>End of logfile</font></body></html>");
			m_tf.save();
		}
	}
	
	void Logfile::createLogfile (const std::string& sLogName) {
		m_tf.setFilename(sLogName);
		m_tf.pushBack("<html><head><title>Logfile "+sLogName+"</title></head>");
		m_tf.pushBack("<body><font face='courier new'>");
		
		m_bCreated = true;
		
		writeTopic ("Logfile", 1);
		
		#ifdef _DEBUG
		m_tf.pushBack("<p>BUILD: DEBUG</p>");
		#else
		m_tf.pushBack("<p>BUILD: RELEASE</p>");
		#endif
		
		addLine();
	}
	
	void Logfile::writeTopic (const std::string& sTopic, int size) {
		if (m_bCreated == false) return;
		m_tf.pushBack("<h"+util::toString(size)+">"+sTopic+"</h"+util::toString(size)+">");
		m_tf.save();
	}
	
	void Logfile::write (std::string s, Color col) {
		if (m_bCreated == false) return;
		std::string sNum[3];
		sNum[0] = util::toStringHexNumber(static_cast<int>(col.r * 255.0));
		sNum[1] = util::toStringHexNumber(static_cast<int>(col.g * 255.0));
		sNum[2] = util::toStringHexNumber(static_cast<int>(col.b * 255.0));
		for (int i = 0; i < 3; ++i) {
			if (sNum[i].length() == 1) {
				sNum[i] = "0" + sNum[i];
			}
		}
		if (s[s.length()-1] == '\n') s = s.substr(0,s.length()-1);
		std::string front = "<p style=\"color:#"+sNum[0]+sNum[1]+sNum[2]+"\">", back = "</p>";
		m_tf.pushBack(front+util::replaceAnyWith(s, "\n", back+front) +back);
		m_tf.save();
	}
	
	void Logfile::addLine() {m_tf.pushBack("<hr></hr>");}
	
} // namespace roe

