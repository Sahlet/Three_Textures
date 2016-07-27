//matrix.h
#pragma once
//#include <memory>
#include <vector>

template<class Type>
class matrix{
public:
	typedef unsigned short T;
private:
	T w, h;
	std::vector< Type > vec;
public:
	typedef typename std::vector<Type>::iterator iterator;
	typedef typename std::vector<Type>::const_iterator const_iterator;
	matrix(const T& width, const T& height)  : w(width), h(height), vec(w*h){}
	matrix(matrix&& m) : w(m.w), h(m.h), vec(std::move(m.vec)){}
	matrix(const matrix& m) : w(m.w), h(m.h), vec(m.vec) {}
	matrix& operator = (matrix&& m){
		w = m.w;
		h = m.h;
		vec = std::move(m.vec);
	}

	inline const T& get_w() const {return w;}
	inline const T& get_h() const {return h;}
	Type& operator()(const T& x, const T& y) /*throw(std::exception)*/{
		return vec[x + y*w];
	}
	const Type& operator()(const T& x, const T& y) const /*throw(std::exception)*/{
		return vec[x + y*w];
	}

	Type& operator[](const std::pair< T, T > &coordinates) /*throw(std::exception)*/{
		return vec[coordinates.first + coordinates.second*w];
	}
	const Type& operator[](const std::pair< T, T > &coordinates) const /*throw(std::exception)*/{
		return vec[coordinates.first + coordinates.second*w];
	}

	iterator begin(){
		return vec.begin();
	}
	iterator end(){
		return vec.end();
	}
	const_iterator begin() const{
		return vec.begin();
	}
	const_iterator end() const{
		return vec.end();
	}
};