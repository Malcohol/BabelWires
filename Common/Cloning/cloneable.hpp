/**
 * Functionality supporting clone methods.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <memory>
#include <type_traits>

namespace babelwires {

    /// Use this at the base of a hierarchy of cloneable classes.
    /// Note: The expected semantics of copy contructors of any class deriving from Cloneable
    /// is that it should deep-clone its members.
    struct Cloneable {
        virtual ~Cloneable();
        virtual Cloneable* cloneImpl() const& = 0;
        virtual Cloneable* cloneImpl() && = 0;
    };

/// Use this macros in abstract classes which derive from Cloneable.
#define CLONEABLE_ABSTRACT(CLASS)                                                                                      \
    std::unique_ptr<CLASS> clone() const& { return std::unique_ptr<CLASS>(static_cast<CLASS*>(cloneImpl())); }         \
    std::unique_ptr<CLASS> clone()&& {                                                                                 \
        return std::unique_ptr<CLASS>(static_cast<CLASS*>(std::move(*this).cloneImpl()));                              \
    }

/// Use this macros in classes which derive from Cloneable.
#define CLONEABLE(CLASS)                                                                                               \
    std::unique_ptr<CLASS> clone() const& { return std::unique_ptr<CLASS>(cloneImpl()); }                              \
    virtual CLASS* cloneImpl() const& override { return new CLASS(*this); }                                            \
    std::unique_ptr<CLASS> clone()&& { return std::unique_ptr<CLASS>(std::move(*this).cloneImpl()); }                  \
    virtual CLASS* cloneImpl() && override { return new CLASS(std::move(*this)); }

    /// Use this at the base of a hierarchy of cloneable classes to support non-default clone behaviour.
    /// Note: Classes T deriving from CustomCloneable must have a constructor taking a T and a Context.
    /// The motivating idea is to support deep clones with non-owning-pointer patching: See the unit test.
    template <typename Context> struct CustomCloneable {
        using CustomCloneContext = Context;

        virtual ~CustomCloneable(){};
        virtual CustomCloneable* customCloneImpl(CustomCloneContext& c) const = 0;
    };

/// Use this macros in abstract classes which derive from Cloneable.
#define CUSTOM_CLONEABLE_ABSTRACT(CLASS)                                                                               \
    std::unique_ptr<CLASS> customClone(CustomCloneContext& c) const {                                                  \
        return std::unique_ptr<CLASS>(static_cast<CLASS*>(customCloneImpl(c)));                                        \
    }                                                                                                                  \
    std::enable_if_t<std::is_default_constructible_v<CustomCloneContext>, std::unique_ptr<CLASS>> customClone()        \
        const {                                                                                                        \
        CustomCloneContext c;                                                                                          \
        return customClone(c);                                                                                         \
    }

/// Use this macros in classes which derive from Cloneable.
#define CUSTOM_CLONEABLE(CLASS)                                                                                        \
    std::unique_ptr<CLASS> customClone(CustomCloneContext& c) const {                                                  \
        return std::unique_ptr<CLASS>(customCloneImpl(c));                                                             \
    }                                                                                                                  \
    std::enable_if_t<std::is_default_constructible_v<CustomCloneContext>, std::unique_ptr<CLASS>> customClone()        \
        const {                                                                                                        \
        CustomCloneContext c;                                                                                          \
        return customClone(c);                                                                                         \
    }                                                                                                                  \
    virtual CLASS* customCloneImpl(CustomCloneContext& c) const override { return new CLASS(*this, c); }
} // namespace babelwires
