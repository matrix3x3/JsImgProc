#include <io.h>
#include <direct.h>
#include <vector>
#include <string>
#include <tuple>

#include "stdafx.h"
#include "sisr.h"
#include "sisrsample.h"
#include "debug.h"
#include "common.h"

void initConfiguration(Confs& confs, KConfs& kconfs,
					   float s, int l, int w, int b, int o, int u, int iK, int nCPCA,
					   int ts, int noi, int ds, int te)
{
	//Confs _confs(3.0f, 1, 3, 1 ,1, 3, cv::INTER_CUBIC);
	//KConfs kcf(3, 30, 1000, 0);
	Confs _confs(s,l,w,b,o,u,iK, nCPCA);
	KConfs _kconfs(ts, noi, ds, te);
	confs = _confs;
	kconfs = _kconfs;
}

void sisrLearnDictionary()
{
	/* step 1 */
	Confs confs;
	KConfs kconfs;
	initConfiguration(confs, kconfs,
		3.0f, 1, 3, 1 ,1, 3, cv::INTER_CUBIC, 30,
		3, 30, 1000, 0);

	/* step 2 */
	Images images;
	loadImages(images , "..\\Training", "*.bmp");

	/* step 3*/
	cv::Mat lowD, highD, Err, matPCA;
	learnDic(confs, kconfs, images, lowD, highD, Err, matPCA);

	/* step 4 :save all configurations */
	saveConf(".\\configuration\\SISR_confs.xml", lowD, highD, confs, kconfs, Err, matPCA);
}

void sisrScalupImages()
{
	/* step 1 */
	ImagesYcrcb images;
	loadImages(images , "..\\scaleup", "*.bmp");

	/*step 2*/
	int stat;
	cv::Mat lowResDic, highResDic, Err, matPCA;
	Confs confs;
	KConfs kconfs;
	stat = loadConf(".\\configuration\\SISR_confs.xml", lowResDic, highResDic, confs, kconfs, Err, matPCA);
	//CheckFail(stat == FAIL, "Fail to load configurations");
	if (stat == FAIL)
	{
		printf("Fail to load configuration...\n");
		return;
	}

	/* step 3*/
	ImagesYcrcb outs, diffs, bicubic;
	cv::PCA pca = learnPCA(confs, matPCA);
	scaleUpImages(confs, kconfs, images, lowResDic, highResDic, pca, outs, diffs, bicubic);
	/*for (int i = 0; i < outs.size(); i++)
	{
		printf("%s\n" , std::get<0>(outs[i]));
	}*/
	saveImages(outs, "scaleup", true);
	saveImages(diffs, "diff", false);
	saveImages(bicubic, "bicubic", true);

}