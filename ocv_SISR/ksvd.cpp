#include "stdafx.h"
#include "ksvd.h"
#include "debug.h"
#include "omp.h"

/*
*	function declarations
*/
SISR_DOUBLE compute_err(const cv::Mat& D, const cv::Mat& Gamma, const cv::Mat& X);
void reperror(const cv::Mat& D, const cv::Mat& Gamma, const cv::Mat& X, SISR_DOUBLE* err);
SISR_INT sprow(const cv::Mat& Gamma, const SISR_INT j, cv::Mat& gamma_j, cv::Mat& gamma_j_indx);
SISR_INT optimize_atom(const cv::Mat& X, const cv::Mat& D, const SISR_INT j, const cv::Mat& Gamma,
					  SISR_INT* unused_sigs, cv::Mat& replaced_atoms,
					  cv::Mat& atom, cv::Mat& gamma_j, cv::Mat& gamma_j_indx);
SISR_INT cleardict(cv::Mat& D, const cv::Mat& Gamma, const cv::Mat& X, 
				  SISR_DOUBLE muthresh, SISR_INT* unused_sigs, const cv::Mat& replaced_atoms, SISR_INT use_thresh = 4);

/*end of declarations*/

SISR_INT ksvd(cv::Mat& X, const SISR_INT dictsize, const SISR_INT K, const SISR_INT k, cv::Mat& D, cv::Mat& Gamma, cv::Mat& Errs, const cv::Mat& D0, char c_or_e)
{
	PrtAndLog("entering ksvd...");

	CheckFail( X.type() != SISR_MAT_TYPE, "currently only support SISR_DOUBLE type (Dictionary)");

	if (X.cols <= dictsize)
	{
		PrtAndLog("FAIL: no enough input signals...");
		return FAIL;
	}

	/* newly added 2013.8.9 */
	/*if (X.cols > 40000)
	{
		cv::Mat subX = X(cv::Range::all(), cv::Range(1, 40000));
		X = subX;
	}
	printf("%d", X.cols);*/

	/* validate input parameters */
	CheckFail(X.cols == 0, "Fail: X is empty");
	//CheckFail(D0.cols == 0, "Fail: D0 is empty");
	CheckFail(K == 0, "Fail: K is too small(0)");

	/* end of validation */

	/* make D by selecting signals */
	if (D0.empty())
	{
		cv::Mat XT = X.t();
		cv::Mat DT;

		for (SISR_INT i = 0, j = i * 3 ; i < dictsize; i++,j++)
		{
			if (j >= X.cols)
			{
				j = j / 2;
			}
			SISR_INT num = cv::countNonZero(XT.row(j));
			if/*while*/ (num == 0)
				j++;
			ASSERT(j < X.cols);
			DT.push_back(XT.row(j));
		}
		D = DT.t();
	}
	else
	{
		D = D0.clone();
	}

	/* init */
	SISR_INT iternum = k;
	//err = cv::Mat::zeros(1, iternum, SISR_MAT_TYPE);

	/* normalize the input dictionary */
	for (SISR_INT i = 0; i < D.cols; i++)
	{
		cv::normalize(D.col(i),D.col(i));
	}

	//cv::Mat Errs;

	/* main loop */
	for (SISR_INT iter = 0; iter < iternum; iter++)
	{
		/* this time just use comp */
		cv::Mat _Gamma;
		SISR_INT status;
		if (c_or_e == 'c')
		{
			status = comp(D,X,_Gamma,K);
		}
		else if(c_or_e == 'e')
		{
			float e = 0.001;
			status = bomp(D,X,_Gamma,K, e);
		}

		array_print(_Gamma);

		CheckFail(status == FAIL, "FAIL: when computing sparse coding");

		/* dictionary update */
		SISR_INT dictsize = D.cols;
		/* mark one atom if it is replaced by a signal */
		cv::Mat replaced_atoms = cv::Mat::zeros(1, dictsize, CV_32S);

		/* make a SISR_INT array in heap to tracks the signals that were used to replace "dead" atoms. */
		SISR_INT sigs_num = X.cols;

		//SISR_INT* unused_sigs = (SISR_INT*)malloc(sigs_num * sizeof(SISR_INT));
		SISR_INT* unused_sigs = new SISR_INT[sigs_num * sizeof(SISR_INT)];
		for (SISR_INT i = 0; i < sigs_num; i++)
		{
			/* 1 means not used */
			unused_sigs[i] = 1;
		}

		for (SISR_INT j = 0; j < dictsize; j++)
		{
			/* optimize_atom */
			cv::Mat atom, gamma_j, gamma_j_indx;
			status = optimize_atom(X, D, j, _Gamma, unused_sigs, replaced_atoms, atom, gamma_j, gamma_j_indx);
			CheckFail(status, "FAIL: when optimizing atom... ");
			/* gamma_j is a column vector */
			ASSERT(atom.cols == 1);
			//ASSERT(gamma_j.cols == 1); bug: gamma_j maybe empty
			for (SISR_INT k = 0; k < D.rows; k++)
			{
				//D.col(j)
				GetSetValueXY(SISR_DOUBLE, D, k, j) = 
					GetSetValueXY(SISR_DOUBLE, atom, k, 0);
			}
			array_print(D.col(j));

			array_print(_Gamma.row(j));
			for (SISR_INT k = 0; k < gamma_j.rows; k++)
			{
				GetSetValueXY(SISR_DOUBLE, _Gamma, j, GetSetValueXY(SISR_INT, gamma_j_indx, k, 0))
					= GetSetValueXY(SISR_DOUBLE, gamma_j, k, 0);
			}
			array_print(gamma_j);
			array_print(_Gamma.row(j));
		}
		/* end of dictionary update */

		/* compute error */
		/* print unused_sigs */
		array_print(_Gamma);
		SISR_DOUBLE err = compute_err(D,_Gamma,X);
		Errs.push_back(err);

		/* clear dictinorary - set muthresh = 0.99 */
		SISR_DOUBLE muthresh = 0.90;
		status = cleardict(D,_Gamma, X,muthresh, unused_sigs, replaced_atoms);
		CheckFail(status == FAIL, "FAIL: when clearing dictionary...");
		/* release mem */

		/* newly added 2013.7.15 */
		/* memory insufficient */
		Gamma = _Gamma/*.clone()*/;
		//Gamma = _Gamma;

		//free(unused_sigs);
		delete unused_sigs;
		/*  */
		PrtAndLog("Iteration %d / %d complete", iter, iternum);
		PrtAndLog("err: %f", err);
		printf("%f ",err);
		PrtAndLog("\n");
	}

	return SUCCESS;
}


