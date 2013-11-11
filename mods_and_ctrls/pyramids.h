#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "debug.h"
#include "interface.h"
#include "macros.h"

static enum pyramidstype{pyr_gaussian, pyr_laplacian, pyr_resize};

/*
*	Pyramids
*/
class MODS_AND_CTRLS_API CPyramids : public IModule
{
public:
	CPyramids();
	CPyramids(double x, double y, int method, int interm);
	~CPyramids();

	void process(const cv::Mat& src, cv::Mat& dst);
	void setXscale(double x);
	double getXscale();
	void setYscale(double y);
	double getYscale();
	void setMethod(int method);
	int getMethod();
	void setInterpolateMethod(int m);
	int getInterpolateMethod();

private:
	int _method;
	//int _scale;
	//used by resize only, set pyrimads factor to 2 all the time.
	double _xScale;
	double _yScale;
	int _interpolateMethod;
	void doPyrUp(const cv::Mat& src, cv::Mat& dst);
	void doPyrDown(const cv::Mat& src, cv::Mat& dst);
	void doResize(const cv::Mat& src, cv::Mat& dst);
};

/*
*	PyramidsController
*/
class MODS_AND_CTRLS_API CPyramidsController: public IController
{
private:
	static CPyramidsController *_singleton;
	CPyramids *_pyramids;

	CPyramidsController()
	{
		_pyramids = new CPyramids();
	}

	CPyramidsController(CPyramidsController&){}

public:
	void setXscale(double x)
	{
		_pyramids->setXscale(x);
	}
	double getXscale()
	{
		return _pyramids->getXscale();
	}

	void setYscale(double y)
	{
		_pyramids->setYscale(y);
	}
	double getYscale()
	{
		return _pyramids->getYscale();
	}

	void setMethod(int method)
	{
		_pyramids->setMethod(method);
	}
	int getMethod()
	{
		return _pyramids->getMethod();
	}

	void setInterpolateMethod(int interm)
	{
		_pyramids->setInterpolateMethod(interm);
	}
	int getInterpolateMethod()
	{
		return _pyramids->getInterpolateMethod();
	}

	void process()
	{
		_pyramids->process(_src,_dst);
	}

	~CPyramidsController()
	{
		delete _pyramids;
	}

	//release the instance
	static void destroy()
	{
		if (_singleton != nullptr)
		{
			delete _singleton;
			_singleton = nullptr;
		}
		PrtAndLog("destroying CPyramidsController...");
	}

		// Singleton static member
	static CPyramidsController *getInstance()
	{
		if (_singleton == nullptr)
		{
			_singleton = new CPyramidsController();
		}
		return _singleton;
	}
};