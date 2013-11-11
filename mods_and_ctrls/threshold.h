#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "debug.h"
#include "interface.h"
#include "macros.h"

enum thresholdtype{BINARY, BINARY_INV, TRUNC, TOZERO, TOZERO_INV};

/*
*	CThreshold: class smooth
*/
class MODS_AND_CTRLS_API CThreshold : public IModule
{
public:
	CThreshold();
	CThreshold(int type, double maxval, double thresh);
	~CThreshold();

	void process(const cv::Mat& src, cv::Mat& dst);
	void setType(int type);
	int getType();
	void setMaxval(double maxval);
	double getMaxval();
	void setThresh(double thresh);
	double getThresh();

private:
	int _type;
	double _maxval;
	double _thresh;

	void doThreshold(const cv::Mat& src, cv::Mat& dst);
};

/*
*	ThresholdController
*/
class MODS_AND_CTRLS_API CThresholdController: public IController
{
private:
	static CThresholdController *_singleton;
	CThreshold *_threshold;

	CThresholdController()
	{
		_threshold = new CThreshold();
	}

	CThresholdController(CThresholdController&){}

public:
	
	// only process single channel image
	/*bool setInputImage(std::string filename)
	{
		_src = cv::imread(filename);
		if (!_src.data)
			return false;
		else
		{
			if (_src.channels() != 1)
			{
				_src.release();
				return false;
			}
			return true;
		}
	}*/
	int setInputImage(std::string name, cv::Mat src)
	{
		_name = name;
		if (!src.data)
		{
			return fail;
		}
		//else if (_src.channels() != 1)
		//{
		//	//_src.release();
		//	return fail;
		//}
		_src = src;
		return success;
	}

	void setType(int type)
	{
		_threshold->setType(type);
	}
	int getType()
	{
		return _threshold->getType();
	}

	void setMaxval(double maxval)
	{
		_threshold->setMaxval(maxval);
	}
	double getMaxval()
	{
		return _threshold->getMaxval();
	}

	void setThresh(double thresh)
	{
		_threshold->setThresh(thresh);
	}
	double getThresh()
	{
		return _threshold->getThresh();
	}

	void process()
	{
		_threshold->process(_src, _dst);
	}

	~CThresholdController()
	{
		delete _threshold;
	}

	// Singleton static member
	Func_getInstance(CThresholdController);

	//release this class
	Func_destroy("CThresholdController");

};