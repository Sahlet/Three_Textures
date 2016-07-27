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

//template<class _T>
//list<_T> move_to_list(vector<_T> &&vec){
//	auto size = vec.size();
//	list<_T> res(size);
//	auto iter = res.begin();
//	for (size_t i = 0; i < size; i++){
//		*iter++ = std::move(vec[i]);
//	}
//	return res;
//}
//template<class _T>
//vector<_T> move_to_vector(list<_T> &&list){
//	auto size = list.size();
//	vector<_T> res(size);
//	auto iter = list.begin();
//	for (size_t i = 0; i < size; i++){
//		res[i] = std::move(*iter++);
//	}
//	return res;
//}

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

//изменить в матрицу, которая может иметь решение
void change_to_matrix_that_may_be_parsed_to_res(matrix< T >& res){
	map< T , int > textures;
	list< node > neighbors;
	T w = res.get_w(), h = res.get_h(), max_x = w - 1, max_y = h - 1;
	T texture1 = 0, texture2;
	int min, max;
	for (T x = max_x > 1 ? 1 : 0; x < max_x; x++){
		for (T y = max_y > 1 ? 1 : 0; y < max_y; y++){
			textures.clear();
			neighbors = make_neighbors(x, y, w, h);
			neighbors.push_front(node(x, y));
			for (const auto& i : neighbors){
				textures[res(i.x, i.y)]++;
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
matrix< T > res_to_source(const  matrix< array<pair<T, bool>, 3> >& res) {
	auto w = res.get_w(), h = res.get_h();
	matrix< T > source(w, h);
	for (T x = 0; x < w; x++) {
		for (T y = 0; y < h; y++) {
			source(x, y) = res(x, y)[0].first;
			if (res(x, y)[1].second) source(x, y) = res(x, y)[1].first;
			else if (res(x, y)[2].second) source(x, y) = res(x, y)[2].first;
		}
	}

	return std::move(source);
}

const T max_T = /*T(0u - 1)*/ _UI32_MAX;
const T MAX_DISTANCE = 6, MIN_DISTANCE = 3;
const T MAX_COUNT_OF_USABLE = 20;

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
			return this->texture == max_T || this->texture == texture;
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
		operator bool() { return bool(l); }
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
		bool operator()(const INFO & l_INFO, const INFO & r_INFO) const {
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
	inline bool give(const char& l, const T& texture){
		if (!can_give(l, texture)){
			cerr << "if (!can_give(l, texture))" << endl;
			return false;
		}
		filling[l/2].give(texture);
		return true;
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
		l = level((FIRST | SECOND | THIRD) ^ used);
		if (!l) return false;

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
	info(const T& texture) : cur_level(texture), free(false), filling(), tag(), used (NONE){}
	info(const info& inf) :
		cur_level(inf.cur_level),
		used(inf.used),
		filling(inf.filling),
		memory(inf.memory),
		free(inf.free),
		tag(inf.tag){}
	info& operator=(const info& inf){
		used = inf.used;
		filling = inf.filling;
		memory = inf.memory;
		free = inf.free;
		tag = inf.tag;
		cur_level = inf.cur_level;
		return *this;
	}
	info(info&& inf) :
		cur_level(inf.cur_level),
		used(inf.used),
		filling(inf.filling),
		memory(std::move(inf.memory)),
		free(inf.free),
		tag(std::move(inf.tag)){}
	info& operator=(info&& inf){
		used = inf.used;
		filling = inf.filling;
		memory = std::move(inf.memory);
		free = inf.free;
		tag = std::move(inf.tag);
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
	void set_and_use_the_most_popular_texture_from_neighbors(const node& n, matrix< info/*< TAG >*/ >& owner) {
		map<T, char> counter;
		T old_texture = cur_level.texture;
		clear();
		for (auto neighbour : neighbors) {
			if (neighbour->cur_level.texture != max_T) counter[neighbour->cur_level.texture]++;
		}
		vector< pair<T, char> > textures_in_order(counter.size());
		size_t i = 0;
		for (const auto& val : counter) {
			textures_in_order[i++] = val;
		}

		auto pair_T_char_less = [](const pair< T, char>& l, const pair< T, char>& r) -> bool{
			return l.second < r.second;
		};

		sort(textures_in_order.rbegin(), textures_in_order.rend(), pair_T_char_less);
		for (const auto& val : textures_in_order) {
			cur_level.texture = val.first;
			if ((cur_level.texture != old_texture) && use()) {
				cur_level.texture = old_texture;
				set_texture(val.first, n, owner);
				use();
				return;
			}
		}

		cerr << "impossible happened in set_and_use_the_most_popular_texture_from_neighbours" << endl;
		throw exception();
	}
	void set_neighbors(const node& n, matrix<info /*< TAG >*/ >& owner){
		auto neighbors_x_y = make_neighbors(n, owner.get_w(), owner.get_h());
		neighbors = vector<info*>(neighbors_x_y.size());
		auto iter = neighbors_x_y.begin();
		for (size_t i = 0; i < neighbors.size(); i++, iter++){
			neighbors[i] = &owner(iter->x, iter->y);
		}
	}
	bool refresh_filling() {
		struct fix_if_not_OK{
			bool OK;
			INFO& filling;
			INFO old_filling;
			fix_if_not_OK(INFO& filling) : OK(true), filling(filling), old_filling(filling) {}
			~fix_if_not_OK() {
				if (!OK) filling = old_filling;
			}
		} fixer(filling);
		filling = INFO();
		for (auto i : neighbors){
			if (!i->cur_level) continue;
			if(!(fixer.OK = this->give(i->cur_level.l, i->cur_level.texture))) break;
		}
		if (fixer.OK && this->cur_level) fixer.OK = this->give(this->cur_level.l, this->cur_level.texture);
		return fixer.OK;
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

matrix< array<pair<T, bool>, 3> > infos_to_res(matrix< info/*< TAG >*/ >& infos, bool without_edge){
	auto w = infos.get_w() - without_edge, h = infos.get_h() - without_edge;
	matrix< array<pair<T, bool>, 3> > res(w - without_edge, h - without_edge);

	for (T x = 0 + without_edge; x < w; x++){
		for (T y = 0 + without_edge; y < h; y++){
			infos(x, y).use_free();
		}
	}

	for (T x = 0 + without_edge; x < w; x++){
		for (T y = 0 + without_edge; y < h; y++){
			auto& inf = infos(x, y);
			auto& res_inf = res(x - without_edge, y - without_edge);
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

//#include <fstream>

struct funcs{

	struct next_pred{
		const node& start, &end;
		node &cur;
		next_pred(const node& start, const node &end, node &cur) : start(start), end(end), cur(cur){}
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
	class sub_matrix{
		matrix< info/*< TAG >*/ >& infos;
		matrix< info/*< TAG >*/ > m;//хранит
		const node start, end;
		const node LU_point;
		const node goal;//целевой узел
		list< node > usable_nodes/*еще могут менять уровень*/;

		//граница (реализовует смысл меморизации - отсеивание уже проверенных вариантов перебора)
		class edge {
			info/*< TAG >*/ *inf;
			unique_ptr< edge > left, right; //правый узел старше (right < left) - его обрабатывать первым
											//bool &solved;
			edge(const edge&);
		public:

			edge(matrix< info /*< TAG >*/ >& infos, const node& n/*, bool& solved*/) : inf(&infos(n.x, n.y))/*, solved(solved)*/ {
				if (inf->tag) {
					cerr << "if (inf.tag)" << endl;
				}
				inf->tag = true;
				if (n.y > 0 && !infos(n.x, n.y - 1).tag) right.reset(new edge(infos, node(n.x, n.y - 1)/*, solved*/));
				if (n.x > 0 && n.y + 1 < infos.get_h() && !infos(n.x - 1, n.y + 1).tag) left.reset(new edge(infos, node(n.x - 1, n.y + 1)/*, solved*/));
			}
			edge(edge&& e) : inf(e.inf), /*solved(e.solved), */left(std::move(e.left)), right(std::move(e.right)) {
				e.inf = nullptr;
			}
			void clear() {
				inf->clear();
				if (right) right->clear();
				if (left) left->clear();
			}
			bool next_use() {
				if (!(left || right)) {
					if (inf->get_free()) return false;
					if (inf->use()) return true;
					inf->clear();
					return false;
				}

				if (inf->was_used() && inf->use()) return true;
				if (!inf->was_used()) {
					if (right && !right->next_use()) return false;
					if (!left && inf->use()) return true;
				}
				else inf->clear();

				if (left && right) {
					while (true) {
						if (!left->next_use()) {
							if (!right->next_use()) return false;
						}
						else if (inf->use()) return true;
					}
				}
				else if (left) {
					while (true) {
						if (!left->next_use()) return false;
						if (inf->use()) return true;
					}
				}
				else /*if (right)*/ {
					while (true) {
						if (!right->next_use()) return false;
						if (inf->use()) return true;
					}
				}
				return false;
			}

			//использовать только тогда, когда edge чистое (если next_use() или use() вернул false, то не нужно вызывать clear потому, что edge уже чистый)
			bool use() {
				if (!(left || right)) return inf->use();

				if (left && right) {
					if (!right->use()) return false;
					else if (!left->use()) {
						if (!right->next_use()) return false;
					}
					else if (inf->use()) return true;
					while (true) {
						if (!left->next_use()) {
							if (!right->next_use()) return false;
						}
						else if (inf->use()) return true;
					}
				}
				else if (left) {
					if (!left->use()) return false;
					if (inf->use()) return true;
					while (true) {
						if (!left->next_use()) return false;
						if (inf->use()) return true;
					}
				}
				else /*if (right)*/ {
					if (!right->use()) return false;
					if (inf->use()) return true;
					while (true) {
						if (!right->next_use()) return false;
						if (inf->use()) return true;
					}
				}

				return false;
			}
			bool was_here() {
				bool res = true;
				if (right) res &= right->was_here();
				if (left) res &= left->was_here();

				if (!inf->was_here()) {
					res = false;
					inf->remember();
				}
				return res;
			}
			~edge() {
				//if (!solved) inf.clear();
				if (inf) inf->tag = false;
			}
		};
		
		sub_matrix(const sub_matrix&);
	public:
		//прямоугольник [real_goal.x - l_distance - 1; real_goal.x + r_distance + 1] X [real_goal.y - t_distance - 1; real_goal.y + b_distance + 1] должен входить в infos
		sub_matrix(matrix< info/*< TAG >*/ >& infos, const node& real_goal, T l_distance, const T r_distance, T t_distance, const T b_distance, const T& max_count_of_usable) :
			infos(infos),
			m(1 + ((min(real_goal.x, l_distance) + 1) + (min(T(infos.get_w() - real_goal.x), min(T(r_distance + 1), max(t_distance, T(1)))))),
			  1 + ((min(real_goal.y, t_distance) + 1) + (min(T(infos.get_h() - real_goal.y), T(min(b_distance, l_distance) + 1))))),
			
			start(	(real_goal.x <= l_distance) ? 1 : 0,
					(real_goal.y <= t_distance) ? 1 : 0),
			end((T(infos.get_w() - real_goal.x) <= min(T(r_distance + 1), max(t_distance, T(1)))) ? (m.get_w() - 2) : (m.get_w() - 1),
				(T(infos.get_h() - real_goal.y) <= T(min(b_distance, l_distance) + 1)) ? (m.get_h() - 2) : (m.get_h() - 1)),

			LU_point(real_goal.x - min(real_goal.x, T(1 + l_distance)), real_goal.y - min(real_goal.y, T(1 + t_distance))),
			goal((1 + min(real_goal.x, l_distance)), (1 + min(real_goal.y, t_distance)))
		{
			node cur(start);
			T &x = cur.x, &y = cur.y;
			next_pred iter(start, end, cur);
			while (!(cur > end)){
				(m(x, y) = infos(LU_point.x + x, LU_point.y + y)).set_neighbors(cur, m);
				iter.next();
			}

			if (start.x) for (y = 0, x = 0; y < m.get_h(); y++) { m(x, y).set_neighbors(cur, m); m(x, y).refresh_filling(); }
			if (start.y) for (x = 0, y = 0; x < m.get_w(); x++) { m(x, y).set_neighbors(cur, m); m(x, y).refresh_filling(); }
			if (end.x == m.get_w() - 2) for (y = 0, x = m.get_w() - 1; y < m.get_h(); y++) { m(x, y).set_neighbors(cur, m); m(x, y).refresh_filling(); }
			if (end.y == m.get_h() - 2) for (x = 0, y = m.get_h() - 1; x < m.get_w(); x++) { m(x, y).set_neighbors(cur, m); m(x, y).refresh_filling(); }

			node n1, n2, *n_ptr;
			l_distance = min(real_goal.x, l_distance);
			t_distance = min(real_goal.y, t_distance);
			T new_max_y = m.get_h() - 2, max_distance = max(l_distance, t_distance);

			for (T distance = max(T(MIN_DISTANCE), T(1)); distance <= max_distance && usable_nodes.size() < max_count_of_usable; distance++){
				n1.y = min(T(goal.y + distance), new_max_y);
				n1.x = (goal.x >= distance) && (distance <= l_distance) ? goal.x - distance : n1.y = 0;
				n2.x = goal.x;
				n2.y = (goal.y >= distance) && (distance <= t_distance) ? goal.y - distance : n2.x = 0;
				do{
					n_ptr = n1 > n2 ? &n1 : &n2;//надо, чтоб было в начале цикла
					//
					//useful code
					if (m(n_ptr->x, n_ptr->y).can_be_used())
						usable_nodes.push_back(*n_ptr);
					//
					if (n_ptr == &n1) n_ptr->y--;
					else n_ptr->x--;
				} while (n_ptr->x && n_ptr->y && n_ptr->distance(goal) == distance && usable_nodes.size() < max_count_of_usable);
			}
		}
		info/*< TAG >*/& operator[](const node& n){return m(n.x, n.y);}
		void save(){
			node cur = start;
			T &x = cur.x, &y = cur.y;
			
			next_pred iter(start, end, cur);
			while (!(cur > end)){
				infos(LU_point.x + x, LU_point.y + y) = m(x, y);
				iter.next();
			}



			//////////////////////////////////////////////жесткая проверка на правильность работы
			/*
			cur = start;
			while (!(cur > goal)) {
				infos(LU_point.x + x, LU_point.y + y).clear();
				infos(LU_point.x + x, LU_point.y + y).use();
				iter.next();
			}
			*/
		}
		bool solve_the_problem() {

			//static int i = 0;

			if (!usable_nodes.size()) {
				//cout << "false " << ++i << endl;
				return false;
			}
			T distance = 0, last_distance;
			T max_x = m.get_w() - 1, max_y = m.get_h() - 1;
			node max_child;
			node last = goal;
			list< pair< edge, node > > enumerating_nodes;//перебираемые узлы
			list< pair< edge, node > >::iterator iter;
			bool used;

			for (T y = 0; y <= max_y; y++) {
				for (T x = 0; x <= max_x; x++) {
					m(x, y).tag = true;
				}
			}

			max_x--; max_y--;

			auto make_max_child = [this](const node& n) -> node {
				node max_child;

				for (const auto& child : make_neighbors(n, this->m.get_w(), this->m.get_h(), (neighbor_number)(N3 | N4 | N5 | N6))) {
					if (!m(child.x, child.y).tag && !(child > goal) && child.x <= goal.x && max_child < child) max_child = child;
				}

				if (max_child == node(0, 0)) {
					cerr << "if (max_kid == node(0, 0))" << endl;
				}

				return max_child;
			};

			for (const node root_node : usable_nodes) {
			#pragma region preparing
				last_distance = last.distance(goal);
				last = root_node;

				if (distance != root_node.distance(goal)) {
					distance = root_node.distance(goal);
					if (goal.x < max_x && goal.y > 2) {
						for (T i = 0, y = goal.y - min(T(goal.y - 1), distance); (y + 2 <= goal.y) && m(goal.x + 1, y).was_used(); y++, i++) {
							for (T x = goal.x + 1; x <= min(max_x, T(goal.x + min(T(goal.y - 1), distance) - 1 - i)); x++) {
								m(x, y).clear();
							}
						}
					}
				}

				node n1(root_node.x, root_node.y + 1), n2, *n_ptr = &n1;

				for (; *n_ptr != root_node; last_distance++) {
					n1.y = min(T(goal.y + last_distance), max_y);
					n1.x = (goal.x > last_distance) ? goal.x - last_distance : n1.y = 0;
					n2.x = goal.x;
					n2.y = (goal.y > last_distance) ? goal.y - last_distance : n2.x = 0;
					do {
						n_ptr = n1 > n2 ? &n1 : &n2;//надо, чтоб было в начале цикла
						if (*n_ptr == root_node) break;
						//
						//useful code
						m[*n_ptr].clear();
						m[*n_ptr].tag = 0;
						//
						if (n_ptr == &n1) n_ptr->y--;
						else n_ptr->x--;
					} while (n_ptr->x && n_ptr->y && n_ptr->distance(goal) == last_distance);
				}
				#pragma endregion

				enumerating_nodes.clear();

				if (!m(root_node.x, root_node.y).use()) {
					cerr << "if (m[root_node].use())" << endl;
					continue;
				}
				m(root_node.x, root_node.y).remember();

				max_child = make_max_child(root_node);
				enumerating_nodes.push_front(pair< edge, node >(edge(m, max_child), max_child));

				do {
					iter = --enumerating_nodes.end();
					if (!iter->first.use()) continue;
					used = true;
					while (true) {
						while (iter->first.was_here() && (used = iter->first.next_use(), used)) {}
						if (!used) {
							do {
								if (++iter == enumerating_nodes.end()) break;
							} while (!(used = iter->first.next_use(), used));
							if (!used) break;
							else continue;
						}
						if (iter->second == goal) {
							if (distance == 1 || goal.x == max_x || goal.y <= 2 ||
								fill_infos(m,
									/*start*/node(goal.x + 1, goal.y - min(T(goal.y - 1), distance)),
									/*goal*/node(min(max_x,
										T(goal.x + min(T(goal.y - 1), distance) - 1))
										, (goal.y - min(T(goal.y - 1), distance)
											+ (goal.x + min(T(goal.y - 1), distance) - 1 > max_x
												? (goal.x + min(T(goal.y - 1), distance) - 1 - max_x) : 0
												)
											))
								)) {
								//cout << "true " << ++i << endl;
								return true;
							}
							if (iter->first.next_use()) continue;
						}
						else {
							if (iter == enumerating_nodes.begin()) {
								max_child = make_max_child(iter->second);
								enumerating_nodes.push_front(pair< edge, node >(edge(m, max_child), max_child));
							}
							if ((--iter)->first.use()) continue;
						}

						do {
							if (++iter == enumerating_nodes.end()) break;
						} while (!(used = iter->first.next_use(), used));
						if (!used) break;
					}
				} while (m(root_node.x, root_node.y).use());
			}
			//cout << "false " << ++i << endl;
			return false;
		}
	};
	
	static bool fill_infos(matrix< info/*< TAG >*/ >& infos, node start, const node& goal, const node& end, const bool& may_change_texture = false){
		node cur = start;
		T &x = cur.x, &y = cur.y, w = end.x - start.x + 1, h = end.y - start.y + 1;

		next_pred iter(start, end, cur);

		while (!(goal < cur)){
			if (infos(x, y).use()) infos(x, y).remember();
			else {
				info* problem_info = &infos(x, y);
				bool solved = false;
				auto problem_node = cur;
				#pragma region try_to_solve
				{
					////сохранение имеющегося результата (маленькая область)
					//sub_matrix saver(/*barrier, */infos, problem_node, min(1, x - start.x) + 1, min(1, y - start.y) + 1, 0);
					//sub_matrix saver(/*barrier, */infos, problem_node, min(2, x - start.x) + 1, min(2, y - start.y) + 1, 0);
						
					/*тут барьер использовать не надо потому, что маленькая сложность*/
					value_saver< node > old_start(start);
					value_saver< node > old_cur(cur);
					//прежде чем менять, подумай!(сейчас все ок)
					//тут устанавливается новый старт, чтоб перебрать ближайшие узлы для решения проблемы
					auto neighbors = make_neighbors(x - old_start.value.x, y - old_start.value.y, w, h, (neighbor_number)(N1 | N2 | N7 | N8));//приходит список соседей в порядке по часовой стрелке начиная с первого (смотри make_neighbors)
					if (neighbors.size()){
						for (const auto &i : neighbors){
							if (i < *neighbors.begin()) *neighbors.begin() = i;
						}
						start = node(neighbors.begin()->x + old_start.value.x, neighbors.begin()->y + old_start.value.y);
						neighbors = make_neighbors(start.x - old_start.value.x, start.y - old_start.value.y, 1 + end.x - old_start.value.x, 1 + end.y - old_start.value.x, (neighbor_number)(N1 | N2 | N7 | N8));//приходит список соседей в порядке по часовой стрелке начиная с первого
						if (neighbors.size()){
							for (const auto &j : neighbors){
								if (j < *neighbors.begin()) *neighbors.begin() = j;
							}
							start = node(neighbors.begin()->x + old_start.value.x, neighbors.begin()->y + old_start.value.y);
						}
					}
					#pragma region fill_infos_SOLVING()
					#define fill_infos_SOLVING()\
					{\
						bool use = false;\
						while(!solved){\
							while (cur != start){\
								infos(x, y).clear();\
								iter.pred();\
								use = infos(x, y).use();\
								if (use){\
									infos(x, y).remember();\
									break;\
								}\
							}\
							if (!use) break;\
							while (cur < problem_node){\
								iter.next();\
								if (infos(x, y).use()) infos(x, y).remember();\
								else break;\
							}\
							if (cur == problem_node && problem_info->was_used()) solved = true;\
						}\
					}
					#pragma endregion
						
					fill_infos_SOLVING();
						
					//if (!solved) saver.save_to(infos, problem_node);
						
					//следующий if вместо сохранения старой расстановки в saver (так быстрее)
					if (!solved && problem_node != start){
						cur = start;
						while (cur < problem_node){
							infos(x, y).clear();
							iter.next();
						}
						value_saver < node > old_problem_node(problem_node);
						value_saver < info* > old_problem_info(problem_info);
						iter.pred();
						problem_node = cur;
						problem_info = &infos(x, y);
						cur = start;
						iter.next();
						fill_infos_SOLVING();
						if (!solved) cerr << "if (!solved)" << endl;
						solved = false;
					}
				}
				#pragma endregion
					
				if (!solved && cur.distance(start) > 2){
					sub_matrix sub_(infos, cur,
						/*l_distance*/min(MAX_DISTANCE, T(x - start.x)),
						/*r_distance*/end.x - x,
						/*t_distance*/min(MAX_DISTANCE, T(y - start.y)),
						/*b_distance*/end.y - y,
						MAX_COUNT_OF_USABLE);
					if (solved = sub_.solve_the_problem()) sub_.save();
				}

				if (!solved && may_change_texture){
					problem_info->set_and_use_the_most_popular_texture_from_neighbors(cur, infos);
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
	static bool fill_infos(matrix< info/*< TAG >*/ >& infos, const node start, const node& goal, const bool& may_change_texture = false) {
		node end(goal.x, min(goal.y + goal.x - start.x, infos.get_h() - 1));
		return fill_infos(infos, start, goal, end, may_change_texture);
	}
};

matrix< array<pair<T, bool>, 3> > get_textures_arrangement(matrix<T>& source) {
	/*auto err = ofstream("Three_Textures_ERRORs.txt");
	cerr.rdbuf(err.rdbuf());*/
	/*auto out = ofstream("Three_Textures_out.txt");
	cout.rdbuf(out.rdbuf());*/

	change_to_matrix_that_may_be_parsed_to_res(source);

	matrix< info/*< TAG >*/ > infos(source.get_w(), source.get_h());

	//инициализация
	{
		node cur;
		T &x = cur.x, &y = cur.y;
		auto iter = source.begin();
		for (y = 0; y < source.get_h(); y++) {
			for (x = 0; x < source.get_w(); x++, iter++) {
				(infos(x, y) = info/*< TAG >*/(*iter)).set_neighbors(cur, infos);
			}
		}
	}
	info/*< TAG >*/::set_free(infos);

	funcs::fill_infos(infos, node(0, 0), node(infos.get_w() - 1, infos.get_h() - 1), true);

	return infos_to_res(infos, false);
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

	//change_to_matrix_that_may_be_parsed_to_res(res);

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