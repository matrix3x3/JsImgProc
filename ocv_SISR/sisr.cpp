#include <io.h>
#include <direct.h>
#include <vector>
#include <string.h>

#include "sisr.h"
#include "debug.h"
#include "omp.h"
#include "ksvd.h"

/*
*	disable or enable verbose
*	enable VERBOSE if want to just debug specific function 
*		because there will be so many output in command window.
*/
#define VERBOSE 0

/*
*	getcwd
*/
SISR_INT get_cwd(char* dir)
{
	char buffer[512] = {0};

	// Get the current working directory: 
	if( (_getcwd( buffer, 512 )) == NULL )
	{
		PrtAndLog("get_cwd error");
		return FAIL;
	}
	else
	{
		PrtAndLog( "%s \nLength: %d\n", buffer, strlen(buffer) );
	}

	if (dir != NULL)
	{
		memcpy(dir, buffer, strlen(buffer));
		dir[strlen(buffer)] = '\0';
	}
	return SUCCESS;
}

#ifdef _DEBUG
/* show img */
void myimshow(const cv::Mat& m, char* winname)
{
	cv::namedWindow(  winname );
	cv::imshow( winname, m);
	cv::waitKey(30);
	//cv::destroyWindow("My imshow");
}
#endif

/*
*	loadImages: used by learnDic Just save the image raw data without image name.
*	In\Out: imgs
*	Input: path, filetype
*/
SISR_INT loadImages(Images& imgs, const char* path, const char* filetype)
{
	PrtAndLog("entering loadImages...");
	//
	char buffer[512];
	get_cwd(buffer);

	/* -1 failure */
	int stat = _chdir(path);
	if (stat == -1)
	{
		printf("error\n");
		return FAIL;
	}
	get_cwd();

	char reg_expr[512] = {0};
	sprintf(reg_expr, "%s%s%s", path, "\\", filetype);

	struct _finddata_t c_file;
	intptr_t hFile;

	if( (hFile = _findfirst( reg_expr, &c_file )) == -1L )
		printf( "No *.c files in current directory!\n" );
	else
	{
		do {
			//printf( " %-12s  %9ld\n", c_file.name, c_file.size );
			cv::Mat temp = cv::imread(c_file.name);
			cv::cvtColor(temp, temp, CV_BGR2YCrCb);

			std::vector<cv::Mat> _ycrcb;
			cv::split(temp, _ycrcb);
			temp = _ycrcb[0];
			imgs.push_back(temp);

		} while( _findnext( hFile, &c_file ) == 0 );
		_findclose( hFile );
	}

	stat = _chdir(buffer);
	if (stat == -1)
	{
		PrtAndLog("error in loadImages caused by _chdir\n");
		return FAIL;
	}

	PrtAndLog("SUCCESS: leaving loadImages...");
	return SUCCESS;
}

/*
*	used by scaleupImages
*	In\Out: imgs
*	Input: path, filetype
*/
SISR_INT loadImages(ImagesYcrcb& imgs, const char* path, const char* filetype)
{
	PrtAndLog("entering loadImage(for scaling up)...");
	//
	char buffer[512];
	get_cwd(buffer);

	/* -1 failure */
	int stat = _chdir(path);
	if (stat == -1)
	{
		printf("error\n");
		return FAIL;
	}
	get_cwd();

	char reg_expr[512] = {0};
	sprintf(reg_expr, "%s%s%s", path, "\\", filetype);

	struct _finddata_t c_file;
	intptr_t hFile;

	if( (hFile = _findfirst( reg_expr, &c_file )) == -1L )
		printf( "No files in current directory!\n" );
	else
	{
		do {
			//printf( " %-12s  %9ld\n", c_file.name, c_file.size );
			Image in = cv::imread(c_file.name);
			cv::cvtColor(in, in, CV_BGR2YCrCb);

			std::string strtmp(c_file.name);
			imgs.insert(ImagesYcrcb::value_type(strtmp, in));
		} while( _findnext( hFile, &c_file ) == 0 );
		_findclose( hFile );
	}

	stat = _chdir(buffer);
	if (stat == -1)
	{
		PrtAndLog("error in loadImage caused by _chdir\n");
		return FAIL;
	}

	PrtAndLog("SUCCESS: leaving loadImage((for scaling up))...");
	return SUCCESS;
}

