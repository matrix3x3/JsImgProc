#pragma once
#include "macros.h"
#include <opencv2/highgui/highgui.hpp>

enum status{success = 0, fail = 1, notSingleChannel, not3Channels, notSupported};

/*
*	interface of Module
*/
class MODS_AND_CTRLS_API IModule
{
public:

	IModule(){}
	virtual ~IModule()=0 {}

	virtual void process(const cv::Mat& src, cv::Mat& dst)=0 {}

private:
	// singleton
};

/*
*	interface of Controller
*/
class MODS_AND_CTRLS_API IController
{
public:
	IController(){}
	virtual ~IController()=0 {}

	// set input image
	// make sure input image is set correctly before processing.
	/*virtual bool setInputImage(std::string filename)
	{
		_src = cv::imread(filename);

		if (!_src.data)
			return false;
		else
			return true;
	}*/

	virtual int setInputImage(std::string name, cv::Mat src)
	{
		_name = name;
		if (!src.data)
		{
			return fail;
		}
		_src = src;
		return success;
	}

	// get input image
	virtual const cv::Mat getInputImage() const
	{
		return _src;
	}

	// get result
	virtual const cv::Mat getLastResult()
	{
		cv::Mat res = _dst;
		_dst.release();
		return res;
	}

	// get name
	virtual std::string getImgName() const
	{
		return _name;
	}

	virtual void process()=0 {};

protected:
	cv::Mat _src;
	cv::Mat _dst;
	std::string _name;

private:

};

/*
*	useful macros to generate function
*/

#define Func_getInstance(Controller) \
	static Controller *getInstance() \
	{\
		if (_singleton == nullptr)\
		{\
			_singleton = new Controller();\
		}\
		return _singleton;\
	}

#define Func_destroy(Controller) \
	static void destroy() \
	{\
		if (_singleton != nullptr)\
		{\
			delete _singleton;\
			_singleton = nullptr;\
		}\
		PrtAndLog("destroying %s...", Controller);\
	}