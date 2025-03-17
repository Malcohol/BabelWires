/**
 * A TypeRef describes a type.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/TypeSystem/Detail/typeNameFormatter.hpp>

#include <BabelWiresLib/TypeSystem/typeConstructorArguments.hpp>

#include <sstream>

#include <variant>

namespace {
    constexpr char openType = '{';
    constexpr char closeType = '}';
    constexpr char openValue = '[';
    constexpr char closeValue = ']';
    constexpr char operatorDelimiter = '|';
    constexpr char escapeCharacter = '\\';

    struct RepeatedOperator {
        unsigned int m_startIndex;
        std::string m_operator;
    };
    using SpecificArgument = unsigned int;
    // monostate == error.
    using FormattingExpression = std::variant<std::monostate, SpecificArgument, RepeatedOperator>;

    // Foo{0}[[[0], [0|+]]]

    // <integer>[|<operator>]<closingDelimiter>
    FormattingExpression parseFormattingExpression(std::string_view::const_iterator& it,
                                                   std::string_view::const_iterator end, const char closingDelimiter) {
        const char currentChar = *it;
        enum { parsingInteger, justReadEscapeCharacter, parsingOperator } state = parsingInteger;

        unsigned int argumentIndex = 0;

        std::ostringstream op;

        // Prevent "{}" meaning the same as "{0}".
        if (*it == closingDelimiter) {
            return {};
        }

        while (it < end) {
            const char currentChar = *it;
            switch (state) {
                case parsingInteger:
                    if ((currentChar >= '0') && (currentChar <= '9')) {
                        const std::size_t indexCharAsIndex = currentChar - '0';
                        argumentIndex = (10 * argumentIndex) + indexCharAsIndex;
                    } else if (currentChar == operatorDelimiter) {
                        state = parsingOperator;
                    } else if (currentChar == closingDelimiter) {
                        return argumentIndex;
                    } else {
                        return {};
                    }
                    break;
                case parsingOperator:
                    if (currentChar == closingDelimiter) {
                        return RepeatedOperator{argumentIndex, op.str()};
                    } else if (currentChar == escapeCharacter) {
                        state = justReadEscapeCharacter;
                    } else {
                        op << currentChar;
                    }
                    break;
                case justReadEscapeCharacter:
                    if ((currentChar == escapeCharacter) || (currentChar == closeType) || (currentChar == closeValue)) {
                        op << currentChar;
                        state = parsingOperator;
                    } else {
                        return {};
                    }
                    break;
            }
            ++it;
        }

        return {};
    }

    bool processFormattingExpression(const FormattingExpression& formattingExpression, std::ostream& os,
                                     const std::vector<std::string>& args) {
        struct VisitorMethods {
            bool operator()(std::monostate) { return false; }
            bool operator()(const SpecificArgument& index) {
                if (index >= m_args.size()) {
                    return false;
                }
                m_os << m_args[index];
                return true;
            }
            bool operator()(const RepeatedOperator& op) {
                if (op.m_startIndex >= m_args.size()) {
                    return false;
                }
                m_os << m_args[op.m_startIndex];
                for (int i = op.m_startIndex + 1; i < m_args.size(); ++i) {
                    m_os << op.m_operator << m_args[i];
                }
                return true;
            }
            std::ostream& m_os;
            const std::vector<std::string>& m_args;
        } visitorMethods{os, args};
        return std::visit(visitorMethods, formattingExpression);
    }
} // namespace

std::string babelwires::typeNameFormatter(std::string_view format, const std::vector<std::string>& typeArguments,
                                          const std::vector<std::string>& valueArguments) {
    if (format.size() < 2) {
        return {};
    }
    if (typeArguments.size() > babelwires::TypeConstructorArguments::s_maxNumArguments) {
        return {};
    }
    if (valueArguments.size() > babelwires::TypeConstructorArguments::s_maxNumArguments) {
        return {};
    }
    std::ostringstream oss;

    enum {
        normal,
        justReadOpenType,
        justReadCloseType,
        justReadOpenValue,
        justReadCloseValue,
    } state = normal;
    std::string_view::const_iterator it = format.begin();
    const std::string_view::const_iterator end = format.end();
    while (it < end) {
        const char currentChar = *it;

        switch (state) {
            case normal: {
                if (currentChar == openType) {
                    state = justReadOpenType;
                } else if (currentChar == closeType) {
                    state = justReadCloseType;
                } else if (currentChar == openValue) {
                    state = justReadOpenValue;
                } else if (currentChar == closeValue) {
                    state = justReadCloseValue;
                } else {
                    oss << currentChar;
                }
                break;
            }
            case justReadOpenType: {
                if (currentChar == openType) {
                    oss << openType;
                } else {
                    const auto formatExpression = parseFormattingExpression(it, end, closeType);
                    if (!processFormattingExpression(formatExpression, oss, typeArguments)) {
                        return {};
                    }
                }
                state = normal;
                break;
            }
            case justReadCloseType: {
                if (currentChar == closeType) {
                    oss << closeType;
                    state = normal;
                } else {
                    return {};
                }
                break;
            }
            case justReadOpenValue: {
                if (currentChar == openValue) {
                    oss << openValue;
                } else {
                    const auto formatExpression = parseFormattingExpression(it, end, closeValue);
                    if (!processFormattingExpression(formatExpression, oss, valueArguments)) {
                        return {};
                    }
                }
                state = normal;
                break;
            }
            case justReadCloseValue: {
                if (currentChar == closeValue) {
                    oss << closeValue;
                    state = normal;
                } else {
                    return {};
                }
                break;
            }
        }
        ++it;
    }
    if (state == normal) {
        return oss.str();
    } else {
        return {};
    }
}
