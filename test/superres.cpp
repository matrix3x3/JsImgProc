//#include "superres.h"
//
////Confs cf(_scale, 1, _window, 1, _overlap, (int)_scale, cv::INTER_CUBIC, _nComponentPCA);
////	_confs = cf;
////	KConfs kcf(_targetSparsity, _numOfIteration, _dictsize, /*target error*/0);
////	_kConfs = kcf;
////	delete _pca;
//
//CSisr::CSisr():_scale(3.0), _window(3),_overlap(1),_nComponentPCA(30),
//	_targetSparsity(3),_numOfIteration(50),_dictsize(1000)
//{
//	
//}
//
//CSisr::~CSisr(){}
//
//void CSisr::setScale(float s)
//{
//	_scale = s;
//}
//float CSisr::getScale()
//{
//	return _scale;
//}
//
//void CSisr::setWindow(int w)
//{
//	_window = w;
//}
//int CSisr::getWindow()
//{
//	return _window;
//}
//
//void CSisr::setOverlap(int o)
//{
//	_overlap = o;
//}
//int CSisr::getOverlap()
//{
//	return _overlap;
//}
//
//void CSisr::setSparsity(int s)
//{
//	_targetSparsity = s;
//}
//int CSisr::getSparsity()
//{
//	return _targetSparsity;
//}
//
//void CSisr::setIter(int i)
//{
//	_numOfIteration = i;
//}
//int CSisr::getIter()
//{
//	return _numOfIteration;
//}
//
//void CSisr::setDictsize(int s)
//{
//	_dictsize = s;
//}
//int CSisr::getDictsize()
//{
//	return _dictsize;
//}
//
///**/
//void CSisr::setConfsForDicLearning()
//{
//	Confs cf(_scale, 1, _window, 1, _overlap, (int)_scale, cv::INTER_CUBIC, _nComponentPCA);
//	_confs = cf;
//	KConfs kcf(_targetSparsity, _numOfIteration, _dictsize, /*target error*/0);
//	_kConfs = kcf;
//	delete _pca;
//}
//
///**/
//bool CSisr::loadConfs(std::string path)
//{
//	/* path need to be convert to char* */
//	const char* fname = path.c_str();
//
//	int stat = loadConf(fname, _lowD, _highD, _confs, _kConfs, cv::Mat(), _matPCA);
//
//	if (stat == FAIL)
//	{
//		return false;
//	}
//
//	*_pca = compressPCA(_matPCA, _confs.nComponentPCA, cv::Mat(), cv::Mat());
//
//	return true;
//}
//
//bool CSisr::saveConfs(std::string path)
//{
//	const char* fname = path.c_str();
//
//	saveConf(fname, _lowD, _highD, _confs, _kConfs, cv::Mat(), _matPCA);
//	return true;
//}
//
//bool CSisr::loadTrainningSet(std::string path)
//{
//	const char* cpath = path.c_str();
//	loadImages(_trainningSet, cpath, "*.bmp");
//	loadImages(_trainningSet, cpath, "*.jpg");
//	loadImages(_trainningSet, cpath, "*.png");
//	//loadImages(_trainningSet, cpath, "*.bmp");
//	return true;
//}
//
//bool CSisr::learnDictionary()
//{
//	int stat;
//	stat = learnDic(_confs, _kConfs, _trainningSet, _lowD, _highD, cv::Mat(), _matPCA);
//	if (stat == FAIL)
//	{
//		return false;
//	}
//	return true;
//}
//
//bool CSisr::scaleUp()
//{
//	return true;
//}