#include "stdafx.h"
#include "omp.h"
#include "debug.h"
//#include <cmath>
#ifdef ENABLE_ARRAY_PRINT
void array_print(const cv::Mat& omega)
{
	SISR_INT max_disp_cols = 5;
	//ASSERT(omega.rows != 0 && omega.cols != 0);
	if (omega.type() == SISR_MAT_TYPE)
	{
		cv::MatConstIterator_<SISR_DOUBLE> it = omega.begin<SISR_DOUBLE>(), it_end = omega.end<SISR_DOUBLE>();
		for (SISR_INT i = 0; it != it_end; ++it, i++)
		{
			SISR_DOUBLE v = *it;
			if (*it != 0)
			{
				printf("(%d,%d) : %lf  ", i/omega.cols, i - (i/omega.cols)*omega.cols, v);
			}

			if (omega.cols < max_disp_cols)
			{
				if ((i+1) % omega.cols == 0)
					printf("\n");
			}
			else
			{
				if ((i+1) % max_disp_cols == 0)
					printf("\n");
			}

		}
	}
	else if (omega.type() == CV_32F)
	{
		cv::MatConstIterator_<float> it = omega.begin<float>(), it_end = omega.end<float>();
		for (SISR_INT i = 0; it != it_end; ++it, i++)
		{
			float v = *it;
			if (*it != 0)
			{
				printf("(%d,%d) : %f  ", i/omega.cols, i - (i/omega.cols)*omega.cols, v);
			}

			if (omega.cols < max_disp_cols)
			{
				if ((i+1) % omega.cols == 0)
					printf("\n");
			}
			else
			{
				if ((i+1) % max_disp_cols == 0)
					printf("\n");
			}
		}
	}
	else if (omega.type() == CV_32S)
	{
		cv::MatConstIterator_<SISR_INT> it = omega.begin<SISR_INT>(), it_end = omega.end<SISR_INT>();
		for (SISR_INT i = 0; it != it_end; ++it, i++)
		{
			SISR_INT v = *it;

			if (*it != 0)
			{
				printf("(%d,%d) : %d  ", i/omega.rows, i - (i/omega.rows)*omega.rows, v);

			}

			if (omega.cols < max_disp_cols)
			{
				if ((i+1) % omega.cols == 0)
					printf("\n");
			}
			else
			{
				if ((i+1) % max_disp_cols == 0)
					printf("\n");
			}
		}
	}
	
	printf("\n");
}

#else
	#ifndef array_print/*(omega)*/
		#define  array_print(omega) do{}while(false)
	#endif