/*
*	crop image a bit (to simplify scaling issues)
*	used by learnDic
*	In\Out: imgs
*	Input: confs
*/
void imageCrop(Images& imgs, const Confs& confs)
{
	PrtAndLog("entering imageCrop...");
	Images::iterator iter;
	int modulo = confs.scale * (confs.window - confs.overlap);

	for ( iter = imgs.begin() ; iter != imgs.end() ; iter++ )
	{
		cv::Mat::MSize sz = (*iter).size;
		int h = sz[0] - (sz[0] - confs.window * (int)confs.scale) % modulo;
		int v = sz[1] - (sz[1] - confs.window * (int)confs.scale) % modulo;

		PrtAndLog("%d %d", sz[0], sz[1]);
		PrtAndLog("%d %d", h, v);

		*iter = (*iter)(cv::Range(0,h), cv::Range(0,v))/*.clone()*/;

		ASSERT(((*iter).size[0] - confs.window * (int)confs.scale) % modulo == 0 
			&&
				((*iter).size[1] - confs.window * (int)confs.scale) % modulo == 0);
	}
	PrtAndLog("leaving imageCrop...");
}

/*
*	crop image a bit (to simplify scaling issues)
*	used by scaleupImages
*	In\Out: imgs
*	Input: confs
*/
void imageCrop(ImagesYcrcb& imgs, const Confs& confs)
{
	PrtAndLog("entering imageCrop(for scaling up)...");
	
	int modulo = confs.scale * (confs.window - confs.overlap);

	std::for_each(imgs.begin(), imgs.end(),
		[confs, modulo] (ImagesYcrcb::value_type& elem){
			cv::Mat::MSize sz = elem.second.size;
		int h = sz[0] - (sz[0] - confs.window * (int)confs.scale) % modulo;
		int v = sz[1] - (sz[1] - confs.window * (int)confs.scale) % modulo;

		PrtAndLog("%d %d", sz[0], sz[1]);
		PrtAndLog("%d %d", h, v);

		elem.second = elem.second(cv::Range(0,h), cv::Range(0,v))/*.clone()*/;

		ASSERT(((elem.second).size[0] - confs.window * (int)confs.scale) % modulo == 0 
			&&
				((elem.second).size[1] - confs.window * (int)confs.scale) % modulo == 0);
	});

	PrtAndLog("leaving imageCrop(for scaling up)...");
}


/*
*	resize imgs. used by learnDic
*	Input: in_imgs, scale, method
*	Output: out_imgs
*/
SISR_INT resizeImgs(const Images& in_imgs,Images& out_imgs, float scale, int method)
{
	PrtAndLog("entering resizeImgs...");
	CheckFail(scale == 0, "FAIL: in resizeImgs...");

	if (!out_imgs.empty())
	{
		out_imgs.clear();
	}

	cv::Size sz;
	//cv::Mat tmp;

	std::vector<cv::Mat>::const_iterator iter;
	for ( iter = in_imgs.begin() ; iter != in_imgs.end() ; iter++ )
	{
		//myimshow(*iter, "resize");
		cv::Mat tmp;
		cv::resize(*iter, tmp, sz, scale, scale, method);
		//printf("type : %d\n", (*iter).type());
		//myimshow(tmp, "resize");
		out_imgs.push_back(tmp);
	}

	return SUCCESS;
	PrtAndLog("SUCCESS: leaving resizeImgs...");
}

/*
*	resize imgs. used by scaleupImages
*	Input: in_imgs, scale, method
*	Output: out_imgs
*/
SISR_INT resizeImgs(ImagesYcrcb& in_imgs, ImagesYcrcb& out_imgs, float scale, int method)
{
	PrtAndLog("entering resizeImgs(for scaling up)...");
	CheckFail(scale == 0, "FAIL: in resizeImgs(for scaling up)...");

	if (!out_imgs.empty())
	{
		out_imgs.clear();
	}

	std::for_each(in_imgs.begin(), in_imgs.end(),
		[scale, method, &out_imgs] (ImagesYcrcb::value_type& elem){
			
			cv::Mat tmp;
			cv::resize(elem.second, tmp, cv::Size(), scale, scale, method);
			out_imgs.insert(ImagesYcrcb::value_type(elem.first, tmp));
	});

	return SUCCESS;
	PrtAndLog("SUCCESS: leaving resizeImgs(for scaling up)...");
}

