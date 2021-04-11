#pragma once

namespace babelwires {

    /// A trait which identifies types which can be serialized as values, by checking for serializeToString and
    /// deserializeFromString members.
    // TODO Consider using an ADL-based approach instead.
    template <typename T> class IsSerializableValue {
      private:
        using Yes = char[1];
        using No = char[2];

        template <typename U> static Yes& test(decltype(&U::serializeToString), decltype(&U::deserializeFromString));
        template <typename U> static No& test(...);

      public:
        enum { value = sizeof(test<T>(0, 0)) == sizeof(Yes) };
    };

} // namespace babelwires
