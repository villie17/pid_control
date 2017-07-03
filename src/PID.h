#ifndef PID_H
#define PID_H

class PID {
public:
  /*
  * Errors: Will not use this
  */
  double p_error;
  double i_error;
  double d_error;

  /*
  * Coefficients
  */ 
  double Kp;
  double Ki;
  double Kd;

  // diff
  double d_Kp;
  double d_Ki;
  double d_Kd;

  int update_freq;
  double batch_error;
  double best_error;
  double* cur_K;
  double* cur_d_K;

  int turn;

  /*
  * Constructor
  */
  PID();

  /*
  * Destructor.
  */
  virtual ~PID();

  /*
  * Initialize PID.
  */
  void Init(double Kp, double Ki, double Kd);

  /*
  * Update the PID error variables given cross track error.
  */
  void UpdateError(double cte);

  /*
  * Calculate the total PID error.
  */
  double TotalError();
};

#endif /* PID_H */
