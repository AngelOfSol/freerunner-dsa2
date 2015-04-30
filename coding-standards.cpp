#ifndef APPLICATION_H
#define APPLICATION_H

struct return_type_t
{
	int a;
	bool good;
	return_type_t () 
		: a(0) { };
};

return_type_t snake_case(int param, int& param, int* param)
{
	if(param != NULL)
	{

	}
	return 0;
}

class snake_case_t
{
public:
	snake_case(int xyz);
	~snake_case();

private:
	int m_data;
};

// has no methods
struct snake_case_t
{
	/* data */
};

void main()
{
	std::unique_ptr<int> pointer(new int(4));
	std::unique_ptr<int> second; // does not work, can only be one owner
	second = std::move(pointer); // works, gives up ownership
	if(true)
	{

	}
	while(true)
	{

	}
}

#endif