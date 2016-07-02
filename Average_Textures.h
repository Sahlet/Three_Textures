//Average_Textures.h
#pragma once
#include "matrix.h"
#include <array>
using namespace std;
typedef unsigned short T;

matrix< array<pair<T, bool>, 3> > get_textures_arrangement/*взять расстановку текстур*/(const matrix<T>& source) throw(exception);
matrix< T > res_to_source(const matrix< array<pair<T, bool>, 3> >& res);

//в первых двух аргументах приходит размерность ширина и высота, а третим количество текстур
matrix<T> gen_sourse_data(const T& w, const T& h, const T& n_textures);
matrix< array<pair<T, bool>, 3> > gen_solution_data(const T& w, const T& h, const T& n_textures) throw(exception);