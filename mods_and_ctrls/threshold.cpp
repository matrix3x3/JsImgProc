#include "threshold.h"
#include "debug.h"

// initialize the static class member
CThresholdController * CThresholdController::_singleton = nullptr;
//
CThreshold::CThreshold():_type(BINARY), _maxval((float)255), _thresh((float)255/2)
{
}

CThreshold::CThreshold(int type, double maxval, double thresh)
{
	_type = type;
	_maxval = maxval;
	_thresh = thresh;
}

CThreshold::~CThreshold(){}

void CThreshold::setType(int type)
{
	_type = type;
}
int CThreshold::getType()
{
	return _type;
}

void CThreshold::setMaxval(double maxval)
{
	_maxval = maxval;
}
double CThreshold::getMaxval()
{
	return _maxval;
}

void CThreshold::setThresh(double thresh)
{
	_thresh = thresh;
}
double CThreshold::getThresh()
{
	return _thresh;
}

void CThreshold::doThreshold(const cv::Mat& src, cv::Mat& dst)
{
	int type;
	switch (_type)
	{
	case BINARY:
		type = cv::THRESH_BINARY;
		break;
	case BINARY_INV:
		type = cv::THRESH_BINARY_INV;
		break;
	case TRUNC:
		type = cv::THRESH_TRUNC;
		break;
	case TOZERO:
		type = cv::THRESH_TOZERO;
		break;
	case TOZERO_INV:
		type = cv::THRESH_TOZERO_INV;
		break;
	default:
		PrtAndLog("input type error!");
		ASSERT(0);
		return;
		//break;
	}
	cv::threshold(src, dst, _thresh, _maxval, type);
}

void CThreshold::process(const cv::Mat& src, cv::Mat& dst)
{
	doThreshold(src,dst);
}