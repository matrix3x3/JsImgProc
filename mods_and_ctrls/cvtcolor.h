#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "debug.h"
#include "interface.h"
#include "macros.h"

//enum cvttype{};

/*
*	CCvtColor:
*/
class MODS_AND_CTRLS_API CCvtColor: public IModule
{
public:
	CCvtColor();
	CCvtColor(int code);
	~CCvtColor();

	void process(const cv::Mat& src, cv::Mat& dst);
	void setCode(int code);
	int getCode();

private:
	int _code;
	void doCvt(const cv::Mat& src, cv::Mat& dst);
};

/*
*	CvtColorController
*/
class MODS_AND_CTRLS_API CCvtColorController: public IController
{
private:
	static CCvtColorController *_singleton;
	CCvtColor *_cvtColor;

	CCvtColorController()
	{
		_cvtColor = new CCvtColor();
	}

	CCvtColorController(CCvtColorController&){}

public:
	int setInputImage(std::string name, cv::Mat src)
	{
		// store the state to return
		int stat = success;

		_name = name;
		if (!src.data)
		{
			return fail;
		}

		switch (getCode())
		{
		case cv::COLOR_RGB2GRAY:
		case cv::COLOR_RGB2XYZ:
		case cv::COLOR_XYZ2RGB:
		case cv::COLOR_RGB2YCrCb:
		case cv::COLOR_YCrCb2RGB:
		case cv::COLOR_RGB2HSV:
		case cv::COLOR_HSV2RGB:
		case cv::COLOR_RGB2YUV:
		case cv::COLOR_YUV2RGB:
			if (src.channels() != 3)
			{
				stat = not3Channels;
			}
			break;
		case cv::COLOR_GRAY2RGB:
			if (src.channels() != 1)
			{
				stat = notSingleChannel;
			}
			break;
		default:
			stat = notSupported;
			break;
		}

		if (stat == success)
		{
			_src = src;
		}

		return stat;
	}

	void setCode(int code)
	{
		_cvtColor->setCode(code);
	}
	int getCode()
	{
		return _cvtColor->getCode();
	}

	void process()
	{
		_cvtColor->process(_src,_dst);
	}

	~CCvtColorController()
	{
		delete _cvtColor;
	}

	// Singleton static member
	Func_getInstance(CCvtColorController);

	//release this class
	Func_destroy("CCvtColorController");

};