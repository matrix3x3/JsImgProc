#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "debug.h"
#include "interface.h"
#include "macros.h"

enum hist{equalization};

class MODS_AND_CTRLS_API CHist: public IModule
{
public:
	CHist():_method(equalization)
	{}
	~CHist(){}

	void process(const cv::Mat& src, cv::Mat& dst)
	{
		switch (_method)
		{
		case equalization:
			cv::equalizeHist(src, dst);
		default:
			break;
		}
	}

	void setMethod(int m)
	{
		_method = m;
	}
	int getMethod()
	{
		return _method;
	}

private:
	int _method;
};

class MODS_AND_CTRLS_API CHistController: public IController
{
private:
	static CHistController *_singleton;
	CHist *_hist;

	CHistController()
	{
		_hist = new CHist();
	}

	CHistController(CHistController&);
	CHistController& operator = (CHistController&);

public:
	int setInputImage(std::string name, cv::Mat src)
	{
		int stat = success;

		_name = name;
		if (!src.data)
		{
			return fail;
		}

		if (src.channels() != 1)
		{
			stat = notSingleChannel;
		}

		if (stat == success)
		{
			_src = src;
		}

		return stat;
	}

	void setMethod(int m)
	{
		_hist->setMethod(m);
	}
	int getMethod()
	{
		return _hist->getMethod();
	}

	void process()
	{
		_hist->process(_src, _dst);
	}

	~CHistController()
	{
		delete _hist;
	}
	
	// Singleton static member
	Func_getInstance(CHistController);

	//release this class
	Func_destroy("CHistController");
};