

/**
 * Created by ikki on 2016/06/15.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "ResidualsFunction.h"

ResidualsFunction::ResidualsFunction()
{
}

ResidualsFunction::~ResidualsFunction()
{
}

double ResidualsFunction::evaluate(double * values, double * parameters)
{
	return sqrt(pow(values[0] - parameters[0], 2) + pow(values[1] - parameters[1], 2) + pow(values[2] - parameters[2], 2));
}

double ResidualsFunction::evaluateFixZ(double fixZ, double * parameters)
{
    return sqrt(pow(fixZ - parameters[2], 2));
}

int ResidualsFunction::getNParameters()
{
	return 3;
}

int ResidualsFunction::getNInputs()
{
	return 3;
}
