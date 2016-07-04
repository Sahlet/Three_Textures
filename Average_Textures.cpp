﻿//Average_Textures.cpp


#include "Average_Textures.h"
#include <iostream>
#include <list>
#include <map>
#include <stdlib.h>
#include <time.h>


using namespace std;

struct node{
	T x, y;
	node(const T& x = 0, const T& y = 0) : x(x), y(y){}
	node(const pair< T , T >& p) : x(p.first), y(p.second){}
	node(const node& n) : x(n.x), y(n.y){}
	inline operator pair< T , T >(){
		return pair< T , T >(x, y);
	}
	inline bool operator<(const node& n){
		if ((x + y < n.x + n.y) || ((x + y == n.x + n.y) && (y < n.y))) return true;
		return false;
	}
	inline bool operator==(const node& n){
		return y == n.y && x == n.x;
	}
	inline bool operator!=(const node& n){
		return !(*this == n);
	}
	inline bool operator>(const node& n){
		return !(*this == n || *this < n);
	}
};

enum limits {NOTHING = 0, UP = 1, DOWN = 2, LEFT = 4, RIGHT = 8};
list< node > make_neighbors(const T& x, const T& y, const T& w, const T& h){
	limits limitless = (limits)(
			(y > 0) | 
			((y + 1 < h) << 1) | 
			((x > 0) << 2) | 
			((x + 1 < w) << 3) );

	list< node > neighbors;
	if (limitless & limits::DOWN){
		neighbors.push_back(node(x, y + 1));
		if (limitless & limits::RIGHT) neighbors.push_back(node(x + 1, y + 1));
		if (limitless & limits::LEFT) neighbors.push_back(node(x - 1, y + 1));
	}
	if (limitless & limits::RIGHT) neighbors.push_back(node(x + 1, y));
	if (limitless & limits::LEFT) neighbors.push_back(node(x - 1, y));
	if (limitless & limits::UP){
		neighbors.push_back(node(x, y - 1));
		if (limitless & limits::RIGHT) neighbors.push_back(node(x + 1, y - 1));
		if (limitless & limits::LEFT) neighbors.push_back(node(x - 1, y - 1));
	}
	
	return neighbors;
}
inline list< node > make_neighbors(const node& n, const T& w, const T& h){
	return make_neighbors(n.x, n.y, w, h);
}



/*
		1 2 3
		8 x 4 //номера соседей
		7 6 5
*/
enum neighbor_number {N1 = 1, N2 = 2, N3 = 4, N4 = 8, N5 = 16, N6 = 32, N7 = 64, N8 = 128};
list< node > make_neighbors(const T& x, const T& y, const T& w, const T& h, const neighbor_number& neighbors_number){
	limits limitless = (limits)(
			(y > 0) | 
			((y + 1 < h) << 1) | 
			((x > 0) << 2) | 
			((x + 1 < w) << 3) );

	list< node > neighbors;
	if (limitless & limits::DOWN){
		if (neighbors_number & neighbor_number::N6) neighbors.push_back(node(x, y + 1));
		if (neighbors_number & neighbor_number::N5 && limitless & limits::RIGHT) neighbors.push_back(node(x + 1, y + 1));
		if (neighbors_number & neighbor_number::N7 && limitless & limits::LEFT) neighbors.push_back(node(x - 1, y + 1));
	}
	if (neighbors_number & neighbor_number::N4 && limitless & limits::RIGHT) neighbors.push_back(node(x + 1, y));
	if (neighbors_number & neighbor_number::N8 && limitless & limits::LEFT) neighbors.push_back(node(x - 1, y));
	if (limitless & limits::UP){
		if (neighbors_number & neighbor_number::N2) neighbors.push_back(node(x, y - 1));
		if (neighbors_number & neighbor_number::N3 && limitless & limits::RIGHT) neighbors.push_back(node(x + 1, y - 1));
		if (neighbors_number & neighbor_number::N1 && limitless & limits::LEFT) neighbors.push_back(node(x - 1, y - 1));
	}
	
	return neighbors;
}
inline list< node > make_neighbors(const node& n, const T& w, const T& h, const neighbor_number& neighbors_number){
	return make_neighbors(n.x, n.y, w, h, neighbors_number);
}

