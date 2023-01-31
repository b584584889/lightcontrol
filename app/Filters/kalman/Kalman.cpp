
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "string.h"
#include "Kalman.h"
#include "../../common.h"
/**
  * The construstor allocates JKalman filter and all its matrices and
  * initializes them somehow.
  * @param dynam_params
  * @param measure_params
  * @param control_params
  *
  */
Kalman::Kalman(int dynam_params, int measure_params, int control_params)
{
	//printf ("Kalman::Kalman() dynam_params = %d, measure_params = %d, control_params = %d\n", dynam_params, measure_params, control_params);
	if( control_params < 0 )
	{
	    control_params = dynam_params;
	}

	// init
	dp = dynam_params;
	mp = measure_params;
	cp = control_params;

	//state_pre = new Matrix(dp, 1, 0.0); // init by transition _matrix*state_post
    state_pre = NULL;

	// following variables must be initialized properly in advance
	state_post = new Matrix(dp, 1, 0.0); // init by the first measurement!!!
	transition_matrix = Matrix::identity(dp, dp);

	//process_noise_cov = Matrix::identity(dp, dp, 1e-3); // 1e-5 (1 in OpenCV)
    //process_noise_cov = Matrix::identity(dp, dp, 1e-5); // 1e-5 VILS
    process_noise_cov = Matrix::identity(dp, dp, 1e-4); // 1e-4 VILS orig
    //process_noise_cov = Matrix::identity(dp, dp, 1e-3); // 1e-3. more smooth
    //process_noise_cov = Matrix::identity(dp, dp, 1e-2); // 1e-2. less jump position
    //process_noise_cov = Matrix::identity(dp, dp, 1e-1); // 1e-1. more jump position

	measurement_matrix = Matrix::identity(mp, dp); // 1 (0 in OpenCV)
	measurement_noise_cov = Matrix::identity(mp, mp, 1e-1); // 1e-1 (1 in OpenCV), VILS orig
    //measurement_noise_cov = Matrix::identity(mp, mp, 1e-2); // 1e-2
    //measurement_noise_cov = Matrix::identity(mp, mp, 1e-3); // 1e-3

	//error_cov_pre = new Matrix(dp, dp, 0.0); // initialized in Predict
    error_cov_pre = NULL; // initialized in Predict
    error_cov_post = Matrix::identity(dp, dp); // 1 (0 in OpenCV)

	//gain = new Matrix(dp, mp, 0.0);
    gain = NULL;

	if( cp > 0 )
	{
	    control_matrix = new Matrix(dp, cp, 0.0);
	}
	else
	{ // TODO possibly an error in OpenCV
	    control_matrix = NULL;
	}

	//temp1 = new Matrix(dp, dp, 0.0);
	//temp2 = new Matrix(mp, dp, 0.0);
	//temp3 = new Matrix(mp, mp, 0.0);
	//temp4 = new Matrix(mp, dp, 0.0);
	//temp5 = new Matrix(mp, 1, 0.0);

    temp1 = NULL;
    temp2 = NULL;
    temp3 = NULL;
    temp4 = NULL;
    temp5 = NULL;
}

/**
* Constructor in case of no control.
* @param dynam_params
* @param measure_params
*/

Kalman::Kalman(int dynam_params, int measure_params)
{
	//printf ("Kalman::Kalman() dynam_params = %d, measure_params = %d\n", dynam_params, measure_params);
	Kalman(dynam_params, measure_params, 0);
}

Kalman::~Kalman()
{
    if (state_pre != NULL)
        delete state_pre;
    if (state_post != NULL)
        delete state_post;
    if (transition_matrix != NULL)
        delete transition_matrix;
    if (control_matrix != NULL)
        delete control_matrix;
    delete measurement_matrix;
    delete process_noise_cov;
    delete measurement_noise_cov;
    if (error_cov_pre != NULL)
        delete error_cov_pre;
    if (gain != NULL)
        delete gain;
    if (error_cov_post != NULL)
        delete error_cov_post;
    if (temp1 != NULL)
        delete temp1;
    if (temp2 != NULL)
        delete temp2;
    if (temp3 != NULL)
        delete temp3;
    if (temp4 != NULL)
        delete temp4;
    if (temp5 != NULL)
        delete temp5;
}

/**
 * Alias for prediction with no control.
 * @return Predict(no control).
 */
Matrix * Kalman::Predict()
{
    return Predict(NULL);
}

/**
* Estimates subsequent model state.
* <p> The function estimates the subsequent
* stochastic model state by its current state and stores it at
* <code>state_pre</code>:
* <pre>
* x'<sub>k</sub>=A*x<sub>k</sub>+B*u<sub>k</sub>
* P'<sub>k</sub>=A*P<sub>k-1</sub>*A<sup>T</sup> + Q,
* where
* x'<sub>k</sub> is predicted state (state_pre),
* x<sub>k-1</sub> is corrected state on the previous step (state_post)
*     (should be initialized somehow in the beginning, zero vector by default),
* u<sub>k</sub> is external control (<code>control</code> parameter),
* P'<sub>k</sub> is prior error covariance matrix (error_cov_pre)
* P<sub>k-1</sub> is posteriori error covariance matrix on the previous step (error_cov_post)
*     (should be initialized somehow in the beginning, identity matrix by default),
* </pre>
* @param control Control vector (u<sub>k</sub>), should be NULL if there
* is no external control (<code>control_params</code>=0).
* @return The function returns the estimated state.
*/
Matrix * Kalman::Predict(Matrix * control)
{
	//printf ("Kalman::Predict()\n");

   // (1) Project the state ahead
   // update the state: x'(k) = A*x(k)
    if (state_pre != NULL)
        delete state_pre;
   state_pre = transition_matrix->times(state_post);
   if( control != NULL && cp > 0 )
   {
        // x'(k) = x'(k) + B*u(k)
       state_pre = control_matrix->gemm(control, state_pre, 1, 1);
   }

   // (2) Project the error covariance ahead
   // update error covariance matrices: temp1 = A*P(k)
    if (temp1 != NULL)
        delete temp1;
    temp1 = transition_matrix->times(error_cov_post);
   // P'(k) = temp1*At + Q
    if (error_cov_pre != NULL)
        delete error_cov_pre;
   Matrix * transpose = transition_matrix->transpose();
   error_cov_pre = temp1->gemm(transpose, process_noise_cov, 1, 1);

   delete transpose;

    if (temp1 != NULL)
    {
        delete temp1;
        temp1 = NULL;
    }

    return state_pre;
}