/*
*	split Ycrcb img to independent 3 imgs used by scaleupImages
*	Input: in_imgs
*	Output: out_imgs
*/
SISR_INT splitImgs(ImagesYcrcb& in_imgs, SplitImagesYcrcb& out_imgs)
{
	PrtAndLog("entering splitImgs(for scaling up)...");
	
	std::for_each(in_imgs.begin(), in_imgs.end(),
		[&out_imgs] (ImagesYcrcb::value_type& elem){
			
			Images _ycrcb;
			Image img = elem.second.clone();
			cv::split(img, _ycrcb);
			out_imgs.insert(SplitImagesYcrcb::value_type(elem.first, _ycrcb));
	});

	PrtAndLog("leaving splitImgs(for scaling up)...");
	return SUCCESS;
}

/*
*	make patches
*	Input: img - an image, windowsize - mask size, overlap - overlap size
*	Output: patches
*/
void makePatchesOfImg(const Image& img, Patches& patches ,int windowsize, int overlap)
{
	PrtAndLog("entering makePatches...");

	if (!patches.empty())
	{
		patches.clear();
	}

	int step = windowsize - overlap;

	for (int i = 0; i <= img.cols - windowsize; i = i + step)
	{
		for (int j = 0; j <= img.rows - windowsize; j = j + step)
		{
			cv::Mat roi(img, cv::Rect(i, j, windowsize, windowsize));
			patches.push_back(roi.clone());
		}
	}

	PrtAndLog("leaving makePatches...");
}

/*
*	make filters - modify filters according to interpolation factor
*	In: upsamplefactor
*	Out: 4 filters - gradient and laplacian
*/
void makeFilters(Kernels& filters ,int upsamplefactor)
{
	PrtAndLog("entering makeFilters...");

	if (!filters.empty())
	{
		filters.clear();
	}

	/* base filters */
	float gradient[2] = {1,-1}, laplacian[3] = {1.0f/2,-2.0f/2,1.0f/2};
	float* newGradient = new float[2 + upsamplefactor - 1];
	float* newLaplacian = new float[3 + 2 * (upsamplefactor - 1)];

	for (int i = 0; i < 2 + upsamplefactor - 1; i++)
		newGradient[i] = 0;
	for (int j = 0; j < 3 + 2 * (upsamplefactor - 1); j++)
		newLaplacian[j] = 0;

	newGradient[0] = gradient[0];
	newGradient[upsamplefactor] = gradient[1];

	newLaplacian[0] = laplacian[0];
	newLaplacian[upsamplefactor] = laplacian[1];
	newLaplacian[2 * upsamplefactor] = laplacian[2];

	cv::Mat G(1, 2 + upsamplefactor - 1, CV_32F, newGradient);
	cv::Mat L(1, 3 + 2 * (upsamplefactor - 1), CV_32F, newLaplacian);

	filters.push_back(G);
	filters.push_back(G.t());
	filters.push_back(L);
	filters.push_back(L.t());

#if VERBOSE
	for (int i = 0; i < 4; i++)
	{
		array_print(filters[i]);
	}
#endif

	//delete newGradient, newLaplacian;
	//these two will be handled by Mat?? to free them

	PrtAndLog("leaving makeFilters...");
}