template<class _T>
list<_T> move_to_list(vector<_T> &&vec){
	auto size = vec.size();
	list<_T> res(size);
	auto iter = res.begin();
	for (size_t i = 0; i < size; i++){
		*iter++ = std::move(vec[i]);
	}
	return res;
}
template<class _T>
vector<_T> move_to_vector(list<_T> &&list){
	auto size = list.size();
	vector<_T> res(size);
	auto iter = list.begin();
	for (size_t i = 0; i < size; i++){
		res[i] = std::move(*iter++);
	}
	return res;
}

//изменить в матрицу, которая может иметь решение
static void change_to_matrix_that_may_be_parsed_to_res(matrix< T >& res){
	map< T , int > textures;
	list< node > neighbors;
	T w = res.get_w(), h = res.get_h();
	T texture1, texture2;
	int min, max;
	for (T x = 1; x < w; x++){
		textures.clear();
		for (T y = 0; y < h; y++){
			neighbors = make_neighbors(x, y, w, h);
			neighbors.push_front(node(x, y));
			for (const auto& i : neighbors){
				textures[res(i.x, i.y)] += 1;
			}
			max = 0;
			for (const auto& i : textures){
				if (i.second > max){
					max = i.second;
					texture1 = i.first;
				}
			}
			while (textures.size() > 3){
				min = max;
				for (const auto& i : textures){
					if (i.second <= min && i.first != texture1){
						min = i.second;
						texture2 = i.first;
					}
				}
				textures.erase(texture2);
			}

			for (const auto& i : neighbors){
				if (!textures.count(res(i.x, i.y))) res(i.x, i.y) = texture1;
			}
		}
	}
}

class info{//информация про узел
	enum level : char {NONE = 0, FIRST = 1, SECOND = 2, THIRD = 4};
	static const T max_T;
	static const T MAX_DISTANCE;
	char used;//использованные уровни
	char taken;//уровни, взятые в предыдущих соседних узлах.
	char given;//уровни, которые дали следующие узлы.
	/*в given будем помещать только уровень, который дал один следующий узел, а в taken все возможные (в соответствии с его описанием см.выше.)*/
	bool can_be_used_again_;
	T texture;

	vector< info* > valuable_neighbors;
	info* pred_neighbor;
	map< array< T , 3 > , char> memory;

	int cur_level;//показывает на каком уровне сейчас стоит 1
	array< T , 3 > textures;//(значимые только те текстуры, которые отмечены в taken или в given или в cur_level)
	
	bool free;

///////////////////////////////////////////
public:
	info() : free(false), pred_neighbor(nullptr), texture(max_T){
		clear();
	}
private:

