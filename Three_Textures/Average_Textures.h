//Average_Textures.h
#pragma once
#include "info.h"
using namespace std;

bool fill_infos(matrix< info/*< TAG >*/ >& infos, node start, const node& goal, const node& end, const bool& may_change_texture = false, const bool& quickly = false);
bool fill_infos(matrix< info/*< TAG >*/ >& infos, const node start, const node& goal, const bool& may_change_texture = false, const bool& quickly = false);

matrix< array<pair<T, bool>, 3> > get_textures_arrangement/*����� ����������� �������*/(matrix<T>& source, bool quickly = false);
matrix< T > res_to_source(const matrix< array<pair<T, bool>, 3> >& res);

//� ������ ���� ���������� �������� ����������� ������ � ������, � ������ ���������� �������
matrix<T> gen_sourse_data(const T& w, const T& h, const T& n_textures);