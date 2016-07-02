//main.cpp
#include <iostream>
#include "Average_Textures.h"
#include <string>
#include <memory>

void print(const matrix< T >& source, const matrix< array<pair<T, bool>, 3> >& res){
	T w = source.get_w(), h = source.get_h();

	if (w != res.get_w() || h != res.get_h()) throw exception();

	T max_n_textures_in_row, tmp, str_len = 6;
	string s, space("  ");
	unique_ptr<char, default_delete<char[]> > line(new char[str_len + space.length() + 1]), big_space(new char[str_len + space.length() + 1]);

	for (char* ptr = line.get(), *end = line.get() + str_len + space.length(); ptr != end; ptr++){ *ptr = '_'; }
	line.get()[str_len + space.length()] = '\0';
	for (char* ptr = big_space.get(), *end = big_space.get() + str_len + space.length(); ptr != end; ptr++){ *ptr = ' '; }
	big_space.get()[str_len + space.length()] = '\0';

	for (T y = 0; y < h; y++){
		max_n_textures_in_row = 0;
		for (T x = 0; x < w; x++){
			if ( (tmp = res(x, y).size()) > max_n_textures_in_row)
				max_n_textures_in_row = tmp;
			cout << line.get();
		}
		cout << endl;
		for (T x = 0; x < w; x++){
			s = to_string(source(x, y));
			for (T i = 0; i < str_len; i++) cout.put(i < s.length() ? s[i] : ' ');
			cout << space;
		}
		cout << endl;
		for (T i = 0; i < max_n_textures_in_row; i++){
			for (T x = 0; x < w; x++){
				auto res_x_y = res(x, y);
				if (i < res_x_y.size()){
					s = to_string(res_x_y[i].first) + '|' + to_string(res_x_y[i].second);
					for (T i = 0; i < str_len; i++) cout.put(i < s.length() ? s[i] : ' ');
					cout << space;
				} else cout << big_space.get();
			}
			cout << endl;
		}
		cout << endl;
	}
}

size_t difference(const matrix< T >& source1, const matrix< T >& source2){
	T w = source1.get_w(), h = source1.get_h();
	if (w != source2.get_w() || h != source2.get_h()) throw exception();
	size_t res = 0;
	for (T y = 0; y < h; y++){
		for (T x = 0; x < w; x++){
			res += source1(x, y) != source2(x, y);
		}
	}
	return res;
}

void print(const matrix< array<pair<T, bool>, 3> >& res){
	print(res_to_source(res), res);
}

int main(){
	const T w = 6, h = 4;
	//T arr[w * h] = {
	//	3, 3, 3, 1,
	//	3, 2, 2, 1,
	//	3, 2, 2, 1,
	//	0, 0, 0, 0
	//};
	//T arr[w * h] = {
	//	0, 0, 0, 0,
	//	1, 1, 1, 2,
	//	1, 1, 1, 2,
	//	2, 2, 2, 2,
	//	3, 3, 3, 3
	//};
	T arr[w * h] = {
		3, 3, 3, 0, 0, 4,
		3, 2, 2, 2, 2, 4,
		3, 2, 2, 2, 2, 4,
		1, 1, 1, 0, 0, 4
	};
	//T arr[w * h] = {
	//	3, 3, 3, 1, 0,
	//	3, 2, 4, 1, 3,
	//	4, 2, 2, 1, 2,
	//	0, 0, 4, 4, 4,
	//	0, 0, 1, 4, 3
	//};
	vector<T> vec(arr, (T*)((char*)arr + sizeof(arr)));
	matrix<T> source(w, h);
	auto iter = vec.begin();
	for (auto i = source.begin(), end = source.end(); i != end; i++) *i = *iter++;

	////////////////////////////////////////////////////////

	//matrix<T> source = res_to_source(gen_solution_data(w, h, 10));
	//matrix<T> source = gen_sourse_data(w, h, 5);

	////////////////////////////////////////////////////////	
	
	auto res = get_textures_arrangement(source);


	auto diff = difference(source, res_to_source(res));
	cout << "difference is " << diff << " = " << (diff / (float)(w * h)) * 100 << "%" << endl << endl;
	print(source, res);

	system("pause");
}