	inline bool give(const int& l, const T& texture){
		//следующий узел хочет записать текстуру texture на уровень l и вызывает эту функцию.
		if (l & (taken | cur_level)){
			if (textures[l/2] != texture) return false/*нельзя добавлять текстуру*/;
		} else {
			if(!l){
				cerr << "if(!l)" << endl;
				throw(exception("после отладки программы этот throw надо удалить"));
			}
			textures[l/2] = texture;
		}
		given = l;
		return true;
	}
	inline bool can_give(const int& l, const T& texture) const{
		//некоторый узел спрашивает, можно ли на уровень l поставить текстуру texture.
		if (l & (taken | given | cur_level)){
			if (textures[l/2] != texture) return false/*нельзя добавлять текстуру*/;
		} else {
			if(!l){
				cerr << "if(!l)" << endl;
				throw(exception("после отладки программы этот throw надо удалить"));
			}
		}
		return true;
	}
	//текстура должна стать на любой свободный уровень, который еще не использован
	inline void use(){
		if (free) return;
		if (!can_be_used_again_){
			cerr << "if (!can_be_used_again_)" << endl;
			throw(exception("после отладки программы этот throw надо удалить"));
		}
		if (!used){//первый раз выставляем уровень - значит надо определить taken
			int l;
			for (const auto& i : valuable_neighbors){
				if (l = i->cur_level){
					if (l & taken){
						if (textures[l/2] != i->textures[l/2]){
							//этот код, как мне кажется, не должен никогда выполниться
							cerr << "if (textures[l/2] != infos(i.x, i.y).textures[l/2])" << endl;
							throw(exception("после отладки программы этот throw надо удалить"));
						}
					} else {
						taken |= l;
						textures[l/2] = i->textures[l/2];
					}
				}
			}

			if (pred_neighbor){
				if (pred_neighbor->given && !(pred_neighbor->given & (pred_neighbor->taken | pred_neighbor->cur_level))){
					pred_neighbor->textures[pred_neighbor->given/2] = max_T;
				}
				pred_neighbor->given = NONE;
			}

			struct tmp_struct{
				static bool func(const vector< info* >& neighbors, const T& texture, const level& l){
					for (const auto& i : neighbors){
						if (!i->can_give(l, texture)) return false;
					}
					return true;
				}
			};

			if (!tmp_struct::func(valuable_neighbors, texture, FIRST)) used |= FIRST;
			if (!tmp_struct::func(valuable_neighbors, texture, SECOND)) used |= SECOND;
			if (!tmp_struct::func(valuable_neighbors, texture, THIRD)) used |= THIRD;
		}

		//сначала пытаемся добавить текстуру на тот уровень, на котором есть такие же
		if (!(FIRST & used) && FIRST & taken) cur_level = FIRST;
		else if(!(SECOND & used) && SECOND & taken) cur_level = SECOND;
		else if(!(THIRD & used) && THIRD & taken) cur_level = THIRD;
		else if(!(FIRST & used))  cur_level = FIRST;
		else if(!(SECOND & used)) cur_level = SECOND;
		else if(!(THIRD & used))  cur_level = THIRD;
		else throw(exception(/*нельзя добавлять текстуру - либо все варианты перебраны, либо такую текстуру никогда нельзя вставить (этот throw не надо удалять)*/));
		
		if (pred_neighbor){
			pred_neighbor->give(cur_level, texture);
		}
		used |= cur_level;
		textures[cur_level / 2] = texture;

		if ((FIRST | SECOND | THIRD) ^ (taken | used)){//какой-то уровень остался и его можно было бы использовать при другой расстановке
			can_be_used_again_ = true;
		} else can_be_used_again_ = false;
	}
	bool can_be_used_again() const {
		if (free) return false;
		if (!used) throw(exception("после отладки программы этот throw надо удалить"));
		return can_be_used_again_;
	}

	bool was_here(){
		return memory[textures] & cur_level;
	}
	void remember(){		
		memory[textures] |= cur_level;
	}

	void clear(){
		used = NONE; taken = NONE; given = NONE; cur_level = NONE; can_be_used_again_ = true;
		for (size_t i = 0; i < textures.size(); i++) textures[i] = max_T;
	}
	void clear_memory(){
		memory.clear();
	}

