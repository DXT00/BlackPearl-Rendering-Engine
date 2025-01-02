#pragma once

#include <cstdlib>
#include <crtdbg.h>
#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <optional>

//#include "BlackPearl/Log.h"



#ifdef _DEBUG
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#else
#define DBG_NEW new
#endif

#define _CRTDBG_MAP_ALLOC
//#ifdef HZ_PLATFORM_WINDOWS
//#include <Windows.h>
//#endif