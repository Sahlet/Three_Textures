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

const T max_T = T(0u - 1) /*_UI32_MAX*/;
const T MAX_DISTANCE = 4, MIN_DISTANCE = 1;
const T MAX_COUNT_OF_USABLE = 15;

typedef bool TAG;
//template<class TAG = void*>
class info{//информация про узел
public:
	enum level : char {NONE = 0, FIRST = 1, SECOND = 2, THIRD = 4};
	TAG tag;
	class Texture {
		char count;
		T texture;//значимые только те текстуры, которые не равны max_T
	public:
		Texture() : count(), texture(max_T){}
		inline void operator --(int){
			if (texture == max_T){
				cerr << "if (texture == max_T)" << endl;
			}
			if (!count){
				cerr << "if (!count)" << endl;
			}
			if (--count == 0) texture = max_T;
		}
		inline bool can_give(const T& texture)const{
			if (texture == max_T){
				cerr << "if (texture == max_T)" << endl;
			}
			if (this->texture == max_T || this->texture == texture) return true;
			return false;
		}
		inline void give(const T& texture){
			if (!can_give(texture)){
				cerr << "if (!can_give(texture))" << endl;
			}
			if (!count++) this->texture = texture;
		}
		inline char get_count(){ return count; }
		inline T get_texture(){ return texture; }
		inline bool operator < (const Texture& t)const{
			return (this->count < t.count || (this->count == t.count && this->texture < t.texture));
		}
		inline bool operator == (const Texture& t)const{
			return  (this->count == t.count && this->count == t.count);
		}
	};
	struct Level {
		level l;
		T texture;
		Level() : l(NONE), texture(max_T){}
		Level(const T& texture) : l(NONE), texture(texture){
			if (texture == max_T){
				cerr << "if (texture == max_T)" << endl;
			}
		}
		Level& operator = (const level& l){
			this->l = l;
			return *this;
		}
	};
private:
	bool free;//показывает, является ли этот узел таким, что вокруг него много узлов с таким же цветом
	level used;//уровни, что были использованы этим узлом
	typedef array< Texture , NUMBER_OF_LEVELS> INFO;
	INFO filling;//заполнение
	Level cur_level;//показывает на каком уровне сейчас стоит 1

	vector< info* > neighbors;
	//template<> struct less< INFO >
	struct INFO_less{
		bool operator()(const INFO & l_INFO, const INFO & r_INFO){
			for (T i = 0; i < NUMBER_OF_LEVELS; i++){
				if (!(l_INFO[i] == r_INFO[i])) return l_INFO[i] < r_INFO[i];
			}
			return false;
		}
	};
	map< INFO , level, INFO_less> memory;


	inline bool can_give(const int& l, const T& texture) const{
		if(!l){
			cerr << "if(!l)" << endl;
		}
		if (filling[l/2].can_give(texture)) return true;
		return false;
	}
	inline void give(const char& l, const T& texture){
		if (!can_give(l, texture)){
			cerr << "if (!can_give(l, texture))" << endl;
		}
		filling[l/2].give(texture);
	}
	
	void remove_from_the_level(){
		if (!cur_level.l) return;
		for (auto i : neighbors){
			i->filling[cur_level.l/2]--;
		}
		this->filling[cur_level.l/2]--;
		cur_level.l = NONE;
	}
	bool can_be_used(level& l) const {
		if (free) return false;
		if (!(l = level((FIRST | SECOND | THIRD) ^ used))) return false;

		struct tmp_struct{
			static bool func(const vector< info* >& neighbors, const T& texture, const level& l){
				for (const auto& i : neighbors){
					if (!i->can_give(l, texture)) return false;
				}
				return true;
			}
		};

		if ((l & FIRST) && !this->can_give(FIRST, cur_level.texture)) l = level(l ^ FIRST);
		if ((l & SECOND) && !this->can_give(SECOND, cur_level.texture)) l = level(l ^ SECOND);
		if ((l & THIRD) && !this->can_give(THIRD, cur_level.texture)) l = level(l ^ THIRD);

		if ((l & FIRST) && !tmp_struct::func(neighbors, cur_level.texture, FIRST)) l = level(l ^ FIRST);
		if ((l & SECOND) && !tmp_struct::func(neighbors, cur_level.texture, SECOND)) l = level(l ^ SECOND);
		if ((l & THIRD) && !tmp_struct::func(neighbors, cur_level.texture, THIRD)) l = level(l ^ THIRD);

		if (!l) return false;
		return true;
	}
	inline bool check_on_free(){
		//такую сложность можно позволить, так как эта функция будет редко вызываться
		for (auto i : neighbors){
			for (auto j : i->neighbors){
				if (j->cur_level.texture != cur_level.texture) return (free = false);
				
			}
		}
		free = true;
		clear();
		return true;
	}
public:

