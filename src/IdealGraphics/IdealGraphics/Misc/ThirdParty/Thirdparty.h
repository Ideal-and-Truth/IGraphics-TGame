#pragma once

#include "ThirdParty/Include/assimp/config.h"

#ifdef _DEBUG
#pragma comment(lib, "ThirdParty/Libraries/ForDebug/assimp/assimp-vc143-mtd.lib")

#else
#pragma comment(lib, "ThirdParty/Libraries/ForRelease/assimp/assimp-vc143-mt.lib")
#endif