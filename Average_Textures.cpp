//Average_Textures.cpp


#include "Average_Textures.h"
#include <iostream>
#include <list>
//#include <set>


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
		if (x < n.x) return true;
		if (x > n.x) return false;
		if (y < n.y) return true;
		return false;
	}
	inline bool operator==(const node& n){
		return y == n.y && x == n.x;
	}
	inline bool operator!=(const node& n){
		return !(*this == n);
	}
	inline bool operator>(const node& n){
		if (x > n.x) return true;
		if (x < n.x) return false;
		if (y > n.y) return true;
		return false;
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

struct info{//информация про узел
	enum level {NONE = 0, FIRST = 1, SECOND = 2, THIRD = 4};
private:
	char used;//использованные уровни
	char taken;//уровни, взятые в предыдущих соседних узлах.
	char given;//уровни, которые дали следующие узлы.
	/*в given будем помещать только уровень, который дал один следующий узел, а в taken все возможные (в соответствии с его описанием см.выше.)*/
	bool can_be_used_again_;
	bool edge_of_space;
	T texture;
	vector< node > valuable_neighbors;
	array<int, 8> memory;
public:
	int cur_level;//показывает на каком уровне сейчас стоит 1
	array< T , 3 > textures;//(значимые только те текстуры, которые отмечены в taken или в given или в cur_level)
	node n;
	
	static matrix< info >* owner;
///////////////////////////////////////////
	info() : used(NONE), taken(NONE), given(NONE), cur_level(NONE), can_be_used_again_(true), edge_of_space(false){
		for (size_t i = 0; i < textures.size(); i++) textures[i] = 0;
	}

	inline bool give(const int& l, const T& texture){
		//следующий узел хочет записать текстуру texture на уровень l и вызывает эту функцию.
		if (!cur_level){
			cerr << "if (!cur_level)" << endl;
			throw(exception("после отладки программы этот throw надо удалить"));
		}
		if (l & (taken | (edge_of_space ? given : 0) | cur_level)){
			if (textures[l/2] != texture) return false/*нельзя добавлять текстуру*/;
		} else {
			if(!l){
				cerr << "if(!l)" << endl;
				throw(exception("после отладки программы этот throw надо удалить"));
			}
			textures[l/2] = texture;
		}
		edge_of_space ? given |= l : given = l;
		return true;
	}
	inline bool can_give(const int& l, const T& texture) const{
		//некоторый узел спрашивает, можно ли на уровень l поставить текстуру texture.
		if (!cur_level){
			cerr << "if (!cur_level)" << endl;
			throw(exception("после отладки программы этот throw надо удалить"));
		}
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
	inline void use() throw(exception){
		if (!can_be_used_again_){
			cerr << "if (!can_be_used_again_)" << endl;
			throw(exception("после отладки программы этот throw надо удалить"));
		}
		if (!used){//первый раз выставляем уровень - значит надо определить taken
			int l;
			for (const auto& i : valuable_neighbors){
				l = (*owner)(i.x, i.y).cur_level;
				if (l & taken){
					//этот код, как мне кажется, не должен никогда выполниться
					//if (textures[l/2] != infos(i.x, i.y).textures[l/2]) throw(exception(/*нельзя добавлять текстуру (этот throw не надо удалять)*/));
					if (textures[l/2] != (*owner)(i.x, i.y).textures[l/2]){
						cerr << "if (textures[l/2] != infos(i.x, i.y).textures[l/2])" << endl;
						throw(exception("после отладки программы этот throw надо удалить"));
					}
				} else {
					taken |= l;
					textures[l/2] = (*owner)(i.x, i.y).textures[l/2];
				}
			}

			if (n.y > 0){
				(*owner)(n.x, n.y - 1).given = NONE;
			}

			struct tmp_struct{
				static bool func(const vector< node >& neighbors, const matrix< info >& infos, const T& texture, const level& l){
					for (const auto& i : neighbors){
						if (!infos(i.x, i.y).can_give(l, texture)) return false;
					}
					return true;
				}
			};

			if (!tmp_struct::func(valuable_neighbors, (*owner), texture, FIRST)) used |= FIRST;
			if (!tmp_struct::func(valuable_neighbors, (*owner), texture, SECOND)) used |= SECOND;
			if (!tmp_struct::func(valuable_neighbors, (*owner), texture, THIRD)) used |= THIRD;
		}

		//сначала пытаемся добавить текстуру на тот уровень, на котором есть такие же
		if (!(FIRST & used) && FIRST & taken) cur_level = FIRST;
		else if(!(SECOND & used) && SECOND & taken) cur_level = SECOND;
		else if(!(THIRD & used) && THIRD & taken) cur_level = THIRD;
		else if(!(FIRST & used))  cur_level = FIRST;
		else if(!(SECOND & used)) cur_level = SECOND;
		else if(!(THIRD & used))  cur_level = THIRD;
		else throw(exception(/*нельзя добавлять текстуру - либо все варианты перебраны, либо такую текстуру никогда нельзя вставить (этот throw не надо удалять)*/));
		
		if (n.y > 0){
			(*owner)(n.x, n.y - 1).give(cur_level, texture);
		}
		used |= cur_level;
		memory[taken] |= cur_level;
		textures[cur_level / 2] = texture;

		if ((FIRST | SECOND | THIRD) ^ (taken | used)){//какой-то уровень остался и его можно было бы использовать при другой расстановке
			can_be_used_again_ = true;
		} else can_be_used_again_ = false;
	}
	bool can_be_used_again() const {
		if (!used) throw(exception("после отладки программы этот throw надо удалить"));
		return can_be_used_again_;
	}

	bool was_here(){
		return memory[taken] & cur_level;
	}

	void clear(){
		used = NONE; taken = NONE; edge_of_space ? 0 : (given = NONE); cur_level = NONE; can_be_used_again_ = true;
	}
	void clear_memory(){
		for (auto& i: memory){
			i = 0;
		}		
	}

	void set_texture(const T& texcure){
		if (this->texture != texcure){
			clear();
			clear_memory();
			this->texture = texcure;
		}
	}
	inline T  get_texture() throw(exception){
		if (!cur_level) throw(exception("текстура в узле не определена"));
		return textures[cur_level/2];
	}
	void set(const T& texture, const node& n){
		set_texture(texture); this->n = n;
		valuable_neighbors = move_to_vector(make_neighbors(n, owner->get_w(), owner->get_h(), (neighbor_number)(N1 | N2 | N7 | N8)));
	}
};

matrix< info >* info::owner = nullptr;

matrix< array<pair<T, bool>, 3> > infos_to_res(matrix< info >& infos){
	auto w = infos.get_w(), h = infos.get_h();
	matrix< array<pair<T, bool>, 3> > res(w, h);
	limits limitless;
	list< node > neighbors;
	for (T x = 0; x < w; x++){
		for (T y = 0; y < h; y++){
			auto& inf = infos(x, y);
			///////////////////////////
			neighbors = make_neighbors(x, y, w, h, (neighbor_number)(N3 | N4| N5 | N6));
			for (const auto& n : neighbors){
				inf.give(infos(n.x, n.y).cur_level, infos(n.x, n.y).get_texture());
			}
			///////////////////////////
			auto& res_inf = res(x, y);
			for (int i = 0; i < 3; i++) res_inf[i].first = inf.textures[i];
			res_inf[inf.cur_level/2].second = true;
		}
	}

	return std::move(res);
}
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

#define NUMBER_OF_VARIABLE_COLUMNS 3

matrix< array<pair<T, bool>, 3> > get_textures_arrangement(const matrix<T>& source) throw(exception){
	T w = source.get_w(), h = source.get_h();
	//пока сделаем тупо: отбор узлов которые могут меняться, но они не обязательно влияют на текущий узел (могут находиться ниже диагональки), а потом надо и это сделать...)
	node barrier;//за этот барьер нельзя заступать
	matrix< info > infos(w, h);

	//инициализация
	info::owner = &infos;
	for (T x = 0; x < w; x++){
		for (T y = 0; y < h; y++){
			infos(x, y).set(source(x, y), node(x, y));
		}
	}
	
	list<node> usable_nodes/*еще могут менять уровень*/;
	list<info> buffer/*хранит лучций варифнт*/;
	node n, end_n;

	for (T x = 0; x < w; x++){
		//если сейчас используется больше NUMBER_OF_VARIABLE_COLUMNS столбцов для перебора вариантов
		if(x - barrier.x >= NUMBER_OF_VARIABLE_COLUMNS){
			//подвинуть барьер
			barrier = node(x - (NUMBER_OF_VARIABLE_COLUMNS - 1), 0);
			//удалить старые узлы из usable_nodes
			while (usable_nodes.size() && *usable_nodes.rbegin() < barrier) usable_nodes.pop_back();
		}

		for (T y = 0; y < h; y++){
			infos(x, y).clear();
			try{
				infos(x, y).use();
				if (infos(x, y).can_be_used_again()) usable_nodes.push_front(node(x, y));
			} catch(const exception&) {
				if (buffer.size()){
					if (node(x, y) > buffer.begin()->n){
						buffer.clear();
						buffer.push_front(infos(x, y));
					}
				} else buffer.push_front(infos(x, y));

				if (usable_nodes.size()){
					n = *usable_nodes.begin(), end_n = buffer.rbegin()->n;
					//записать в buffer информацию про узлы с узла buffer.rbegin()->get_node() по узел n
					if (n < end_n) if (buffer.size() > 1 && buffer.rbegin()->n.y) infos(buffer.rbegin()->n.x, buffer.rbegin()->n.y - 1).give(buffer.rbegin()->cur_level, buffer.rbegin()->get_texture());//это надо!!!!
					while (n < end_n){
						if (end_n.y != 0) end_n.y--;
						else {
							end_n.y = h - 1;
							end_n.x--;
						}
						buffer.push_back(infos(end_n.x, end_n.y));
					}
					infos(n.x, n.y).use();
					if (!infos(n.x, n.y).can_be_used_again()) usable_nodes.pop_front();
					x = n.x; y = n.y;
				} else {
					//все варианты перебрали. возвращаем все, что в буфере назад
					for (const auto& i : buffer){
						infos(i.n.x, i.n.y) = i;
					}
					if (buffer.size() > 1 && buffer.rbegin()->n.y) infos(buffer.rbegin()->n.x, buffer.rbegin()->n.y - 1).give(buffer.rbegin()->cur_level, buffer.rbegin()->get_texture());//это надо!!!!
					//надо ставить новый цвет
					//левый сосед посередине смежный со всеми остальными соседями этого узла - это значит, что не ошибемся, если поставим туже тексткрк и на тот же уровень, что и у левого соседа
					//левый сосед есть, так как если бы его не было - не возникло бы проблем с выбором цвета (мы сюда не пришли бы)
					n = buffer.begin()->n;
					barrier = n;
					infos(n.x, n.y).set_texture(infos(n.x - 1, n.y).get_texture());
					infos(n.x, n.y).use();
					x = n.x; y = n.y;
				}
			}
		}
	}

	return infos_to_res(infos);

}

#include <stdlib.h>
#include <time.h>
//#include <set>
#include <map>

//изменить в матрицу, которая может иметь решение
void change_to_matrix_that_may_be_parsed_to_res(matrix< T >& res){
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

matrix< array<pair<T, bool>, 3> > gen_solution_data(const T& w, const T& h, const T& n_textures) throw(exception){
	if (w > 10000 || h > 10000 || n_textures > 200) throw(exception("1"));
	if (w == 0 || h == 0 || n_textures == 0) throw(exception("2"));

	srand ((unsigned int)time(NULL));

	matrix< info > infos(w, h);

	info::owner = &infos;
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
				infos(x, y).set(tmp(x, y), node(x, y));
			}
		}
	
	}

	list< node > neighbors;
	T number, texture;

#define SET_TEXTURE(info_){\
		try{\
			info_.use();\
		}\
		catch(const exception&){\
			info_.set_texture(infos(info_.n.x - 1, info_.n.y).get_texture());\
			info_.use();\
		}\
	}

	for (T x = 0; x < w; x++){
		T y = 0;
		if (x == 0 && y == 0){
			SET_TEXTURE(infos(x, y));
			y++;
		}
		 
		for (; y < h; y++){
			if(rand() % 4){
				neighbors = make_neighbors(x, y, w, h, (neighbor_number)(N1 | N2 | N7 | N8));
				number = rand() % neighbors.size();
				for (const auto& i : neighbors){
					if (!number--){
						try{
							texture = infos(i.x, i.y).get_texture();
							infos(x, y).set_texture(texture);
						} catch (const exception&){}
						break;
					}
				}
			}
			SET_TEXTURE(infos(x, y));
		}
	}

	return infos_to_res(infos);
}