
#ifndef NonLinearSolver_H
#define NonLinearSolver_H

#include "Fitter.h"
#include "Function.h"

#define MAX_Z_LENGTH 10
#define MAX_A_LENGTH 4

/**
 * Created by ikki on 2016/06/15.
 */

class NonLinearSolver : public Fitter
{
public:
	NonLinearSolver(Function * function);
	~NonLinearSolver();


    void setData(int valuecount, double ** xvalues, double * zvalues);
    void setParameters(double * parameters);
    double * getParameters();
    double * getUncertainty();
    bool fitData();
    double calculateErrors();

	void setStepSize(double step);
	void setMinError(double error);
	void setMinChange(double change);
    void setFixZ(int index, double zValue);
private:

    /**
     * Ignores second derivatives, not very good.
     * for solving non-linear least squares fit of f(x:A) = z with sets of x,z data points.
     * */
    double **  X;    //values
    double *   A;    //Parameter Set
    double *   Z;    //output vaues

    int A_Length;
    int Z_Length;

	Function * mFunction;
    double * ERROR;
    double ** DERIVATIVES;

    double DELTA;   //used for calculating derivatives
    double MIN_ERROR; //for evaluating
    double MIN_CHANGE;    //minimumchanges
    double STEP;
    double MAX_ITERATIONS;

    int mFixIndex;
    double mFixZ;

	double * m_abefore;
	double * m_aafter;
	double * m_right;
	double ** m_matrix;

	void initializeWorkspace();
    void clearWorkspace();
	void releaseWorkspace();
	void calculateDerivatives();
	double iterateValues();
};

#endif