	info() : free(false), used (NONE), filling(), tag(){}
	info(const node& n, matrix< info/*< TAG >*/ >& owner, const T& texture) : cur_level(texture), free(false), filling(), tag(), used (NONE){
		set_neighbors(n, owner);
	}
	info(const info& inf) : 
		used(inf.used),
		filling(inf.filling),
		memory(inf.memory),
		free(inf.free),
		tag(inf.tag){
			if (cur_level.texture == max_T) neighbors = inf.neighbors;
			cur_level = inf.cur_level;
	}
	info& operator=(const info& inf){
		used = inf.used;
		filling = inf.filling;
		memory = inf.memory;
		free = inf.free;
		tag = inf.tag;
		if (cur_level.texture == max_T) neighbors = inf.neighbors;
		cur_level = inf.cur_level;
		return *this;
	}
	info(info&& inf) :
		used(inf.used),
		filling(inf.filling),
		memory(std::move(inf.memory)),
		free(inf.free),
		tag(std::move(inf.tag)){
			if (cur_level.texture == max_T) neighbors = std::move(inf.neighbors);
			cur_level = inf.cur_level;
	}
	info& operator=(info&& inf){
		used = inf.used;
		filling = inf.filling;
		memory = std::move(inf.memory);
		free = inf.free;
		tag = std::move(inf.tag);
		if (cur_level.texture == max_T) neighbors = std::move(inf.neighbors);
		cur_level = inf.cur_level;
		return *this;
	}
	
	inline bool can_be_used() const {
		level l;
		return can_be_used(l);
	}
	inline void clear(){
		remove_from_the_level();
		used = NONE;
	}
	//текстура должна стать на любой свободный уровень, который еще не использован
	inline bool use(){
		if (free) return true;
		level can_be_cur;
		if (!can_be_used(can_be_cur)) return false;
		
		remove_from_the_level();

		//сначала пытаемся добавить текстуру на тот уровень, на котором есть такие же
		if ((FIRST & can_be_cur) && filling[0].get_count()) cur_level = FIRST;
		else if((SECOND & can_be_cur) && filling[0].get_count()) cur_level = SECOND;
		else if((THIRD & can_be_cur) && filling[0].get_count()) cur_level = THIRD;
		else if(FIRST & can_be_cur)  cur_level = FIRST;
		else if(SECOND & can_be_cur) cur_level = SECOND;
		else if(THIRD & can_be_cur)  cur_level = THIRD;
		else {
			cerr << "impossible" << endl;//потом удалить
		}
		
		for (auto i : neighbors){
			i->give(cur_level.l, cur_level.texture);
		}
		this->give(cur_level.l, cur_level.texture);

		used = level(used | cur_level.l);

		return true;
	}
	inline bool was_used(){
		return bool(used);
	}

	inline bool was_here(){
		if (free) return true;
		if (!used){
			cerr << "if (!used)" << endl;
		}
		
		return memory[filling] & cur_level.l;
		//return false;
	}
	inline void remember(){
		if (free) return;
		auto& level_ = memory[filling];
		level_ = level(level_ | cur_level.l);
	}
	inline void clear_memory(){
		memory.clear();
	}

