//Average_Textures.cpp


#include "Average_Textures.h"
#include <map>
#include <list>
#include <algorithm>
//#include <typeinfo>

typedef pair< T, T > node;
typedef list< pair<T, float> > info;//информаци€ про узел (про текстуры в нем и в его сосед€х)

template<> struct less< info >{
	bool operator()(const info& left, const info& right){
		auto left_i = left.begin(), right_i = right.begin(), left_end = left.end(), right_end = right.end();
		while (left_i != left_end && right_i != right_end){
			if (left_i->second != right_i->second){
				if (left_i->second < right_i->second) return true;
				return false;
			}
			left_i++; right_i++;
		};
		if (left_i == left_end &&  right_i != right_end) return true;
		return false;
	}
};



template<class _T>
list<_T> move_to_list(vector<_T> &&vec){
	auto size = vec.size();
	list<T> res(size);
	auto iter = list.begin();
	for (int i = 0; i < size; i++){
		*iter++ = std.move(vec[i]);
	}
	return res;
}

matrix< vector< pair<T, bool> > > get_textures_merge(const matrix<T>& source, size_t number_of_textures_in_node) throw(exception){
	if (!number_of_textures_in_node) throw exception("bad arg");
	if (source.get_w() < 2 || source.get_h() < 2){
		//тривиально
		//сделать
		matrix< vector< pair<T, bool> > > sdelay_menya_normalno(source.get_w(), source.get_h());
		return sdelay_menya_normalno;
	}

	T w = source.get_w(), h = source.get_h();

	auto func = [&source](const T& x, const T& y) throw(exception) -> info{
		/*веса (важность узла и его соседей)
			
			zyz
			yxy		дл€ узлов в серединке
			zyz
			x + 4y + 4z = 1
			

			yxy		дл€ узлов на одном из краев матрицы
			zyz
			x + 3y + 2z = 1

			xy		дл€ узлов на двух кра€х
			yz
			x + 2y + z = 1
			
			дл€ всех случаев
			1 >= x >= y >= z >= 0

			ќ—“јЋ№Ќџ≈ —Ћ”„ј» Ќ≈ —„»“ј≈ћ (ширина и высоьа матрицы на входе больше чем 2)
		*/
		static const float x1 = 6 / (float)9, y1 = (2 * (1 - x1) / 3) / 4, z1 = (1 - (x1 + 4 * y1)) / 4;
		static const float x2 = 1 / (float)2, y2 = (5 * (1 - x2) / 6) / 3, z2 = (1 - (x2 + 3 * y2)) / 2;
		static const float x3 = 1 / (float)2, y3 = (2 * (1 - x3) / 3) / 2, z3 = (1 - (x3 + 2 * y3)) / 1;

		/*auto check1 = x1 + 4*y1 + 4*z1;
		auto check2 = x2 + 3*y2 + 2*z2;
		auto check3 = x3 + 2*y3 + 1*z3;
		if (check1 != 1 || check2 != 1 || check3 != 1){
			throw exception("very bad!");
		}*/
		
		enum limits {NOTHING = 0, UP = 1, DOWN = 2, LEFT = 4, RIGHT = 8} limitless = (limits)(
			(y > 0) | 
			((y + 1 < source.get_h()) << 1) | 
			((x > 0) << 2) | 
			((x + 1 < source.get_w()) << 3) );

		int limitless_edges = (bool)(limitless & limits::UP) + (bool)(limitless & limits::DOWN) + (bool)(limitless & limits::LEFT) + (bool)(limitless & limits::RIGHT);

		map<T, float> weight;

#define get_textures_merge_func_DUPLICATE_CODE(x_, y_, z_)\
			weight[source(x, y)] += x_;\
			if (limitless & limits::DOWN){\
				weight[source(x, y + 1)] += y_;\
				if (limitless & limits::RIGHT) weight[source(x + 1, y + 1)] += z_;\
				if (limitless & limits::LEFT) weight[source(x - 1, y + 1)] += z_;\
			}\
			if (limitless & limits::RIGHT) weight[source(x + 1, y)] += y_;\
			if (limitless & limits::LEFT) weight[source(x - 1, y)] += y_;\
			if (limitless & limits::UP){\
				weight[source(x, y - 1)] += y_;\
				if (limitless & limits::RIGHT) weight[source(x + 1, y - 1)] += z_;\
				if (limitless & limits::LEFT) weight[source(x - 1, y - 1)] += z_;\
			}

		switch (limitless_edges){
			case 4:
				weight[source(x, y)] += x1;
				weight[source(x, y + 1)] += y1;
				weight[source(x + 1, y + 1)] += z1;
				weight[source(x - 1, y + 1)] += z1;
				weight[source(x + 1, y)] += y1;
				weight[source(x - 1, y)] += y1;
				weight[source(x, y - 1)] += y1;
				weight[source(x + 1, y - 1)] += z1;
				weight[source(x - 1, y - 1)] += z1;
			break;
			case 3:
				get_textures_merge_func_DUPLICATE_CODE(x2, y2, z2);
			break;
			case 2:
				get_textures_merge_func_DUPLICATE_CODE(x3, y3, z3);
			break;
			default:
				//impossible
			break;
		};

		vector < pair< T, float > > res(weight.size());
		auto j = res.begin();
		for (const auto& i : weight) *j++ = i;
		
		struct for_sort{
			static bool less(const pair<T, float>& left, const pair<T, float>& right){
				return left.second < right.second;
			}
		};

		sort(res.rbegin(), res.rend(), for_sort::less);
		//if (typeid(info) == typeid(list< pair<T, float> >)){
			return move_to_list(std::move(res));
		//} else {
			//return res;
		//}
	};

	less< info > less_info;

	list< pair< node, info > > order(w*h);
#pragma region составление начальной очереди
	{
		auto order_iter = order.begin();
	
		for (T y = 0; y < h; y++){
			for (T x = 0; x < w; x++){
				 *order_iter++ = pair< node, info >(node(x, y), func(x, y));
			}
		}

		auto less_pair_node_info = [&less_info](const pair< node, info >& left, const pair< node, info >& right) -> bool{
			return less_info(right.second, left.second);
		};

		sort(order.begin(), order_iter, less_pair_node_info);
	}
#pragma endregion
	
	matrix< map< T, bool > > subres(w, h);//чтоб сначала определить лишь наличие текстур в узлах (без услови€, что одинаковые текстурки на одном месте)
#pragma region определить наличие текстур в узлах
	{
	auto neighbors_maker = [w, h](const node& n) -> list< node >{
		T x = n.first,y = n.second;
		enum limits {NOTHING = 0, UP = 1, DOWN = 2, LEFT = 4, RIGHT = 8} limitless = (limits)(
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
	};
	pair< node, info >* on_order;
	map< T, bool >* subres_element, *subres_neighbor_element;
	T cur_texture;
	list< node > neighbors;
	bool i_can_add_texture = false;

	for (size_t i = 0; i < w*h; i++){
		on_order = &order[i];
		subres_element = &subres[on_order->first];
		
		neighbors = neighbors_maker(on_order->first);
		neighbors.push_front(on_order->first);

		for (auto texture = on_order->second.begin(), texture_end = on_order->second.end(); texture != texture_end; texture++){
			i_can_add_texture = true;
			cur_texture = texture->first;
			for (const auto &neighbor : neighbors){
				subres_neighbor_element = &subres[neighbor];
				//если нет места дл€ добавлени€ и такой текстуры еще нет у соседа
				if (subres_neighbor_element->size() == number_of_textures_in_node && !subres_neighbor_element->count(cur_texture)){
					i_can_add_texture = false;
					break;
				}
			}
			if (i_can_add_texture){
				auto &rest = rests[on_order->first];
				for (; texture != texture_end; texture++){
					rest.push_back(texture->first);
				}
			}
		}


		if (i_can_add_texture){
			(*subres_element)[cur_texture] = true;
			neighbors.pop_front();//важно
			for (const auto &i : neighbors){
				subres_neighbor_element = &subres[i];
				//если такой текстуры еще нет у соседа
				if (!subres_neighbor_element->count(cur_texture)){
					//вставл€ем
					(*subres_neighbor_element)[cur_texture] = false;
				}
			}
		}

		on_order->second.pop_front();
		//еще остались текстуры в этом узле
		if (!on_order->second.empty()){
			auto i = ++order.begin(), end = order.end();
			for (; i != end; i++){
				if (less_info(i->second, on_order->second)){
					order.insert(i, std::move(*order.begin()));
					break;
				}
			}
			if (i == end) order.push_back(std::move(*order.begin()));
		}
		order.pop_front();
	}
	}
#pragma endregion

#pragma region удовлетворение условию, что у каждой текстуры на том же уровне должны быть либо нулевые коэффициенты, либо така€ же текстура
	//если получаетс€ так, что разные текстуры станов€тс€ на одно место, то нужно сделать коэффициенты нулевыми. 
	matrix< vector< pair<T, bool> > > res(w, h);
	typedef matrix< bool > labels;
	vector< labels > levels(number_of_textures_in_node);
	size_t level = 0;
	for (int i = 0; i < number_of_textures_in_node; i++){
		levels[level] = labels(w, h);


	}





	//////////////////////////////
	for (T y = 0; y < h; y++){
		for (T x = 0; x < w; x++){
			auto & vec = res(x, y) = vector< pair<T, bool> >(number_of_textures_in_node);
			size_t i = 0;
			for (const auto& pair_ : subres(x, y)){
				vec[i++] = pair_;
			}
		}
	}
	/////////////////////////
#pragma endregion
	return std::move(res);
//#pragma region EASY
//	{
//		T w = source.get_w(), h = source.get_h();
//		
//		matrix< vector< pair<T, float> > > res(source.get_w(), source.get_h());
//		
//		info func_res;
//
//		for (T y = 0; y < h; y++){
//			for (T x = 0; x < w; x++){
//				func_res = func(x, y);
//				if (func_res.size() <= number_of_textures_in_node) res(x, y) = func_res;
//				else {
//					auto &info_obj = res(x, y) = info(number_of_textures_in_node);
//					float value = 0, k;
//					for (size_t i = 0; i < number_of_textures_in_node; i++) value += func_res[i].second;
//					k = 1 / value;
//					for (size_t i = 0; i < number_of_textures_in_node; i++){
//						info_obj[i].first = func_res[i].first;
//						info_obj[i].second = func_res[i].second * k;
//					}
//				}
//			}
//		}
//
//		return std::move(res);
//	}
//#pragma endregion
}