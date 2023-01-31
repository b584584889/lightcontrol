

#ifndef KALMAN_H
#define KALMAN_H

#include "../../Math/Matrix.h"

/**
 * Created by ikki on 2016/06/06. Modified from JKalman.java.
 */
/**
 * Kalman filter (state).
 * <p>
 * The structure <code>JKalman</code> is used to keep
 * Kalman filter state. It is created by constructor function, updated by
 * <code>Predict</code> and <code>Correct</code> functions.
 * <p>
 * Normally, the structure is used for standard JKalman filter
 * (notation and the formulae below are borrowed from the JKalman
 * tutorial <a href="http://www.cs.unc.edu/~welch/kalman/">[Welch95]</a>):
 * <pre>
 * x<sub>k</sub>=A*x<sub>k-1</sub>+B*u<sub>k</sub>+w<sub>k</sub>
 * z<sub>k</sub>=Hx<sub>k</sub>+v<sub>k</sub>,
 * </pre>
 * <p>where:
 * <pre>
 * x<sub>k</sub> (x<sub>k-1</sub>) - state of the system at the moment k (k-1)
 * z<sub>k</sub> - measurement of the system state at the moment k
 * u<sub>k</sub> - external control applied at the moment k
 * w<sub>k</sub> and v<sub>k</sub> are normally-distributed process and measurement noise, respectively:
 * p(w) ~ N(0,Q)
 * p(v) ~ N(0,R),
 * that is,
 * Q - process noise covariance matrix, constant or variable,
 * R - measurement noise covariance matrix, constant or variable
 * </pre>
 * <p>
 * In case of standard JKalman filter, all the matrices: A, B, H, Q and R
 * are initialized once after JKalman structure is allocated via constructor.
 * However, the same structure and the same functions may be used to simulate
 * extended JKalman filter by linearizing extended JKalman filter equation in the
 * current system state neighborhood, in this case A, B, H (and, probably,
 * Q and R) should be updated on every step.
 */

class Kalman
{
private:

    /** number of measurement vector dimensions */
    int mp;
    /** number of state vector dimensions */
    int dp;
    /** number of control vector dimensions */
    int cp;

    /** predicted state (x'(k)): x(k)=A*x(k-1)+B*u(k) */
    Matrix * state_pre;
    /** corrected state (x(k)): x(k)=x'(k)+K(k)*(z(k)-H*x'(k)) */
    Matrix * state_post;
    /** state transition matrix (A) */
    Matrix * transition_matrix;
    /** control matrix (B) (it is not used if there is no control)*/
    Matrix * control_matrix;
    /** measurement matrix (H) */
    Matrix * measurement_matrix;
    /** process noise covariance matrix (Q) */
    Matrix * process_noise_cov;
    /** measurement noise covariance matrix (R) */
    Matrix * measurement_noise_cov;
    /** priori error estimate covariance matrix (P'(k)): P'(k)=A*P(k-1)*At + Q) */
    Matrix * error_cov_pre;
    /** Kalman gain matrix (K(k)): K(k)=P'(k)*Ht*inv(H*P'(k)*Ht+R) */
    Matrix * gain;
    /** posteriori error estimate covariance matrix (P(k)): P(k)=(I-K(k)*H)*P'(k) */
    Matrix * error_cov_post;

    /** temporary matrices */
    Matrix * temp1;
    Matrix * temp2;
    Matrix * temp3;
    Matrix * temp4;
    Matrix * temp5;

public:
    Kalman(int dynam_params, int measure_params, int control_params);
    Kalman(int dynam_params, int measure_params);
    ~Kalman();

    Matrix * Predict();
    Matrix * Predict(Matrix * control);
    Matrix * Correct(Matrix * measurement);

    void setTransition_matrix(Matrix * transition_matrix);
    void setError_cov_post(Matrix * error_cov_post);
    Matrix * getError_cov_post();
};

#endif // KALMAN_H
