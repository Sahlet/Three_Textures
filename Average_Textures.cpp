//Average_Textures.cpp


#include "Average_Textures.h"
#include <iostream>
#include <list>
#include <map>
#include <functional>
#include <memory>
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
	inline bool operator<(const node& n) const {
		if ((x + y < n.x + n.y) || ((x + y == n.x + n.y) && (y > n.y))) return true;
		return false;
	}
	inline bool operator==(const node& n) const {
		return y == n.y && x == n.x;
	}
	inline bool operator!=(const node& n) const {
		return !(*this == n);
	}
	inline bool operator>(const node& n) const {
		return !(*this == n || *this < n);
	}
	inline T distance(const node& n) const {
		return max((x > n.x ? x - n.x : n.x - x), (y > n.y ? y - n.y : n.y - y));
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
	if (limitless & limits::UP){
		if (limitless & limits::LEFT) neighbors.push_back(node(x - 1, y - 1));
		neighbors.push_back(node(x, y - 1));
		if (limitless & limits::RIGHT) neighbors.push_back(node(x + 1, y - 1));
	}
	if (limitless & limits::RIGHT) neighbors.push_back(node(x + 1, y));
	if (limitless & limits::DOWN){
		if (limitless & limits::RIGHT) neighbors.push_back(node(x + 1, y + 1));
		neighbors.push_back(node(x, y + 1));
		if (limitless & limits::LEFT) neighbors.push_back(node(x - 1, y + 1));
	}
	if (limitless & limits::LEFT) neighbors.push_back(node(x - 1, y));
	
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
//приходит список соседей в порядке по часовой стрелке начиная с первого
list< node > make_neighbors(const T& x, const T& y, const T& w, const T& h, const neighbor_number& neighbors_number){
	limits limitless = (limits)(
			(y > 0) | 
			((y + 1 < h) << 1) | 
			((x > 0) << 2) | 
			((x + 1 < w) << 3) );

	list< node > neighbors;
	if (limitless & limits::UP){
		if (neighbors_number & neighbor_number::N1 && limitless & limits::LEFT) neighbors.push_back(node(x - 1, y - 1));
		if (neighbors_number & neighbor_number::N2) neighbors.push_back(node(x, y - 1));
		if (neighbors_number & neighbor_number::N3 && limitless & limits::RIGHT) neighbors.push_back(node(x + 1, y - 1));
	}
	if (neighbors_number & neighbor_number::N4 && limitless & limits::RIGHT) neighbors.push_back(node(x + 1, y));
	if (limitless & limits::DOWN){
		if (neighbors_number & neighbor_number::N5 && limitless & limits::RIGHT) neighbors.push_back(node(x + 1, y + 1));
		if (neighbors_number & neighbor_number::N6) neighbors.push_back(node(x, y + 1));
		if (neighbors_number & neighbor_number::N7 && limitless & limits::LEFT) neighbors.push_back(node(x - 1, y + 1));
	}
	if (neighbors_number & neighbor_number::N8 && limitless & limits::LEFT) neighbors.push_back(node(x - 1, y));
	
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
	char used;//использованные уровни
	mutable char taken;//уровни, взятые в предыдущих соседних узлах.
	char given;//уровни, которые дали следующие узлы.
	/*в given будем помещать только уровень, который дал один следующий узел, а в taken все возможные (в соответствии с его описанием см.выше.)*/
	T texture;
	int cur_level;//показывает на каком уровне сейчас стоит 1
	mutable array< T , 3 > textures;//(значимые только те текстуры, которые отмечены в taken или в given или в cur_level)
	map< array< T , 3 > , char> memory;

	vector< info* > valuable_neighbors;//узлы, от которых зависит этот.
	vector< info* > depending_neighbors;//узлы, которые зависят от этого.

public:
	bool free;//показывает, является ли этот узел таким, что вокруг него много узлов с таким же цветом
	static const T max_T;

///////////////////////////////////////////

	info() : texture(max_T) /*, free(false), used (NONE), taken (NONE), given (NONE), cur_level (NONE)*/ {}
	info(const node& n, matrix<info>& owner, const T& texture) : texture(texture), free(false), used (NONE), taken (NONE), given (NONE), cur_level (NONE){
		if (texture == max_T) throw exception();
		for (size_t i = 0; i < textures.size(); i++) textures[i] = max_T;
		set_neighbors(n, owner);
	}
	info(const info& inf) : 
		used(inf.used),
		taken(inf.taken),
		given(inf.given),
		cur_level(inf.cur_level),
		textures(inf.textures),
		memory(inf.memory),
		free(inf.free){
			if (texture == max_T){
				valuable_neighbors = inf.valuable_neighbors;
				depending_neighbors = inf.depending_neighbors;
			}
			texture = inf.texture;
	}
	info& operator=(const info& inf){
		used = inf.used;
		taken = inf.taken;
		given = inf.given;
		cur_level = inf.cur_level;
		textures = inf.textures;
		memory = inf.memory;
		free = inf.free;
		if (texture == max_T){
			valuable_neighbors = inf.valuable_neighbors;
			depending_neighbors = inf.depending_neighbors;
		}
		texture = inf.texture;
		return *this;
	}
	info(info&& inf) : 
		used(inf.used),
		taken(inf.taken),
		given(inf.given),
		cur_level(inf.cur_level),
		textures(inf.textures),
		memory(std::move(inf.memory)),
		free(inf.free){
			if (texture == max_T){
				valuable_neighbors = std::move(inf.valuable_neighbors);
				depending_neighbors = std::move(inf.depending_neighbors);
			}
			texture = inf.texture;
	}
	info& operator=(info&& inf){
		used = inf.used;
		taken = inf.taken;
		given = inf.given;
		cur_level = inf.cur_level;
		textures = inf.textures;
		memory = std::move(inf.memory);
		free = inf.free;
		if (texture == max_T){
			valuable_neighbors = std::move(inf.valuable_neighbors);
			depending_neighbors = std::move(inf.depending_neighbors);
		}
		texture = inf.texture;
		return *this;
	}
	
	inline bool give(const char& l, const T& texture){
		if(!l){
			cerr << "if(!l)" << endl;
			throw(exception("после отладки программы этот throw надо удалить"));
		}
		//следующий узел хочет записать текстуру texture на уровень l и вызывает эту функцию.
		if (l & (taken | given | cur_level)){
			if (textures[l/2] != texture) return false/*нельзя добавлять текстуру*/;
		} else textures[l/2] = texture;
		given |= l;
		return true;
	}
	inline bool can_give(const int& l, const T& texture) const{
		if(!l){
			cerr << "if(!l)" << endl;
			throw(exception("после отладки программы этот throw надо удалить"));
		}
		//некоторый узел спрашивает, можно ли на уровень l поставить текстуру texture.
		if (l & (taken | given | cur_level)){
			if (textures[l/2] != texture) return false/*нельзя добавлять текстуру*/;
		}
		return true;
	}
	
	void remove_from_the_level(){
		if (!cur_level) return;
		bool can_remove_from_this_accepting_neighbor;
		for (auto i : valuable_neighbors){
			if (!(i->given & cur_level)) continue;
			can_remove_from_this_accepting_neighbor = true;
			for (auto j : i->depending_neighbors){
				if (j != this && j->texture == texture && j->cur_level == cur_level){
					can_remove_from_this_accepting_neighbor = false;
					break;
				}
			}
			if (can_remove_from_this_accepting_neighbor){
				i->given ^= cur_level;
				if (!((i->cur_level == cur_level) || (i->taken & cur_level)))i->textures[cur_level/2] = max_T;
			}
		}
		textures[cur_level/2] = max_T;
		cur_level = NONE;
	}
	
	bool can_be_used() const {
		level l;
		return can_be_used(l);
	}
	bool can_be_used(level& l) const {
		l = level((FIRST | SECOND | THIRD) ^ used);
		if (free || !l) return false;

		struct tmp_struct{
			static bool func(const vector< info* >& neighbors, const T& texture, const level& l){
				for (const auto& i : neighbors){
					if (!i->can_give(l, texture)) return false;
				}
				return true;
			}
		};

		if ((l & FIRST) && !tmp_struct::func(valuable_neighbors, texture, FIRST)) l = level(l ^ FIRST);
		if ((l & SECOND) && !tmp_struct::func(valuable_neighbors, texture, SECOND)) l = level(l ^ SECOND);
		if ((l & THIRD) && !tmp_struct::func(valuable_neighbors, texture, THIRD)) l = level(l ^ THIRD);

		if (!l) return false;
		return true;
	}
	void clear(){
		remove_from_the_level();
		used = NONE;
		taken = NONE;
		given = NONE;
		cur_level = NONE;
		free = false;
		for (size_t i = 0; i < textures.size(); i++) textures[i] = max_T;
	}
	//текстура должна стать на любой свободный уровень, который еще не использован
	inline bool use(){
		if (free) return true;
		level can_be_cur;
		if (!can_be_used(can_be_cur)) return false;

		if (!taken){
			//определяем taken
			char l;
			for (const auto& i : valuable_neighbors){
				//i может быть free
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
		}
		
		if (used) remove_from_the_level();

		//сначала пытаемся добавить текстуру на тот уровень, на котором есть такие же
		if ((FIRST & can_be_cur) && (FIRST & taken)) cur_level = FIRST;
		else if((SECOND & can_be_cur) && (SECOND & taken)) cur_level = SECOND;
		else if((THIRD & can_be_cur) && (THIRD & taken)) cur_level = THIRD;
		else if(FIRST & can_be_cur)  cur_level = FIRST;
		else if(SECOND & can_be_cur) cur_level = SECOND;
		else if(THIRD & can_be_cur)  cur_level = THIRD;
		else {
			cerr << "impossible" << endl;//потом удалить
			throw exception();
		}
		
		for (auto i : valuable_neighbors){
			i->give(cur_level, texture);
		}

		used |= cur_level;
		textures[cur_level / 2] = texture;

		return true;
	}

	bool was_here(){
		return memory[textures] & cur_level;
	}
	void remember(){		
		memory[textures] |= cur_level;
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
	void set_neighbors(const node& n, matrix<info>& owner){
		auto neighbors = make_neighbors(n, owner.get_w(), owner.get_h(), neighbor_number(N1 | N2 | N7 | N8));
		valuable_neighbors = vector<info*>(neighbors.size());
		auto iter = neighbors.begin();
		for (size_t i = 0; i < valuable_neighbors.size(); i++, iter++){
			valuable_neighbors[i] = &owner(iter->x, iter->y);
		}

		neighbors = make_neighbors(n, owner.get_w(), owner.get_h(), neighbor_number(N3 | N4 | N5 | N6));
		depending_neighbors = vector<info*>(neighbors.size());
		iter = neighbors.begin();
		for (size_t i = 0; i < depending_neighbors.size(); i++, iter++){
			depending_neighbors[i] = &owner(iter->x, iter->y);
		}
	}
	T get_texture(){ return texture; }
	T get_cur_level(){ return cur_level; }
	T operator[](const size_t& i){ return textures[i]; }
};

const T info::max_T = 0u - 1;
const T MAX_DISTANCE = 5;
const T MAX_COUNT_OF_USABLE = 15;

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

matrix< array<pair<T, bool>, 3> > infos_to_res(matrix< info >& infos){
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
					infos(n.x, n.y).give(inf.get_cur_level(), inf.get_texture());
				}
			}
			neighbors = make_neighbors(x, y, w, h, (neighbor_number)(N5 | N6));
			for (const auto& n : neighbors){
				if (!infos(n.x, n.y).free) inf.give(infos(n.x, n.y).get_cur_level(), infos(n.x, n.y).get_texture());
			}
			///////////////////////////
			auto& res_inf = res(x, y);
			for (int i = 0; i < 3; i++) res_inf[i].first = inf[i] != info::max_T ? inf[i] : 0;
			res_inf[inf.get_cur_level()/2].second = true;
		}
	}

	return std::move(res);
}

