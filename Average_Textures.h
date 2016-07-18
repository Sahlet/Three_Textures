//Average_Textures.h
#pragma once
#include "matrix.h"
#include <array>
using namespace std;
typedef unsigned short T;
#define NUMBER_OF_LEVELS 3
extern const T max_T;
extern const T MAX_DISTANCE, MIN_DISTANCE;
extern const T MAX_COUNT_OF_USABLE;

//�������� � �������, ������� ����� ����� �������
void change_to_matrix_that_may_be_parsed_to_res(matrix< T >& res);

matrix< array<pair<T, bool>, 3> > get_textures_arrangement/*����� ����������� �������*/(const matrix<T>& source);
matrix< T > res_to_source(const matrix< array<pair<T, bool>, 3> >& res);

//� ������ ���� ���������� �������� ����������� ������ � ������, � ������ ���������� �������
matrix<T> gen_sourse_data(const T& w, const T& h, const T& n_textures);