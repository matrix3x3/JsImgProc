#pragma once

#ifdef DBGSUPPORT_EXPORTS
#define DBGSUPPORT_API __declspec(dllexport) 
#else
#define DBGSUPPORT_API __declspec(dllimport) 
#endif