matrix< array<pair<T, bool>, 3> > get_textures_arrangement(const matrix<T>& source) {
	T w = source.get_w(), h = source.get_h();
	matrix< info > infos(w, h);

	//инициализация
	for (T x = 0; x < w; x++){
		for (T y = 0; y < h; y++){
			infos(x, y) = info(node(x, y), infos, source(x, y));
		}
	}
	//не надо совмещать этот цикл и предыдущий
	for (T x = 0; x < w; x++){
		for (T y = 0; y < h; y++){
			infos(x, y).free = true;
			for (int x_ = int(x) - 2; x_ <= x + 2; x_++){
				for (int y_ = int(y) - 2; y_ <= y + 2; y_++){
					if (x_ >= 0 && y_ >= 0){
						if (infos(x_, y_).get_texture() != infos(x, y).get_texture()){
							infos(x, y).free = false;
							x_ = x + 2;
							break;
						}
					}
				}
			}
		}
	}
	
	struct funcs{

		struct next_pred{
			node& start, &end, &cur;
			next_pred(node& start, node &end, node &cur) : start(start), end(end), cur(cur){}
			void pred(){
				if (cur.x > start.x && cur.y < end.y){
					cur.x--; cur.y++;
				} else {
					T diagonal = cur.x + cur.y;
					if (diagonal <= end.x + start.y){
						cur.y = start.y;
						cur.x = diagonal - cur.y - 1;
					} else {
						cur.x = end.x;
						cur.y = diagonal - cur.x - 1;
					}
				}
			}
			void next(){
				if (cur.x < end.x && cur.y > start.y){
					cur.x++; cur.y--;
				} else {
					T diagonal = cur.x + cur.y;
					if (diagonal < end.y + start.x){
						cur.x = start.x;
						cur.y = diagonal - cur.x + 1;
					} else {
						cur.y = end.y;
						cur.x = diagonal - cur.y + 1;
					}
				}
			}
		};

		struct node_tree {
			node n;
			unique_ptr< node_tree > left, right;//правый узел ставше (right < left)
			node_tree(const node& n) : n(n){}
		};

	struct sub_matrix{
		matrix<info> m;//хранит
		node goal;//целевой узел
		list< pair< node , unique_ptr< node_tree > > > usable_nodes/*еще могут менять уровень*/;
		sub_matrix(const node& barrier, const matrix<info> infos, const node& n_, T max_distance_x, T max_distance_y, const T& max_count_of_usable) : m(1, 1){
			T size = 0, count_of_usable = 0, max_x = infos.get_w() - 1, max_y = infos.get_h() - 1;
			max_distance_x = min(max_distance_x, goal.x);
			max_distance_y = min(max_distance_y, goal.y);
			auto max_distance = max(max_distance_x, max_distance_y);
			node n1, n2, *n_ptr, LU_point(n_.x - min(1 + max_distance_x, int(n_.x)), n_.y - min(1 + max_distance_y, int(n_.y))), tmp;
			m = matrix<info>(min(1 + max_distance_x, int(n_.x)) + min(1 + max_distance_y, max_x - n_.x), 1 + /*этот + 1 тут надо, а в первом аргументе он не надо*/min(1 + max_distance_y, (int)n_.y) + min(1 + max_distance_x, max_y - n_.y));
			T new_w = m.get_w(), new_h = m.get_h();
			goal = node(n_.x - LU_point.x, n_.y - LU_point.y);

			T diagonals_number = goal.x + goal.y + 1, x, y;
			for (T diagonal = 1; diagonal <= diagonals_number; diagonal++){
				if (diagonal < new_h){
					y = diagonal;
					x = 0;
				} else {
					y = new_h;
					x = diagonal - new_h;
				}
				for (;y-- > 0 && x < new_w; x++){
					(m(x, y) = infos(LU_point.x + x, LU_point.y + y)).set_neighbors(node(x, y), m);
				}
			}

			for (T distance = 3; distance <= max_distance && usable_nodes.size() < max_count_of_usable; distance++){
				n1.y = min((T)(n_.y + distance), max_y);
				n1.x = (n_.x >= distance) && (distance <= max_distance_x) ? n_.x - distance : n1.y = 0;
				n2.x = n_.x;
				n2.y = (n_.y >= distance) && (distance <= max_distance_y) ? n_.y - distance : n2.x = 0;
				do{
					n_ptr = n1 > n2 ? &n1 : &n2;//надо, чтоб было в начале цикла
					tmp.x = n_ptr->x - LU_point.x;
					tmp.y = n_ptr->y - LU_point.y;
					if (m(tmp.x, tmp.y).can_be_used()) usable_nodes.push_back( pair< node , unique_ptr< node_tree > >(tmp, nullptr));
					if (n_ptr == &n1) n_ptr->y--;
					else n_ptr->x--;
				} while (n_ptr->distance(goal) == distance && !(*n_ptr < barrier) && usable_nodes.size() < max_count_of_usable);
				//} while (n_ptr->distance(goal) == distance && /**n_ptr > barrier && */usable_nodes.size() < max_count_of_usable);
			}
		}
		info& operator[](const node& n){return m(n.x, n.y);}
		void save_to(matrix<info>& infos, const node& n_){
			node LU_point(n_.x - goal.x, n_.y - goal.y);
			T diagonals_number = goal.x + goal.y + 1, w = m.get_w(), h = m.get_h(), x, y;
			for (T diagonal = 1; diagonal <= diagonals_number; diagonal++){
				if (diagonal < h){
					y = diagonal;
					x = 0;
				} else {
					y = h;
					x = diagonal - h;
				}
				for (;y-- > 0 && x < w; x++){
					infos(LU_point.x + x, LU_point.y + y) = m(x, y);
				}
			}
		}
	};

		struct node_saver{
			node old_n, &n;
			node_saver(node& n) : n(n), old_n(n){}
			~node_saver(){
				n = old_n;
			}
		};

		static bool fill_infos(matrix<info>& infos, node start, const node& goal, const bool& may_change_texture = false){
			node end(goal.x, min(goal.x + goal.y - start.x, infos.get_h() - 1));
			node barrier = start;//за этот барьер нельзя заступать
			node cur = start;
			T &x = cur.x, &y = cur.y;

			next_pred iter(start, end, cur);
			while (!(goal < cur)){
				infos(x, y).clear();
				iter.next();
			}
			cur = start;

			while (!(goal < cur)){
				if (infos(x, y).use()) infos(x, y).remember();
				else {
					bool solved = false;
					#pragma region try_to_solve
					{
						/*тут барьер использовать не надо потому, что маленькая сложность*/
						node_saver old_start(start);
						auto problem_node = cur;//прежде чем менять, подумай!(сейчас все ок)
						auto neighbors = make_neighbors(x - old_start.old_n.x, y - old_start.old_n.y, 1 + end.x - old_start.old_n.x, 1 + end.y - old_start.old_n.x, (neighbor_number)(N1 | N2 | N7 | N8));//приходит список соседей в порядке по часовой стрелке начиная с первого
						if (neighbors.size()){
							for (const auto &i : neighbors){
								if (i < *neighbors.begin()) *neighbors.begin() = i;
							}
							start = node(neighbors.begin()->x + old_start.old_n.x, neighbors.begin()->y + old_start.old_n.y);
							neighbors = make_neighbors(start.x - old_start.old_n.x, start.y - old_start.old_n.y, 1 + end.x - old_start.old_n.x, 1 + end.y - old_start.old_n.x, (neighbor_number)(N1 | N2 | N7 | N8));//приходит список соседей в порядке по часовой стрелке начиная с первого
							if (neighbors.size()){
								for (const auto &i : neighbors){
									if (i < *neighbors.begin()) *neighbors.begin() = i;
								}
								start = node(neighbors.begin()->x + old_start.old_n.x, neighbors.begin()->y + old_start.old_n.y);
							}
						}
						
						bool use = false;
						while(!solved){
							do{
								infos(x, y).clear();
								iter.pred();
								if (use = infos(x, y).use()){
									infos(x, y).remember();
									break;
								}
							} while (cur != start);
							
							if (!use) break;

							while (cur < problem_node){
								iter.next();
								if (infos(x, y).use()) infos(x, y).remember();
								else break;
							}
							if (cur == problem_node) solved = true;
						}
					}
					#pragma endregion
					
					if (!solved && cur.distance(start) > 2){
						sub_matrix sub_(barrier, infos, cur, min((int)MAX_DISTANCE, x - start.x), min((int)MAX_DISTANCE, y - start.y), MAX_COUNT_OF_USABLE);
						if (solved = /*не менять на ==*/ funcs::solve_the_problem(sub_)) {
							sub_.save_to(infos, node(x, y));
							barrier = node(x, y);
						}
					}

					if (!solved && may_change_texture){
						infos(x, y).set_texture(infos(x - 1, y).get_texture());
						infos(x, y).use();
						infos(x, y).remember();
						for (int x_ = (int)x - 2; x_ <= x + 2; x_++){
							for (int y_ = (int)y - 2; y_ <= y + 2; y_++){
								if (x_ >= 0 && y_ >= 0){
									if (infos(x_, y_).free){
										infos(x_, y_).free = false;//это удалять нельзя
										if (node(x_, y_) < cur){
											//этот код, как мне кажется, не должен никогда выполниться
											//тут вообще должно быть исправление infos(x_, y_); его нужно исправить, так как он уже не free (нужно что-то типа infos(x_, y_).use(), но учитывать всех соседе, а не только значимых)
											cerr << "if (node(x_, y_) < cur)" << endl;
										}
									}
								}
							}
						}
						solved = true;
					}
					
					if (!solved) return false;
				}
				iter.next();
			}
			return true;
		}

		//решает проблему с выбором уровня для структуры так, чтоб удовлетворить условиям, которые выполнены с помощью sub_matrix
		static bool solve_the_problem (sub_matrix& m){
			T distance = 0;
			T w = m.m.get_w(), h = m.m.get_h();
			auto max_y = h - 1, max_x = w - 1;
			matrix<bool> marks(1, 1);
			node LU_point, start, end, cur, last = m.goal, curent_usable;
			next_pred iter(start, end, cur);

			std::function< unique_ptr< node_tree > (const node& n) > build;
			std::function< bool(node_tree&) > next_use;
			std::function< bool(node_tree&) > add_to_usable_nodes;
			std::function< void(node_tree&) > remember_all;
			std::function< void(node_tree&) > clear_all;
			//build = [&m, &marks, &max_y, &next_use, &LU_point, &distance, &curent_usable, &build](const node& n) throw(exception) -> unique_ptr< node_tree >{
			build = [&](const node& n) throw(exception) -> unique_ptr< node_tree >{
				unique_ptr< node_tree > res(new node_tree(n));
				node tmp(n.x, n.y - 1);
				#define REPEATABLE_CODE_IN_build_and_clean(left_or_right)\
				if (tmp > curent_usable && m.goal.distance(tmp) <= distance && !marks.get(tmp.x - LU_point.x, tmp.y - LU_point.y)){\
					marks.set(tmp.x - LU_point.x, tmp.y - LU_point.y, true);\
					res->left_or_right = build(tmp);\
				}

				if (n.y > 0 ) REPEATABLE_CODE_IN_build_and_clean(right);
				if (n.x > 0 && n.y < max_y) REPEATABLE_CODE_IN_build_and_clean(left);
				m[n].clear();
				if (!next_use(*res)) return nullptr;
				return std::move(res);
			};
			next_use = [&next_use, &m](node_tree& tree) -> bool {
				while (true){
					if (m.m[tree.n].use()) return true;
					else {
						m.m[tree.n].clear();
						if (!((tree.left && next_use(*tree.left)) || (tree.right && next_use(*tree.right)))) return false;
					}
				}
			};
			add_to_usable_nodes = [&add_to_usable_nodes, &m](node_tree& tree){
				bool res = false;
				if (tree.right) res |= add_to_usable_nodes(*tree.right);
				if (tree.left) res |= add_to_usable_nodes(*tree.left);

				if (res |= !m[tree.n].was_here()) m.usable_nodes.push_front( pair< node, unique_ptr< node_tree > >(tree.n, nullptr));
				return res;
			};
			remember_all = [&remember_all, &m](node_tree& tree){
				m[tree.n].remember();
				if (tree.right) remember_all(*tree.right);
				if (tree.left) remember_all(*tree.left);
			};
			clear_all = [&clear_all, &m](node_tree& tree){
				if (tree.right) clear_all(*tree.right);
				if (tree.left) clear_all(*tree.left);
				m[tree.n].clear();
			};

			while (m.usable_nodes.size()){
				curent_usable = m.usable_nodes.begin()->first;
				while (m[curent_usable].was_here() && !m.usable_nodes.begin()->second){
					if (!m[curent_usable].can_be_used()){
						m.usable_nodes.pop_front();
						if (m.usable_nodes.size()) curent_usable = m.usable_nodes.begin()->first;
						else return false;
					} else m[curent_usable].use();
				}

				unique_ptr< node_tree >& edge = m.usable_nodes.begin()->second;//край (облипляет curent_usable), сделан из "детей", правй указатель старше левого, а корень младше того и того.

				if (edge) {
					try {
						next_use(*edge);
					} catch (const exception&){
						clear_all(*edge);
						while (m[curent_usable].can_be_used()){
							m[curent_usable].use();
							if (!m[curent_usable].was_here()){
								if (next_use(*edge)) break;
								else m[curent_usable].remember();
							}
						}
						if (m[curent_usable].was_here()){
							m.usable_nodes.pop_front();
							continue;
						} else { m[curent_usable].remember(); }
					}
				} else {
					m[curent_usable].remember();
					
					if (distance < curent_usable.distance(m.goal)){
						distance = curent_usable.distance(m.goal);
						marks = matrix<bool>(min(distance, m.goal.x) + 1, min(distance, m.goal.y) + min(int(distance), m.m.get_h() - m.goal.y - 1) + 1);
						LU_point = node(m.goal.x - min(distance, m.goal.x), m.goal.y - min(distance, m.goal.y));

						/*
							где-то тут надо сделать очистку детей узла, чтоб его можно было use

							так, чтоб еще правый треугольник очистился, а потом где-то надо еще вставить очистку детей при переходе междк usable_nodes
						*/
						iter.start = LU_point;
						iter.end = ;
					}
					
					{
						node max_child;//зависит от curent_usable и от всех остальных детей (если они есть)
						
						for (const auto& child : make_neighbors(curent_usable, w, h, (neighbor_number)(N3 | N4 | N5 | N6))){
							if (child < m.goal && child.x <= m.goal.x && max_child < child) max_child = child;
						}
						if (!(edge = build(max_child))) continue;
						//так получается частичная упорядоченность и можно применить какую-то несложную рекурсивную функцию для того, чтоб выполнить use() всеу узлам, что в edge
					}

					if (edge->n == m.goal){
						remember_all(*edge);
						if (distance == 1 || (m.goal.x == max_x && m.goal.y <= 1) ||
							fill_infos(m.m, 
							/*start*/node(m.goal.x + 1, m.goal.y - min(m.goal.y, distance)),
							/*end*/node(m.goal.x + distance - 1, m.goal.y - min(m.goal.y, distance))))
							return true;
					}
				}
				
				add_to_usable_nodes(*edge);

			}
			return false;
		}
	};

	funcs::fill_infos(infos, node(0, 0), node(infos.get_w() - 1, infos.get_h() - 1), true);

	return infos_to_res(infos);
}

matrix< array<pair<T, bool>, 3> > gen_solution_data(const T& w, const T& h, const T& n_textures) {
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
				infos(x, y).set_neighbors(node(x, y), infos);
			}
		}
	
	}

	list< node > neighbors;
	T number, texture;

#define SET_TEXTURE(x_, y_){\
		if (!infos(x_, y_).use()){\
			infos(x_, y_).set_texture(infos(x_ - 1, y_).get_texture());\
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
							texture = infos(i.x, i.y).get_texture();
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