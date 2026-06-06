/**
 * Queryable interface macros.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/Hash/hash.hpp>
#include <BaseLib/Utilities/classUniqueString.hpp>
#include <BaseLib/Utilities/utilityMacros.hpp>

#include <concepts>
#include <cstdint>

namespace babelwires {
	namespace detail {
		template <typename T>
		concept QueryableInterface = requires {
			{
				T::getInterfaceIdStatic()
			} -> std::convertible_to<std::uint64_t>;
		};
	} // namespace detail
} // namespace babelwires

#define BW_QUERY_INTERFACE_CASE(INTERFACE)                                                                            \
	if (interfaceId == INTERFACE::getInterfaceIdStatic()) {                                                           \
		return static_cast<INTERFACE*>(this);                                                                         \
	}

#define BW_QUERY_INTERFACE_CASE_CONST(INTERFACE)                                                                      \
	if (interfaceId == INTERFACE::getInterfaceIdStatic()) {                                                           \
		return static_cast<const INTERFACE*>(this);                                                                   \
	}

/// Declare a pure capability interface that can be queried from an owning hierarchy.
#define QUERYABLE_INTERFACE(INTERFACE)                                                                                \
	static constexpr std::uint64_t getInterfaceIdStatic() {                                                           \
		return babelwires::hash::stableStringHash(babelwires::getClassUniqueString<INTERFACE>());                    \
	}

/// Add support for querying capability interfaces from a base class.
///
/// Classes using this macro can expose capabilities with tryInterface<T>().
/// Derived classes should use INTERFACE_QUERYABLE(PARENT, ...) to advertise the
/// interfaces they implement.
#define INTERFACE_QUERYABLE_BASE()                                                                                    \
	template <typename T> T* tryInterface() {                                                                         \
		static_assert(babelwires::detail::QueryableInterface<T>, "T must declare QUERYABLE_INTERFACE");             \
		return static_cast<T*>(queryInterface(T::getInterfaceIdStatic()));                                            \
	}                                                                                                                 \
	template <typename T> const T* tryInterface() const {                                                             \
		static_assert(babelwires::detail::QueryableInterface<T>, "T must declare QUERYABLE_INTERFACE");             \
		return static_cast<const T*>(queryInterface(T::getInterfaceIdStatic()));                                      \
	}                                                                                                                 \
	virtual void* queryInterface(std::uint64_t interfaceId) {                                                         \
		(void)interfaceId;                                                                                            \
		return nullptr;                                                                                               \
	}                                                                                                                 \
	virtual const void* queryInterface(std::uint64_t interfaceId) const {                                             \
		(void)interfaceId;                                                                                            \
		return nullptr;                                                                                               \
	}

/// Advertise one or more capability interfaces from a derived class.
///
/// PARENT must already provide INTERFACE_QUERYABLE_BASE() or INTERFACE_QUERYABLE(...).
/// Each listed interface must use QUERYABLE_INTERFACE.
/// Supports up to 8 interfaces per class.
#define INTERFACE_QUERYABLE(PARENT, ...)                                                                              \
	void* queryInterface(std::uint64_t interfaceId) override {                                                        \
		BW_FOR_EACH(BW_QUERY_INTERFACE_CASE, __VA_ARGS__)                                                             \
		return PARENT::queryInterface(interfaceId);                                                                   \
	}                                                                                                                 \
	const void* queryInterface(std::uint64_t interfaceId) const override {                                            \
		BW_FOR_EACH(BW_QUERY_INTERFACE_CASE_CONST, __VA_ARGS__)                                                       \
		return PARENT::queryInterface(interfaceId);                                                                   \
	}
