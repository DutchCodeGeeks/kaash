#pragma once

template <typename T>
class Maybe{
	typedef struct {
		T v;
	} t_v_wrapper;

	bool is_something;
	t_v_wrapper *wr;
public:
	Maybe(void):is_something(false),wr(nullptr){}
	Maybe(const T &_v):is_something(true),wr(new t_v_wrapper({_v})){}
	Maybe(const T &&_v):is_something(true),wr(new t_v_wrapper({move(_v)})){}
	~Maybe(void){if(wr)delete wr;}

	operator bool() const{return this->is_something;}

	T value(void){
		if(is_something)return wr->v;
		throw "Nothing";
	}
};
