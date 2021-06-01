#ifndef DLP_SRC_CORE_PARSER_EXPRESSIONS_EXPRESSION_H_
#define DLP_SRC_CORE_PARSER_EXPRESSIONS_EXPRESSION_H_

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "../../elements/types.h"
#include "../../cache.h"
#include "../../vocabulary_info.h"


namespace dlp {
namespace core {
namespace parser {

/**
 * An Expression represents a node in an abstract syntax tree.
 */
class Expression {
protected:
    std::string m_name;
    std::vector<std::unique_ptr<Expression>> m_children;

public:
    Expression(const std::string &name,
        std::vector<std::unique_ptr<Expression>> &&children)
        : m_name(name), m_children(std::move(children)) {
    }
    // ~Expression() { };

    /**
     * Returns true if the expression is a leaf, i.e.,
     * an expression that represents a position in a predicate for projection.
     */
    bool is_leaf() const {
        return m_children.empty();
    }

    /**
     * Getters.
     */
    const std::string& get_name() const {
        return m_name;
    }

    // Note: since children are stored as std::vector<Expression>
    // we do not know what type the children will be
    // and each concrete type must decide what method to call
    // in order to parse into the desired type.
    // As a result we cannot change this into a template.

    /**
     * Tries to parse the Expression into a Concept.
     * If unsuccessful print human readable error messages and throw and exception.
     */
    virtual element::Concept_Ptr parse_concept(const VocabularyInfoImpl& vocabulary, ElementCache &cache) const {
        return nullptr;
    }

    /**
     * Tries to parse the Expression into a Role.
     * If unsuccessful print human readable error messages and throw and exception.
     */
    virtual element::Role_Ptr parse_role(const VocabularyInfoImpl& vocabulary, ElementCache &cache) const {
        return nullptr;
    }

    /**
     * Tries to parse the Expression into a Numerical.
     * If unsuccessful print human readable error messages and throw and exception.
     */
    virtual element::Numerical_Ptr parse_numerical(const VocabularyInfoImpl& vocabulary, ElementCache &cache) const {
        return nullptr;
    }

    /**
     * Tries to parse the Expression into a Boolean.
     * If unsuccessful print human readable error messages and throw and exception.
     */
    virtual element::Boolean_Ptr parse_boolean(const VocabularyInfoImpl& vocabulary, ElementCache &cache) const {
        return nullptr;
    }
};

}
}
}

#endif
