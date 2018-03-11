#pragma once

#include <iterator>


namespace memory {

template <typename T, typename = void> struct has_no_iterators : std::true_type {};
template <typename T> struct has_no_iterators<T, std::void_t<decltype(std::declval<T>().begin())>> : std::false_type {};

template <typename T, typename = void> struct has_bucket_methods : std::false_type {};
template <typename T> struct has_bucket_methods<T, std::void_t<decltype(std::declval<T>().bucket_count())>> : std::true_type {};


template <typename Container, typename Iterator = decltype(std::declval<Container>().begin(0))>
struct bucket_iterator : std::iterator<std::forward_iterator_tag, std::decay_t<decltype(*std::declval<Iterator>())>> {
	Container const& container; Iterator begin, end; size_t bucket;
	bucket_iterator(Container const& container, size_t bucket, Iterator begin, Iterator end)
		: container(container), begin(begin), end(end), bucket(bucket) { }

	auto operator*() const { return *begin; }
	bool operator!=(bucket_iterator rhs) const { return begin != rhs.begin; }
	bool operator==(bucket_iterator rhs) const { return not (*this != rhs); }

	auto operator++(int) { auto previous = *this; ++(*this); return previous; }
	auto& operator++() {
		if (++begin == end and ++bucket != container.bucket_count()) {
			begin = container.begin(bucket);
			end = container.end(bucket);
		}

		return *this;
	}
};


}


namespace std {

template <class T,
	typename = std::enable_if_t<memory::has_no_iterators<T>::value>,
	typename = std::enable_if_t<memory::has_bucket_methods<T>::value>>
auto begin(T const& x) {
	using type = decltype(x.begin(0));
	return memory::bucket_iterator<T>(x, 0,
		x.bucket_count() ? x.begin(0) : type{},
		x.bucket_count() ? x.end(0) : type{});
}

template <class T,
	typename = std::enable_if_t<memory::has_no_iterators<T>::value>,
	typename = std::enable_if_t<memory::has_bucket_methods<T>::value>>
auto end(T const& x) {
	using type = decltype(x.end(0));
	auto bucket = x.bucket_count();
	return memory::bucket_iterator<T>(x, bucket,
		x.bucket_count() ? x.end(bucket - 1) : type{},
		x.bucket_count() ? x.end(bucket - 1) : type{});
}


}


