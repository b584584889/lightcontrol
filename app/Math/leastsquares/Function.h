
#ifndef FUNCTION_H
#define FUNCTION_H

/**
 * Created by ikki on 2016/06/15.
 */

class Function
{
protected:
    virtual ~Function(){};
public:
    /**
     *      Returns the functions evaluated at the specific parameter set
     * @return needs to evaluate the function
     * */
    virtual double evaluate(double * values, double * parameters) = 0;
    virtual double evaluateFixZ(double fixZ, double * parameters) = 0;
    virtual int getNParameters() = 0;
    virtual int getNInputs() = 0;

};

#endif
