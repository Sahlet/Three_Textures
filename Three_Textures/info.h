//info.h
#pragma once
#include <iostream>

#include "matrix.h"
#include "consts.h"
#include "node.h"
#include <array>
#include <map>
#include <set>
#include <vector>
#include <memory>


typedef bool TAG;
//template<class TAG = void*>
class info {//информация про узел
public:
	enum level : char { NONE = 0, FIRST = 1, SECOND = 2, THIRD = 4 };
	TAG tag;
	class Texture {
		char count;
		T texture;//значимые только те текстуры, которые не равны max_T
	public:
		Texture() : count(), texture(max_T) {}
		inline void operator --(int) {
			if (texture == max_T) {
				std::cerr << "if (texture == max_T)" << std::endl;
				//throw std::exception();
			}
			if (!count) {
				std::cerr << "if (!count)" << std::endl;
				//throw std::exception();
			}
			if (--count == 0) texture = max_T;
		}
		inline bool can_give(const T& texture)const {
			if (texture == max_T) {
				std::cerr << "if (texture == max_T)" << std::endl;
				//throw std::exception();
			}
			return this->texture == max_T || this->texture == texture;
		}
		inline void give(const T& texture) {
			if (!can_give(texture)) {
				std::cerr << "if (!can_give(texture))" << std::endl;
				//throw std::exception();
			}
			if (!count++) this->texture = texture;
		}
		inline char get_count() { return count; }
		inline T get_texture() { return texture; }
		inline bool operator < (const Texture& t)const {
			return (this->count < t.count || (this->count == t.count && this->texture < t.texture));
		}
		inline bool operator == (const Texture& t)const {
			return  (this->count == t.count && this->count == t.count);
		}
	};
	struct Level {
		level l;
		T texture;
		Level() : l(NONE), texture(max_T) {}
		Level(const T& texture) : l(NONE), texture(texture) {
			if (texture == max_T) {
				std::cerr << "if (texture == max_T)" << std::endl;
				//throw std::exception();
			}
		}
		inline Level& operator = (const level& l) {
			this->l = l;
			return *this;
		}
	};
private:
	bool free;//показывает, является ли этот узел таким, что вокруг него много узлов с таким же цветом
	level used;//уровни, что были использованы этим узлом
	typedef std::array< Texture, NUMBER_OF_LEVELS> INFO;
	INFO filling;//заполнение
	std::list< std::shared_ptr< Level > > levels;//список уровней, которые занимает этот узел
	std::shared_ptr< Level > cur_level;//показывает на каком уровне сейчас стоит 1

	std::vector< info* > neighbors;
	//template<> struct less< INFO >
	struct INFO_less {
		bool operator()(const INFO & l_INFO, const INFO & r_INFO) const {
			for (T i = 0; i < NUMBER_OF_LEVELS; i++) {
				if (!(l_INFO[i] == r_INFO[i])) return l_INFO[i] < r_INFO[i];
			}
			return false;
		}
	};
	std::map< INFO, level, INFO_less> memory;


	inline bool can_give(const int& l, const T& texture) const {
		if (!l) {
			std::cerr << "if(!l)" << std::endl;
			//throw std::exception();
		}
		if (filling[l / 2].can_give(texture)) return true;
		return false;
	}
	inline bool give(const char& l, const T& texture) {
		if (!can_give(l, texture)) {
			std::cerr << "if (!can_give(l, texture))" << std::endl;
			//throw std::exception();
			return false;
		}
		filling[l / 2].give(texture);
		return true;
	}

