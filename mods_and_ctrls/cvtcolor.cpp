#include "cvtcolor.h"
#include "debug.h"

// initialize the static class member
CCvtColorController * CCvtColorController::_singleton = nullptr;
//
CCvtColor::CCvtColor() : _code(cv::COLOR_RGB2YCrCb)
{
}

CCvtColor::CCvtColor(int code)
{
	_code = code;
}

CCvtColor::~CCvtColor(){}

void CCvtColor::setCode(int code)
{
	_code = code;
}
int CCvtColor::getCode()
{
	return _code;
}

void CCvtColor::doCvt(const cv::Mat& src, cv::Mat& dst)
{
	cv::cvtColor(src, dst, _code);
}

void CCvtColor::process(const cv::Mat& src, cv::Mat& dst)
{
	doCvt(src,dst);
}