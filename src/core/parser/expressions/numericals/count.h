#ifndef DLP_SRC_CORE_PARSER_EXPRESSIONS_NUMERICAL_COUNT_H_
#define DLP_SRC_CORE_PARSER_EXPRESSIONS_NUMERICAL_COUNT_H_

#include "../numerical.h"
#include "../../../elements/numericals/count.h"

namespace dlp {
namespace core {
namespace parser {

class CountNumerical : public Numerical {
protected:
    virtual element::Numerical_Ptr parse_numerical_impl(const VocabularyInfoImpl& vocabulary, ElementCache &cache) const override {
        if (m_children.size() != 1) {
            throw std::runtime_error("CountNumerical::parse_numerical_impl - number of children ("s + std::to_string(m_children.size()) + " != 1).");
        }
        // 1. Parse children
        element::Concept_Ptr concept = m_children[0]->parse_concept(vocabulary, cache);
        if (concept) {
            return std::make_shared<element::CountNumerical<element::Concept_Ptr>>(vocabulary, concept);
        }
        element::Role_Ptr role = m_children[0]->parse_role(vocabulary, cache);
        if (role) {
            return std::make_shared<element::CountNumerical<element::Role_Ptr>>(vocabulary, role);
        }
        // 2. Construct element
        throw std::runtime_error("CountNumerical::parse_numerical_impl - unable to construct children elements.");
    }

public:
    CountNumerical(const std::string &name, std::vector<std::unique_ptr<Expression>> &&children)
    : Numerical(name, std::move(children)) { }
};

}
}
}

#endif
