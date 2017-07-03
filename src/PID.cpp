#include "PID.h"
#include <iostream>
using namespace std;

// Update parameters after every 10 cte updates
#define UPDATE_FREQ 10
using namespace std;

/*
* PID Class
*/

PID::PID() {}

PID::~PID() {}

void PID::Init(double Kp, double Ki, double Kd) {
	this->Kp = Kp;
	this->Kd = Kd;
	this->Ki = Ki;
	this->update_freq = 100;
	this->batch_error = 0.0;
	this->best_error = -1;
	this->d_Kp = 0.1;
	this->d_Kd = 0.01;
	this->d_Ki = 0;
	this->turn = -1;

}

void PID::UpdateError(double cte) {

	update_freq--;
	batch_error += (cte*cte); // get rid of sign


	if (update_freq <= 0){
		batch_error/= double(UPDATE_FREQ);

		// init
		if (this->turn == -1){
			this->best_error = batch_error;
			cur_K = &this->Kp;
			cur_d_K = &this->d_Kp;
		}
		if (this->turn == 0){
			*cur_K += *cur_d_K;
		}
		if (this->turn == 1){
			if (this->batch_error < this->best_error){
				this->best_error = this->batch_error;
				*cur_d_K *= 1.1;
			}else{
				*cur_K -= 2*(*cur_d_K);
			}
		}
		if (this->turn == 2){
			if (this->batch_error < this->best_error){
				this->best_error = this->batch_error;
				*cur_d_K *= 1.1;
			}else{
				*cur_d_K *= 0.9;
			}
		}
		this->turn++;
		if (this->turn > 2){
			this->turn = 0;
			if (cur_K == &this->Kp){
				cout <<" PARAM: switched to d"<<endl;
				cur_K = &this->Kd;
				cur_d_K = &this->d_Kd;
			}
			else if (cur_K == &this->Kd){
#if 1
				cout <<" PARAM: switched to i"<<endl;
				cur_K = &this->Ki;
				cur_d_K = &this->d_Ki;
#else
				cout <<" PARAM: switched to p"<<endl;
				cur_K = &this->Kp;
				cur_d_K = &this->d_Kp;
#endif
			}
			else if (cur_K == &this->Ki){
				cout <<" PARAM: switched to p"<<endl;
				cur_K = &this->Kp;
				cur_d_K = &this->d_Kp;
			}
		}

		// Reset
		update_freq = UPDATE_FREQ;
		batch_error = 0;
		cout << "PARAM Kp:" << Kp << " d_Kp:"<< d_Kp << endl;
		cout << "PARAM Kd:" << Kd << " d_Kd:"<< d_Kd << endl;
		cout << "PARAM Ki:" << Ki << " d_Ki:"<< d_Ki << endl;
	}


}

double PID::TotalError() {
	return 0.0;
}


