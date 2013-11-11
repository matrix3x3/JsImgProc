#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "macros.h"
#include "interface.h"
#include "debug.h"

enum morphtype{eroding, dilating};

/*
*	CMorphTrans
*/
class MODS_AND_CTRLS_API CMorphTrans: public IModule
{
public:
	CMorphTrans();
	CMorphTrans(int elemType, int elemSize, int morphTranType);
	~CMorphTrans();

	void process(const cv::Mat& src, cv::Mat& dst);
	void setElemType(int et);
	int getElemType();
	void setElemSize(int es);
	int getElemSize();
	void setMorphTranType(int t);
	int getMorphTranType();

private:
	// 0,1,2 ->
	//	MORPH_RECT, MORPH_CROSS, MORPH_ELLIPSE
	// kernel shape
	int _elemType;
	// size of structuring element
	int _elemSize;
	int _morphTranType;

	void doErode(const cv::Mat& src, cv::Mat& dst);
	void doDilate(const cv::Mat& src, cv::Mat& dst);
};

/*
*	MorphTransController
*/
class MODS_AND_CTRLS_API CMorphTransController: public IController
{
private:
	static CMorphTransController *_singleton;
	CMorphTrans *_morphTrans;

	CMorphTransController()
	{
		_morphTrans = new CMorphTrans();
	}

	CMorphTransController(CMorphTransController&){}

public:
	
	void setElemType(int et)
	{
		_morphTrans->setElemType(et);
	}
	int getElemType()
	{
		return _morphTrans->getElemType();
	}

	void setElemSize(int es)
	{
		_morphTrans->setElemSize(es);
	}
	int getElemSize()
	{
		return _morphTrans->getElemSize();
	}

	void setMorphTranType(int t)
	{
		_morphTrans->setMorphTranType(t);
	}
	int getMorphTranType()
	{
		return _morphTrans->getMorphTranType();
	}

	void process()
	{
		_morphTrans->process(_src,_dst);
	}

	~CMorphTransController()
	{
		delete _morphTrans;
	}

	//release the instance
	static void destroy()
	{
		if (_singleton != nullptr)
		{
			delete _singleton;
			_singleton = nullptr;
		}
		PrtAndLog("destroying CMorphTransController...");
	}

	// Singleton static member
	static CMorphTransController *getInstance()
	{
		if (_singleton == nullptr)
		{
			_singleton = new CMorphTransController();
		}
		return _singleton;
	}
};
