

/**
 * Created by ikki on 2016/06/15.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "../Math/Matrix.h"
#include "NonLinearSolver.h"
#include "../../common.h"

NonLinearSolver::NonLinearSolver(Function * function)
{
	mFunction = function;
//	DELTA = 0.000001;   //used for calculating derivatives
//	MIN_ERROR = 1e-6; //for evaluating
//	MIN_CHANGE = 1e-6;    //minimumchanges
//	STEP = 0.1;
//	MAX_ITERATIONS = 500;

	//DELTA = 0.01;   //used for calculating derivatives
	//MIN_ERROR = 0.01; //for evaluating
    //MIN_CHANGE = 0.01;    //minimumchanges
    //STEP = 0.1;
    DELTA = 0.01;   //used for calculating derivatives
	MIN_ERROR = 0.01; //for evaluating
	MIN_CHANGE = 0.01;    //minimumchanges
	STEP = 0.1;
	MAX_ITERATIONS = 1500;

    mFixIndex = -1;
    mFixZ = 0.0;


	A_Length = mFunction->getNParameters();
	A = new double[A_Length];

	m_abefore = new double[A_Length];
	m_aafter = new double[A_Length];

	m_matrix = new double*[A_Length];
	for(int j=0;j<A_Length;j++)
		m_matrix[j] = new double[A_Length];
	m_right = new double[A_Length];

    initializeWorkspace();

}

NonLinearSolver::~NonLinearSolver()
{
	delete [] A;

	delete [] m_abefore;
	delete [] m_aafter;

	for(int i = 0; i < A_Length; ++i)
		delete [] m_matrix[i];
	delete [] m_matrix;
	delete [] m_right;

    releaseWorkspace();
}

void NonLinearSolver::setData(int valuecount, double ** xvalues, double * zvalues)
{
    Z_Length = valuecount;
	X = xvalues;
	Z = zvalues;
}

void NonLinearSolver::setParameters(double * parameters)
{
	for(int i=0;i<A_Length;i++)
	{
		A[i] = parameters[i];
    	//printf("NonLinearSolver::setParameters() A[%d] = %f\n", i, A[i]);
	}
}

double * NonLinearSolver::getParameters()
{
	return A;
}

double * NonLinearSolver::getUncertainty()
{
	return new double[0];
}

bool NonLinearSolver::fitData()
{
	bool ret = true;

	clearWorkspace();

	int i;
	double changes = 0.0;
	double last_error = DBL_MAX;

    //printf("NonLinearSolver::fitData() last_error = %f\n", last_error);

	for(i = 0; i<MAX_ITERATIONS; i++)
	{
	    double e = calculateErrors();

    	//printf("NonLinearSolver::fitData() e = %f, changes = %f\n", e, changes);

	    if(e < MIN_ERROR)
	    {
	        //printf("NonLinearSolver::fitData() e(%f) < MIN_ERROR(%f) \n", e, MIN_ERROR);
            ret = true;
            break;
	    }
	    if(e > last_error)
	    {
	        //printf("NonLinearSolver::fitData() Error increased: consider smaller step size.\n");
	        ret = false;
	        break;
	    }
	    last_error = e;
	    calculateDerivatives();
		changes = iterateValues();
        if (changes == -1)
        {
            //printf("\n\nNonLinearSolver::fitData() ERROR iterateValues() changes == -1 \n\n\n");
            return false;
        }


		if(changes < MIN_CHANGE)
		{
	        //printf("NonLinearSolver::fitData() changes(%f) < MIN_CHANGE(%f) \n", changes, MIN_CHANGE);
            //ret = false;
            ret = true;
            break;
		}
	}
	if(i==MAX_ITERATIONS)
	{
	    vilslog::printf("NonLinearSolver::fitData() Warning: Maximum iteration reached.\n");
	}

	return ret;
}

double NonLinearSolver::calculateErrors()
{
	double v = 0.0;
	double new_error = 0.0;

    //printf("NonLinearSolver::calculateErrors() Z_Length = %d, v = %f\n", Z_Length, v);

	for(int i = 0; i<Z_Length; i++)
	{
        //printf("NonLinearSolver::evaluate() X[i][%f,%f,%f] A[%f,%f,%f]\n", X[i][0], X[i][1], X[i][2], A[0], A[1], A[2]);

        if (i == mFixIndex)
        {
            v = mFunction->evaluateFixZ(mFixZ, A);
            ERROR[i] = v;
        }
        else
        {
            v = mFunction->evaluate(X[i], A);
            ERROR[i] = Z[i] - v;
		}
		new_error += pow(ERROR[i], 2.0);

   		//printf("NonLinearSolver::calculateErrors() i = %d, v = %f\n", i, v);
   		//printf("NonLinearSolver::calculateErrors() i = %d, ERROR[i] = %f\n", i, ERROR[i]);
   		//printf("NonLinearSolver::calculateErrors() i = %d, new_error = %f\n", i, new_error);
	}
   	//printf("NonLinearSolver::calculateErrors() new_error = %f\n", new_error);
	return new_error;
}

void NonLinearSolver::setStepSize(double step)
{
	STEP = step;
}

void NonLinearSolver::setMinError(double error)
{
	MIN_ERROR = error;
}

/**
 * When the parameters change less than this the program will stop iterating
 *
 * @param change
 */
void NonLinearSolver::setMinChange(double change)
{
	MIN_CHANGE = change;
}

