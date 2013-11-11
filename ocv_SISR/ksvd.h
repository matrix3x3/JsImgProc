#pragma once
#include "stdafx.h"

/*******************************************
* Author: Jethro
* Date: 
* Description:
*	ksvd: ksvd
*	function [ D, Gamma , err] = myksvd( X, D0, K, k )
*	Input: D0 - dictionary, X - signals, K -
*		target sparsity, k - iterations
*	Output: D - updated dic, Gamma - new sprse coding,
*		err - error of each step
*******************************************/
OCV_SISR_API int ksvd(cv::Mat& X, const int dictsize, const int K, const int k, cv::Mat& D, cv::Mat& Gamma, cv::Mat& err, const cv::Mat& D0 = cv::Mat(), char c_or_e = 'c'); 