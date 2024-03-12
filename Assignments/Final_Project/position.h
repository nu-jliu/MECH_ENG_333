#ifndef POSITION__H__
#define POSITION__H__

#define MAX_NUM_DATA 5000

extern volatile float kp_pos;
extern volatile float ki_pos;
extern volatile float kd_pos;

extern volatile int index_traj;
extern volatile int traj_len;

extern volatile float traj[MAX_NUM_DATA];

#endif