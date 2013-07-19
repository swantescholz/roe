#include "RoeMusic.h"

#ifdef ROE_COMPILE_AUDIO

#include "RoeException.h"

namespace roe {

	std::map<std::string, S_MusicIntern >           Music::s_mpMusic;
	std::map<std::string, S_MusicIntern >::iterator Music::s_mpi;
	std::pair<std::string, S_MusicIntern >          Music::s_pair;
	std::string                                     Music::s_sPathStart = "";
	std::string                                     Music::s_sPathEnd   = ".ogg";
	
	
	Music::Music(const float fVolume)
	{
		m_fVolume = fVolume;
	}
	
	Music::Music(const std::string &sPath, const float fVolume) {
		m_fVolume = fVolume;
		
		load(sPath);
	}
	
	Music::~Music()
	{
	}
	
	void Music::load(std::string sPath) {
		m_sName = sPath;
		sPath = s_sPathStart + sPath + s_sPathEnd;
		
		s_mpi = s_mpMusic.find(sPath);
		if (s_mpi == s_mpMusic.end()) {
			roe_except("requested music not loaded("+sPath+")", "load");
		}
		
		m_sPath = sPath;
	}
	
	float Music::getStdVolume() {
		s_mpi = s_mpMusic.find(m_sPath);
		if (s_mpi == s_mpMusic.end()) {
			throw Exception("std volume cannot be found", "getStdVolume", "RoeMusic.cpp");
			return -1;
		} else {
			return s_mpi->second.fStdVolume;
		}
	}
	double Music::getStdFadeIn() {
		s_mpi = s_mpMusic.find(m_sPath);
		if (s_mpi == s_mpMusic.end()) {
			throw Exception("std FadeIn cannot be found", "getStdFadeIn", "RoeMusic.cpp");
			return -1;
		} else {
			return s_mpi->second.dStdFadeIn;
		}
	}
	double Music::getStdPos() {
		s_mpi = s_mpMusic.find(m_sPath);
		if (s_mpi == s_mpMusic.end()) {
			throw Exception("std position cannot be found", "getStdPos", "RoeMusic.cpp");
			return -1;
		} else {
			return s_mpi->second.dStdPos;
		}
	}
	Mix_Music* Music::getMusic() {
		s_mpi = s_mpMusic.find(m_sPath);
		if (s_mpi == s_mpMusic.end()) {
			throw Exception("music("+m_sPath+") cannot be found", "getMusic", "RoeMusic.cpp");
			return NULL;
		} else {
			return s_mpi->second.music;
		}
	}
	
	void Music::play(const int loops, const double dFadeIn, const double dPos) {
		Mix_VolumeMusic(MIX_MAX_VOLUME * getStdVolume() * m_fVolume);
		Mix_FadeInMusicPos(getMusic(), loops,
			static_cast<int> ( (dFadeIn + getStdFadeIn() ) * 1000) , dPos + getStdPos() );
	}
	
	
	
	void Music::loadMusic(std::string sPath, const float fVolume, const double dFadeIn, const double dPos) {
		sPath = s_sPathStart + sPath + s_sPathEnd;
		
		s_mpi = s_mpMusic.find(sPath);
		if (s_mpi == s_mpMusic.end()) {
			S_MusicIntern music;
			music.music = Mix_LoadMUS(sPath.c_str());
			if (music.music) {
				music.fStdVolume = fVolume;
				music.dStdFadeIn = dFadeIn;
				music.dStdPos	 = dPos;
				
				//map erweitern
				s_pair = make_pair (sPath, music);
				s_mpMusic.insert(s_pair);
			} else {
				roe_except("music '" + sPath + "' does not exist in path", "loadMusic");
			}
		} else {
			roe_except("music '" + sPath + "' already loaded", "loadMusic");
		}
	}
	void Music::deleteMusic	(std::string sPath, bool bCompleteThePath)	{
		if (bCompleteThePath)
			sPath = s_sPathStart + sPath + s_sPathEnd;
		
		s_mpi = s_mpMusic.find(sPath);
		if (s_mpi == s_mpMusic.end()) {
			roe_except("music to be deleted ('" + sPath + "') does not exist", "deleteMusic");
		} else {
			if(s_mpi->second.music) {
				Mix_FreeMusic(s_mpi->second.music);
				s_mpi->second.music = NULL;
			}
			s_mpMusic.erase(s_mpi);
		}
	}
	bool Music::isMusicLoaded(std::string sPath, bool bCompleteThePath) {
		if (bCompleteThePath)
			sPath = s_sPathStart + sPath + s_sPathEnd;
		
		s_mpi = s_mpMusic.find(sPath);
		if (s_mpi == s_mpMusic.end()) {
			return false;
		} else {
			return true;
		}
	}
	
	void Music::init(std::string sStart, std::string sEnd)	{
		s_sPathStart = sStart;
		s_sPathEnd = sEnd;
	}
	void Music::quit()	{
		for(s_mpi=s_mpMusic.begin(); s_mpi!=s_mpMusic.end(); ++s_mpi) {
			if(s_mpi->second.music) {
				Mix_FreeMusic(s_mpi->second.music);
				s_mpi->second.music = NULL;
			}
		}
		s_mpMusic.clear();
	}

} // namespace roe

#endif //ROE_COMPILE_AUDIO

