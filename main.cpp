//main.cpp
#include <iostream>
#include "Average_Textures.h"
#include <string>
#include <memory>
#include <fstream>
#include <ctime>

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
			T i_ = 0;
			for (; i_ < NUMBER_OF_LEVELS; i_++){
				if (res(x, y)[i_].second) break;
			}
			if (i_ == NUMBER_OF_LEVELS || res(x, y)[i_].first != source(x, y)) s += "x";
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

#define READ
//#define WRITE
#define SOLVE
//#define PRINT_SOLUTION

int main(){
	string buffer("buffer.txt");
	string solution("solution.txt");
	

	#ifdef READ
		T w, h;
		ifstream in_stream(buffer);
		in_stream >> w >> h;
		matrix<T> source(w, h);
		for (auto& i : source){
			in_stream >> i;
		}
		in_stream.close();
	#else
		const T w = 100, h = 100;
		matrix<T> source = gen_sourse_data(w, h, 4);
	#endif
	#ifdef WRITE
		ofstream out_stream(buffer);
		out_stream << w << ' ' << h;
		T j = 0;
		for (const auto& i : source){
			if ((j++ % w) == 0) out_stream << '\n';
			out_stream << i << ' ';
		}
		out_stream.close();
	#endif
	#ifdef SOLVE
		cout << "w = " << w << "; h = " << h << ";" << endl;
		auto start_time = clock();
		matrix<T> source_copy(source);//эта матрица будет преобразована в такую, что в каждом квадрате 3х3 будет не больше 3 текстур
		matrix< array<pair<T, bool>, 3> > res = get_textures_arrangement(source_copy);
		double time = (clock() - start_time) / double(1000);


		auto source_from_res = res_to_source(res);
		auto diff1 = difference(source, source_from_res);
		auto diff2 = difference(source, source_copy);
		cout << "time is " << time << " second" << endl;
		cout << "full difference is " << diff1 << " = " << (diff1 / (float)(w * h)) * 100 << "%" << endl;
		cout << "difference between source and 3x3 condition source is " << diff2 << " = " << (diff2 / (float)(w * h)) * 100 << "%" << endl << endl;
		#ifdef PRINT_SOLUTION
			ofstream solution_stream(solution);
			cout.rdbuf(solution_stream.rdbuf());
			cout << "w = " << w << "; h = " << h << ";" << endl;
			cout << "time is " << time << " second" << endl;
			cout << "full difference is " << diff1 << " = " << (diff1 / (float)(w * h)) * 100 << "%" << endl;
			cout << "difference between source and 3x3 condition source is " << diff2 << " = " << (diff2 / (float)(w * h)) * 100 << "%" << endl << endl;
			print(source, res);
			solution_stream.close();
		#endif

		system("pause");
	#endif
}