/*
*	extract high freq infos from input image
*	Input: in - input image , scale - scale factor, need pass to makePatches()
*	Output: out - high freq infos
*/
SISR_INT extractHifreqInfoFromImg(const Image& in, cv::Mat& features, const Confs confs, bool filtrate )
{
	PrtAndLog("entering extract...");

	/* compute convolution */
	if (filtrate)
	{
		PrtAndLog("compute convolution...");

		//cv::Point anchor(0,0);

		Images resultOfConverlution;

		//for (unsigned int i = 0; i < confs.filters.size(); i++)
		//{
		//	cv::Mat res;
		//	cv::filter2D(in, res, -1, confs.filters[i], anchor, cv::BORDER_REPLICATE);

		//	myimshow(res, "1");

		//	resultOfConverlution.push_back(res);
		//}
		for (unsigned int i = 0; i < confs.filters.size(); i++)
		{
			cv::Mat res;

			if (i < 2)
			{
				cv::Point anchor(0,0);
				cv::filter2D(in, res, -1, confs.filters[i], anchor, cv::BORDER_REPLICATE);
				resultOfConverlution.push_back(res);
			}
			else
			{
				cv::filter2D(in, res, -1, confs.filters[i], /*anchor,*/cv::Point(), cv::BORDER_REPLICATE);
				resultOfConverlution.push_back(res);
			}
		}


#if VERBOSE
		for (int i = 0; i < resultOfConverlution.size(); i++)
		{
			myimshow(resultOfConverlution[i],"i");
		}
#endif

		/* patches of image filtered by 4 filters: gradient, gradient.t(), laplacian, laplacian.t() */
		Patches pOfG,pOfGT,pOfL,pOfLT;

		makePatchesOfImg(resultOfConverlution[0], pOfG, confs.window * confs.scale, confs.overlap * confs.scale);
		makePatchesOfImg(resultOfConverlution[1], pOfGT, confs.window * confs.scale, confs.overlap * confs.scale);
		makePatchesOfImg(resultOfConverlution[2], pOfL, confs.window * confs.scale, confs.overlap * confs.scale);
		makePatchesOfImg(resultOfConverlution[2], pOfLT, confs.window * confs.scale, confs.overlap * confs.scale);

		/* features */
		/*std::vector<Patches> features;*/
		for (unsigned int i = 0; i < pOfG.size(); i++)
		{
			cv::Mat m1,m2,m3,m4,tmp;

			m1 = pOfG[i].t();
			m1 = m1.reshape(0,1);
			m2 = pOfGT[i].t();
			m2 = m2.reshape(0,1);
			m3 = pOfL[i].t();
			m3 = m3.reshape(0,1);
			m4 = pOfLT[i].t();
			m4 = m4.reshape(0,1);

			tmp.push_back(m1);
			tmp.push_back(m2);
			tmp.push_back(m3);
			tmp.push_back(m4);
			tmp = tmp.reshape(0,1);

			features.push_back(tmp.clone());
		}
	}
	/* don't compute convolution */
	else
	{
		PrtAndLog("donot convolution...");

		Patches pOfIn;

		makePatchesOfImg(in, pOfIn, confs.window * confs.scale, confs.overlap * confs.scale);

		/* features */
		/*std::vector<Patches> features;*/
		for (unsigned int i = 0; i < pOfIn.size(); i++)
		{
			cv::Mat m;

			m = pOfIn[i].t();
			m = m.reshape(0,1);
#if VERBOSE
			std::cout << m << std::endl;
#endif
			features.push_back(m.clone());
		}
	}

	PrtAndLog("leaving extract...");
	return SUCCESS;

}

/*
*	collect features of all input images by calling extractHiresInfoFromImg()
*	Input: Images, confs
*	Output: features
*/
SISR_INT extractFeaturesOfAllImages(const Images& in, cv::Mat& features, const Confs confs, bool filtrate)
{
	PrtAndLog("entering extractFeaturesOfAllImages");

	for (unsigned int i = 0; i < in.size(); i++)
	{
		extractHifreqInfoFromImg(in[i], features, confs, filtrate);
	}

	/* make a big array : each signal correspond to a column vector */

	PrtAndLog("SUCCESS: leaving extractFeaturesOfAllImages");
	return SUCCESS;
}

