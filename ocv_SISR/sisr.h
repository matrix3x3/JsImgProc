#pragma once
#include "stdafx.h"
#include <tuple>

/* basic types */
typedef std::vector<cv::Mat> Images, Patches, Kernels;
typedef cv::Mat Image,Patch;
/* sisr and ksvd configurations */
typedef struct _Confs Confs, *pConfs;
typedef struct _ksvdConfs KConfs, *pKConfs;

/* Function declarations */
OCV_SISR_API SISR_INT get_cwd(char * dir = NULL);
OCV_SISR_API SISR_INT loadImages(Images& imgs, const char* path, const char* filetype = "*.h");
OCV_SISR_API void imageCrop(Images& imgs, const Confs& confs);
OCV_SISR_API SISR_INT learnDic(const Confs confs, const KConfs kcf, Images& hires, 
							   cv::Mat& lowD = cv::Mat(), cv::Mat& highD = cv::Mat(),
							   cv::Mat& Err = cv::Mat(), cv::Mat& matPCA = cv::Mat());
OCV_SISR_API SISR_INT resizeImgs(const Images& in_imgs,Images& out_imgs, float scale, int method = cv::INTER_CUBIC);
OCV_SISR_API void makePatchesOfImg(const Image& img, Images& patches ,int windowsize, int overlap);
OCV_SISR_API SISR_INT extractHifreqInfoFromImg(const Image& in, cv::Mat& features, const Confs confs, bool filtrate = true );
OCV_SISR_API void makeFilters(Kernels& filters ,int upsamplefactor);
OCV_SISR_API SISR_INT extractFeaturesOfAllImages(const Images& in, cv::Mat& features, const Confs confs, bool filtrate = true);
OCV_SISR_API cv::PCA compressPCA(const cv::Mat& _pcaset, int maxComponents, const cv::Mat& testset, cv::Mat& _compressed);

OCV_SISR_API void saveMatUsedByPCA(char* name, const cv::Mat& mat, int nComponents);
OCV_SISR_API SISR_INT loadMatUsedByPCA(char* name, cv::Mat& mat, int& nComponents);
OCV_SISR_API void saveConf(/* */const char* name, const cv::Mat& lowResDic, const cv::Mat& hiResDic,
						   const Confs& confs, const KConfs& kconfs, const cv::Mat& ksvdErr,  const cv::Mat& matPCA);
OCV_SISR_API SISR_INT loadConf(/* */const char* name, cv::Mat& lowResDic, cv::Mat& hiResDic,
							   Confs& confs, KConfs& kconfs, cv::Mat& ksvdErr, cv::Mat& matPCA);
OCV_SISR_API SISR_INT reConstructImg(const cv::Mat mat, cv::Mat::MSize szOfImg, const Confs& confs, Image& out);


/* SplitImages - to store 3 channel image split to 3 independent images - Ycrcb */
typedef std::map<std::string, std::vector<cv::Mat>> SplitImagesYcrcb;
typedef std::map<std::string, Image> ImagesYcrcb;

OCV_SISR_API cv::PCA learnPCA(Confs confs, cv::Mat& matPCA);

OCV_SISR_API SISR_INT scaleUpImages(/*const */Confs confs, const KConfs kconfs, ImagesYcrcb& lowres, cv::Mat& lowD, cv::Mat& highD, cv::PCA pca,
					   ImagesYcrcb& results = ImagesYcrcb(), ImagesYcrcb& diffs = ImagesYcrcb(), ImagesYcrcb& bicubic = ImagesYcrcb());


/* for scaling up - 3 overloading functions corresponding to them used by learnDic */
OCV_SISR_API SISR_INT loadImages(ImagesYcrcb& imgs, const char* path, const char* filetype = "*.h");
OCV_SISR_API void imageCrop(ImagesYcrcb& imgs, const Confs& confs);
OCV_SISR_API SISR_INT resizeImgs(ImagesYcrcb& in_imgs, ImagesYcrcb& out_imgs, float scale, int method);
OCV_SISR_API SISR_INT splitImgs(ImagesYcrcb& in_imgs, SplitImagesYcrcb& out_imgs);

OCV_SISR_API SISR_INT saveImages(ImagesYcrcb& imgs, const std::string imgtype , bool doCvtColor);

/* helper functions to show a image */
#ifdef _DEBUG
/* show img */
	OCV_SISR_API void myimshow(const cv::Mat& m, char* winname);
#else
	#define myimshow(mat) do{}while(false)
#endif

/* input parameters used in sisr */
struct _Confs
{
	float scale;
	int level;
	int window;
	int border;
	int overlap;
	int upSampleFactor;
	Kernels filters;
	int interpolateKernel;

	/* add nComponentPCA */
	int nComponentPCA;

	_Confs(){};
	_Confs(float s, int l, int w, int b, int o, int u, int iK, int nCPCA)
	{
		scale = s;
		level = l;
		window = w;
		border = b;
		overlap = o;
		upSampleFactor = u;

		makeFilters(filters, upSampleFactor);

		interpolateKernel = iK;
		nComponentPCA = nCPCA;
	}

	~_Confs(){}
};

struct _ksvdConfs
{
	int targetSparsity;
	int numOfIteration;
	int dictsize;
	/* reserved */
	int targetError;

	_ksvdConfs(){}
	_ksvdConfs(int ts, int noi, int ds, int te)
	{
		targetSparsity = ts;
		numOfIteration = noi;
		dictsize = ds;
		targetError = te;
	}

	~_ksvdConfs(){}
};