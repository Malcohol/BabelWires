/**
 * The type of text values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/babelWiresLibExport.hpp>
#include <BabelWiresLib/TypeSystem/type.hpp>
#include <BabelWiresLib/TypeSystem/registeredType.hpp>

namespace babelwires {

    class BABELWIRESLIB_API TextType : public Type {
      public:
        DOWNCASTABLE(TextType, Type);
        
        TextType(TypeExp&& typeExpOfThis, size_t maxLength = s_maxPossibleLength);

        NewValueHolder createValue(const TypeSystem& typeSystem) const override;

        bool visitValue(const TypeSystem& typeSystem, const Value& v, ChildValueVisitor& visitor) const override;

        std::string getFlavour() const override;

        std::optional<SubtypeOrder> compareSubtypeHelper(const TypeSystem& typeSystem, const Type& other) const override;

        std::string valueToString(const TypeSystem& typeSystem, const ValueHolder& v) const override;

      private:
        /// The length value used when no length limit is specified.
        static constexpr std::size_t s_maxPossibleLength = std::numeric_limits<std::size_t>::max();

        /// The maximum length of the text.
        std::size_t m_maxLength = s_maxPossibleLength;
    };

    /// The standard Text type which doesn't specify a length.
    class BABELWIRESLIB_API DefaultTextType : public TextType {
      public:
        DOWNCASTABLE(DefaultTextType, TextType);

        DefaultTextType();

        REGISTERED_TYPE("Text", "Text", "0e422e25-cb94-40a3-8790-4918c918e637", 1);
    };
}
