#include "superres.h"

CSisrController * CSisrController::_singleton = nullptr;

CSisr::CSisr():_scale(3.0), _window(3),_overlap(1),_nComponentPCA(50),
	_targetSparsity(3),_numOfIteration(1),_dictsize(1000)
{
	
}

CSisr::~CSisr(){}

void CSisr::setScale(float s)
{
	_scale = s;
}
float CSisr::getScale()
{
	return _scale;
}

void CSisr::setWindow(int w)
{
	_window = w;
}
int CSisr::getWindow()
{
	return _window;
}

void CSisr::setOverlap(int o)
{
	_overlap = o;
}
int CSisr::getOverlap()
{
	return _overlap;
}

void CSisr::setSparsity(int s)
{
	_targetSparsity = s;
}
int CSisr::getSparsity()
{
	return _targetSparsity;
}

void CSisr::setIter(int i)
{
	_numOfIteration = i;
}
int CSisr::getIter()
{
	return _numOfIteration;
}

void CSisr::setDictsize(int s)
{
	_dictsize = s;
}
int CSisr::getDictsize()
{
	return _dictsize;
}

/**/
void CSisr::setConfsForDicLearning()
{
	Confs cf(_scale, 1, _window, 1, _overlap, (int)_scale, cv::INTER_CUBIC, _nComponentPCA);
	_confs = cf;
	KConfs kcf(_targetSparsity, _numOfIteration, _dictsize, /*target error*/0);
	_kConfs = kcf;
	//delete _pca;
}

/**/
bool CSisr::loadConfs(std::string path)
{
	/* path need to be convert to char* */
	const char* fname = path.c_str();

	int stat = loadConf(fname, _lowD, _highD, _confs, _kConfs, cv::Mat(), _matPCA);

	if (stat == FAIL)
	{
		return false;
	}

	_pca = compressPCA(_matPCA, _confs.nComponentPCA, cv::Mat(), cv::Mat());

	//
	_matPCA.release();

	return true;
}

bool CSisr::saveConfs(std::string path)
{
	const char* fname = path.c_str();

	saveConf(fname, _lowD, _highD, _confs, _kConfs, cv::Mat(), _matPCA);
	return true;
}

bool CSisr::isTrainningSetEmpty()
{
	return _trainningSet.empty();
}

bool CSisr::loadTrainningSet(std::string path)
{
	if (_trainningSet.size() != 0)
	{
		_trainningSet.clear();
	}
	const char* cpath = path.c_str();
	loadImages(_trainningSet, cpath, "*.bmp");
	loadImages(_trainningSet, cpath, "*.jpg");
	loadImages(_trainningSet, cpath, "*.png");
	//loadImages(_trainningSet, cpath, "*.bmp");
	return true;
}

bool CSisr::learnDictionary()
{
	/* make sure parameters are set to default if user doesn't config this */
	setConfsForDicLearning();

	if (_trainningSet.size() == 0)
	{
		return false;
	}
	/* make sure confs is initialized */
	int stat;
	stat = learnDic(_confs, _kConfs, _trainningSet, _lowD, _highD, cv::Mat(), _matPCA);
	if (stat == FAIL)
	{
		return false;
	}

	_pca = compressPCA(_matPCA, _confs.nComponentPCA, cv::Mat(), cv::Mat());

	return true;
}

//void CSisr::scaleUp(const cv::Mat& src, cv::Mat& dst)
void CSisr::scaleUp(const cv::Mat& src, cv::Mat& dst, cv::Mat& diff)
{
	if (_lowD.data == nullptr || _highD.data == nullptr)
	{
		SisrExcept e;
		throw e;
	}

	ImagesYcrcb images, outs, diffs, bicubic;
	std::string name = "inScaleUp";
	images.insert(ImagesYcrcb::value_type(name, src));

	scaleUpImages(_confs, _kConfs, images, _lowD, _highD, _pca, outs, diffs, bicubic);

	dst = outs.at("inScaleUp");
	diff = diffs.at("inScaleUp");
}

void CSisr::process(const cv::Mat& src, cv::Mat& dst)
{
	cv::Mat diff;
	scaleUp(src, dst, diff);
}

void CSisr::process(const cv::Mat& src, cv::Mat& dst, cv::Mat& diff)
{
	scaleUp(src, dst, diff);
}