template <typename F, typename Ret, typename A, typename... Rest>
A FirstArgumentHelper(Ret (F::*)(A, Rest...));

template <typename F, typename Ret, typename A, typename... Rest>
A FirstArgumentHelper(Ret (F::*)(A, Rest...) const);

template <typename F>
struct TFirstArgumentOf
{
	using Type = decltype(FirstArgumentHelper(&F::operator()));
};
