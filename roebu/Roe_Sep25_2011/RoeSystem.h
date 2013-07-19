#ifndef ROESYSTEM_H_
#define ROESYSTEM_H_

#include "RoeCommon.h"

#ifdef ROE_COMPILE_SYSTEM_SPECIFICS

#include "RoeSingleton.h"

namespace roe {
namespace system {
	
	class DirParser {
	public:
		DirParser();
		~DirParser();
	};
	
} // namespace system
} // namespace roe

#endif //ROE_COMPILE_SYSTEM_SPECIFICS
#endif /* ROESYSTEM_H_ */
