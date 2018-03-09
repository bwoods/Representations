#pragma once

template <class Container, class Function,
   typename = std::void_t<decltype(std::declval(Container().begin()))>,
   typename = std::void_t<decltype(std::declval(Container().end()))>
> inline
void apply(Container const& container, Function&& function) {
	std::for_each(container.begin(), container.end(), std::forward<Function>(function));
};
