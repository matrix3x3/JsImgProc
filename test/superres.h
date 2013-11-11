//#pragma once
//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/imgproc/imgproc.hpp>
//#include "debug.h"
//#include "interface.h"
//#include "macros.h"
//#include "sisr.h"
//
///*
//*	CSisr: class sisr
//*	1. load config from file
//*	2. get input params from GUI and training dic
//*
//*/
//class MODS_AND_CTRLS_API CSisr: public IModule
//{
//public:
//	CSisr();
//	~CSisr();
//	
//	void process(const cv::Mat& src, cv::Mat& dst);
//	//initialize configurations
//	void initConfs();
//	//path should be hidden by Controller
//	/* not included in process() */
//	bool learnDictionary();
//	void scaleup();
//	void learnPCA();
//
//	/*
//	*	call loadConf and train PCA
//	*/
//	bool loadConfs(std::string path);
//	bool saveConfs(std::string path);
//	bool loadTrainningSet(std::string path);
//	void setConfsForDicLearning();
//	bool scaleUp();
//
//
//	//void checkDic();
//	
//	/* getters and setters */
//	void setScale(float s);
//	float getScale();
//	void setWindow(int w);
//	int getWindow();
//	void setOverlap(int o);
//	int getOverlap();
//	void setSparsity(int sparsity);
//	int getSparsity();
//	void setIter(int i);
//	int getIter();
//	void setDictsize(int s);
//	int getDictsize();
//
//private:
//	/* sisr configurations */
//	cv::PCA* _pca;
//	Confs _confs;
//	KConfs _kConfs;
//	cv::Mat _lowD;
//	cv::Mat _highD;
//	cv::Mat _matPCA;
//
//	/**/
//	Images _trainningSet;
//
//	/* confs */
//	float _scale;
//	int _window;
//	int _overlap;
//	//int _interpolateKernel;
//	int _nComponentPCA;
//
//	/* kConfs */
//	int _targetSparsity;
//	int _numOfIteration;
//	int _dictsize;
//	//int _targetError;
//
//
//
//};
//
/////*
////*	SisrController
////*/
////class MODS_AND_CTRLS_API CSisrController: public IController
////{
////private:
////	static CSisrController *_singleton;
////	CSisr *_sisr;
////
////	/*cv::Mat _src;
////	cv::Mat _dst;*/
////
////	CSisrController() {
////		_sisr = new CSisr();
////	}
////
////	CSisrController(CSisrController&);
////	CSisrController& operator = (CSisrController& cs);
////
////public:
////	
////};