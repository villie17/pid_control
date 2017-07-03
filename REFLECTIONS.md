# Reflection
* The project implements the PID algorithm presented in lessons. It tries to stay as true to suggested implementation as possible.
* For parameter tuning I have used the Twiddle algorithm as presented.

## PID Algorithm
* For PID algorithm I use basically the same algo as suggested in the lesson.
  * However the lesson assumed time update as 1 second but I calculate the time between lessons as use that in calculations. 
  * Apart from that its the same algorithm
  * I found that simulator doesn't have any discernible steering drift and setting any value to parameter I other than 0 causes inferior performance.

## Parameter Tuning
* Asynchronous version of Twiddle algorithm was used to tune parameters.
  * 10 updates of CTE were counted as 1 run. 
  * Contrary to synchronous working on Python algorithm where run of robot could be calculated our car got asynchronous updates to CTE. 
  * To cater for this situation a state control variable 'turn' was introduced. It was used as flag to 'remember' the position in algorithm between calls.
    * Turn = -1 initialization (only once)
    * Turn = 0  ADD differential to original parameter
    * Turn = 1 Check with best error and update best error and differential or SUBTRACT from original parameter if best error is not updated
    * Turn = 2 Check with best error and update best error and differential 
    * The parameters switches between three parameters
    * Initial values chosen for Kp=1, Kd=0.5 and Ki=0 and for differential d_Kp=0.1, d_Kd=0.01 and d_Ki=0
    * Afte oscillations the parameters finally tended to settle to Kp=0.11 Kd=0.4 and Ki=0

### Final values of parameter
* The final value of parameter Kp=0.11 Kd=0.4 and Ki=0 at throttle value of 0.3. It is bit surprising.
    * It gives outsize weightage to differential part i-e difference b/w current CTE and previous CTE. 
    * My intuition is that because we get updated frequently and don't veer away from center much the P term is not important and differential has more importance
    * I also tested at little higher speed
    * At throttle 0.4 Kp=0.11, Kd=0.38, Ki=0, so higher speed makes updates seem less frequent and Kd is slightly lower
    
