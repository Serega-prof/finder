#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
using namespace std; 
// Функция для суммирования элементов вектора
//Прототипы функций
vector <float> open_file(const string& filename);
float calc_general_middle(const vector<float>& samples, size_t num_samples);
vector<float> mid_gap(const vector<float>& samples, int gap_small);
int find_general_differences(const vector<float>& mid_val_gap, float general_middle, float level_change, bool sens);
vector<float> mid_gap_shift(const vector<float>& samples, int small_gap, int big_gap);
vector<float> comparison(const vector<float>& samples, const vector<float>& mid_gap, const vector<float>& mid_gap_sh, int small_gap, int big_gap, float sensitivity);
vector <float> symmetric(const vector<float>& input, float alpha);
vector<float> comparison100(const vector<float>& samples, const vector<float>& sig_sm, const vector<float>& mid_gap, const vector<float>& mid_gap_sh, int small_gap, int big_gap, float sensitivity);
vector <float> single_smooth(const vector<float>& samples, float sensitivity, int gap, vector<float>& sig_sm);
vector <float> single_smooth100(const vector<float>& samples, float sensitivity, int gap, const vector<float>& prev);
vector <float> single_smooth2(const vector<float>& samples, float sensitivity, int gap);