/*
*	learn dic
*	Input: confs, kcf, hires
*	Output: lowD, highD, Err
*/
SISR_INT learnDic(const Confs confs, const KConfs kcf, Images& hires,
				  cv::Mat& lowD, cv::Mat& highD, cv::Mat& Err, cv::Mat& matPCA)
{
	PrtAndLog("entering leranDic...");

	/* store current status */
	int stat = 0;

	//imageCrop(hires, (int)(confs.scale * (confs.window - confs.overlap)));
	imageCrop(hires, confs);

	/* make lores and midres images */
	Images lores;
	stat = resizeImgs(hires, lores, 1.0 / confs.scale , confs.interpolateKernel);
	CheckFail(stat == FAIL, "Fail to resize hires to lores");
	Images midres, interpolated;
	stat = resizeImgs(lores, midres, confs.upSampleFactor , confs.interpolateKernel);
	CheckFail(stat == FAIL, "Fail to resize lowres to midres");
	stat = resizeImgs(lores, interpolated, confs.scale , confs.interpolateKernel);
	CheckFail(stat == FAIL, "Fail to resize lowres to interpolated");

	/* extract features of lowres */
	cv::Mat featuresOfLowres;
	extractFeaturesOfAllImages(midres, featuresOfLowres, confs);
	midres.clear();
	cv::Mat featuresOfLowres64;
	featuresOfLowres.convertTo(featuresOfLowres64, SISR_MAT_TYPE);
	featuresOfLowres.release();
	featuresOfLowres64 = featuresOfLowres64/255;
	/* end of extract features of lowres */

	/* extract features of hires */
	Images diffs;
	for (unsigned int i = 0; i < hires.size(); i++)
	{
		cv::Mat tmp;
		tmp = hires[i] - interpolated[i];
		diffs.push_back(tmp);
		myimshow(tmp,"1");
	}

	interpolated.clear();
	hires.clear();
	cv::Mat featuresOfHires;
	extractFeaturesOfAllImages(diffs, featuresOfHires, confs, false);
	cv::Mat featuresOfHires64;
	featuresOfHires.convertTo(featuresOfHires64, SISR_MAT_TYPE);
	featuresOfHires64 = featuresOfHires64/255;
	featuresOfHires.release();

	diffs.clear();
	/* end of extract features of hires */

	/* make PCA */
	//int nComponents = 30;
	//saveMatUsedByPCA("PCA_data.xml", featuresOfLowres64, nComponents);

	matPCA = featuresOfLowres64;

	cv::Mat out;
	//compressPCA(featuresOfLowres64, nComponents, featuresOfLowres64,out);
	compressPCA(featuresOfLowres64, confs.nComponentPCA, featuresOfLowres64,out);
	featuresOfLowres64.release();

	/* apply ksvd training to learn lowres dic */
	cv::Mat /*D0,lowD,*/Gamma/*,Err*/;
	// dicsize

	/* using bomp */
	out = out.t();
	stat = ksvd(out, kcf.dictsize, kcf.targetSparsity, kcf.numOfIteration, lowD, Gamma, Err, cv::Mat(), 'e');
	CheckFail(stat == FAIL, "Fail to train dic using ksvd");
	/* compute hiDic */
	//highD = featuresOfHires64.t() * Gamma.inv(cv::DECOMP_SVD);
	/*cv::Mat GammaT = Gamma.t();
	highD = featuresOfHires64.t() * (GammaT * (Gamma*GammaT).inv());*/

	cv::Mat GammaT = Gamma.t();
	cv::Mat GmGT = Gamma*GammaT;
	highD = (featuresOfHires64.t() * GammaT) * (GmGT.inv());

	return SUCCESS;
	PrtAndLog("SUCCESS: leaving leranDic...");
}

/*
*	do PCA to reduce dimentions of input signals
*	Input: _pcaset, maxComponents, testset
*	Output: _compressed
*/
cv::PCA compressPCA(const cv::Mat& _pcaset, int maxComponents, const cv::Mat& testset, cv::Mat& _compressed)
{
	PrtAndLog("entering compressPCA");

	cv::PCA pca(_pcaset, // pass the data
		cv::Mat(), // there is no pre-computed mean vector,
		// so let the PCA engine to compute it
		CV_PCA_DATA_AS_ROW, // indicate that the vectors
		// are stored as matrix rows
		// (use CV_PCA_DATA_AS_COL if the vectors are
		// the matrix columns)
		maxComponents // specify how many principal components to retain
		);
	// if there is no test data, just return the computed basis, ready-to-use
	std::cout << pca.eigenvalues <<std::endl;
	if( !testset.data )
		return pca;
	
	CV_Assert( testset.cols == _pcaset.cols );
	_compressed.create(testset.rows, maxComponents, testset.type());
	cv::Mat reconstructed;
	for( int i = 0; i < testset.rows; i++ )
	{
		cv::Mat vec = testset.row(i), coeffs = _compressed.row(i);
#if VERBOSE
		cv::Mat temp = vec * pca.eigenvectors.t();
		std::cout<< temp << std::endl;
#endif
		// compress the vector, the result will be stored
		// in the i-th row of the output matrix
		pca.project(vec, coeffs);
#if VERBOSE
		std::cout<<coeffs<<std::endl;
#endif
		// and then reconstruct it
		pca.backProject(coeffs, reconstructed);

		//------newly added---------
		cv::Mat recon;
		reconstructed.convertTo(recon, vec.type());
		//--------------------------
		// and measure the error
#if VERBOSE
		printf("%d. diff = %g\n", i, norm(vec, recon, cv::NORM_L2));
#endif
	}
	
	PrtAndLog("leaving compressPCA");
	return pca;
}

