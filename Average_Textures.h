//Average_Textures.h
#pragma once
#include "matrix.h"
using namespace std;
typedef unsigned short T;

matrix< vector< pair<T, bool> > > get_textures_merge(const matrix<T>& source, size_t number_of_textures_in_node = 3) throw(exception);