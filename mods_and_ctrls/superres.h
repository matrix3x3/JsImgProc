#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "debug.h"
#include "interface.h"
#include "macros.h"
#include "sisr.h"

/*
*	CSisr: class sisr
*	1. load config from file
*	2. get input params from GUI and training dic
*
*/
class MODS_AND_CTRLS_API CSisr: public IModule
{
public:
	CSisr();
	~CSisr();
	
	void process(const cv::Mat& src, cv::Mat& dst);
	void process(const cv::Mat& src, cv::Mat& dst, cv::Mat& diff);

	//initialize configurations
	//void initConfs();
	//path should be hidden by Controller
	/* not included in process() */
	bool learnDictionary();
	//void scaleup();
	//void learnPCA();

	/*
	*	call loadConf and train PCA
	*/
	bool loadConfs(std::string path);
	bool saveConfs(std::string path);
	bool loadTrainningSet(std::string path);
	bool isTrainningSetEmpty();
	void setConfsForDicLearning();

	class SisrExcept : cv::Exception
	{};

	void scaleUp(const cv::Mat& src, cv::Mat& dst, cv::Mat& diff);


	//void checkDic();
	
	/* getters and setters */
	void setScale(float s);
	float getScale();
	void setWindow(int w);
	int getWindow();
	void setOverlap(int o);
	int getOverlap();
	void setSparsity(int sparsity);
	int getSparsity();
	void setIter(int i);
	int getIter();
	void setDictsize(int s);
	int getDictsize();

private:
	/* sisr configurations */
	cv::PCA _pca;
	Confs _confs;
	KConfs _kConfs;
	cv::Mat _lowD;
	cv::Mat _highD;
	cv::Mat _matPCA;

	/**/
	Images _trainningSet;

	/* confs */
	float _scale;
	int _window;
	int _overlap;
	//int _interpolateKernel;
	int _nComponentPCA;

	/* kConfs */
	int _targetSparsity;
	int _numOfIteration;
	int _dictsize;
	//int _targetError;



};


/*
*	SisrController
*/
class MODS_AND_CTRLS_API CSisrController: public IController
{
private:
	cv::Mat _diff;
	std::string _path;
	/**/
	static CSisrController *_singleton;
	CSisr *_sisr;

	CSisrController() {
		_sisr = new CSisr();
	}

	CSisrController(CSisrController&);
	CSisrController& operator = (CSisrController& cs);

public:
	const cv::Mat getDiff()
	{
		return _diff;
	}
//#pragma region getXXX_setXXX
	void setPath(std::string path)
	{
		_path = path;
	}
	std::string getPath()
	{
		return _path;
	}

		/* getters and setters */
	void setScale(float s)
	{
		_sisr->setScale(s);
	}
	float getScale()
	{
		return _sisr->getScale();
	}
	
	void setWindow(int w)
	{
		_sisr->setWindow(w);
	}
	int getWindow()
	{
		return _sisr->getWindow();
	}

	void setOverlap(int o)
	{
		_sisr->setOverlap(o);
	}
	int getOverlap()
	{
		return _sisr->getOverlap();
	}

	void setSparsity(int sparsity)
	{
		_sisr->setSparsity(sparsity);
	}
	int getSparsity()
	{
		return _sisr->getSparsity();
	}

	void setIter(int i)
	{
		_sisr->setIter(i);
	}
	int getIter()
	{
		return _sisr->getIter();
	}

	void setDictsize(int s)
	{
		_sisr->setDictsize(s);
	}
	int getDictsize()
	{
		return _sisr->getDictsize();
	}
//#pragma endregion

	bool isTrainningSetEmpty()
	{
		return _sisr->isTrainningSetEmpty();
	}

	//void initConfs()
	//{
	//	_sisr->initConfs();
	//}

	bool learnDic()
	{
		return _sisr->learnDictionary();
	}

	void process()
	{
		//_sisr->process(_src,_dst);
		_sisr->process(_src,_dst, _diff);
	}

	bool loadConfs()
	{
		return _sisr->loadConfs(_path);
	}
	bool saveConfs()
	{
		return _sisr->saveConfs(_path);
	}

	bool loadTrainningSet()
	{
		return _sisr->loadTrainningSet(_path);
	}

	void setConfsForDicLearning()
	{
		_sisr->setConfsForDicLearning();
	}

	~CSisrController()
	{
		delete _sisr;
	}

	// Singleton static member
	static CSisrController *getInstance()
	{
		if (_singleton == nullptr)
		{
			_singleton = new CSisrController();
		}
		return _singleton;
	}

	//release the instance
	static void destroy()
	{
		if (_singleton != nullptr)
		{
			delete _singleton;
			_singleton = nullptr;
		}
		PrtAndLog("destroying CSisrController...");
	}
};