/*
*	Get the nonzero items of j-th row and its indices
*	Input: Gamma, j
*	Output: gamma_j, gamma_j_indx (all are column vectors)
*/
SISR_INT sprow(const cv::Mat& Gamma, const SISR_INT j, cv::Mat& gamma_j, cv::Mat& gamma_j_indx)
{
	PrtAndLog("entering sprow...");
	/* validate input parameters */
	CheckFail(Gamma.empty(), "FAIL: Input Mat is empty");
	CheckFail(j < 0, "FAIL: j should be more than or equal to 0");

	cv::Mat temp_vec = Gamma.row(j);
	cv::Mat temp_gamma_j;
	cv::Mat temp_gamma_j_indx;
	ASSERT(temp_vec.cols != 0);
	ASSERT(temp_vec.type() == SISR_MAT_TYPE);
	for (SISR_INT i = 0; i < temp_vec.cols; i++)
	{
		SISR_INT to_be_pushed = i;
		SISR_DOUBLE temp = GetSetValueXY(SISR_DOUBLE, temp_vec, 0, i);
		if (temp != 0.0)
		{
			temp_gamma_j.push_back(temp);
			temp_gamma_j_indx.push_back(to_be_pushed);
		}
	}

	/* gamma_j is a column vector */
	gamma_j = temp_gamma_j;
	gamma_j_indx = temp_gamma_j_indx;

	array_print(gamma_j);
	array_print(gamma_j_indx);

	PrtAndLog("SUCCESS: leaving sprow...");
	return SUCCESS;
}

