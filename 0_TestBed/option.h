#pragma once
#include <exception>
template <typename T>

struct option_t 
{
	option_t()
		: on(false) { };
	option_t(T val)
		: on(true), val(val) { };

	static option_t none() { return option_t(); };

	bool is_none()
	{
		return !this->on;
	}
	
	T& unwrap() 
	{
		return this->val;
	}
	const T& unwrap() const
	{
		return this->val;
	}
private:
	
	T val;
	bool on;
};

template <typename T>
option_t<T> make_option(T&& val)
{
	return option_t<T>(val);
}

