#pragma once

/*
*	M : modules, C: controlers
*/
#ifdef MODS_AND_CTRLS_EXPORTS
#define MODS_AND_CTRLS_API __declspec(dllexport) 
#else
#define MODS_AND_CTRLS_API __declspec(dllimport) 
#endif