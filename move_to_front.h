/*************************************************************************
* Name:        move_to_front.h
* Author:      Qi Wang
* Description: MTF coder/decoder interface.
* Copyright:   2017 Blippar
*-------------------------------------------------------------------------
*************************************************************************/
#ifndef _move_to_front_h_
#define _move_to_front_h_

#include <vector>
#include <netinet/in.h>

void moveToFront(std::vector<int> &input);
void moveToFrontApprox(std::vector<int> &input);
int approxCopyCounter(std::vector<int> &input);
int exactCopyCounter(std::vector<int> &input);
void getDiffValues(std::vector<int> &input, std::vector<int> &output);
void displayComparison(std::vector<int> &input);
void remapAngle(std::vector<int> &angle_values);

#endif /* _move_to_front_h_ */