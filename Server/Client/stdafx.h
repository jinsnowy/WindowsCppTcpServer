#pragma once

#include <networkengine\pch.h>


#pragma comment(lib, "ws2_32.lib")
#ifdef _DEBUG
#pragma comment(lib, "NetworkEngined.lib")
#else
#pragma comment(lib, "NetworkEngine.lib")
#endif