	void remove_from_the_level() {
		if (!cur_level->l) return;
		for (auto i : neighbors) {
			i->filling[cur_level->l / 2]--;
		}
		this->filling[cur_level->l / 2]--;
		cur_level->l = NONE;
	}
	bool can_be_used(level& l) const {
		if (free) return false;
		l = level((FIRST | SECOND | THIRD) ^ used);
		if (!l) return false;

		struct tmp_struct {
			static bool func(const std::vector< info* >& neighbors, const T& texture, const level& l) {
				for (const auto& i : neighbors) {
					if (!i->can_give(l, texture)) return false;
				}
				return true;
			}
		};

		if ((l & FIRST) && !this->can_give(FIRST, cur_level->texture)) l = level(l ^ FIRST);
		if ((l & SECOND) && !this->can_give(SECOND, cur_level->texture)) l = level(l ^ SECOND);
		if ((l & THIRD) && !this->can_give(THIRD, cur_level->texture)) l = level(l ^ THIRD);

		if ((l & FIRST) && !tmp_struct::func(neighbors, cur_level->texture, FIRST)) l = level(l ^ FIRST);
		if ((l & SECOND) && !tmp_struct::func(neighbors, cur_level->texture, SECOND)) l = level(l ^ SECOND);
		if ((l & THIRD) && !tmp_struct::func(neighbors, cur_level->texture, THIRD)) l = level(l ^ THIRD);

		if (!l) return false;
		return true;
	}
	inline bool check_on_free() {
		//такую сложность можно позволить, так как эта функция будет редко вызываться
		for (auto i : neighbors) {
			for (auto j : i->neighbors) {
				if (j->cur_level->texture != cur_level->texture) return (free = false);
			}
		}
		free = true;
		clear();
		return true;
	}
	void set_texture(const T& texcure) {
		if (texcure == max_T) {
			std::cerr << "if (texcure == max_T)" << std::endl;
			//throw std::exception();
		}
		if (cur_level->texture != texcure) {
			clear();
			clear_memory();
			cur_level->texture = texcure;
			//не экономно, но по-другому сложно и это относительно редко используется (не жалко)
			for (auto i : neighbors) {
				for (auto j : i->neighbors) {
					if (!j->used) {
						if (j->free) j->free = false;
						else j->check_on_free();
					}
				}
			}
		}
	}
	void set_texture(const T& texcure, const node& n, matrix< info/*< TAG >*/ >& owner) {
		if (texcure == max_T) {
			std::cerr << "if (texcure == max_T)" << std::endl;
			//throw std::exception();
		}
		if (cur_level->texture != texcure) {
			clear();
			clear_memory();
			cur_level->texture = texcure;
			if (n.x + 2 < owner.get_w()) {
				T x = n.x + 2, max_y = std::min(owner.get_h() - 1, n.y + 2);
				for (T y = n.y > 2 ? n.y - 2 : 0; y <= max_y; y++) {
					if (owner(x, y).free) owner(x, y).free = false;
					else owner(x, y).check_on_free();
				}
			}
		}
	}
public:

	info() : free(false), used(NONE), filling(), tag(), cur_level(std::make_shared< Level >()) {}
	info(const T& texture) : cur_level(std::make_shared< Level >(texture)), free(false), filling(), tag(), used(NONE) {
		levels.push_back(cur_level);
	}
	info(const std::list< T > & textures) : free(false), filling(), tag(), used(NONE) {
		if (!textures.size()) {
			std::cerr << "if (!textures.size())" << std::endl;
		}
		std::set< T > set_of_textures;
		for (const T& texture : textures) {
			if (!set_of_textures.count(texture)) {
				levels.push_back(std::make_shared< Level >(texture));
				set_of_textures.insert(texture);
			}
		}
		cur_level = *levels.begin();
	}
	info(const info& inf) :
		used(inf.used),
		filling(inf.filling),
		memory(inf.memory),
		free(inf.free),
		tag(inf.tag) {
		for (const auto& val : inf.levels){
			levels.push_back(std::shared_ptr<Level>(new Level(*val)));
			if (!cur_level && val == inf.cur_level) cur_level = *levels.rbegin();
		}
		if (!cur_level) cur_level.reset(new Level(*inf.cur_level));
	}
	info& operator=(const info& inf) {
		used = inf.used;
		filling = inf.filling;
		memory = inf.memory;
		free = inf.free;
		tag = inf.tag;

		cur_level = nullptr;
		levels.clear();
		for (const auto& val : inf.levels) {
			levels.push_back(std::shared_ptr<Level>(new Level(*val)));
			if (!cur_level && val == inf.cur_level) cur_level = *levels.rbegin();
		}
		if (!cur_level) cur_level.reset(new Level(*inf.cur_level));

		return *this;
	}
	info(info&& inf) :
		cur_level(std::move(inf.cur_level)),
		levels(std::move(inf.levels)),
		used(inf.used),
		filling(inf.filling),
		memory(std::move(inf.memory)),
		free(inf.free),
		tag(std::move(inf.tag)) {}
	info& operator=(info&& inf) {
		used = inf.used;
		filling = inf.filling;
		memory = std::move(inf.memory);
		free = inf.free;
		tag = std::move(inf.tag);
		cur_level = std::move(inf.cur_level);
		levels = std::move(inf.levels);
		return *this;
	}

	inline bool can_be_used() const {
		level l;
		return can_be_used(l);
	}
	inline void clear() {
		remove_from_the_level();
		used = NONE;
	}
	//текстура должна стать на любой свободный уровень, который еще не использован
	inline bool use() {
		if (free) return true;
		level can_be_cur;
		if (!can_be_used(can_be_cur)) return false;

		remove_from_the_level();

		//сначала пытаемся добавить текстуру на тот уровень, на котором есть такие же
		if ((FIRST & can_be_cur) && filling[0].get_count()) *cur_level = FIRST;
		else if ((SECOND & can_be_cur) && filling[1].get_count()) *cur_level = SECOND;
		else if ((THIRD & can_be_cur) && filling[2].get_count()) *cur_level = THIRD;
		else if (FIRST & can_be_cur)  *cur_level = FIRST;
		else if (SECOND & can_be_cur) *cur_level = SECOND;
		else if (THIRD & can_be_cur)  *cur_level = THIRD;
		else {
			std::cerr << "impossible" << std::endl;//потом удалить
												   //throw std::exception();
		}

		for (auto i : neighbors) {
			i->give(cur_level->l, cur_level->texture);
		}
		this->give(cur_level->l, cur_level->texture);

		used = level(used | cur_level->l);

		return true;
	}
	inline bool was_used() {
		return bool(used);
	}

