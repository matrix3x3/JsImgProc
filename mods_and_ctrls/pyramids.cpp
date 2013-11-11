#include "pyramids.h"
#include "debug.h"

//initialize the static class member
CPyramidsController * CPyramidsController::_singleton = nullptr;

//
CPyramids::CPyramids():_method(pyr_gaussian), _xScale(2.0), _yScale(2.0), _interpolateMethod(CV_INTER_LINEAR)
{
}

CPyramids::CPyramids(double x, double y, int method, int interm)
{
	_xScale = x;
	_yScale = y;
	_method = method;
	_interpolateMethod = interm;
}

CPyramids::~CPyramids(){}

void CPyramids::setMethod(int method)
{
	_method = method;
}

int CPyramids::getMethod()
{
	return _method;
}

void CPyramids::setXscale(double x)
{
	_xScale = x;
}

double CPyramids::getXscale()
{
	return _xScale;
}

void CPyramids::setYscale(double y)
{
	_yScale = y;
}

double CPyramids::getYscale()
{
	return _yScale;
}

void CPyramids::setInterpolateMethod(int m)
{
	_interpolateMethod = m;
}

int CPyramids::getInterpolateMethod()
{
	return _interpolateMethod;
}

void CPyramids::doResize(const cv::Mat& src, cv::Mat& dst)
{
	cv::resize(src, dst, cv::Size(), _xScale, _yScale, _interpolateMethod);
}

void CPyramids::doPyrUp(const cv::Mat& src, cv::Mat& dst)
{
	//cv::pyrUp( src, dst, cv::Size( src.cols*_scale, src.rows*_scale ) );
	cv::pyrUp( src, dst, cv::Size( src.cols*2, src.rows*2 ) );
}

void CPyramids::doPyrDown(const cv::Mat& src, cv::Mat& dst)
{
	//pyrDown( src, dst, cv::Size( src.cols/_scale, src.rows/_scale ) );
	pyrDown( src, dst, cv::Size( src.cols/2, src.rows/2 ) );
}

/*
*	dispatch according to method
*/
void CPyramids::process(const cv::Mat& src, cv::Mat& dst)
{
	switch (_method)
	{
	case pyr_gaussian:
		doPyrDown(src,dst);
		break;
	case pyr_laplacian:
		doPyrUp(src,dst);
		break;
	case pyr_resize:
		doResize(src,dst);
		break;
	default:
		PrtAndLog("method code error!");
		ASSERT(0);
		break;
	}
}