/*
*	optimize_atom
*	Input: X, D, j, Gamma
*	In/Out: unused_sigs, replaced_atoms(column vector)
*	Output: atom, gamma_j, gamma_j_indx
*/
SISR_INT optimize_atom(const cv::Mat& X, const cv::Mat& D, const SISR_INT j, const cv::Mat& Gamma,
					  SISR_INT* unused_sigs, cv::Mat& replaced_atoms,
					  cv::Mat& atom, cv::Mat& gamma_j, cv::Mat& gamma_j_indx)
{
	PrtAndLog("entering optimize_atom...");

	/* validate input parameters */
	CheckFail(X.empty(), "FAIL: X is empty");
	CheckFail(D.empty(), "FAIL: D is empty");
	CheckFail(Gamma.empty(), "FAIL: Gamma is empty");

	if (FAIL == sprow(Gamma, j, gamma_j, gamma_j_indx))
	{
		PrtAndLog("FAIL: Gamma is empty");
		return FAIL;
	}

	if (gamma_j_indx.empty())
	{
#if 1
		/* to be done */
		/* make sub mat D and X */
		cv::Mat XT = X.t();
		cv::Mat GammaT = Gamma.t();
		cv::Mat XSub, GammaSub;

#if VERBOSE
		for (int i = 0; i < X.cols; i++)
		{
			printf("%d ", unused_sigs[i]);
		}
		printf("\n");

#endif

		for (SISR_INT i = 0; i < X.cols; i++)
		{
			if (unused_sigs[i] != 0)
			{
				XSub.push_back(XT.row(i));
				GammaSub.push_back(GammaT.row(i));
			}
		}
		XSub = XSub.t();
#if VERBOSE
		for (int i = 0; i < XSub.cols; i++)
		{
			array_print(XSub.col(i));
		}
#endif
		GammaSub = GammaSub.t();

#if VERBOSE
		for (int i = 0; i < GammaSub.cols; i++)
		{
			array_print(GammaSub.col(i));
		}
#endif


		/* make sub mat D and X */

		//SISR_DOUBLE* err = (SISR_DOUBLE*)malloc(GammaSub.cols * sizeof(SISR_DOUBLE));
		SISR_DOUBLE* err = new SISR_DOUBLE[GammaSub.cols * sizeof(SISR_DOUBLE)];
		reperror(D,GammaSub,XSub,err);

#if VERBOSE
		for (int i = 0; i < GammaSub.cols; i++)
		{
			printf("%.16lf ", err[i]);
		}
		printf("\n");

#endif

		SISR_DOUBLE max_err = err[0];
		SISR_INT pos = 0;
		/* get max and pos of err */
		for (SISR_INT i = 0; i< GammaSub.cols ; i++)
		{
			if (max_err < err[i])
			{
				max_err = err[i];
				pos = i;
			}
		}
#endif

//#if 1
//		for (SISR_INT i = 0; i < X.cols; i++)
//		{
//			if (unused_sigs[i] != 0)
//			{
//				/* find a singal x as atom */
//				int num = cv::countNonZero(X.col(i));
//				if (num != 0)
//				{
//					atom = X.col(i);
//					unused_sigs[i] = 0;
//					break;
//				}
//				
//				
//			}
//		}
//
//#endif
#if 1
		atom = XSub.col(pos);
#endif
		ASSERT(atom.cols == 1);
		cv::normalize(atom,atom);

		array_print(atom);

		gamma_j = cv::Mat::zeros(gamma_j.rows, gamma_j.cols, gamma_j.type());
		/* transpose gamma_j to a column vector */
		gamma_j = gamma_j.t();
#if 1
		unused_sigs[pos] = 0;
#endif
		GetSetValueXY(SISR_INT, replaced_atoms, 0, j) = 1;

		/* release the heap alloc */
#if 1
		delete err;
#endif
		PrtAndLog("SUCCESS: update atom by using signal x");
		return SUCCESS;

	}/* end of if (gamma_j_indx.empty()) */

	cv::Mat smallGamma;
	getSubMat(Gamma, gamma_j_indx, smallGamma, gamma_j_indx.rows - 1);

	array_print(smallGamma);

	cv::Mat dj = D.col(j);
	array_print(dj);

	//atom gamma_j
	//svds(X(:,data_indices) - D*smallGamma + Dj*gamma_j, 1)
	cv::Mat smallX;
	getSubMat(X, gamma_j_indx, smallX, gamma_j_indx.rows - 1);
	array_print(smallX);

	cv::Mat svdsrc = smallX - D*smallGamma + dj*gamma_j.t();
	array_print(svdsrc);

	/* exact svd */
	if (0)
	{
		cv::Mat s, vt, u;
		cv::SVDecomp(svdsrc, s, u, vt, cv::SVD::MODIFY_A);
		atom = u.col(0);
		gamma_j = vt.row(0);
		gamma_j = s.row(0) * gamma_j;
		/* transpose gamma_j to a column vector */
		gamma_j = gamma_j.t();

		array_print(gamma_j);
		array_print(gamma_j_indx);
		array_print(atom);
	}

	/* approximate svd */
	/* newly added */
	if (1)
	{
		atom = svdsrc * gamma_j;
		cv::normalize(atom, atom);
		gamma_j = svdsrc.t() * atom;
	}


	PrtAndLog("SUCCESS: leaving optimize_atom...");
	return SUCCESS;
}

