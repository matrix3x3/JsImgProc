#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "debug.h"
#include "interface.h"
#include "macros.h"

/*
*	CConvo: class convolution
*/
class MODS_AND_CTRLS_API CConvo: public IModule
{
public:
	CConvo();
	~CConvo();

	void process(const cv::Mat& src, cv::Mat& dst);
	void setKernel(cv::Mat k);
	const cv::Mat getKernel();

private:
	cv::Mat _kernel;
	void doConvo(const cv::Mat& src, cv::Mat& dst);
};

/*
*	CConvoController
*/
class MODS_AND_CTRLS_API CConvoController: public IController
{
private:
	static CConvoController *_singleton;
	CConvo *_convo;

	CConvoController()
	{
		_convo = new CConvo();
	}

	CConvoController(CConvoController&);
	CConvoController& operator = (CConvoController&);

public:
	void setKernel(cv::Mat k)
	{
		_convo->setKernel(k);
	}

	const cv::Mat getKernel()
	{
		return _convo->getKernel();
	}

	void process()
	{
		_convo->process(_src, _dst);
	}

	~CConvoController()
	{
		delete _convo;
	}

	// Singleton static member
	Func_getInstance(CConvoController);

	//release this class
	Func_destroy("CConvoController");
};