#endif
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
SISR_INT comp(const cv::Mat& D, const cv::Mat& X, cv::Mat& S, const int K)
{
	PrtAndLog("entering comp...");

	//ASSERT(D.type() == SISR_MAT_TYPE && X.type() == SISR_MAT_TYPE);
	if (D.type() != SISR_MAT_TYPE || X.type() != SISR_MAT_TYPE)
	{
		PrtAndLog("FAIL: D and X should be type of SISR_MAT_TYPE.(currently only support this type)");
		ASSERT(0);
		return FAIL;
	}
	// Transpose of S
	//cv::Mat ST;

	/* n: rows, m: columns */
	SISR_INT X_n, X_m, D_n, D_m;
	X_n = X.rows;
	X_m = X.cols;
	D_n = D.rows;
	D_m = D.cols;

	if (X_m == 0)
	{
		PrtAndLog("FAIL: X is empty");
		ASSERT(0);
		return FAIL;
	}

	for (SISR_INT i = 0; i < X_m; i++)
	{
		/* initial helper data structures */
		cv::Mat v = cv::Mat::zeros(D_m, 1, SISR_MAT_TYPE);
		cv::Mat L = cv::Mat::ones(1, 1, SISR_MAT_TYPE);
		cv::Mat x(X_n, 1, SISR_MAT_TYPE);
		x = X.col(i);
		cv::Mat residual;
		x.copyTo(residual);
		cv::Mat gammaI; /* sub set of gamma indexed by I */
		cv::Mat alpha(X_n, 1, SISR_MAT_TYPE);
		alpha = D.t() * x;

		cv::Mat I = cv::Mat::zeros(X_n, 1, CV_32S);
		cv::Mat atom_selected = cv::Mat::zeros(D_m, 1, CV_32S);
		cv::Mat DIT;/* sub D indexed by I */

		/*main loop*/
		for (SISR_INT inner = 0; inner < K; inner++)
		{
			/*DT * residual: to find the most corretive atom*/
			cv::Mat DTxR = D.t() * residual;
			//ASSERT(DTxR.rows == 2 * residual.rows);
			
			/*find the max value and its index*/
			double minVal, maxVal;
			cv::Point minLoc, maxLoc;
			cv::minMaxLoc(cv::abs(DTxR), &minVal, &maxVal, &minLoc, &maxLoc);

			SISR_INT pos = maxLoc.y;

			SISR_DOUBLE temp_val = GetSetValueXY(SISR_DOUBLE, DTxR, pos, 0);
			if (GetSetValueXY(int, atom_selected, pos, 0) == 1)
			{
				PrtAndLog("Break out...");
				//ASSERT(0);
				break;
			}
			if (temp_val * temp_val <1e-14)
			{
				PrtAndLog("Break out because temp_val is too small...");
				//ASSERT(0);
				break;
			}

			/*update index set*/
			GetSetValueXY(SISR_INT, I, inner, 0) = pos;
			GetSetValueXY(SISR_INT, atom_selected, pos, 0) = 1;

			/*compute omega*/
			if (inner > 0)
			{
				cv::Mat w_temp = DIT * D.col(GetSetValueXY(SISR_INT, I, inner, 0));

				cv::Mat omega;
				omega.create(w_temp.size(), SISR_MAT_TYPE);
				ASSERT(omega.cols == 1);
				/* L*omega = temp */
				cv::solve(L, w_temp, omega);
#ifdef _DEBUG
				//array_print(omega);
#endif
				SISR_DOUBLE last_element = sqrt(1 - (SISR_DOUBLE)omega.dot(omega));
				
				/* update L */
				cv::Mat l_temp = omega.t();
				L.push_back(l_temp);
				L = L.t();
				cv::Mat zero_temp = cv::Mat::zeros(omega.size(), SISR_MAT_TYPE);
				zero_temp.push_back(last_element);
				l_temp = zero_temp.t();
				L.push_back(l_temp);
				L = L.t();

#ifdef _DEBUG
				//array_print(L);
#endif

			}//endif

			/* alphaI */
			cv::Mat alphaIT;
			
			for (SISR_INT j = 0; j <= inner; j++)
			{
				/* alpha is a vector */
				SISR_INT index = GetSetValueXY(SISR_INT, I, j, 0);
				alphaIT.push_back(alpha.row(index));
			}

			cv::Mat alphaI = alphaIT;
			/* compute gamma */
			cv::Mat g_temp1;
			g_temp1.create(alphaI.size(), L.type());
			/* L*g_temp1 = alphaI */
			cv::solve(L, alphaI, g_temp1);
			/* L*g_temp1 = alphaI */
			cv::solve(L.t(), g_temp1, gammaI);

			/* D : sub D indexed by I */
			SISR_INT index_temp = GetSetValueXY(SISR_INT, I, inner, 0);
			cv::Mat vec_temp = (D.col(index_temp)).t();
			DIT.push_back(vec_temp);

			residual = x - DIT.t() * gammaI;

		}// end mainloop

		// BUG: gammaI maybe shorter than K because of Break Out in comp.
		//for (SISR_INT i = 0; i < K; i++)
		for (SISR_INT iter = 0; iter < gammaI.rows; iter++)
		{
			GetSetValueXY(SISR_DOUBLE, v, GetSetValueXY(SISR_INT, I, iter, 0), 0) = GetSetValueXY(SISR_DOUBLE, gammaI, iter, 0);
		}

#if VERBOSE
		printf("%d\n",i);
		for (SISR_INT i = 0; i < K; i++)
		{
			printf("%d ", *((SISR_INT*)(I.data)+i));
		}
		printf("\n");
#endif

		array_print(gammaI);

		v = v.t();
		S.push_back(v);
	}

	/* S = ST.t() consume too much memories - will alloc new Mat??*/
	/* transpose is memory cosuming... */
	S = S.t();

	PrtAndLog("SUCCESS: leaving comp...");
	return SUCCESS;
}
