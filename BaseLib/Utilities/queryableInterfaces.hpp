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

#define BABELWIRES_CAT_IMPL(A, B) A##B
#define BABELWIRES_CAT(A, B) BABELWIRES_CAT_IMPL(A, B)

#define BABELWIRES_ARG_COUNT_IMPL(_1, _2, _3, _4, _5, _6, _7, _8, COUNT, ...) COUNT
#define BABELWIRES_ARG_COUNT(...) BABELWIRES_ARG_COUNT_IMPL(__VA_ARGS__ __VA_OPT__(,) 8, 7, 6, 5, 4, 3, 2, 1, 0)

#define BABELWIRES_FOR_EACH_0(MACRO)
#define BABELWIRES_FOR_EACH_1(MACRO, A1) MACRO(A1)
#define BABELWIRES_FOR_EACH_2(MACRO, A1, A2) MACRO(A1) MACRO(A2)
#define BABELWIRES_FOR_EACH_3(MACRO, A1, A2, A3) MACRO(A1) MACRO(A2) MACRO(A3)
#define BABELWIRES_FOR_EACH_4(MACRO, A1, A2, A3, A4) MACRO(A1) MACRO(A2) MACRO(A3) MACRO(A4)
#define BABELWIRES_FOR_EACH_5(MACRO, A1, A2, A3, A4, A5) MACRO(A1) MACRO(A2) MACRO(A3) MACRO(A4) MACRO(A5)
#define BABELWIRES_FOR_EACH_6(MACRO, A1, A2, A3, A4, A5, A6) MACRO(A1) MACRO(A2) MACRO(A3) MACRO(A4) MACRO(A5) MACRO(A6)
#define BABELWIRES_FOR_EACH_7(MACRO, A1, A2, A3, A4, A5, A6, A7)                                                     \
	MACRO(A1) MACRO(A2) MACRO(A3) MACRO(A4) MACRO(A5) MACRO(A6) MACRO(A7)
#define BABELWIRES_FOR_EACH_8(MACRO, A1, A2, A3, A4, A5, A6, A7, A8)                                                 \
	MACRO(A1) MACRO(A2) MACRO(A3) MACRO(A4) MACRO(A5) MACRO(A6) MACRO(A7) MACRO(A8)

#define BABELWIRES_FOR_EACH(MACRO, ...)                                                                               \
	BABELWIRES_CAT(BABELWIRES_FOR_EACH_, BABELWIRES_ARG_COUNT(__VA_ARGS__))(MACRO, __VA_ARGS__)

#define BABELWIRES_QUERY_INTERFACE_CASE(INTERFACE)                                                                    \
	if (interfaceId == INTERFACE::getInterfaceIdStatic()) {                                                           \
		return static_cast<INTERFACE*>(this);                                                                         \
	}

#define BABELWIRES_QUERY_INTERFACE_CASE_CONST(INTERFACE)                                                              \
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
		BABELWIRES_FOR_EACH(BABELWIRES_QUERY_INTERFACE_CASE, __VA_ARGS__)                                             \
		return PARENT::queryInterface(interfaceId);                                                                   \
	}                                                                                                                 \
	const void* queryInterface(std::uint64_t interfaceId) const override {                                            \
		BABELWIRES_FOR_EACH(BABELWIRES_QUERY_INTERFACE_CASE_CONST, __VA_ARGS__)                                       \
		return PARENT::queryInterface(interfaceId);                                                                   \
	}
