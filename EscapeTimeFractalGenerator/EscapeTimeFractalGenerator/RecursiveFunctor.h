#pragma once
#include <tuple>

template<class TOut, class... TIn> struct RecursiveFunctor // Functor used for efficiently passing recursive functions into recursive functions. 
//"parent" should be a RecursiveFunctor on the stack that you can guarantee won't go out of scope when this functor is used.
// If "parent" is null you need to be careful not to use it in the associate "func" function
{
public:
	RecursiveFunctor(const RecursiveFunctor&) = delete;
	RecursiveFunctor(RecursiveFunctor&&) = delete;
	RecursiveFunctor& operator =(const RecursiveFunctor&) = delete;
	RecursiveFunctor& operator =(RecursiveFunctor&&) = delete;
	static void* operator new     (size_t) = delete;
	static void* operator new[](size_t) = delete;
	static void  operator delete  (void*) = delete;
	static void  operator delete[](void*) = delete;

	typedef TOut(*Func)(const RecursiveFunctor<TOut, TIn...>* parent, TIn... inputs);
	static RecursiveFunctor<TOut, TIn...> MakeInstance(const RecursiveFunctor<TOut, TIn...>* parent, Func func) 
	{
		return RecursiveFunctor(parent, func);
	};
	TOut operator()(TIn... inputs) const
	{
		return func(parent, std::forward<TIn>(inputs)...);
	};

	std::tuple<TIn...> inputStorage;

private:
	const RecursiveFunctor<TOut, TIn...>* parent;
	const Func func;
	RecursiveFunctor(const RecursiveFunctor<TOut, TIn...>* parent, Func func) : parent(parent), func(func) {}
};