#include "convolution.h"

CConvoController * CConvoController::_singleton = nullptr;

CConvo::CConvo()
{

}

CConvo::~CConvo(){}

void CConvo::setKernel(cv::Mat k)
{
	_kernel = k;
}

const cv::Mat CConvo::getKernel()
{
	return _kernel;
}

void CConvo::doConvo(const cv::Mat& src, cv::Mat& dst)
{
	cv::filter2D(src, dst, -1, _kernel);
}

void CConvo::process(const cv::Mat& src, cv::Mat& dst)
{
	doConvo(src, dst);
}