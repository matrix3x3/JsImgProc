#include "morphtrans.h"
#include "debug.h"

//initialize the static class member
CMorphTransController* CMorphTransController::_singleton = nullptr;

//
CMorphTrans::CMorphTrans():_elemSize(2), _elemType(0), _morphTranType(eroding)
{
}

CMorphTrans::CMorphTrans(int es, int et, int t)
{
	_elemSize = es;
	_elemType = et;
	_morphTranType = t;
}

CMorphTrans::~CMorphTrans(){}

void CMorphTrans::setElemSize(int es)
{
	_elemSize = es;
}

int CMorphTrans::getElemSize()
{
	return _elemSize;
}

void CMorphTrans::setElemType(int et)
{
	_elemType = et;
}

int CMorphTrans::getElemType()
{
	return _elemType;
}

void CMorphTrans::setMorphTranType(int t)
{
	_morphTranType = t;
}

int CMorphTrans::getMorphTranType()
{
	return _morphTranType;
}

void CMorphTrans::doErode(const cv::Mat& src, cv::Mat& dst)
{
	int erosion_type;
	if( _elemType == 0 ){ erosion_type = cv::MORPH_RECT; }
	else if( _elemType == 1 ){ erosion_type = cv::MORPH_CROSS; }
	else if( _elemType == 2) { erosion_type = cv::MORPH_ELLIPSE; }

	cv::Mat element = cv::getStructuringElement( erosion_type,
		cv::Size( 2*_elemSize + 1, 2*_elemSize+1 ),
		cv::Point( _elemSize, _elemSize ) );

	/// Apply the erosion operation
	cv::erode( src, dst, element );
}

void CMorphTrans::doDilate(const cv::Mat& src, cv::Mat& dst)
{
	int dilation_type;
	if( _elemType == 0 ){ dilation_type = cv::MORPH_RECT; }
	else if( _elemType == 1 ){ dilation_type = cv::MORPH_CROSS; }
	else if( _elemType == 2) { dilation_type = cv::MORPH_ELLIPSE; }

	cv::Mat element = cv::getStructuringElement( dilation_type,
		cv::Size( 2*_elemSize + 1, 2*_elemSize+1 ),
		cv::Point( _elemSize, _elemSize ) );

	/// Apply the dilation operation
	cv::dilate( src, dst, element );
}

/*
*	dispatch according to morphtrantype
*/
void CMorphTrans::process(const cv::Mat& src, cv::Mat& dst)
{
	switch (_morphTranType)
	{
	case eroding:
		doErode(src, dst);
		break;
	case dilating:
		doDilate(src, dst);
		break;
	default:
		PrtAndLog("method code error!");
		ASSERT(0);
		break;
	}
}