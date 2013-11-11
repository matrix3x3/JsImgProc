/*  vernier  */
/* сн╠Й */
#pragma once

/* singleton */
class Vernier
{
private:
	static Vernier *_singleton;
	int *_v;
	int _capacity;
	/* point to current index */
	int _pointer;
	/* user specific length */
	int _length;

	Vernier(Vernier&);
	Vernier & operator = (Vernier&);

	Vernier()
	{
		_capacity = 25;
		_v = new int[_capacity];
	}
	~Vernier()
	{
		delete _v;
	}

public:
	static Vernier *getInstance()
	{
		if (_singleton == nullptr)
		{
			_singleton = new Vernier();
		}
		return _singleton;
	}

	/* user specify length */
	void setLength(int l)
	{
		_length = l;
	}
	int getLength()
	{
		return _length;
	}

	bool init(int length)
	{
		setLength(length);

		if (_length > _capacity)
		{
			return false;
		}

		for (int i = 0; i < _length; i++)
		{
			_v[i] = i;
		}

		/* set vernier to Zero */
		_pointer = 0;
	}

	void increase()
	{
		if (_pointer < _length)
		{
			_pointer ++;
		}
	}

	void decrease()
	{
		if (_pointer > 0)
		{
			_pointer --;
		}
	}
};