	void set_texture(const T& texcure){
		if (texcure == max_T){
			cerr << "if (texcure == max_T)" << endl;
		}
		if (cur_level.texture != texcure){
			clear();
			clear_memory();
			cur_level.texture = texcure;
			//не экономно, но по-другому сложно и это относительно редко используется (не жалко)
			for (auto i : neighbors){
				for (auto j : i->neighbors){
					if (!j->used){
						if (j->free) j->free = false;
						else j->check_on_free();
					}
				}
			}
		}
	}
	void set_texture(const T& texcure, const node& n, matrix< info/*< TAG >*/ >& owner){
		if (texcure == max_T){
			cerr << "if (texcure == max_T)" << endl;
		}
		if (cur_level.texture != texcure){
			clear();
			clear_memory();
			cur_level.texture = texcure;
			if (n.x + 2 < owner.get_w()){
				T x = n.x + 2, max_y = min(owner.get_h() - 1, n.y + 2);
				for (T y = n.y > 2 ? n.y - 2 : 0; y <= max_y; y++){
					if (owner(x, y).free) owner(x, y).free = false;
					else owner(x, y).check_on_free();
				}
			}
		}
	}
	void set_neighbors(const node& n, matrix<info /*< TAG >*/ >& owner){
		auto neighbors_x_y = make_neighbors(n, owner.get_w(), owner.get_h());
		neighbors = vector<info*>(neighbors_x_y.size());
		auto iter = neighbors_x_y.begin();
		for (size_t i = 0; i < neighbors.size(); i++, iter++){
			neighbors[i] = &owner(iter->x, iter->y);
		}
	}
	inline Level get_cur_level(){ return cur_level; }
	inline T get_texture(){ return cur_level.texture; }
	inline T operator[](const size_t& i){ return filling[i].get_texture(); }

	static void set_free(matrix< info /*< TAG >*/ >& owner){
		T w = owner.get_w(), h = owner.get_h(), texture;
		for (T x = 0; x < w; x++){
			for (T y = 0; y < h; y++){
				owner(x, y).free = true;
				texture = owner(x, y).cur_level.texture;
				for (int x_ = int(x) - 2; x_ <= x + 2; x_++){
					for (int y_ = int(y) - 2; y_ <= y + 2; y_++){
						if (x_ >= 0 && y_ >= 0 && x_ < w && y_ < h){
							if (owner(x_, y_).cur_level.texture != texture){
								owner(x, y).free = false;
								x_ = x + 2;
								break;
							}
						}
					}
				}
				if (owner(x, y).free) owner(x, y).clear();
			}
		}
	}
	inline void use_free(){
		if (!free || used) return;
		free = false;
		use();
	}
	inline bool get_free(){ return free; }
};

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

matrix< array<pair<T, bool>, 3> > infos_to_res(matrix< info/*< TAG >*/ >& infos){
	auto w = infos.get_w(), h = infos.get_h();
	matrix< array<pair<T, bool>, 3> > res(w, h);

	for (T x = 0; x < w; x++){
		for (T y = 0; y < h; y++){
			infos(x, y).use_free();
		}
	}

	for (T x = 0; x < w; x++){
		for (T y = 0; y < h; y++){
			auto& inf = infos(x, y);
			auto& res_inf = res(x, y);
			for (int i = 0; i < 3; i++) res_inf[i].first = inf[i] != max_T ? inf[i] : 0;
			if (!inf.get_cur_level().l){
				cerr << "if (!inf.get_cur_level().l)" << endl;
			}
			res_inf[inf.get_cur_level().l/2].second = true;
		}
	}

	return std::move(res);
}

template<class Type> struct value_saver{
	Type value, &ref;
	value_saver(Type& ref) : value(ref), ref(ref){}
	~value_saver(){
		ref = value;
	}
};