/**
* Adjusts model state.
* The function <code>KalmanCorrect</code> adjusts stochastic model state
* on the basis of the given measurement of the model state:</p>
* <pre>
* K<sub>k</sub>=P'<sub>k</sub>*H<sup>T</sup>*(H*P'<sub>k</sub>*H<sup>T</sup>+R)<sup>-1</sup>
* x<sub>k</sub>=x'<sub>k</sub>+K<sub>k</sub>*(z<sub>k</sub>-H*x'<sub>k</sub>)
* P<sub>k</sub>=(I-K<sub>k</sub>*H)*P'<sub>k</sub>
* where
* z<sub>k</sub> - given measurement (<code>mesurement</code> parameter)
* K<sub>k</sub> - JKalman "gain" matrix.
* </pre>
* <p>The function stores adjusted state at <code>state_post</code> and
* returns it on output.
* @param measurement Matrix containing the measurement vector.
* @return
*/
Matrix * Kalman::Correct(Matrix * measurement)
{
	//printf ("Kalman::Correct() (1) Compute the Kalman gain\n");

   // (1) Compute the Kalman gain
   // temp2 = H*P'(k)
    if (temp2 != NULL)
        delete temp2;
   temp2 = measurement_matrix->times(error_cov_pre);

   // temp3 = temp2*Ht + R
    if (temp3 != NULL)
        delete temp3;
   Matrix * transpose = measurement_matrix->transpose();
   temp3 = temp2->gemm(transpose, measurement_noise_cov, 1, 1);
   delete transpose;

	//printf ("Kalman::Correct() temp4 = inv(temp3)*temp2 = Kt(k) \n");
   // temp4 = inv(temp3)*temp2 = Kt(k)
    if (temp4 != NULL)
        delete temp4;
   temp4 = temp3->solve(temp2);

   if ( temp4 == NULL )
   {
       vilslog::printf ("Kalman::Correct() temp4 == NULL \n");
       return NULL;
   }

   // hokus pokus...
   // temp4 = temp3.svd().getU().times(temp2);

	//printf ("Kalman::Correct() K(k)  \n");
    // K(k)
    if (gain != NULL)
        delete gain;
    gain = temp4->transpose();

	//printf ("Kalman::Correct() (2) Update estimate with measurement z(k)\n");
   // (2) Update estimate with measurement z(k)
   // temp5 = z(k) - H*x'(k)
    if (temp5 != NULL)
        delete temp5;
   temp5 = measurement_matrix->gemm(state_pre, measurement, -1, 1);

   // x(k) = x'(k) + K(k)*temp5
    if (state_post != NULL)
        delete state_post;
   state_post = gain->gemm(temp5, state_pre, 1, 1);

	//printf ("Kalman::Correct() (3) Update the error covariance.\n");
   // (3) Update the error covariance.
   // P(k) = P'(k) - K(k)*temp2
    if (error_cov_post != NULL)
        delete error_cov_post;
   error_cov_post = gain->gemm(temp2, error_cov_pre, -1, 1);

    if (temp2 != NULL)
    {
        delete temp2;
        temp2 = NULL;
    }
    if (temp3 != NULL)
    {
        delete temp3;
        temp3 = NULL;
    }
    if (temp4 != NULL)
    {
        delete temp4;
        temp4 = NULL;
    }
    if (temp5 != NULL)
    {
        delete temp5;
        temp5 = NULL;
    }
    if (gain != NULL)
    {
        delete gain;
        gain = NULL;
    }
    return state_post;
}

/**
 * Setter
 * @param transition_matrix
 */
void Kalman::setTransition_matrix(Matrix * transition_matrix)
{
	//printf ("Kalman::setTransition_matrix() this->mp = %d, this->dp = %d, this->cp = %d\n", this->mp, this->dp, this->cp);
    if (this->transition_matrix != NULL)
        delete this->transition_matrix;
    this->transition_matrix = transition_matrix;
}

/**
* Setter
* @param error_cov_post
*/
void Kalman::setError_cov_post(Matrix * error_cov_post)
{
    if (this->error_cov_post != NULL)
        delete this->error_cov_post;
    this->error_cov_post = error_cov_post;
}

/**
* Getter
* @return
*/
Matrix * Kalman::getError_cov_post()
{
	//printf ("Kalman::getError_cov_post() this->mp = %d, this->dp = %d\n", this->mp, this->dp);

   return this->error_cov_post;
}
