#include "blur.h"
#include "debug.h"

// initialize the static class member
CBlurController* CBlurController::_singleton = nullptr;

//
CBlur::CBlur():_smoothMethod(blur), _kernelsize(3)
{
}

CBlur::CBlur(int smoothmethod, int kernelsize)
{
	_smoothMethod = smoothmethod;
	_kernelsize = kernelsize;
}

CBlur::~CBlur(){}

/* kernelsize getter setter */
void CBlur::setKernelsize(int ksize)
{
	_kernelsize = ksize;
}
int CBlur::getKernelsize()
{
	return _kernelsize;
}

/* method getter setter */
void CBlur::setMethod(int method)
{
	_smoothMethod = method;
}
int CBlur::getMethod()
{
	return _smoothMethod;
}

void CBlur::doBlur(const cv::Mat& src, cv::Mat& dst)
{
	int ks = getKernelsize();
	cv::blur( src, dst, cv::Size( ks, ks ), cv::Point(-1,-1));
}

void CBlur::doGaussianBlur(const cv::Mat& src, cv::Mat& dst)
{
	int ks = getKernelsize();
	cv::GaussianBlur( src, dst, cv::Size( ks, ks ), 0, 0 );
}

void CBlur::doMedianBlur(const cv::Mat& src, cv::Mat& dst)
{
	int ks = getKernelsize();
	cv::medianBlur( src, dst, ks );
}

void CBlur::doBilateralFiter(const cv::Mat& src, cv::Mat& dst)
{
	int ks = getKernelsize();
	cv::bilateralFilter ( src, dst, ks, ks*2, ks/2 );
}

/*
*	dispatch according to method
*/
void CBlur::process(const cv::Mat& src, cv::Mat& dst)
{
	switch (_smoothMethod)
	{
	case blur:
		doBlur(src,dst);
		break;
	case gaussianBlur:
		doGaussianBlur(src,dst);
		break;
	case medianBlur:
		doMedianBlur(src,dst);
		break;
	case bilateralFilter:
		doBilateralFiter(src,dst);
		break;
	default:
		PrtAndLog("method code error!");
		ASSERT(0);
		break;
	}
}