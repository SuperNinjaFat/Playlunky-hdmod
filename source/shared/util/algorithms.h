#pragma once

#include <filesystem>
#include <type_traits>
#include <string_view>
#include <utility>

namespace algo {
	template<class T, class U>
	concept is_comparable = requires(T&& lhs, T&& rhs) {
		lhs == rhs;
		rhs == lhs;
	};
	
	template<class T>
	concept range = requires (T&& cont) {
		begin(cont);
		end(cont);
	};

	template<class T>
	requires range<T>
	using range_element_t = decltype(*begin(std::declval<T>()));
	template<class T>
	requires range<T>
	using range_value_t = std::remove_reference_t<decltype(*begin(std::declval<T>()))>;
	
	template<class T, class U>
	requires range<T>
	inline constexpr bool range_contains_v = std::is_same_v<std::decay_t<range_element_t<T>>, std::decay_t<U>>;

	template<class ContainerT, class FunT>
	requires range<ContainerT> && std::is_invocable_v<FunT, range_element_t<ContainerT>>
	void erase_if(ContainerT&& container, FunT&& fun) {
		const auto begin_it = begin(container);
		const auto end_it = end(container);
		container.erase(std::remove_if(begin_it, end_it, std::forward<FunT>(fun)), end_it);
	}
	template<class ContainerT, class ValueT>
	requires range<ContainerT> && is_comparable<range_element_t<ContainerT>, ValueT>
	void erase(ContainerT&& container, ValueT&& value) {
		const auto begin_it = begin(container);
		const auto end_it = end(container);
		container.erase(std::remove(begin_it, end_it, std::forward<ValueT>(value)), end_it);
	}
	template<class ContainerT, class T, class U, class V>
	requires range<ContainerT> && range_contains_v<ContainerT, T> && is_comparable<U, V>
	void erase(ContainerT&& container, U T::* member, V&& val) {
		erase_if(std::forward<ContainerT>(container), [member, val = std::forward<V>(val)](auto& element) {
			return element.*member == val;
		});
	}

	template<class ContainerT, class FunT>
	requires range<ContainerT> && std::is_invocable_v<FunT, range_element_t<ContainerT>>
	auto find_if(ContainerT&& container, FunT&& fun) -> range_value_t<ContainerT>* {
		const auto begin_it = begin(container);
		const auto end_it = end(container);
		auto found_it = std::find_if(begin_it, end_it, std::forward<FunT>(fun));
		if (found_it != end_it) {
			return &*found_it;
		}
		return nullptr;
	}
	template<class ContainerT, class ValueT>
	requires range<ContainerT> && is_comparable<range_element_t<ContainerT>, ValueT>
	auto find(ContainerT&& container, ValueT&& value) -> range_value_t<ContainerT>* {
		const auto begin_it = begin(container);
		const auto end_it = end(container);
		auto found_it = std::find(begin_it, end_it, std::forward<ValueT>(value));
		if (found_it != end_it) {
			return &*found_it;
		}
		return nullptr;
	}
	template<class ContainerT, class T, class U, class V>
	requires range<ContainerT> && range_contains_v<ContainerT, T> && is_comparable<U, V>
	auto find(ContainerT&& container, U T::* member, V&& val) -> range_value_t<ContainerT>* {
		return find_if(std::forward<ContainerT>(container), [member, val = std::forward<V>(val)](auto& element) {
			return element.*member == val;
		});
	}

	template<class ContainerT, class FunT>
	requires range<ContainerT> && std::is_invocable_v<FunT, range_element_t<ContainerT>>
	bool contains_if(ContainerT&& container, FunT&& fun) {
		return find_if(std::forward<ContainerT>(container), std::forward<FunT>(fun)) != nullptr;
	}
	template<class ContainerT, class T, class U, class V>
	requires range<ContainerT> && range_contains_v<ContainerT, T> && is_comparable<U, V>
		bool contains(ContainerT&& container, U T::* member, V&& val) {
		return contains_if(std::forward<ContainerT>(container), [member, val = std::forward<V>(val)](auto& element) {
			return element.*member == val;
		});
	}
	template<class ContainerT, class ValueT>
	requires range<ContainerT> && is_comparable<range_element_t<ContainerT>, ValueT>
	bool contains(ContainerT&& container, ValueT&& value) {
		const auto begin_it = begin(container);
		const auto end_it = end(container);
		return std::find(begin_it, end_it, std::forward<ValueT>(value)) != end_it;
	}

	template<class ContainerT, class ValueT>
	requires range<ContainerT> && is_comparable<range_element_t<ContainerT>, ValueT>
	auto count(ContainerT&& container, ValueT&& value) {
		const auto begin_it = begin(container);
		const auto end_it = end(container);
		return static_cast<std::size_t>(std::count(begin_it, end_it, std::forward<ValueT>(value)));
	}
	template<class ContainerT, class FunT>
	requires range<ContainerT> && std::is_invocable_v<FunT, range_element_t<ContainerT>>
	auto count_if(ContainerT&& container, FunT&& fun) {
		const auto begin_it = begin(container);
		const auto end_it = end(container);
		return static_cast<std::size_t>(std::count_if(begin_it, end_it, std::forward<FunT>(fun)));
	}

	bool is_sub_path(const std::filesystem::path& path, const std::filesystem::path& base);
	bool is_end_of_path(const std::filesystem::path& path, const std::filesystem::path& base);

	std::string trim(std::string str);
	std::string trim(std::string str, char to_strip);

	bool case_insensitive_equal(std::string_view lhs, std::string_view rhs);
	template<std::size_t N>
	inline bool case_insensitive_equal(char (&lhs)[N], std::string_view rhs) {
		return case_insensitive_equal(std::string_view{ lhs, N }, rhs);
	}
	template<std::size_t N>
	inline bool case_insensitive_equal(std::string_view lhs, char (&rhs)[N]) {
		return case_insensitive_equal(lhs, std::string_view{ rhs, N });
	}
	template<std::size_t N, std::size_t M>
	inline bool case_insensitive_equal(char (&lhs)[N], char (&rhs)[M]) {
		return case_insensitive_equal(std::string_view{ lhs, N }, std::string_view{ rhs, M });
	}
}
