#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "debug.h"
#include "interface.h"
#include "macros.h"
#include <iostream>

enum methodtype{derivatives_sobel, derivatives_laplacian, derivatives_canny};

/*
*	CDerivatives
*/
class MODS_AND_CTRLS_API CDerivatives: public IModule
{
public:
	CDerivatives();
	CDerivatives(int xorder, int yorder, int method, int ksize, double lowThresh, double highThresh);
	~CDerivatives();

	void process(const cv::Mat& src, cv::Mat& dst);
	void setXorder(int xorder);
	int getXorder();
	void setYorder(int yorder);
	int getYorder();
	void setMethod(int method);
	int getMethod();
	void setKsize(int ksize);
	int getKsize();
	void setLowThresh(double l);
	double getLowThresh();
	void setHighThresh(double h);
	double getHighThresh();


private:
	int _xorder, _yorder;
	int _method;
	// available values: 
	// for Sobel: must be 1, 3, 5, or 7
	// for Laplace:
	int _ksize;
	double _lowThresh;
	double _highThresh;

	void doSobel(const cv::Mat& src, cv::Mat& dst);
	void doLaplace(const cv::Mat& src, cv::Mat& dst);
	void doCanny(const cv::Mat& src, cv::Mat& dst);

};

/*
*	derivativesController
*/
class MODS_AND_CTRLS_API CDerivativesController: public IController
{
private:
	static CDerivativesController *_singleton;
	CDerivatives *_derivatives;

	CDerivativesController()
	{
		_derivatives = new CDerivatives();
	}

	CDerivativesController(CDerivativesController&){}

public:
	// only process single channel image
	/*bool setInputImage(std::string filename)
	{
		_src = cv::imread(filename, 0);
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
		/*else if (_src.channels() != 1)
		{
			_src.release();
			return fail;
		}*/
		_src = src;
		return success;
	}

	void setXorder(int x)
	{
		_derivatives->setXorder(x);
	}
	int getXorder()
	{
		return _derivatives->getXorder();
	}

	void setYorder(int y)
	{
		_derivatives->setYorder(y);
	}
	int getYorder()
	{
		return _derivatives->getYorder();
	}

	void setMethod(int m)
	{
		_derivatives->setMethod(m);
	}
	int getMethod()
	{
		return _derivatives->getMethod();
	}

	/* set ksize. need to validate when method is Sobel */
	void setKsize(int s)
	{
		_derivatives->setKsize(s);
	}
	int getKsize()
	{
		return _derivatives->getKsize();
	}

	void setLowThresh(double l)
	{
		_derivatives->setLowThresh(l);
	}
	double getLowThresh()
	{
		return _derivatives->getLowThresh();
	}

	void setHighThresh(double h)
	{
		_derivatives->setHighThresh(h);
	}
	double getHighThresh()
	{
		return _derivatives->getHighThresh();
	}

	/* validate ksize because sobel has constraint */
	bool checkKsize()
	{
		if (getMethod() == derivatives_sobel)
		{
			int ksize = getKsize();
			if ((ksize != 1) && (ksize != 3) && (ksize != 5) && (ksize != 7))
			{
				return false;
			}
		}
		else
			return true;
	}

	void process()
	{
		_derivatives->process(_src,_dst);
	}

	~CDerivativesController()
	{
		delete _derivatives;
	}

	// Singleton static member
	Func_getInstance(CDerivativesController);

	//release this class
	Func_destroy("CDerivativesController");
};