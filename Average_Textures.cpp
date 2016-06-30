//Average_Textures.cpp


#include "Average_Textures.h"
//#include <map>
#include <list>
//#include <algorithm>
//#include <typeinfo>
//#include <stack>

#include <iostream>

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
		return y == n.y & x == n.x;
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
list< node > make_neighbors(const node& n, const T& w, const T& h, limits lim = NOTHING){
	T x = n.x, y = n.y;
	limits limitless = (limits)((limits)(
			(y > 0) | 
			((y + 1 < h) << 1) | 
			((x > 0) << 2) | 
			((x + 1 < w) << 3) ) ^ lim);

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

class info{//���������� ��� ����
public:
	enum level {NONE = 0, FIRST = 1, SECOND = 2, THIRD = 4};
	int used;//�������������� ������
	int taken_before; //������, ������ � ���������� �������� �����.
	int taken_after;//�������, ������ � ��������� ����.
	/*� taken_after ����� �������� ������ �������, ������� ���� ���� ��������� ����, � � taken_before ��� ��������� (� ������������ � ��� ��������� ��.����.)*/

	int cur_level;//���������� �� ����� ������ ������ ����� 1
	bool can_be_used_again_;
	array< T , 3 > textures;//(�������� ������ �� ��������, ������� �������� � taken_before ��� � taken_after ��� � cur_level)

	T texture;
	node n;
	matrix< info >* owner;
	
public:
	info() : used(NONE), taken_before(NONE), taken_after(NONE), cur_level(NONE), can_be_used_again_(true){
		for (int i = 0; i < textures.size(); i++) textures[i] = 0;
	}

	inline bool take_after(const int& l, const T& texture){
		//��������� ���� ����� �������� �������� texture �� ������� l � �������� ��� �������.
		if (!cur_level){
			cerr << "if (!cur_level)" << endl;
			throw(exception("����� ������� ��������� ���� throw ���� �������"));
		}
		if (l & (taken_before | cur_level)){
			if (textures[l/2] != texture) return false/*������ ��������� ��������*/;
		} else {
			if(!l){
				cerr << "if(!l)" << endl;
				throw(exception("����� ������� ��������� ���� throw ���� �������"));
			}
			textures[l/2] = texture;
		}
		taken_after = l;
		return true;
	}
	inline bool can_take(const int& l, const T& texture) const{
		//��������� ���� ����������, ����� �� �� ������� l ��������� �������� texture.
		if (!cur_level){
			cerr << "if (!cur_level)" << endl;
			throw(exception("����� ������� ��������� ���� throw ���� �������"));
		}
		if (l & (taken_before | taken_after | cur_level)){
			if (textures[l/2] != texture) return false/*������ ��������� ��������*/;
		} else {
			if(!l){
				cerr << "if(!l)" << endl;
				throw(exception("����� ������� ��������� ���� throw ���� �������"));
			}
		}
		return true;
	}

	//�������� ������ ����� �� ����� ��������� �������, ������� ��� �� �����������
	inline void use() throw(exception){
		if (!can_be_used_again_){
			cerr << "if (!can_be_used_again_)" << endl;
			throw(exception("����� ������� ��������� ���� throw ���� �������"));
		}
		if (!used){//������ ��� ���������� ������� - ������ ���� ���������� taken_before
			limits limitless = (limits)(
			(n.y > 0) | 
			((n.y + 1 < owner->get_h()) << 1) | 
			((n.x > 0) << 2));

			list< node > neighbors;
			if (limitless & limits::DOWN && limitless & limits::LEFT) neighbors.push_back(node(n.x - 1, n.y + 1));
			if (limitless & limits::LEFT) neighbors.push_back(node(n.x - 1, n.y));
			if (limitless & limits::UP){
				neighbors.push_back(node(n.x, n.y - 1));
				if (limitless & limits::LEFT) neighbors.push_back(node(n.x - 1, n.y - 1));
			}

			int l;
			for (const auto& i : neighbors){
				l = (*owner)(i.x, i.y).cur_level;
				if (l & taken_before){
					//���� ���, ��� ��� �������, �� ������ ������� �����������
					//if (textures[l/2] != infos(i.x, i.y).textures[l/2]) throw(exception(/*������ ��������� �������� (���� throw �� ���� �������)*/));
					if (textures[l/2] != (*owner)(i.x, i.y).textures[l/2]){
						cerr << "if (textures[l/2] != infos(i.x, i.y).textures[l/2])" << endl;
						throw(exception("����� ������� ��������� ���� throw ���� �������"));
					}
				} else {
					taken_before |= l;
					textures[l/2] = (*owner)(i.x, i.y).textures[l/2];
				}
			}

			if (n.y > 0){
				(*owner)(n.x, n.y - 1).taken_after = NONE;
			}

			struct tmp_struct{
				static bool func(const list< node >& neighbors, const matrix< info >& infos, const T& texture, const level& l){
					for (const auto& i : neighbors){
						if (!infos(i.x, i.y).can_take(l, texture)) return false;
					}
					return true;
				}
			};

			if (!tmp_struct::func(neighbors, (*owner), texture, FIRST)) used |= FIRST;
			if (!tmp_struct::func(neighbors, (*owner), texture, SECOND)) used |= SECOND;
			if (!tmp_struct::func(neighbors, (*owner), texture, THIRD)) used |= THIRD;
		}

		//������� �������� �������� �������� �� ��� �������, �� ������� ���� ����� ��
		if (!(FIRST & used) && FIRST & taken_before) cur_level = FIRST;
		else if(!(SECOND & used) && SECOND & taken_before) cur_level = SECOND;
		else if(!(THIRD & used) && THIRD & taken_before) cur_level = THIRD;
		else if(!(FIRST & used))  cur_level = FIRST;
		else if(!(SECOND & used)) cur_level = SECOND;
		else if(!(THIRD & used))  cur_level = THIRD;
		else throw(exception(/*������ ��������� �������� - ���� ��� �������� ���������, ���� ����� �������� ������� ������ �������� (���� throw �� ���� �������)*/));
		
		if (n.y > 0){
			(*owner)(n.x, n.y - 1).take_after(cur_level, texture);
		}
		used |= cur_level;
		textures[cur_level / 2] = texture;

		if ((FIRST | SECOND | THIRD) ^ (taken_before | used)){//�����-�� ������� ������� � ��� ����� ���� �� ������������ ��� ������ �����������
			can_be_used_again_ = true;
		} else can_be_used_again_ = false;
	}
	bool can_be_used_again() const {
		if (!used) throw(exception("����� ������� ��������� ���� throw ���� �������"));
		return can_be_used_again_;
	}

	//inline T get_texture() throw(exception){
	//	if (!cur_level) throw(exception("�������� � ���� �� ����������"));
	//	return textures[cur_level/2];
	//}
	void clear(){
		used = NONE; taken_before = NONE; taken_after = NONE; cur_level = NONE; can_be_used_again_ = true;
	}
	void set_texture(const T& texcure){
		if (this->texture != texcure){
			clear();
			this->texture = texcure;
		}
	}

	void set(const T& texture, const node& n, matrix< info >* owner){
		set_texture(texture); this->n = n; this->owner = owner;
	}
	inline node get_node() const {return n;}
	inline T get_texture() const {return texture;}
};

matrix< array<pair<T, bool>, 3> > get_textures_arrangement(const matrix<T>& source) throw(exception){
	T w = source.get_w(), h = source.get_h();
	if (w < 2 || h < 2){
		//����������
		//�������
		matrix< array<pair<T, bool>, 3> > sdelay_menya_normalno(w, h);
		return sdelay_menya_normalno;
	}

	//enum number_of_free_columns : int {ONE, TWO, THREE} num_of_columns = ONE;
	//auto changeable_source = source;
	node barrier;//�� ���� ������ ������ ���������
	matrix< info > infos(w, h);

	//�������������
	for (T x = 0; x < w; x++){
		for (T y = 0; y < h; y++){
			infos(x, y).set(source(x, y), node(x, y), &infos);
		}
	}

	//���� ������� ����: ����� ����� ������� ����� ��������, �� ��� �� ����������� ������ �� ������� ���� (��������� ���� �����������), � ����� ���� � ��� �������...)
	
	list<node> usable_nodes/*��� ����� ������ �������*/;
	list<info> buffer/*������ ������ �������*/;

	auto remove_old_nodes = [&barrier](list<node>& nodes) -> void {
		while (nodes.size() && *nodes.rbegin() < barrier) nodes.pop_back();
	};

	for (T x = 0; x < w; x++){
		//���� ������ ������������ ������ 3 �������� ��� �������� ���������
		if(x - barrier.x > 2){
			//��������� ������
			barrier = node(x - 2, 0);
			remove_old_nodes(usable_nodes);
		}
		for (T y = 0; y < h; y++){
			infos(x, y).clear();
			try{
				infos(x, y).use();
				if (infos(x, y).can_be_used_again()) usable_nodes.push_front(node(x, y));
			} catch(const exception&) {
				if (buffer.size()){
					if (node(x, y) > buffer.begin()->get_node()){
						buffer.clear();
						buffer.push_front(infos(x, y));
					}
				} else buffer.push_front(infos(x, y));
				if (usable_nodes.size()){
					auto n = *usable_nodes.begin(), end_n = buffer.rbegin()->get_node();
					//�������� � buffer ���������� ��� ���� � ���� buffer.rbegin()->get_node() �� ���� n
					while (n < end_n){
						if (end_n.y != 0) end_n.y--;
						else {
							end_n.y = h - 1;
							end_n.x--;
						}
						buffer.push_back(infos(end_n.x, end_n.y));
					}
					infos(n.x, n.y).use();
					x = n.x; y = n.y;
					if (!infos(n.x, n.y).can_be_used_again()) usable_nodes.pop_front();
				} else {
					for (const auto& i : buffer){
						infos[(pair< T, T >)i.get_node()] = i;
					}
					//���� ������� ����� ����
					//����� ����� ���������� ������� �� ����� ���������� �������� ����� ���� - ��� ������, ��� �� ��������, ���� �������� ���� �������� � �� ��� �� �������, ��� � � ������ ������
					//����� ����� ����, ��� ��� ���� �� ��� �� ���� - �� �������� �� ������� � ������� ����� (�� ���� �� ������ ��)
					auto n = buffer.begin()->get_node();
					infos(n.x, n.y).set_texture(infos(n.x - 1, n.y).get_texture());
					infos(n.x, n.y).use();
				}
			}
		}
	}

	matrix< array<pair<T, bool>, 3> > res(w, h);
	limits limitless;
	list< node > neighbors;
	for (T x = 0; x < w; x++){
		for (T y = 0; y < h; y++){
			auto& inf = infos(x, y);

			limitless = (limits)(
				(y > 0) | 
				((y + 1 < h) << 1) | 
				((x + 1 < w) << 3) );
			neighbors.clear();
			if (limitless & limits::RIGHT){
				neighbors.push_back(node(x + 1, y));
				if (limitless & limits::DOWN) neighbors.push_back(node(x + 1, y + 1));
				if (limitless & limits::UP) neighbors.push_back(node(x + 1, y - 1));
			}
			for (const auto& n : neighbors){
				inf.take_after(  infos(n.x, n.y).cur_level, infos(n.x, n.y).get_texture());
			}

			auto& res_inf = res(x, y);
			for (int i = 0; i < 3; i++) res_inf[i].first = inf.textures[i];
			res_inf[inf.cur_level/2].second = true;
		}
	}

	return std::move(res);

}