/* save Mat used by PCA */
void saveMatUsedByPCA(char* name, const cv::Mat& mat, int nComponents)
{
	/* store featuresOfLowres64 and nComponents to 'PCA_data.xml' */
	cv::FileStorage fsOfPCA(name, cv::FileStorage::WRITE);
	fsOfPCA << "nComponents" << nComponents;
	fsOfPCA << "featuresOfLowres64" << mat ;
	fsOfPCA.release();
}

/* load Mat used by PCA */
SISR_INT loadMatUsedByPCA(char* name, cv::Mat& mat, int& nComponents)
{
	if ((_access_s(name, 0)) != 0)
	{
		PrtAndLog("%s does not exist...", name);
		return FAIL;
	}

	/* store featuresOfLowres64 and nComponents to 'PCA_data.xml' */
	cv::FileStorage fsOfPCA(name, cv::FileStorage::READ);

	nComponents = (int)fsOfPCA["nComponents"];
	fsOfPCA["featuresOfLowres64"] >> mat ;
	fsOfPCA.release();
	return SUCCESS;
}

/* save conf. */
void saveConf(const char* name, const cv::Mat& lowResDic, const cv::Mat& hiResDic,
			  const Confs& confs, const KConfs& kconfs, const cv::Mat& ksvdErr, const cv::Mat& matPCA)
{
	cv::FileStorage fs(name, cv::FileStorage::WRITE);
	
	/* confs */
	fs << "scale" << confs.scale;
	fs << "level" << confs.level;
	fs << "window" << confs.window;
	fs << "border" << confs.border;
	fs << "overlap" << confs.overlap;
	fs << "upSampleFactor" << confs.upSampleFactor;
	fs << "interpolateKernel" << confs.interpolateKernel;
	fs << "nComponentPCA" << confs.nComponentPCA;
	/* ksvd confs */
	fs << "targetSparsity" << kconfs.targetSparsity;
	fs << "numOfIteration" << kconfs.numOfIteration;
	fs << "dictsize" << kconfs.dictsize;
	fs << "targetError" << kconfs.targetError;
	/* ksvd err */
	fs << "ksvdErr" << ksvdErr;
	/* low res and hi res dics */
	fs << "lowResDic" << lowResDic ;
	fs << "hiResDic" << hiResDic;
	/* mat used to train PCA */
	fs << "matPCA" << matPCA;
	fs.release();
}

/* load conf. */
SISR_INT loadConf(const char* name, cv::Mat& lowResDic, cv::Mat& hiResDic,
				  Confs& confs, KConfs& kconfs, cv::Mat& ksvdErr, cv::Mat& matPCA)
{
	if ((_access_s(name, 0)) != 0)
	{
		PrtAndLog("%s does not exist...", name);
		return FAIL;
	}

	cv::FileStorage fs(name, cv::FileStorage::READ);

	/* confs */
	float s;
	int l, w, b, o, u, iK, nCPCA;

	fs["scale"] >> s;
	fs["level"] >> l;
	fs["window"] >> w;
	fs["border"] >> b;
	fs["overlap"] >> o;
	fs["upSampleFactor"] >> u;
	fs["interpolateKernel"] >> iK;
	fs["nComponentPCA"] >> nCPCA;

	Confs _cfs(s,l,w,b,o,u,iK,nCPCA);
	confs = _cfs;

	/* kconfs */
	int ts, noi, ds, te;
	fs["targetSparsity"] >> ts;
	fs["numOfIteration"] >> noi;
	fs["dictsize"] >> ds;
	fs["targetError"] >> te;

	KConfs _kcfs(ts,noi,ds,te);
	kconfs = _kcfs;

	/* ksvdErr */
	fs["ksvdErr"] >> ksvdErr;
	/* dics */
	fs["lowResDic"] >> lowResDic ;
	fs["hiResDic"] >> hiResDic;
	/* matPCA */
	fs["matPCA"] >> matPCA;
	fs.release();

	return SUCCESS;
}

cv::PCA learnPCA(Confs confs, cv::Mat& matPCA)
{
	cv::PCA pca = compressPCA(matPCA, confs.nComponentPCA, cv::Mat(), cv::Mat());
	return pca;
}

