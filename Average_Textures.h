//Average_Textures.h
#pragma once
#include "matrix.h"
#include <array>
using namespace std;
typedef unsigned short T;
const T h = 3;

matrix< array<pair<T, bool>, 3> > get_textures_arrangement/*взять расстановку текстур*/(const matrix<T>& source) throw(exception);