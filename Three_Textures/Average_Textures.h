//Average_Textures.h
#pragma once
#include "info.h"
using namespace std;

matrix< array<pair<T, bool>, 3> > get_textures_arrangement/*взять расстановку текстур*/(matrix<T>& source);
matrix< T > res_to_source(const matrix< array<pair<T, bool>, 3> >& res);

//в первых двух аргументах приходит размерность ширина и высота, а третим количество текстур
matrix<T> gen_sourse_data(const T& w, const T& h, const T& n_textures);