	void set_texture(const T& texcure){
		if (texcure == max_T){
			cerr << "if (texcure == max_T)" << endl;
			throw(exception("после отладки программы этот throw надо удалить"));
		}
		if (this->texture != texcure){
			clear();
			clear_memory();
			this->texture = texcure;
		}
	}
	void set(const T& texture, const node& n, matrix<info>& owner){
		set_texture(texture);
		auto tmp_list = make_neighbors(n, owner.get_w(), owner.get_h(), (neighbor_number)(N1 | N2 | N7 | N8));
		valuable_neighbors = vector<info*>(tmp_list.size());
		auto iter = tmp_list.begin();
		for (size_t i = 0; i < valuable_neighbors.size(); i++){
			valuable_neighbors[i] = &owner(iter->x, iter->y);
		}
		if (n.x && n.y < owner.get_h() - 1) pred_neighbor = &owner(n.x - 1, n.y + 1);

		this->free = true;
		for (int x_ = (int)n.x - 2; x_ <= n.x + 2; x_++){
			for (int y_ = (int)n.y - 2; y_ <= n.y + 2; y_++){
				if (x_ >= 0 && y_ >= 0){
					if (owner(x_, y_).texture != this->texture){
						this->free = false;
						x_ = n.x + 2;
						break;
					}
				}
			}
		}
	}

static matrix< array<pair<T, bool>, 3> > infos_to_res(matrix< info >& infos){
	auto w = infos.get_w(), h = infos.get_h();
	matrix< array<pair<T, bool>, 3> > res(w, h);
	limits limitless;
	list< node > neighbors;
	for (T x = 0; x < w; x++){
		for (T y = 0; y < h; y++){
			auto& inf = infos(x, y);
			///////////////////////////
			if (inf.free){
				inf.free = false;
				inf.use();
				neighbors = make_neighbors(x, y, w, h, (neighbor_number)(N1 | N2 | N7 | N8));
				for (const auto& n : neighbors){
					infos(n.x, n.y).give(inf.cur_level, inf.texture);
				}
			}
			neighbors = make_neighbors(x, y, w, h, (neighbor_number)(N3 | N4 | N5));
			for (const auto& n : neighbors){
				if (!infos(n.x, n.y).free) inf.give(infos(n.x, n.y).cur_level, infos(n.x, n.y).texture);
			}
			///////////////////////////
			auto& res_inf = res(x, y);
			for (int i = 0; i < 3; i++) res_inf[i].first = inf.textures[i];
			res_inf[inf.cur_level/2].second = true;
		}
	}

	return std::move(res);
}

public:

static matrix< array<pair<T, bool>, 3> > get_textures_arrangement(const matrix<T>& source) {
	T w = source.get_w(), h = source.get_h();
	node barrier;//за этот барьер нельзя заступать
	matrix< info > infos(w, h);

	//инициализация
	for (T x = 0; x < w; x++){
		for (T y = 0; y < h; y++){
			infos(x, y).set(source(x, y), node(x, y), infos);
		}
	}
	
	auto solve_the_problem = [&infos, &barrier] (const T& distance, const node& n) -> bool{
		list<node> usable_nodes/*еще могут менять уровень*/;
		//list<info> buffer/*хранит лучший варифнт*/;
		//if (infos(x, y).can_be_used_again()) usable_nodes.push_front(node(x, y));
		return false;
	};

	T diagonals_number = w + h - 1, distance_to_start = 0, x, y;

	for (; distance_to_start < diagonals_number; diagonals_number++){
		if (distance_to_start < h){
			y = distance_to_start + 1;
			x = 0;
		} else {
			y = h;
			x = distance_to_start - h;
		}
		for (; y-- > 0; x++){
			try{
				infos(x, y).use();
			} catch(const exception&) {
				if (!solve_the_problem(MAX_DISTANCE, node(x, y))){
					infos(x, y).set_texture(infos(x - 1, y).texture);
					infos(x, y).use();
					for (int x_ = (int)x - 2; x_ <= x + 2; x_++){
						for (int y_ = (int)y - 2; y_ <= y + 2; y_++){
							if (x_ >= 0 && y_ >= 0){
								if (infos(x_, y_).free){
									infos(x_, y_).free = false;//это удалять нельзя
									if (node(x_, y_) < node(x, y)){
										//этот код, как мне кажется, не должен никогда выполниться
										cerr << "if (node(x_, y_) < node(x, y))" << endl;
										throw(exception("после отладки программы этот throw надо удалить"));
									}
								}
							}
						}
					}
				} else {barrier = node(x, y);}
			}
		}
	}

	return infos_to_res(infos);
}

static matrix< array<pair<T, bool>, 3> > gen_solution_data(const T& w, const T& h, const T& n_textures) {
	if (w > 10000 || h > 10000 || n_textures > 200) throw(exception("1"));
	if (w == 0 || h == 0 || n_textures == 0) throw(exception("2"));

	srand ((unsigned int)time(NULL));

	matrix< info > infos(w, h);

	{
		matrix< T > tmp(w, h);

		for (T x = 0; x < w; x++){
			for (T y = 0; y < h; y++){
				tmp(x, y) = rand() % n_textures;
			}
		}

		change_to_matrix_that_may_be_parsed_to_res(tmp);

		for (T x = 0; x < w; x++){
			for (T y = 0; y < h; y++){
				infos(x, y).set(tmp(x, y), node(x, y), infos);
			}
		}
	
	}

	list< node > neighbors;
	T number, texture;

#define SET_TEXTURE(x_, y_){\
		try{\
			infos(x_, y_).use();\
		}\
		catch(const exception&){\
			infos(x_, y_).set_texture(infos(x_ - 1, y_).texture);\
			infos(x_, y_).use();\
		}\
	}

