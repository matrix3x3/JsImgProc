#include "stdafx.h"
#include "omp.h"
#include "debug.h"
#include <cmath>

//extern void array_print(const cv::Mat omega);

/*
*	compute M(I) - sub matrice indexed by I 
*	if 'col_or_row' == 'c', the sub matrice consists of the columns of M;
*	otherwise, consists of the rows of M
*	
*	only handle column vector here, make a sub column vector.
*	I is the index set, it is also a column vector
*/
/* to be done --- remove i from parameter list */
void getSubMat(const cv::Mat& In, const cv::Mat I, cv::Mat& Out, const SISR_INT i, const char col_or_row)
{
	ASSERT(I.cols == 1);
	ASSERT(Out.cols == 0 && Out.rows == 0);

	ASSERT(i <= I.rows);

	if (In.cols == 1)
	{
		SISR_INT index;
		for (SISR_INT j = 0; j <= i; j++)
		{
			index = GetSetValueXY(SISR_INT, I, j, 0);
			Out.push_back(In.row(index));
		}
	}
	else
	{
		SISR_INT index;
		cv::Mat vec_temp;
		for (SISR_INT j = 0; j <= i; j++)
			{
				index = GetSetValueXY(SISR_INT, I, j, 0);
				vec_temp = (col_or_row == 'r') ? In.row(index) : (In.col(index)).t();/*(In.col(index)).t();*/
				Out.push_back(vec_temp);
			}
		Out = (col_or_row == 'r') ? Out : Out.t();
	}

#ifdef _DEBUG
	//array_print(Out);
#endif
}

/*******************************************
* Author: Jethro
* Date: 
* Description:
*	BOMP: Batch OMP -- Error Constrained OMP
*	Input: D - dictionary, X - signals, K -
*		target sparsity, e - error costrain
*	Output: S - sparse coding of X
*******************************************/
SISR_INT bomp(const cv::Mat& D, const cv::Mat& X, cv::Mat& S, SISR_INT K, float e)
{
	PrtAndLog("entering bomp...");
	
	//ASSERT(D.type() == SISR_MAT_TYPE && X.type() == SISR_MAT_TYPE);
	if (D.type() != SISR_MAT_TYPE || X.type() != SISR_MAT_TYPE)
	{
		PrtAndLog("FAIL: D and X should be type of SISR_MAT_TYPE.(currently only support this type)");
		ASSERT(0);
		return FAIL;
	}
	// Transpose of S
	cv::Mat ST;

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

	/* memory alloc */
	cv::Mat G = D.t()*D;
	/* need to release after every iteration */
	cv::Mat v;
	cv::Mat x;
	cv::Mat alpha_0;
	cv::Mat L;
	cv::Mat I;
	cv::Mat atom_selected;
	cv::Mat gammaI; /* sub set of gamma indexed by I */
	cv::Mat alpha;

	for (SISR_INT i = 0; i < X_m; i++)
	{
		v = cv::Mat::zeros(D_m, 1, SISR_MAT_TYPE);
		/* initial helper data structures */
		x = X.col(i);
		alpha_0 = D.t() * x;
		SISR_DOUBLE epsilon_0 = (SISR_DOUBLE)x.dot(x);
		/* cv::Mat G = D.t()*D; */
		L = cv::Mat::ones(1, 1, SISR_MAT_TYPE);

		/* Init */
		I = cv::Mat::zeros(X_n, 1, CV_32S);
		atom_selected = cv::Mat::zeros(D_m, 1, CV_32S);
		SISR_DOUBLE delta = 0, delta_prev = 0;

		/* Bug: alpha and alpha_0 have same address, alpha_0 will be changed when changing alpha. alpha should be a new copy of alpha_0 */
		//cv::Mat alpha = alpha_0;
		alpha = alpha_0.clone();
		SISR_DOUBLE epsilon = epsilon_0, epsilon_prev = epsilon_0;

		/* main loop */
		SISR_INT n = 0;
		while ((epsilon > e) && (n < K))
		{
			/* find the most corelative atom */
			double minVal, maxVal;
			cv::Point minLoc, maxLoc;
			minMaxLoc(abs(alpha), &minVal, &maxVal, &minLoc, &maxLoc);
			SISR_INT pos = maxLoc.y;

			SISR_DOUBLE temp_val = GetSetValueXY(SISR_DOUBLE, alpha, pos, 0);
			if (GetSetValueXY(SISR_INT, atom_selected, pos, 0) == 1)
			{
				//ASSERT(0);
				PrtAndLog("break out because atom is already selected...");
				break;
			}
			else if(temp_val * temp_val <1e-14)
			{
				PrtAndLog("break out because temp_val is too small...");
				break;
			}

			/*compute w*/
			if (n > 0)
			{
				cv::Mat G_Sub;/* G(I,k) */
				
				getSubMat(G, I, G_Sub, n - 1, 'r');
				G_Sub = G_Sub.col(pos);
				cv::Mat omega;
				/* Lw = Gsub */
				solve(L, G_Sub, omega);
				SISR_DOUBLE last_element = sqrt(1 - (SISR_DOUBLE)omega.dot(omega));
				if ( last_element <= 1e-14 )
					break;

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

			/*update index set*/
			GetSetValueXY(SISR_INT, I, n, 0) = pos;
			GetSetValueXY(SISR_INT, atom_selected, pos, 0) = 1;

			/*compute gamma*/
			/* alphaI */
			cv::Mat alpha_0_I;
			getSubMat(alpha_0, I, alpha_0_I, n);

			/* compute gamma */
			cv::Mat g_temp1;
			/* L*g_temp1 = alphaI */
			solve(L, alpha_0_I, g_temp1);
			/* L*g_temp1 = alphaI */
			solve(L.t(), g_temp1, gammaI);

			/* compute beta */
			/* Gsub: G(I) */
			cv::Mat G_Sub;
			getSubMat(G, I, G_Sub, n, 'c');

			/* beta = Gsub * gamma , beta is a column vector */
			cv::Mat beta = G_Sub * gammaI;

			/* update alpha */
			alpha = alpha_0 - beta;

			/* update delta and delta_prev */
			delta_prev = delta;
			/* betaI: sub beta indexed by I */
			cv::Mat betaI;
			getSubMat(beta, I, betaI, n);

			delta = (SISR_DOUBLE)(gammaI).dot(betaI);

			/* update epsilon*/
			epsilon_prev = epsilon;
			epsilon = epsilon_prev - delta + delta_prev;

			n = n + 1;
		}//end while
		// bug
		//for (SISR_INT i = 0; i < K; i++)
		for (SISR_INT i = 0; i < gammaI.rows; i++)
		{
			GetSetValueXY(SISR_DOUBLE, v, GetSetValueXY(SISR_INT, I, i, 0), 0) = GetSetValueXY(SISR_DOUBLE, gammaI, i, 0);
		}
#if 0
		for (SISR_INT i = 0; i < K; i++)
		{
			printf("%d ", *((SISR_INT*)(I.data)+i));
		}
		printf("\n");
#endif
		array_print(gammaI);

		v = v.t();
		ST.push_back(v);

		v.release();
		x.release();
		alpha_0.release();
		L.release();
		I.release();
		atom_selected.release();
		gammaI.release(); /* sub set of gamma indexed by I */
		alpha.release();
	}
	S = ST.t();

	PrtAndLog("SUCCESS: leaving bomp...");
	return SUCCESS;
}