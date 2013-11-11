#pragma once
#include "common.h"
#include "stdafx.h"

OCV_SISR_API void sisrLearnDictionary();

OCV_SISR_API void sisrScalupImages();

OCV_SISR_API void initConfiguration(Confs& confs, KConfs& kconfs,
					   float s, int l, int w, int b, int o, int u, int iK, int nCPCA,
					   int ts, int noi, int ds, int te);