SISR_DOUBLE compute_err(const cv::Mat& D, const cv::Mat& Gamma, const cv::Mat& X)
{
	SISR_DOUBLE* err = (SISR_DOUBLE*)malloc(X.cols * sizeof(SISR_DOUBLE));

	reperror(D,Gamma,X,err);
	SISR_DOUBLE total_err = 0;
	for (SISR_INT i = 0; i < X.cols; i++)
		total_err += err[i];
	total_err = sqrt(total_err/X.total());

	free(err);
	return total_err;
}

void reperror(const cv::Mat& D, const cv::Mat& Gamma, const cv::Mat& X, SISR_DOUBLE* err)
{
	for (SISR_INT i = 0; i < X.cols; i++)
	{
		err[i] = 0;
	}

	//SISR_DOUBLE temp;
	cv::Mat mat_err;
	mat_err = X - D*Gamma;

	array_print(X);
	array_print(D);
	array_print(Gamma);
	array_print(mat_err);

	mat_err = mat_err.mul(mat_err);
	array_print(mat_err);
	for (SISR_INT i = 0; i < X.rows; i++)
	{
		for (SISR_INT j = 0; j < X.cols; j++)
		{
			err[j] += GetSetValueXY(SISR_DOUBLE, mat_err, i, j);
		}
	}

#if VERBOSE
	for (SISR_INT i = 0; i < X.cols; i++)
	{
		printf("%.16lf ", err[i]);
	}
	printf("\n");
#endif
}