matrix< array<pair<T, bool>, 3> > get_textures_arrangement(const matrix<T>& source) {
	T w = source.get_w(), h = source.get_h();
	matrix< info/*< TAG >*/ > infos(w, h);

	//инициализация
	for (T x = 0; x < w; x++){
		for (T y = 0; y < h; y++){
			infos(x, y) = info/*< TAG >*/(node(x, y), infos, source(x, y));
		}
	}
	info/*< TAG >*/::set_free(infos);
	
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

	struct sub_matrix{
		node goal;//целевой узел
		list< node > usable_nodes/*еще могут менять уровень*/;
		matrix< info/*< TAG >*/ > m;//хранит
		sub_matrix(/*const node& barrier, */const matrix< info/*< TAG >*/ > infos, const node& n_, T max_distance_x, T max_distance_y, const T& max_count_of_usable) :
			m(1 + min(T(1 + max_distance_x), n_.x) + min(max_distance_y, T(infos.get_w() - 1 - n_.x)), 1 + min(T(1 + max_distance_y), n_.y) + min(1 + max_distance_x, infos.get_h() - 1 - n_.y))
		{
			node n1, n2, *n_ptr, LU_point(n_.x - min(T(1 + max_distance_x), n_.x), n_.y - min(T(1 + max_distance_y), n_.y));
			goal = node(n_.x - LU_point.x, n_.y - LU_point.y);
			T new_max_y = /*min (infos.get_h() - 1 - LU_point.y, */m.get_h() - 1/*)*/;
			max_distance_x = min(max_distance_x, goal.x);
			max_distance_y = min(max_distance_y, goal.y);
			auto max_distance = max(max_distance_x, max_distance_y);

			node start, end(m.get_w() - 1, m.get_h() - 1), cur;
			T &x = cur.x, &y = cur.y;
			next_pred iter(start, end, cur);
			while (!(cur > end)){
				(m(x, y) = infos(LU_point.x + x, LU_point.y + y)).set_neighbors(cur, m);
				iter.next();
			}

			for (T distance = MIN_DISTANCE; distance <= max_distance && usable_nodes.size() < max_count_of_usable; distance++){
				n1.y = min((T)(goal.y + distance), new_max_y);
				n1.x = (goal.x >= distance) && (distance <= max_distance_x) ? goal.x - distance : n1.y = 0;
				n2.x = goal.x;
				n2.y = (goal.y >= distance) && (distance <= max_distance_y) ? goal.y - distance : n2.x = 0;
				do{
					n_ptr = n1 > n2 ? &n1 : &n2;//надо, чтоб было в начале цикла
					//
					//useful code
					if (m(n_ptr->x, n_ptr->y).can_be_used())
						usable_nodes.push_back(*n_ptr);
					//
					if (n_ptr == &n1) n_ptr->y--;
					else n_ptr->x--;
				} while (n_ptr->distance(goal) == distance && /**n_ptr > barrier && */usable_nodes.size() < max_count_of_usable);
			}

			//if (usable_nodes.size() && *usable_nodes.rbegin() == node(0, 0)) usable_nodes.pop_back();
		}
		info/*< TAG >*/& operator[](const node& n){return m(n.x, n.y);}
		void save_to(matrix<info /*< TAG >*/ >& infos, const node& n_){
			node LU_point(n_.x - goal.x, n_.y - goal.y);
			
			node start, end(m.get_w() - 1, m.get_h() - 1), cur;
			T &x = cur.x, &y = cur.y;
			next_pred iter(start, end, cur);
			while (!(cur > end)){
				infos(LU_point.x + x, LU_point.y + y) = m(x, y);
				iter.next();
			}

		}
	};

		static bool fill_infos(matrix< info/*< TAG >*/ >& infos, node start, const node& goal, const bool& may_change_texture = false){
			node end(goal.x, min(goal.x + goal.y - start.x, infos.get_h() - 1));
			//node barrier = start;//за этот барьер нельзя заступать
			node cur = start;
			T &x = cur.x, &y = cur.y, w = end.x - start.x + 1, h = end.y - start.y + 1;

			next_pred iter(start, end, cur);

			while (!(goal < cur)){
				if (infos(x, y).use()) infos(x, y).remember();
				else {
					info* problem_info = &infos(x, y);
					bool solved = false;
					auto problem_node = cur;
					//#pragma region try_to_solve
					//{
					//	////сохранение имеющегося результата (маленькая область)
					//	//sub_matrix saver(/*barrier, */infos, problem_node, min(1, x - start.x) + 1, min(1, y - start.y) + 1, 0);
					//	//sub_matrix saver(/*barrier, */infos, problem_node, min(2, x - start.x) + 1, min(2, y - start.y) + 1, 0);
					//	
					//	/*тут барьер использовать не надо потому, что маленькая сложность*/
					//	value_saver< node > old_start(start);
					//	value_saver< node > old_cur(cur);
					//	//прежде чем менять, подумай!(сейчас все ок)
					//	//тут устанавливается новый старт, чтоб перебрать ближайшие узлы для решения проблемы
					//	auto neighbors = make_neighbors(x - old_start.value.x, y - old_start.value.y, w, h, (neighbor_number)(N1 | N2 | N7 | N8));//приходит список соседей в порядке по часовой стрелке начиная с первого (смотри make_neighbors)
					//	if (neighbors.size()){
					//		for (const auto &i : neighbors){
					//			if (i < *neighbors.begin()) *neighbors.begin() = i;
					//		}
					//		start = node(neighbors.begin()->x + old_start.value.x, neighbors.begin()->y + old_start.value.y);
					//		neighbors = make_neighbors(start.x - old_start.value.x, start.y - old_start.value.y, 1 + end.x - old_start.value.x, 1 + end.y - old_start.value.x, (neighbor_number)(N1 | N2 | N7 | N8));//приходит список соседей в порядке по часовой стрелке начиная с первого
					//		if (neighbors.size()){
					//			for (const auto &j : neighbors){
					//				if (j < *neighbors.begin()) *neighbors.begin() = j;
					//			}
					//			start = node(neighbors.begin()->x + old_start.value.x, neighbors.begin()->y + old_start.value.y);
					//		}
					//	}
					//	
					//	#pragma region fill_infos_SOLVING()
					//	#define fill_infos_SOLVING()\
					//	{\
					//		bool use = false;\
					//		while(!solved){\
					//			while (cur != start){\
					//				infos(x, y).clear();\
					//				iter.pred();\
					//				if (use = infos(x, y).use()){\
					//					infos(x, y).remember();\
					//					break;\
					//				}\
					//			}\
					//			if (!use) break;\
					//			while (cur < problem_node){\
					//				iter.next();\
					//				if (infos(x, y).use()) infos(x, y).remember();\
					//				else break;\
					//			}\
					//			if (cur == problem_node && problem_info->was_used()) solved = true;\
					//		}\
					//	}
					//	#pragma endregion
					//	
					//	fill_infos_SOLVING();
					//	
					//	//if (!solved) saver.save_to(infos, problem_node);
					//	
					//	//следующий if вместо сохранения старой расстановки в saver (так быстрее)
					//	if (!solved){
					//		cur = start;
					//		while (cur < problem_node){
					//			infos(x, y).clear();
					//			iter.next();
					//		}
					//		value_saver < node > old_problem_node(problem_node);
					//		value_saver < info* > old_problem_info(problem_info);
					//		iter.pred();
					//		problem_node = cur;
					//		problem_info = &infos(x, y);
					//		cur = start;
					//		iter.next();
					//		fill_infos_SOLVING();
					//		if (!solved) cerr << "if (!solved)" << endl;
					//		solved = false;
					//	}
					//}
					//#pragma endregion
					
					/*if (!solved && cur.distance(start) > 2)*/{
						sub_matrix sub_(/*barrier, */infos, cur, min((int)MAX_DISTANCE, x - start.x), min((int)MAX_DISTANCE, y - start.y), MAX_COUNT_OF_USABLE);
						if (solved = /*не менять на ==*/ funcs::solve_the_problem(sub_)) {
							sub_.save_to(infos, node(x, y));
							//barrier = node(x, y);
						}
					}

					if (!solved && may_change_texture){
						if (x != 0) problem_info->set_texture(infos(x - 1, y).get_texture(), cur, infos);
						else problem_info->set_texture(infos(x, y - 1).get_texture(), cur, infos);
						problem_info->use();
						problem_info->remember();
						solved = true;
					}
					
					if (!solved){
						cur = start;
						while (cur < problem_node){
							infos(x, y).clear();
							iter.next();
						}
						return false;
					}
				}
				iter.next();
			}
			return true;
		}

		//граница (реализовует смысл меморизации - отсеивание уже проверенных вариантов перебора)
		class edge {
			info/*< TAG >*/ *inf;
			unique_ptr< edge > left, right; //правый узел старше (right < left) - его обрабатывать первым
			//bool &solved;
			edge(const edge&);
		public:

			edge(matrix< info /*< TAG >*/ >& infos, const node& n/*, bool& solved*/) : inf(&infos(n.x, n.y))/*, solved(solved)*/{
				if (inf->tag){
					cerr << "if (inf.tag)" << endl;
				}
				inf->tag = true;
				if (n.y > 0 && !infos(n.x, n.y - 1).tag) right.reset(new edge(infos, node(n.x, n.y - 1)/*, solved*/));
				if (n.x > 0 && n.y + 1 < infos.get_h() && !infos(n.x - 1, n.y + 1).tag) left.reset(new edge(infos, node(n.x - 1, n.y + 1)/*, solved*/));
			}
			edge(edge&& e) : inf(e.inf), /*solved(e.solved), */left(std::move(e.left)), right(std::move(e.right)) {
				e.inf = nullptr;
			}
			void clear(){
				inf->clear();
				if (right) right->clear();
				if (left) left->clear();
			}
			bool next_use(){
				if (!(left || right)){
					if (inf->get_free()) return false;
					if (inf->use()) return true;
					inf->clear();
					return false;
				}

				if (inf->was_used() && inf->use()) return true;
				if (!inf->was_used()){
					if (right && !right->next_use()) return false;
					if (!left && inf->use()) return true;
				} else inf->clear();

				if (left && right){
					while(true) {
						if (!left->next_use()){
							if (!right->next_use()) return false;
						} else if (inf->use()) return true;
					}
				} else if (left){
					while(true) {
						if (!left->next_use()) return false;
						if (inf->use()) return true;
					}
				} else /*if (right)*/{
					while(true) {
						if (!right->next_use()) return false;
						if (inf->use()) return true;
					}
				}
				return false;
			}

			//использовать только тогда, когда edge чистое (если next_use() или use() вернул false, то не нужно вызывать clear потому, что edge уже чистый)
			bool use(){
				if (!(left || right)) return inf->use();

				if (left && right){
					if (!right->use()) return false;
					else if (!left->use()){
						if (!right->next_use()) return false;
					} else if (inf->use()) return true;
					while(true) {
						if (!left->next_use()){
							if (!right->next_use()) return false;
						} else if (inf->use()) return true;
					}
				} else if (left){
					if (!left->use()) return false;
					if (inf->use()) return true;
					while(true) {
						if (!left->next_use()) return false;
						if (inf->use()) return true;
					}
				} else /*if (right)*/{
					if (!right->use()) return false;
					if (inf->use()) return true;
					while(true) {
						if (!right->next_use()) return false;
						if (inf->use()) return true;
					}
				}

				return false;
			}
			bool was_here(){
				bool res = true;
				if (right) res &= right->was_here();
				if (left) res &= left->was_here();

				if (!inf->was_here()){
					res = false;
					inf->remember();
				}
				return res;
			}
			~edge(){
				//if (!solved) inf.clear();
				if (inf) inf->tag = false;
			}
		};

		//решает проблему с выбором уровня для структуры так, чтоб удовлетворить условиям, которые выполнены с помощью sub_matrix
		static bool solve_the_problem (sub_matrix& m){
			if (!m.usable_nodes.size()) return false;
			T distance = 0;
			T w = m.m.get_w(), h = m.m.get_h();
			T max_x = w - 1, max_y = h - 1;
			node max_child;
			node last = m.goal;
			list< pair< edge, node > > enumerating_nodes;//перебираемые узлы
			list< pair< edge, node > >::iterator iter;
			//bool solved = false used;
			bool used;

			static int i = 0;

			for (T y = 0; y < h; y++){
				for (T x = 0; x < w; x++){
					m.m(x, y).tag = true;
				}
			}

			if (i == 23){
				int i = 0;
			}

			auto make_max_child = [w, h, &m](const node& n) -> node{
				node max_child;
				
				for (const auto& child : make_neighbors(n, w, h, (neighbor_number)(N3 | N4 | N5 | N6))){
					if (!(child > m.goal) && child.x <= m.goal.x && max_child < child) max_child = child;
				}

				if (max_child == node(0, 0)){
					cerr << "if (max_kid == node(0, 0))" << endl;
				}

				return max_child;
			};
			
			for (auto root_node : m.usable_nodes){
				#pragma region preparing
				T last_distance = last.distance(m.goal);
				last = root_node;

				if (distance != root_node.distance(m.goal)){
					distance = root_node.distance(m.goal);
					if (m.goal.x < max_x && m.goal.y > 1){
						for (T i = 0, y = m.goal.y - min(m.goal.y, distance); (y + 2 <= m.goal.y) && m.m(m.goal.x + 1, y).was_used(); y++, i++){
							for (T x = m.goal.x + 1; x <= min(max_x, T(m.goal.x + distance - 1 - i)); x++){
								m.m(x, y).clear();
							}
						}
					}
				}

				node n1(root_node.x, root_node.y + 1), n2, *n_ptr = &n1;

				for (; *n_ptr != root_node; last_distance++){
					n1.y = min((T)(m.goal.y + last_distance), max_y);
					n1.x = (m.goal.x >= last_distance) ? m.goal.x - last_distance : n1.y = 0;
					n2.x = m.goal.x;
					n2.y = (m.goal.y >= last_distance) ? m.goal.y - last_distance : n2.x = 0;
					do{
						n_ptr = n1 > n2 ? &n1 : &n2;//надо, чтоб было в начале цикла
						if (*n_ptr == root_node) break;
						//
						//useful code
						m[*n_ptr].clear();
						m[*n_ptr].tag = 0;
						//
						if (n_ptr == &n1) n_ptr->y--;
						else n_ptr->x--;
					} while (n_ptr->distance(m.goal) == last_distance);
				}
				#pragma endregion

				enumerating_nodes.clear();

				if (!m[root_node].use()){
					cerr << "if (m[root_node].use())" << endl;
					continue;
				}
				m[root_node].remember();
				
				max_child = make_max_child(root_node);
				enumerating_nodes.push_front( pair< edge, node > (edge(m.m, max_child/*, solved*/), max_child));

				do {
					iter = --enumerating_nodes.end();
					if (!iter->first.use()) continue;
					used = true;
					while (true) {
						while (iter->first.was_here() && (used = iter->first.next_use())){}
						if (!used){
							do {
								if(++iter == enumerating_nodes.end()) break;
							} while (!(used = iter->first.next_use()));
							if (!used) break;
							else continue;
						}
						if (iter->second == m.goal){
							if (distance == 1 || m.goal.x == max_x || m.goal.y <= 1 ||
								fill_infos(m.m, 
								/*start*/node(min(max_x, T(m.goal.x + 1)), m.goal.y - min(m.goal.y, distance)),
								/*goal*/node(min(max_x, T(m.goal.x + distance - 1)), (m.goal.y - min(m.goal.y, distance) + (T(m.goal.x + distance - 1) > max_x ? T(m.goal.x + distance - 1) - max_x : 0))))){
									//solved = true;
									cout << "true " << ++i << endl;
									if (i == 24){
										int i = 0;
									}
									return true;
							}
							if (iter->first.next_use()) continue;
						} else {
							if (iter == enumerating_nodes.begin()){
								max_child = make_max_child(iter->second);
								enumerating_nodes.push_front(pair< edge, node > (edge(m.m, max_child/*, solved*/), max_child));
							}
							if ((--iter)->first.use()) continue;
						}
						
						do {
							if(++iter == enumerating_nodes.end()) break;
						} while (!(used = iter->first.next_use()));
						if (!used) break;
					}
				} while (m[root_node].use());
			}
			cout << "false " << ++i << endl;
			return false;
		}
	};

	funcs::fill_infos(infos, node(0, 0), node(infos.get_w() - 1, infos.get_h() - 1), true);

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