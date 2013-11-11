// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "macros.h"

#include "modules.h"
#include <tuple>
#include "vtest.h"

#include "superres.h"

Vernier* Vernier::_singleton = nullptr;

//#ifdef MODS_AND_CTRLS_EXPORTS
//	printf();
//#endif

class Ex : cv::Exception
{};

void th()
{
	Ex e;
	throw e;
}

int _tmain(int argc, _TCHAR* argv[])
{

	
	//try
	//{
	//	th();
	//}
	//catch(Ex e)
	//{
	//	printf("Ex!!!");
	//}
	//return 0;

	CSisrController *ctrl = CSisrController::getInstance();
	//sisr.loadConfs("C:\\JsImageProc\\SISR_confs.xml");

	//sisr.saveConfs("C:\\JsImageProc\\test.xml");
	//sisr.setConfsForDicLearning();
	//sisr.loadTrainningSet("C:\\Users\\jiahaozh\\Desktop\\temp\\niagara");

	//sisr.learnDictionary();

	cv::Mat src = cv::imread("C:\\Users\\jiahaozh\\Desktop\\temp\\niagara\\t2.bmp");

	ctrl->setInputImage("test",src);
	ctrl->setConfsForDicLearning();
	ctrl->setPath("C:\\Users\\jiahaozh\\Desktop\\temp\\niagara");
	ctrl->loadTrainningSet();
	bool b = ctrl->learnDic();
	try{
		ctrl->process();
	}
	catch(CSisr::SisrExcept e)
	{
		printf("NO dic!\n");
	}
	//cv::Mat dst;
	//sisr.scaleUp(src, dst);

	try{
		myimshow( ctrl->getLastResult(), "22");
	}
	catch(cv::Exception e)
	{
		printf("%s\n", e.what());
	}
	return 0;



}