/*
*	scaleup Images
*	Input: confs, kconfs, lowres, lowD, highD,
*	Output:  results, diffs, bicubic
*/
SISR_INT scaleUpImages(/*const*/ Confs confs, const KConfs kconfs, ImagesYcrcb& lowres, cv::Mat& lowD, cv::Mat& highD, cv::PCA pca,
					   ImagesYcrcb& results, ImagesYcrcb& diffs, ImagesYcrcb& bicubic)
{
	PrtAndLog("entering scaleUpImages...");

	/* store current status */
	int stat = 0;
	/* load PCA */
	/*cv::Mat matPCA64;
	int nComponents;
	stat = loadMatUsedByPCA("PCA_data.xml", matPCA64, nComponents);*/
	/* make pca */
	//cv::PCA pca = compressPCA(matPCA64, nComponents, cv::Mat(), cv::Mat());
	//cv::PCA pca = compressPCA(matPCA, confs.nComponentPCA, cv::Mat(), cv::Mat());

	imageCrop(lowres, confs);

	/* bicubic interpolation */
	ImagesYcrcb midres;
	stat = resizeImgs(lowres, midres, confs.scale , confs.interpolateKernel);
	CheckFail(stat == FAIL, "Fail to resize lowres to interpolated");
	bicubic = midres;

	/* split to independent Y, cr, cb channel imgs */
	SplitImagesYcrcb splitOfMidres;
	splitImgs(midres, splitOfMidres);

	/* scale up imgs */
	SplitImagesYcrcb splitOfHires;
	SplitImagesYcrcb::iterator iter;
	for(iter = splitOfMidres.begin(); iter != splitOfMidres.end() ; iter ++)
	{
		cv::Mat features, features64, lowDimSigs;

		// full overlap
		confs.overlap = confs.window - confs.overlap;

		extractHifreqInfoFromImg((*iter).second[0], features, confs);
		features.convertTo(features64, SISR_MAT_TYPE);
		features.release();
		features64 = features64/255;

		pca.project(features64, lowDimSigs);
		features64.release();

		/* encoding features using omp */
		cv::Mat Gamma;
		int status = comp(lowD,lowDimSigs.t(), Gamma, kconfs.targetSparsity);
		CheckFail(status == FAIL, "FAIL: when computing sparse code");

		cv::Mat hiresMat = highD * Gamma;
		hiresMat = hiresMat*255;
		cv::Mat lowFeqComponent, lowFeqComponent64;
		extractHifreqInfoFromImg((*iter).second[0], lowFeqComponent, confs, false);
		lowFeqComponent.convertTo(lowFeqComponent64, SISR_MAT_TYPE);
		lowFeqComponent.release();

		Image diffimg;
		reConstructImg(hiresMat, (*iter).second[0].size, confs, diffimg);
		/* store diff img to diffs */
		diffs.insert(ImagesYcrcb::value_type((*iter).first, diffimg));

		//myimshow(diffimg, "diff");
		//cv::imwrite("diffimg.png",diffimg);

		hiresMat += lowFeqComponent64.t();

		Image out;
		reConstructImg(hiresMat, (*iter).second[0].size, confs, out);
		/* store the scaleup results */
		Images highs;
		highs.push_back(out);
		highs.push_back((*iter).second[1]);
		highs.push_back((*iter).second[2]);
		splitOfHires.insert(SplitImagesYcrcb::value_type((*iter).first, highs));
	}
	
	std::for_each(splitOfHires.begin(), splitOfHires.end(),
		[&results] (SplitImagesYcrcb::value_type& elem) {

			Image img;
			cv::merge(elem.second, img);
			//myimshow(img, "result");
			results.insert(ImagesYcrcb::value_type(elem.first, img));

	});

	return SUCCESS;
}

/*
*	reconstruct patches from mat
*/
SISR_INT reConstructPatchesFromMat(const cv::Mat m, Patches& p, int windowsize)
{
	cv::Mat mt = m.t();
	CheckFail(mt.cols != windowsize * windowsize, "window size and image size doesn't match...");

	/* make sure p is empty */
	if (!p.empty())
	{
		p.clear();
	}

	for (int i = 0; i < mt.rows; i++)
	{
		Patch tmp/*, tmp8U*/;
		tmp = (mt.row(i)).reshape(0, windowsize);
		//tmp.convertTo(tmp8U, CV_8U);

		//-------------------------
		p.push_back(tmp.t());
		//-------------------------!!!
	}
	return SUCCESS;
}