	for (T x = 0; x < w; x++){
		T y = 0;
		if (x == 0 && y == 0){
			SET_TEXTURE(x, y);
			y++;
		}
		 
		for (; y < h; y++){
			if(rand() % 4){
				neighbors = make_neighbors(x, y, w, h, (neighbor_number)(N1 | N2 | N7 | N8));
				number = rand() % neighbors.size();
				for (const auto& i : neighbors){
					if (!number--){
						try{
							texture = infos(i.x, i.y).texture;
							infos(x, y).set_texture(texture);
						} catch (const exception&){}
						break;
					}
				}
			}
			SET_TEXTURE(x, y);
		}
	}

	return infos_to_res(infos);
}

};


const T info::max_T = 0u - 1;
const T info::MAX_DISTANCE = 5;

matrix< T > res_to_source(const  matrix< array<pair<T, bool>, 3> >& res){
	auto w = res.get_w(), h = res.get_h();
	matrix< T > source(w, h);
	for (T x = 0; x < w; x++){
		for (T y = 0; y < h; y++){
			source(x, y) = res(x, y)[0].first;
			if (res(x, y)[1].second) source(x, y) = res(x, y)[1].first;
			else if (res(x, y)[2].second) source(x, y) = res(x, y)[2].first;
		}
	}

	return std::move(source);
}

matrix< array<pair<T, bool>, 3> > get_textures_arrangement(const matrix<T>& source) {
	return info::get_textures_arrangement(source);
}

//в первых двух аргументах приходит размерность ширина и высота, а третим количество текстур
matrix<T> gen_sourse_data(const T& w, const T& h, const T& n_textures){
	if (w > 10000 || h > 10000 || n_textures > 200) throw(exception("1"));
	if (w == 0 || h == 0 || n_textures == 0) throw(exception("2"));

	srand ((unsigned int)time(NULL));

	matrix< T > res(w, h);

	list< node > neighbors;
	T number;

	for (T x = 0; x < w; x++){
		for (T y = 0; y < h; y++){
			res(x, y) = rand() % n_textures;
		}
	}

	change_to_matrix_that_may_be_parsed_to_res(res);

	for (T x = 1; x < w; x++){
		for (T y = 0; y < h; y++){
			if(rand() % 4){
				neighbors = make_neighbors(x, y, w, h, (neighbor_number)(N1 | N2 | N7 | N8));
				number = rand() % neighbors.size();
				for (const auto& i : neighbors){
					if (!number--){
						res(x, y) = res(i.x, i.y);
						break;
					}
				}
			}
		}
	}

	return res;
}

matrix< array<pair<T, bool>, 3> > gen_solution_data(const T& w, const T& h, const T& n_textures) {
	return info::gen_solution_data(w, h, n_textures);
}