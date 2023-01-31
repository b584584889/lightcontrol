
#ifndef FILTER_H
#define FILTER_H

/**
 * Created by ikki on 2018/07/30.
 */

class Filter
{
protected:
public:

    virtual ~Filter(){};
    virtual int initial(double * values) = 0;
    virtual int update(double * values, double * results) = 0;
};

#endif