/*
*	reconstruct img from the product of (Dh * Gamma)
*/
SISR_INT reConstructImg(const cv::Mat mat, cv::Mat::MSize szOfImg, const Confs& confs, Image& out)
{
	PrtAndLog("entering reConstructImg...");

	int stat;
	int windowsize = confs.window * confs.scale;
	int overlap = confs.overlap * confs.scale;
	Patches p;
	stat = reConstructPatchesFromMat(mat, p, windowsize);
	CheckFail(stat == FAIL, "error");

	//modify patches
	/* make a mask */
	/*cv::Mat mask = cv::Mat::ones(windowsize, windowsize, mat.type());
	cv::Mat tmp;
	tmp = mask(cv::Range(0,overlap), cv::Range(0,overlap));
	tmp = tmp/4;
	tmp = mask(cv::Range(overlap, windowsize - overlap), cv::Range(0,overlap));
	tmp = tmp/2;
	tmp = mask(cv::Range(windowsize - overlap, windowsize), cv::Range(0,overlap));
	tmp = tmp/4;
	tmp = mask(cv::Range(0,overlap), cv::Range(overlap, windowsize - overlap));
	tmp = tmp/2;
	tmp = mask(cv::Range(windowsize - overlap, windowsize), cv::Range(overlap, windowsize - overlap));
	tmp = tmp/2;
	tmp = mask(cv::Range(0,overlap), cv::Range(windowsize - overlap, windowsize));
	tmp = tmp/4;
	tmp = mask(cv::Range(overlap, windowsize - overlap), cv::Range(windowsize - overlap, windowsize));
	tmp = tmp/2;
	tmp = mask(cv::Range(windowsize - overlap, windowsize), cv::Range(windowsize - overlap, windowsize));
	tmp = tmp/4;*/

	int step = windowsize - overlap;
	int rows = szOfImg[0], cols = szOfImg[1];
	Image _img = cv::Mat::zeros(rows, cols,SISR_MAT_TYPE);
	Image _weight = cv::Mat::zeros(rows, cols, SISR_MAT_TYPE);

	int k = 0;
	for (int i = 0; i <= cols - windowsize; i = i + step)
	{
		for (int j = 0; j <= rows - windowsize; j = j + step)
		{
			cv::Mat roi(_img, cv::Rect(i, j, windowsize, windowsize));
			cv::Mat roi_of_weight(_weight, cv::Rect(i, j, windowsize, windowsize));

			/*cv::Mat roi(_img, cv::Rect(i, j, windowsize, windowsize));
			cv::Mat t = p[k].mul(mask);*/
			//roi = roi + p[k].mul(mask);
			roi = roi + p[k];
			roi_of_weight += 1;
			k ++;
		}
	}
	_img = _img/_weight;

	_img.convertTo(out, CV_8U);

	/*for (int i = 0; i < overlap; i++)
	{
		out.col(i) *= 2;
		out.row(i) *= 2;
		out.col(out.cols - i - 1) *= 2;
		out.row(out.rows - i - 1) *= 2;
	}*/
	
#if 0
	myimshow(out, "out");
#endif
	PrtAndLog("SUCCESS: leaving reConstructImg...");
	return SUCCESS;
}

SISR_INT saveImages(ImagesYcrcb& imgs, const std::string imgtype, bool doCvtColor)
{
	errno_t err = 0;

	// Check for existence. 
	if ((err = _access_s( ".\\results", 0 )) != 0 )
	{
		PrtAndLog( "diretory 'results' doesn't exists.\n" );
		if( _mkdir( ".\\results" ) == 0 )
		{
			PrtAndLog( "Directory '\\testtmp' was successfully created\n" );
		}
		else
		{
			printf( "Problem creating directory '\\results'\n" );
			return FAIL;
		}
	}

	/*system("cd results");
	system("dir");
*/

	ImagesYcrcb::iterator it;
	std::string s;
	Image img;
	for (it = imgs.begin(); it != imgs.end(); it++)
	{
		s = imgtype;
		s.append("_");
		s.append((*it).first);
		if (doCvtColor)
		{
			cv::cvtColor((*it).second, img, cv::COLOR_YCrCb2BGR);
		}
		else
		{
			img = (*it).second;
		}
		cv::imwrite(".\\results\\" + s, img);

	}
	return SUCCESS;
}