//main.cpp
#include <iostream>
#include "Average_Textures.h"
#include <string>
#include <memory>

int main(){
	const T w = 4, h = 5;
	/*T arr[w * h] = {
		3, 3, 3, 1,
		3, 2, 2, 1,
		3, 2, 2, 1,
		0, 0, 0, 0
	};*/
	T arr[w * h] = {
		0, 0, 0, 0,
		1, 1, 1, 2,
		1, 1, 1, 2,
		2, 2, 2, 2,
		3, 3, 3, 3
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
	
	auto res = get_textures_arrangement(source);
	
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

	system("pause");
}