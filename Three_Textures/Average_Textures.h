//Average_Textures.h
#pragma once
#include "info.h"
using namespace std;

matrix< array<pair<T, bool>, 3> > get_textures_arrangement/*����� ����������� �������*/(matrix<T>& source);
matrix< T > res_to_source(const matrix< array<pair<T, bool>, 3> >& res);

//� ������ ���� ���������� �������� ����������� ������ � ������, � ������ ���������� �������
matrix<T> gen_sourse_data(const T& w, const T& h, const T& n_textures);