/*
*	cleardict:
*	[D,cleared_atoms] = cleardict(D,Gamma,X,muthresh,unused_sigs,replaced_atoms);
*	Input:
*	In/Out:
*	Output:
*/
SISR_INT cleardict(cv::Mat& D, const cv::Mat& Gamma, const cv::Mat& X, 
				  SISR_DOUBLE muthresh, SISR_INT* unused_sigs, const cv::Mat& replaced_atoms, SISR_INT use_thresh)
{
	PrtAndLog("entering cleardict...");
	/* validate input parameters */
	CheckFail(X.empty(), "FAIL: X is empty");
	CheckFail(D.empty(), "FAIL: D is empty");
	CheckFail(Gamma.empty(), "FAIL: Gamma is empty");

	/*SISR_INT use_thresh = 4*/;
	SISR_INT dictsize = D.cols;
	SISR_INT sig_nums = X.cols;

	/* make sub mat D and X */
	cv::Mat XT = X.t();
	cv::Mat GammaT = Gamma.t();
	cv::Mat XSub, GammaSub;

	for (SISR_INT i = 0; i < sig_nums; i++)
	{
		if (unused_sigs[i] != 0)
		{
			XSub.push_back(XT.row(i));
			GammaSub.push_back(GammaT.row(i));
		}
	}
	XSub = XSub.t();
	GammaSub = GammaSub.t();
	/* make sub mat D and X */

	//SISR_DOUBLE* err = (SISR_DOUBLE*)malloc(GammaSub.cols * sizeof(SISR_DOUBLE));
	SISR_DOUBLE* err = new SISR_DOUBLE[GammaSub.cols * sizeof(SISR_DOUBLE)];
	reperror(D,GammaSub,XSub,err);

#ifdef _DEBUG
	SISR_INT cleared_atoms = 0;
#endif

	/* make usecount */
	cv::Mat is_use(Gamma.rows, Gamma.cols, CV_32S);
	for (SISR_INT i = 0; i < Gamma.rows; i++)
		for (SISR_INT j = 0; j < Gamma.cols; j++)
			GetSetValueXY(SISR_INT, is_use, i, j) = GetSetValueXY(SISR_DOUBLE, Gamma, i, j) != 0 ? 1 : 0;
	cv::Mat usecount = is_use.col(0);
	for (SISR_INT i = 1; i < is_use.cols; i++)
		usecount = usecount + is_use.col(i);

	array_print(usecount);

	for (SISR_INT i = 0; i < dictsize; i++)
	{
		/* compute the correlation of atoms */
		/* Gj is a column vector */
		cv::Mat Gj = D.t()*D.col(i);
		ASSERT(Gj.cols == 1);
		GetSetValueXY(SISR_DOUBLE, Gj, i, 0) = 0;

		/* replace atom */
		/*find the max value and its index*/
		double minVal, maxVal;
		cv::Point minLoc, maxLoc;
		cv::minMaxLoc(cv::abs(Gj), &minVal, &maxVal, &minLoc, &maxLoc);
		SISR_INT pos = maxLoc.y;

		ASSERT(replaced_atoms.type() == CV_32S);
		ASSERT(replaced_atoms.rows == 1);
		if (minVal*minVal > muthresh*muthresh ||
			(GetSetValueXY(SISR_INT, replaced_atoms, 0, i) == 0 && GetSetValueXY(SISR_INT, usecount, i, 0) < use_thresh))
		{
			SISR_DOUBLE max_err = err[0];
			SISR_INT pos = 0;
			/* get max and pos of err */
			for (SISR_INT j = 0; j < GammaSub.cols ; j++)
			{
				if (max_err < err[j])
				{
					max_err = err[j];
					pos = j;
				}
			}
			cv::Mat d = XSub.col(pos);
			ASSERT(d.cols == 1);
			cv::normalize(d,d);
			/* update atom */
			for (SISR_INT k = 0; k < d.rows; k++)
			{
				//D.col(i)
				GetSetValueXY(SISR_DOUBLE, D, k, i) = 
					GetSetValueXY(SISR_DOUBLE, d, k, 0);
			}
			/* unused_sigs[i] = 0 means sigs[i] is used */
			unused_sigs[i] = 0;
#ifdef _DEBUG
			cleared_atoms ++;
#endif
		}
	}

	//
	delete(err);
	//free(err);
	//ASSERT(err == NULL);

	PrtAndLog("SUCCESS: leaving optimize_atom...");
	return SUCCESS;
}