void NonLinearSolver::setFixZ(int index, double zValue)
{
    mFixIndex = index;
    mFixZ = zValue;
}

void NonLinearSolver::initializeWorkspace()
{
	ERROR = new double[MAX_Z_LENGTH];
	DERIVATIVES = new double*[MAX_Z_LENGTH];
	for(int i=0;i<MAX_Z_LENGTH;i++)
		DERIVATIVES[i] = new double[MAX_A_LENGTH];
}

void NonLinearSolver::clearWorkspace()
{
    for(int i = 0; i < MAX_Z_LENGTH; ++i)
    {
        ERROR[i] = 0.0;
        for(int j = 0; j < MAX_A_LENGTH; ++j)
        {
            DERIVATIVES[i][j] = 0.0;
        }
    }
}

void NonLinearSolver::releaseWorkspace()
{
	delete [] ERROR;
	for(int i = 0; i < MAX_Z_LENGTH; ++i)
    	delete [] DERIVATIVES[i];
	delete [] DERIVATIVES;
}

void NonLinearSolver::calculateDerivatives()
{
	for(int i = 0; i<A_Length; i++)
	{
		m_abefore[i] = A[i];
		m_aafter[i] = A[i];
	}

	for(int j = 0; j<A_Length; j++)
	{
	    m_aafter[j] += DELTA;
	    m_abefore[j] -= DELTA;

		//printf("NonLinearSolver::calculateDerivatives() m_aafter[%d] = %f, m_abefore[j] = %f\n", j, m_aafter[j], m_abefore[j]);

	    if(j>0)
	    {
	        m_aafter[j-1] = A[j-1];
	        m_abefore[j-1] = A[j-1];
	    }

	    for(int i = 0; i<Z_Length; i++)
	    {
	    	//double v1 = mFunction->evaluate(X[i], m_aafter);
	    	//double v2 = mFunction->evaluate(X[i], m_abefore);
	        //double v3 = (2.0*DELTA);
			//printf("i= %d, j = %d, v1 = %f, v2 = %f, v3 = %f\n", i, j, v1, v2, v3);
            if (i == mFixIndex)
            {
                DERIVATIVES[i][j] = (mFunction->evaluateFixZ(mFixZ, m_aafter) - mFunction->evaluateFixZ(mFixZ, m_abefore))/(2.0*DELTA);
                if ( j == A_Length-1)
                {
                    DERIVATIVES[i][j] = 0.0;
                }
            }
            else
            {
                DERIVATIVES[i][j] = (mFunction->evaluate(X[i], m_aafter) - mFunction->evaluate(X[i], m_abefore))/(2.0*DELTA);
            }
			//printf("i= %d, j = %d, DERIVATIVES[i][j] = %f\n", i, j, DERIVATIVES[i][j]);
	    }
	}
}

/**
*  Takes the current error, and the current parameter set and calculates the
*  changes, then returns the maximum changed value
* */
double NonLinearSolver::iterateValues()
{
	//reset values
	for(int i = 0; i<A_Length; i++)
	{
		m_right[i] = 0.0;
		for(int j = 0; j<A_Length; j++)
		{
			m_matrix[i][j] = 0.0;
		}
	}

	for(int i = 0; i<A_Length; i++)
	{
		for(int k = 0; k<Z_Length;k++)
		{
			m_right[i] += ERROR[k] * DERIVATIVES[k][i];
			//printf("NonLinearSolver::iterateValues() DERIVATIVES[k][i] = %f\n", DERIVATIVES[k][i]);
			//printf("NonLinearSolver::iterateValues() m_right[%d] = %f, ERROR[k] = %f\n", i, m_right[i], ERROR[k]);

			for(int l=0; l<A_Length; l++)
			{
			    m_matrix[i][l] += DERIVATIVES[k][i] * DERIVATIVES[k][l];
				//printf("NonLinearSolver::iterateValues()  m_matrix[%d][%d] = %f\n", i, l, m_matrix[i][l]);
			}
		}
	}

	Matrix * coeff = new Matrix(m_matrix, A_Length, A_Length);
	Matrix * sols = new Matrix(m_right, A_Length);

	LUDecomposition * adecom = coeff->lu();

	Matrix * out = adecom->solve(sols);
    if (out == NULL)
    {
        delete coeff;
    	delete sols;
        delete adecom;
        return -1;
    }
	//double ** values = out->getArray();
    AArray * values = out->getArray();

	double max_change = fabs(values->A[0][0]);

	//printf("NonLinearSolver::iterateValues() before max_change = %f, values[0][0] = %f\n", max_change, values[0][0]);

	for(int i = 0; i<A_Length; i++)
	{
        if (mFixIndex >= 0 && i == A_Length-1)
        {
            A[i] = mFixZ;
        }
        else
        {
            double absvalue = fabs(values->A[i][0]);
            max_change = max_change > absvalue ? max_change : absvalue;
            A[i] += values->A[i][0]*STEP;
        }
		//printf("NonLinearSolver::iterateValues() A[%d] = %f, values[i][0] = %f\n", i, A[i], values[i][0]);
		//printf("NonLinearSolver::iterateValues() A[%d] = %f\n", i, A[i]);
	}

	//printf("NonLinearSolver::iterateValues() after max_change = %f\n", max_change);

	delete coeff;
	delete sols;
    delete adecom;
    delete out;

	return max_change;
}
