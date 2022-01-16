#pragma once
#include <mutex>
template <class T> class ThreadSafeBuffer //TODO: Actually make thread safe
{
public:
	ThreadSafeBuffer(int width, int height)
	{
		std::lock_guard<std::mutex> lock1{ mtx };
		data = new T[height * width];
		this->width = width;
		this->height = height;
	}
	~ThreadSafeBuffer() 
	{
		std::lock_guard<std::mutex> lock1{ mtx };
		delete[] data;
	}
	void Resize(int newWidth, int newHeight)
	{
		std::lock_guard<std::mutex> lock1{ mtx };
		delete[] data;
		data = new T[newHeight * newWidth];
		this->width = newWidth;
		this->height = newHeight;
	}
	T& operator[](int index)
	{
		std::lock_guard<std::mutex> lock1{ mtx };
		return data[index];
	}
	unsigned int GetHeight()
	{
		std::lock_guard<std::mutex> lock1{ mtx };
		return this->height;
	}
	unsigned int GetWidth()
	{
		std::lock_guard<std::mutex> lock1{ mtx };
		return this->width;
	}
private:
	T* data;
	unsigned int height;
	unsigned int width;
	std::mutex mtx;
};