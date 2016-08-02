//Average_Textures.h
#pragma once
#include "info.h"
using namespace std;

extern bool fill_infos(matrix< info/*< TAG >*/ >& infos, node start, const node& goal, const node& end, const bool& may_change_texture = false, const bool& quickly = false);
extern bool fill_infos(matrix< info/*< TAG >*/ >& infos, const node start, const node& goal, const bool& may_change_texture = false, const bool& quickly = false);

extern matrix< array<pair<T, bool>, 3> > get_textures_arrangement/*взять расстановку текстур*/(matrix<T>& source, bool quickly = false);
extern matrix< T > res_to_source(const matrix< array<pair<T, bool>, 3> >& res);

//в первых двух аргументах приходит размерность ширина и высота, а третим количество текстур
extern matrix<T> gen_sourse_data(const T& w, const T& h, const T& n_textures);


//проверяет, есть ли у контейнера _Container_Ty функция "bool contains(_Ty)"
template<typename _Container_Ty, typename _Ty> struct has_contains {
private:
	static void detect(...);
	template<typename U, typename V> static decltype(std::declval<U>().contains(std::declval<V>())) detect(const U&, const V&);
public:
	static constexpr bool value = std::is_same<bool, decltype(detect(std::declval<_Container_Ty>(), std::declval<_Ty>()))>::value;
};

//проверяет, есть ли у контейнера _Container_Ty функция "size_t count(_Ty)"
template<typename _Container_Ty, typename _Ty> struct has_count {
private:
	static void detect(...);
	template<typename U, typename V> static decltype(std::declval<U>().count(std::declval<V>())) detect(const U&, const V&);
public:
	static constexpr bool value = std::is_same<size_t, decltype(detect(std::declval<_Container_Ty>(), std::declval<_Ty>()))>::value;
};

template < class _Container_Ty, class _Ty>
inline bool contains(const _Container_Ty& container, const _Ty& val, typename std::enable_if<has_contains<_Container_Ty, _Ty>::value>::type* = 0) {
	return container.contains(val);
}
template < class _Container_Ty, class _Ty>
inline bool contains(const _Container_Ty& container, const _Ty& val,
	typename std::enable_if<!has_contains<_Container_Ty, _Ty>::value && has_count<_Container_Ty, _Ty>::value>::type* = 0) {
	return container.count(val);
}
template < class _Container_Ty, class _Ty>
inline bool contains(const _Container_Ty& container, const _Ty& val,
	typename std::enable_if<!has_contains<_Container_Ty, _Ty>::value && !has_count<_Container_Ty, _Ty>::value>::type* = 0) {
	for (const _Ty& i : container) {
		if (val == i) return true;
	}
	return false;
}

extern std::vector< T > get_popular_textures_in_order(const std::vector< T >& textures, const T& max_vector_len = NUMBER_OF_LEVELS);