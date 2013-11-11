#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "debug.h"
#include "interface.h"
#include "macros.h"

enum stype{blur, gaussianBlur, medianBlur, bilateralFilter};

//MODS_AND_CTRLS_API cv::Mat;

/*
*	CBlur: class smooth
*/
class MODS_AND_CTRLS_API CBlur: public IModule
{
public:
	CBlur();
	CBlur(int smoothmethod, int kernalsize);
	~CBlur();
	
	void process(const cv::Mat& src, cv::Mat& dst);
	void setKernelsize(int ksize);
	int getKernelsize();
	void setMethod(int method);
	int getMethod();
	
private:
	int _smoothMethod;
	// ocv tutorial : 31
	int _kernelsize;
	void doBlur(const cv::Mat& src, cv::Mat& dst);
	void doGaussianBlur(const cv::Mat& src, cv::Mat& dst);
	void doMedianBlur(const cv::Mat& src, cv::Mat& dst);
	void doBilateralFiter(const cv::Mat& src, cv::Mat& dst);
};

/*
*	SMoothController
*/
class MODS_AND_CTRLS_API CBlurController: public IController
{
private:
	static CBlurController *_singleton;
	CBlur *_smooth;

	/*cv::Mat _src;
	cv::Mat _dst;*/

	CBlurController() {
		_smooth = new CBlur();
	}

	CBlurController(CBlurController&);
	CBlurController& operator = (CBlurController& cs);

public:
	// method setter getter
	void setSmoothMethod(int method)
	{
		_smooth->setMethod(method);
	}

	int getSmoothMethod()
	{
		return _smooth->getMethod();
	}

	// kernelsize getter setter
	bool setKernelsize(int ks)
	{
		if (ks % 2 != 1)
		{
			return false;
		}
		_smooth->setKernelsize(ks);
		return true;
	}

	int getKernelsize()
	{
		return _smooth->getKernelsize();
	}

	//
	void process()
	{
		_smooth->process(_src,_dst);
	}

	//// get result
	//const cv::Mat getLastResult() const
	//{
	//	return _dst;
	//}

	~CBlurController()
	{
		//IController::~IController();
		delete _smooth;
	}

	// Singleton static member
	static CBlurController *getInstance()
	{
		if (_singleton == nullptr)
		{
			_singleton = new CBlurController();
		}
		return _singleton;
	}

	//release the instance
	static void destroy()
	{
		if (_singleton != nullptr)
		{
			delete _singleton;
			_singleton = nullptr;
		}
		PrtAndLog("destroying CBlurController...");
	}
};