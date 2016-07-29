//Average_Textures.cpp

#include "Average_Textures.h"
#include <iostream>
#include <memory>
#include <stdlib.h>
#include <time.h>


using namespace std;



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
			for (int i = 0; i < NUMBER_OF_LEVELS; i++) res_inf[i].first = ((inf[i] != max_T) ? inf[i] : 0);
			if (!inf.get_cur_level().l){
				cerr << "if (!inf.get_cur_level().l)" << endl;
				//throw std::exception();
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

#pragma region problem_solving
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
			edge(const edge&) = delete;
		public:

			edge(matrix< info /*< TAG >*/ >& infos, const node& n) : inf(&infos(n.x, n.y)){
				if (inf->tag) {
					cerr << "if (inf.tag)" << endl;
					//throw std::exception();
				}
				inf->tag = true;
				if (n.y > 0 && !infos(n.x, n.y - 1).tag) right.reset(new edge(infos, node(n.x, n.y - 1)));
				else
				if (n.x > 0 && !infos(n.x - 1, n.y).tag) right.reset(new edge(infos, node(n.x - 1, n.y)));
				if (n.x > 0 && n.y + 1 < infos.get_h() && !infos(n.x - 1, n.y + 1).tag) left.reset(new edge(infos, node(n.x - 1, n.y + 1)/*, solved*/));
			}
			edge(edge&& e) : inf(e.inf), left(std::move(e.left)), right(std::move(e.right)) {
				e.inf = nullptr;
			}
			void clear() {
				inf->clear();
				if (right) right->clear();
				if (left) left->clear();
			}

			//использовать только тогда, когда все узлы дерева заюзаны (не очищены) или free. это условие выполнено, после вызова use или next_use, который вернул true.
			bool next_use() {
				if (!(left || right)) {
					if (inf->get_free()) return false;
					if (inf->use()) return true;
					inf->clear();
					return false;
				}

				if (inf->was_used() && inf->use()) return true;
				if (!inf->get_free()) inf->clear();

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
			void check_on_was_used() {
				if (right) right->check_on_was_used();
				if (left) left->check_on_was_used();

				if (inf->was_used()) {
					std::cerr << "if (inf->was_used())" << std::endl;
					//throw std::exception();
				}
			}
			~edge() {
				//if (!solved) inf.clear();
				if (inf) inf->tag = false;
			}
		};
		class smart_edge : private edge {
			node n;
			bool used;
			smart_edge(const smart_edge&) = delete;
		public:
			smart_edge(matrix< info /*< TAG >*/ >& infos, const node& n) : edge(infos, n), n(n), used(false){}
			smart_edge(smart_edge&& e) : edge(std::move(e)), n(e.n) , used(e.used){}
			inline bool use() {
				if (used) return used = edge::next_use();
				edge::check_on_was_used();
				return used = edge::use();
			}
			inline bool was_here() { return edge::was_here(); }
			inline node get_n() { return n; }
		};

		sub_matrix(const sub_matrix&) = delete;
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
				(m(x, y) = infos(LU_point.x + x - start.x, LU_point.y + y - start.y)).set_neighbors(cur, m);
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
			while (!(cur > end)) {
				infos(LU_point.x + x - start.x, LU_point.y + y - start.y) = m(x, y);
				iter.next();
			}



			//////////////////////////////////////////////жесткая проверка на правильность работы
			
			cur = start;
			while (!(cur > goal)) {
				infos(LU_point.x + x - start.x, LU_point.y + y - start.y).clear();
				infos(LU_point.x + x - start.x, LU_point.y + y - start.y).use();
				iter.next();
			}
			
		}
		bool solve_the_problem() {

			//static int i = 0;

			if (!usable_nodes.size()) {
				//cout << "false " << ++i << endl;
				return false;
			}
			T distance = 0, last_distance;
			T max_x = m.get_w() - 1, max_y = m.get_h() - 1;
			node last = goal;
			list< smart_edge > enumerating_nodes;//перебираемые узлы
			list< smart_edge >::iterator iter;
			bool used;

			for (T y = 0; y <= max_y; y++) {
				for (T x = 0; x <= max_x; x++) {
					m(x, y).tag = true;
				}
			}

			max_x--; max_y--;

			auto make_max_child = [this](const node& n) -> node {
				node max_child;

				for (const auto& child : make_neighbors(n, this->m.get_w(), this->m.get_h(), (neighbor_number::N3 | neighbor_number::N4 | neighbor_number::N5 | neighbor_number::N6))) {
					if (!m(child.x, child.y).tag && !(child > goal) && child.x <= goal.x && max_child < child) max_child = child;
				}

				if (max_child == node(0, 0)) {
					cerr << "if (max_kid == node(0, 0))" << endl;
					//throw std::exception();
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
					//throw std::exception();
					continue;
				}
				m(root_node.x, root_node.y).remember();

				enumerating_nodes.push_front(smart_edge(m, make_max_child(root_node)));

				do {
					iter = --enumerating_nodes.end();
					if (!iter->use()) continue;
					used = true;
					while (true) {
						while (iter->was_here() && (used = iter->use(), used)) {}
						if (!used) {
							do {
								if (++iter == enumerating_nodes.end()) break;
							} while (!(used = iter->use(), used));
							if (!used) break;
							else continue;
						}
						if (iter->get_n() == goal) {
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
							if (iter->use()) continue;
						}
						else {
							if (iter == enumerating_nodes.begin()) enumerating_nodes.push_front(smart_edge(m, make_max_child(iter->get_n())));
							if ((--iter)->use()) continue;
						}

						do {
							if (++iter == enumerating_nodes.end()) break;
						} while (!(used = iter->use(), used));
						if (!used) break;
					}
				} while (m(root_node.x, root_node.y).use());
			}
			//cout << "false " << ++i << endl;
			return false;
		}
	};
	
	bool fill_infos(matrix< info/*< TAG >*/ >& infos, node start, const node& goal, const node& end, const bool& may_change_texture, const bool& quickly){
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
					auto neighbors = make_neighbors(x - old_start.value.x, y - old_start.value.y, w, h, (neighbor_number::N1 | neighbor_number::N2 | neighbor_number::N7 | neighbor_number::N8));//приходит список соседей в порядке по часовой стрелке начиная с первого (смотри make_neighbors)
					if (neighbors.size()){
						for (const auto &i : neighbors){
							if (i < *neighbors.begin()) *neighbors.begin() = i;
						}
						start = node(neighbors.begin()->x + old_start.value.x, neighbors.begin()->y + old_start.value.y);
						neighbors = make_neighbors(start.x - old_start.value.x, start.y - old_start.value.y, 1 + end.x - old_start.value.x, 1 + end.y - old_start.value.x, (neighbor_number::N1 | neighbor_number::N2 | neighbor_number::N7 | neighbor_number::N8));//приходит список соседей в порядке по часовой стрелке начиная с первого
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
						if (!solved) {
							cerr << "if (!solved)" << endl;
							//throw std::exception();
						}
						solved = false;
					}
				}
				#pragma endregion
					
				if (!quickly && !solved && cur.distance(start) > 2){
					sub_matrix sub_(infos, cur,
						/*l_distance*/min(MAX_DISTANCE, T(x - start.x)),
						/*r_distance*/end.x - x,
						/*t_distance*/min(MAX_DISTANCE, T(y - start.y)),
						/*b_distance*/end.y - y,
						MAX_COUNT_OF_USABLE);
					if (solved = sub_.solve_the_problem(), solved) sub_.save();
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
	bool fill_infos(matrix< info/*< TAG >*/ >& infos, const node start, const node& goal, const bool& may_change_texture, const bool& quickly) {
		node end(goal.x, min(goal.y + goal.x - start.x, infos.get_h() - 1));
		return fill_infos(infos, start, goal, end, may_change_texture, quickly);
	}
#pragma endregion
	
matrix< array<pair<T, bool>, 3> > get_textures_arrangement(matrix<T>& source, bool quickly) {
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

	fill_infos(infos, node(0, 0), node(infos.get_w() - 1, infos.get_h() - 1), true, quickly);

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
				neighbors = make_neighbors(x, y, w, h, (neighbor_number::N1 | neighbor_number::N2 | neighbor_number::N7 | neighbor_number::N8));
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