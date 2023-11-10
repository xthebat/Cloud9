template <typename F, typename Ret, typename A, typename... Rest>
Ret ResultOfHelper(Ret (F::*)(A, Rest...) const);

template <typename F>
struct TResultOf
{
	using Type = decltype(ResultOfHelper(&F::operator()));
};