	inline bool was_here() {
		if (free) return true;
		if (!used) {
			std::cerr << "if (!used)" << std::endl;
			//throw std::exception();
		}

		return memory[filling] & cur_level->l;
		//return false;
	}
	inline void remember() {
		if (free) return;
		auto& level_ = memory[filling];
		level_ = level(level_ | cur_level->l);
	}
	inline void clear_memory() {
		memory.clear();
	}

	void set_and_use_the_most_popular_texture_from_neighbors(const node& n, matrix< info/*< TAG >*/ >& owner) {
		T old_texture = cur_level->texture;
		clear();
		{
			auto iter = levels.begin();
			auto end = levels.end();
			while ((iter != end) && (*(iter++) != cur_level)) {}

			for (; iter != end; iter++) {
				cur_level = (*iter);
				T texture = (*iter)->texture;
				if ((texture != old_texture) && use()) {
					cur_level->texture = old_texture;
					set_texture(texture, n, owner);
					use();
					return;
				}
			}
			cur_level = std::make_shared< Level >(old_texture);
		}

		//old code
		std::map<T, char> counter;
		for (auto neighbour : neighbors) {
			if (neighbour->cur_level->texture != max_T) counter[neighbour->cur_level->texture]++;
		}
		std::vector< std::pair<T, char> > textures_in_order(counter.size());
		size_t i = 0;
		for (const auto& val : counter) {
			textures_in_order[i++] = val;
		}

		auto pair_T_char_less = [](const std::pair< T, char>& l, const std::pair< T, char>& r) -> bool {
			return l.second < r.second;
		};

		sort(textures_in_order.rbegin(), textures_in_order.rend(), pair_T_char_less);
		for (const auto& val : textures_in_order) {
			cur_level->texture = val.first;
			if ((cur_level->texture != old_texture) && use()) {
				cur_level->texture = old_texture;
				set_texture(val.first, n, owner);
				use();
				return;
			}
		}

		std::cerr << "impossible happened in set_and_use_the_most_popular_texture_from_neighbours" << std::endl;
		//throw std::exception();
	}
	void try_use_rest_of_levels() {
		auto iter = levels.begin();
		auto end = levels.end();
		auto old_cur_level = cur_level;

		while ((iter != end) && (*(iter++) != cur_level)) {}

		for (; iter != end; iter++) {
			cur_level = (*iter);
			clear();
			use();
		}
		cur_level = old_cur_level;
	}

	void set_neighbors(const node& n, matrix<info /*< TAG >*/ >& owner) {
		auto neighbors_x_y = make_neighbors(n, owner.get_w(), owner.get_h());
		neighbors = std::vector<info*>(neighbors_x_y.size());
		auto iter = neighbors_x_y.begin();
		for (size_t i = 0; i < neighbors.size(); i++, iter++) {
			neighbors[i] = &owner(iter->x, iter->y);
		}
	}
	bool refresh_filling() {
		struct fix_if_not_OK {
			bool OK;
			INFO& filling;
			INFO old_filling;
			fix_if_not_OK(INFO& filling) : OK(true), filling(filling), old_filling(filling) {}
			~fix_if_not_OK() {
				if (!OK) filling = old_filling;
			}
		} fixer(filling);
		filling = INFO();
		for (auto i : neighbors) {
			if (!i->cur_level->l) continue;
			if (!(fixer.OK = this->give(i->cur_level->l, i->cur_level->texture), fixer.OK)) break;
		}
		if (fixer.OK && this->cur_level->l) fixer.OK = this->give(this->cur_level->l, this->cur_level->texture);
		return fixer.OK;
	}
	inline Level get_cur_level() { return *cur_level; }
	inline const std::list< std::shared_ptr< Level > >& get_levels() { return levels; }
	inline T get_texture() { return cur_level->texture; }
	inline T operator[](const size_t& i) { return filling[i].get_texture(); }

	static void set_free(matrix< info /*< TAG >*/ >& owner) {
		T w = owner.get_w(), h = owner.get_h(), texture;
		for (T x = 0; x < w; x++) {
			for (T y = 0; y < h; y++) {
				owner(x, y).free = true;
				texture = owner(x, y).cur_level->texture;
				for (int x_ = int(x) - 2; x_ <= x + 2; x_++) {
					for (int y_ = int(y) - 2; y_ <= y + 2; y_++) {
						if (x_ >= 0 && y_ >= 0 && x_ < w && y_ < h) {
							if (owner(x_, y_).cur_level->texture != texture) {
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
	inline void use_free() {
		if (!free || used) return;
		free = false;
		use();
	}
	inline bool get_free() { return free; }
};