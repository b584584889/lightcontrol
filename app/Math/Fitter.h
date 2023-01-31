
#ifndef FITTER_H
#define FITTER_H

/**
 * Created by ikki on 2016/06/15.
 */

class Fitter
{
protected:
    virtual ~Fitter(){};
public:

    virtual void setData(int valuecount, double ** xvalues, double * zvalues) = 0;
    virtual void setParameters(double * parameters) = 0;
    virtual double * getParameters() = 0;
    virtual double * getUncertainty() = 0;
    virtual bool fitData() = 0;
    virtual double calculateErrors() = 0;
};

#endif
