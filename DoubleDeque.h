/*
* Contains the 2D deque class, declaration is inline since template class definition in .cpp seems to mess with the linker
*/
#pragma once
#include <iostream>
#include <deque>

/* DoubleDeque
* Contains a deque of deques. Allows entry at any position, it will resize to accommodate
*/
template <typename T>
class DoubleDeque {
	int m_heigh = 0; // Height of the DoubleDeque
	int m_width = 0; // Width of the DoubleDeque

	// Origin of the DD, change if element is added top or left
	int m_ox = 0; // x origin
	int m_oy = 0; // y origin

	std::deque<std::deque<T>*> rows; // Each rows is it's own deque

public:
	// Default contructor. Return an empty DoubleDeque
	DoubleDeque() {}

	// Construct the DoubleDeque with a given size
	DoubleDeque(int height, int width) : m_heigh(height), m_width(width)
	{
		for (int i = 0; i < height; i++)
		{
			std::deque<T>* row = new std::deque<T>(width);
			rows.push_back(row);
		}
	}

	// Destructor, delete each row. If user gives a pointer type they are in charge of their deletion.
	~DoubleDeque()
	{
		for (int i = 0; i < rows.size(); i++)
		{
			delete rows[i];
		}
		rows.clear();
	}

	// Return the Height of the DoubleDeque
	int getHeight() { return this->m_heigh; }
	// Return the Width of the DoubleDeque
	int getWidth() { return this->m_width; }

	// Check if the given coordinate is inside the DoubleDeck
	bool isInbound(int x, int y)
	{
		return x + m_ox >= 0 && x + m_ox < m_width && y + m_oy >= 0 && y + m_oy < m_heigh;
	}

	// Set the value of the element at the given position
	void set(T value, int x, int y)
	{
		// If out of bounds, error then return
		if (!isInbound())
		{
			std::cerr << "[WARNING] DoubleDeque: Trying to set an element out of bounds. Try using addAt() instead." << std::endl;
			return;
		}
		else
		{
			rows.at(y + m_oy)->at(x + m_ox) = value;
		}
	}

	// Return the element at the given position
	T* get(int x, int y)
	{
		// If out of bounds, return a default value
		if (!isInbound())
		{
			std::cerr << "[WARNING] DoubleDeque: Trying to access an element that is out of bounds." << std::endl;
			return new T();
		}
		else
		{
			return &rows.at(y + m_oy)->at(x + m_ox);
		}
	}

	// Add the value at the given position, resizing the DoubleDeque if necessary
	void addAt(T value, int x, int y)
	{
		if (x < 0) 
		{
			upsizeLeft(abs(x));
		}
		if (y < 0) 
		{
			upsizeUp(abs(y));
		}
		if (x >= m_width)
		{
			upsizeRight(x - m_width + 1);
		}
		if (y >= m_heigh)
		{
			upsizeDown(y - m_heigh + 1);
		}

		rows.at(y + m_oy)->at(x + m_ox) = T(value);
	}

	// Add size to the top of the DoubleDeque
	void upsizeUp(int size)
	{
		for (int y = 0; y < size; y++)
		{
			std::deque<T>* row = new std::deque<T>(m_width);
			rows.push_front(row);
		}
		m_oy += size; // Change in origin
		m_heigh += size;
	}

	// Add size to the bottom of the DoubleDeque
	void upsizeDown(int size)
	{
		for (int y = 0; y < size; y++)
		{
			std::deque<T>* row = new std::deque<T>(m_width);
			rows.push_back(row);
		}
		m_heigh += size;
	}

	// Add size to the left of the DoubleDeque
	void upsizeLeft(int size)
	{
		for (int x = 0; x < size; x++)
		{
			for (int y = 0; y < m_heigh; y++)
			{
				rows.at(y)->push_front(T());
			}
		}
		m_ox += size; // Change in origin
		m_width += size;
	}

	// Add size to the right of the DoubleDeque
	void upsizeRight(int size)
	{
		for (int x = 0; x < size; x++)
		{
			for (int y = 0; y < m_heigh; y++)
			{
				rows.at(y)->push_back(T());
			}
		}
		m_width += size;
	}

	// Draw the content of the DoubleDeque in the console
	void draw()
	{
		for (int y = 0; y < m_heigh; y++)
		{
			for (int x = 0; x < m_width; x++)
			{
				std::cout << this->rows.at(y)->at(x) << " ";
			}
			std::cout << std::endl;
		}
	}
};
