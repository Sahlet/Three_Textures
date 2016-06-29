//Average_Textures.cpp


#include "Average_Textures.h"
#include <map>
#include <list>
#include <algorithm>
//#include <typeinfo>

typedef pair< T, T > node;
//typedef  info;

matrix< array<pair<T, bool>, 3> > get_textures_arrangement(const matrix<T>& source) throw(exception){
	T w = source.get_w(), h = source.get_h();
	if (w < 2 || h < 2){
		//тривиально
		//сделать
		matrix< array<pair<T, bool>, 3> > sdelay_menya_normalno(w, h);
		return sdelay_menya_normalno;
	}

	
	matrix< array<pair<T, bool>, 3> > res(w, h);
	return std::move(res);

}