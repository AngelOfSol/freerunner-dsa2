#pragma once

template <int N>
struct ring_buffer_t
{
	float values[N];

	void push(float v) 
	{
		this->values[m_current_index] = v;
		m_current_index++;
		m_current_index %= N;
	}
	float sum() const 
	{
		float ret = 0.0f;
		for(auto f : this->values)
		{
			ret += f;
		}
		return ret;
	}
	void clear()
	{
		for(auto& f : this->values)
		{
			f = 0;
		}
	}
	ring_buffer_t()
		: m_current_index(0)
	{
		this->clear();
	}
private:
	int m_current_index;
};


