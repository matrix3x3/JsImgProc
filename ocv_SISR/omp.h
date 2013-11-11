#pragma once
#include "stdafx.h"

#ifdef ENABLE_ARRAY_PRINT
	OCV_SISR_API void array_print(const cv::Mat& omega);
#else
	#ifndef array_print/*(omega)*/
		#define  array_print(omega) do{}while(false)
	#endif
#endif
OCV_SISR_API void getSubMat(const cv::Mat& In, const cv::Mat I, cv::Mat& Out, const int i, const char col_or_row = 'c');

/* get a value of 2D matrix of location (x,y) */
#define GetSetValueXY(type, mat, x, y) (*((type *)(mat.data + mat.step[0]*(x) + mat.step[1]*(y))))

/*******************************************
* Author: Jethro
* Date: 
* Description:
*	COMP: OMP-Cholesky
*	function [ S ] = comp( D, X, K )
*	Input: D - dictionary, X - signals, K -
*		target sparsity
*	Output: S - sparse coding of X
*******************************************/
OCV_SISR_API int comp(const cv::Mat& D, const cv::Mat& X, cv::Mat& S, int K);


/*******************************************
* Author: Jethro
* Date: 
* Description:
*	BOMP: Batch OMP -- Error Constrained OMP
*	Input: D - dictionary, X - signals, K -
*		target sparsity, e - error costrain
*	Output: S - sparse coding of X
*******************************************/
OCV_SISR_API int bomp(const cv::Mat& D, const cv::Mat& X, cv::Mat& S, int K, float e);