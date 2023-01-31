
#ifndef ResidualsFunction_H
#define ResidualsFunction_H

#include "Function.h"

/**
 * Created by ikki on 2016/06/15.
 */

class ResidualsFunction : public Function
{
public:
	ResidualsFunction();
	~ResidualsFunction();


    /**
     *      Returns the functions evaluated at the specific parameter set
     * @return needs to evaluate the function
     * */
    double evaluate(double * values, double * parameters);
    double evaluateFixZ(double fixZ, double * parameters);
    int getNParameters();
    int getNInputs();
};


#endif
