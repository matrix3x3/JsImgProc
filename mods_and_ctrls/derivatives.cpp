#include "derivatives.h"
#include "debug.h"
#include <iostream>

//initilize the static class member
CDerivativesController * CDerivativesController::_singleton = nullptr;

//
CDerivatives::CDerivatives():_xorder(1), _yorder(0), _method(derivatives_sobel), _ksize(3), _lowThresh(30.0), _highThresh(100.0)
{
}

CDerivatives::CDerivatives(int xorder, int yorder, int method, int ksize, double lowThresh, double highThresh)
{
	_xorder = xorder;
	_yorder = yorder;
	_method = method;
	_ksize = ksize;
	_lowThresh = lowThresh;
	_highThresh = highThresh;
}

CDerivatives::~CDerivatives(){
	std::cout << "in ~CDerivatives ...." << std::endl; 
}

void CDerivatives::setXorder(int x)
{
	_xorder = x;
}
int CDerivatives::getXorder()
{
	return _xorder;
}

void CDerivatives::setYorder(int y)
{
	_yorder = y;
}
int CDerivatives::getYorder()
{
	return _yorder;
}

void CDerivatives::setMethod(int m)
{
	_method = m;
}
int CDerivatives::getMethod()
{
	return _method;
}

void CDerivatives::setKsize(int s)
{
	_ksize = s;
}
int CDerivatives::getKsize()
{
	return _ksize;
}

void CDerivatives::setLowThresh(double l)
{
	_lowThresh = l;
}
double CDerivatives::getLowThresh()
{
	return _lowThresh;
}

void CDerivatives::setHighThresh(double h)
{
	_highThresh = h;
}
double CDerivatives::getHighThresh()
{
	return _highThresh;
}

void CDerivatives::doCanny(const cv::Mat& src, cv::Mat& dst)
{
	cv::Canny(src, dst, _lowThresh, _highThresh);
}

void CDerivatives::doSobel(const cv::Mat& src, cv::Mat& dst)
{
	cv::Sobel(src, dst, -1, _xorder, _yorder, _ksize);
}

void CDerivatives::doLaplace(const cv::Mat& src, cv::Mat& dst)
{
	cv::Laplacian(src, dst, -1, _ksize);
}

/*
* do dispatch
*/
void CDerivatives::process(const cv::Mat& src, cv::Mat& dst)
{
	switch (_method)
	{
	case derivatives_sobel:
		doSobel(src,dst);
		break;
	case derivatives_laplacian:
		doLaplace(src,dst);
		break;
	case derivatives_canny:
		doCanny(src,dst);
		break;
	default:
		PrtAndLog("method code error!");
		ASSERT(0);
		break;
	}
}