//Average_Textures.cpp


#include "Average_Textures.h"
#include <map>
#include <list>
#include <algorithm>
//#include <typeinfo>


matrix< array<pair<T, bool>, 3> > get_textures_arrangement(const matrix<T>& source) throw(exception){
	T w = source.get_w(), h = source.get_h();
	if (w < 2 || h < 2){
		//����������
		//�������
		matrix< array<pair<T, bool>, 3> > sdelay_menya_normalno(w, h);
		return sdelay_menya_normalno;
	}

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


class info{//���������� ��� ����
public:
	enum level {NONE = 0, FIRST = 1, SECOND = 2, THIRD = 4};
private:
	int used;//�������������� (���� �����) ������
	int taken_before, taken_after;//������ ������ ���-�� (�� ���� �����) �� ������ � ���� ���� � �����.
	/*� taken_after ����� �������� ������ �������, ������� ���� ���� ��������� ����, � � taken_before ��� ��������� (� ������������ � ��� ��������� ��.����.)*/

	int cur_level;//���������� �� ����� ������ ������ ����� 1
	bool can_be_used_again_;
	array< T , 3> textures;
public:
	info() : used(NONE), taken_before(NONE), taken_after(NONE), cur_level(NONE){}
	
	//���-�� ����� �������� �������� �� �������.
	inline bool take(const level& l, const T& texture){
		if (cur_level) throw(exception("����� ������� ��������� ���� throw ���� �������"));
		if (l & (taken_before | cur_level)){
			if (textures[l/2] != texture) return false/*������ ��������� ��������*/;
		} else {
			if(!l) throw(exception("����� ������� ��������� ���� throw ���� �������"));
			if (cur_level) taken_after = l;
			else taken_before |= l;
			textures[l/2] = texture;
		}
		return true;
	}

	inline bool take(const level& l, const T& texture){
		if (cur_level) throw(exception("����� ������� ��������� ���� throw ���� �������"));
		if (l & (taken_before | cur_level)){
			if (textures[l/2] != texture) return false/*������ ��������� ��������*/;
		} else {
			if(!l) throw(exception("����� ������� ��������� ���� throw ���� �������"));
			if (cur_level) taken_after = l;
			else taken_before |= l;
			textures[l/2] = texture;
		}
		return true;
	}
	
	//�������� ������ ����� �� ����� ��������� �������, ������� ��� �� �����������
	inline void use(const T& texture, const node& n, const matrix< info >& infos) throw(exception){
		auto tmp = (FIRST | SECOND | THIRD) ^ (taken_before | used);
		if (used){
		}
	}

	bool can_be_used_again(){
		if (!used) throw(exception("����� ������� ��������� ���� throw ���� �������"));
		return can_be_used_again;
	}

	inline T get_texture() throw(exception){
		if (!cur_level) throw(exception("�������� � ���� �� ����������"));
		return textures[cur_level/2];
	}
};

	enum number_of_free_columns : int {ONE, TWO, THREE} num_of_columns = ONE;
	auto changeable_source = source;
	node barrier;//�� ���� ������ ������ ���������

	matrix< info > infos(w, h);



	matrix< array<pair<T, bool>, 3> > res(w, h);
	return std::move(res);

}