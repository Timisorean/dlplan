#ifndef DLP_SRC_GENERATOR_FEATURE_GENERATOR_IMPL_H_
#define DLP_SRC_GENERATOR_FEATURE_GENERATOR_IMPL_H_

#include <unordered_map>
#include <unordered_set>
#include <memory>

#include "../../include/dlp/core.h"

#include "types.h"

namespace std {
    /**
     * For combining hash value we use the boost::hash_combine one-liner.
     * https://stackoverflow.com/questions/20511347/a-good-hash-function-for-a-vector
     *
     * We provide custom specialization of std::hash that are injected in the namespace std.
     * https://en.cppreference.com/w/cpp/utility/hash
     */
    template<> struct hash<std::vector<int>> {
        std::size_t operator()(const std::vector<int>& denotation) const noexcept {
            std::size_t seed = denotation.size();
            for(const auto & i : denotation) {
                seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        }
    };

    template<> struct hash<std::vector<dlp::core::ConceptDenotation>>
    {
        std::size_t operator()(const std::vector<dlp::core::ConceptDenotation>& denotation) const noexcept {
            std::size_t seed = denotation.size();
            for (const auto& v : denotation) {
                for(const auto& i : v) {
                    seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
                }
            }
            return seed;
        }
    };

    template<> struct hash<std::vector<dlp::core::RoleDenotation>> {
        std::size_t operator()(const std::vector<dlp::core::RoleDenotation>& denotation) const noexcept {
            std::size_t seed = denotation.size();
            for (const auto& v : denotation) {
                for(const auto& i : v) {
                    seed ^= i.first + 0x9e3779b9 + (seed << 6) + (seed >> 2);
                    seed ^= i.second + 0x9e3779b9 + (seed << 6) + (seed >> 2);
                }
            }
            return seed;
        }
    };
}

namespace dlp {
namespace core {
    class Concept;
    class Role;
    class Numerical;
    class Boolean;
    class SyntacticElementFactory;
}
namespace generator {
class FeatureCollection;


class FeatureGeneratorImpl {
private:
    const std::shared_ptr<core::SyntacticElementFactory> m_factory;
    const int m_complexity;
    const int m_time_limit;

    /**
     * Generated features sorted by their complexity.
     */
    std::vector<std::vector<dlp::core::Concept>> m_concept_elements_by_complexity;
    std::vector<std::vector<dlp::core::Role>> m_role_elements_by_complexity;
    std::vector<std::vector<dlp::core::Numerical>> m_numerical_elements_by_complexity;
    std::vector<std::vector<dlp::core::Boolean>> m_boolean_elements_by_complexity;

    /**
     * For checking syntactic equivalence.
     */
    std::unordered_set<std::string> m_concept_element_cache;
    std::unordered_set<std::string> m_role_element_cache;
    std::unordered_set<std::string> m_numerical_element_cache;
    std::unordered_set<std::string> m_boolean_element_cache;

    /**
     * For checking sample state equivalence.
     */
    std::unordered_set<std::vector<bool>> m_boolean_denotation_cache;
    std::unordered_set<std::vector<int>> m_numerical_denotation_cache;
    std::unordered_set<std::vector<core::ConceptDenotation>> m_concept_denotation_cache;
    std::unordered_set<std::vector<core::RoleDenotation>> m_role_denotation_cache;

    /**
     * Collect some statistics
     */
    int m_cache_hits;
    int m_cache_misses;

private:
    /**
     * Adds the respective element if it is syntactically and empirically unique.
     * Additionally, adds information related to them in the FeatureCollection.
     * Those features are the final result of the feature generation.
     */
    void add_concept(const States& states, core::Concept&& concept);
    void add_role(const States& states, core::Role&& role);
    void add_numerical(const States& states, core::Numerical&& numerical, FeatureCollection& feature_collection);
    void add_boolean(const States& states, core::Boolean&& boolean, FeatureCollection& feature_collection);

    /**
     * Generates all Elements with complexity 1.
     */
    void generate_base(const States& states);

    void generate_primitive_concepts(const States& states);
    void generate_primitive_roles(const States& states);
    void generate_top_role(const States& states);
    void generate_bot_concept(const States& states);
    void generate_top_concept(const States& states);

    /**
     * Inductively generate Elements of higher complexity.
     */
    void generate_inductively(const States& states, FeatureCollection& feature_collection);

    void generate_empty_boolean(const States& states, int iteration, FeatureCollection& feature_collection);
    void generate_all_concept(const States& states, int iteration);
    void generate_and_concept(const States& states, int iteration);
    void generate_diff_concept(const States& states, int iteration);
    void generate_not_concept(const States& states, int iteration);
    void generate_one_of_concept(const States& states, int iteration);
    void generate_or_concept(const States& states, int iteration);
    void generate_some_concept(const States& states, int iteration);  // TODO(dominik): should we add this?
    void generate_subset_concept(const States& states, int iteration);

    void generate_concept_distance_numerical(const States& states, int iteration, FeatureCollection& feature_collection);
    void generate_count_numerical(const States& states, int iteration, FeatureCollection& feature_collection);
    void generate_role_distance_numerical(const States& states, int iteration, FeatureCollection& feature_collection);
    void generate_sum_concept_distance_numerical(const States& states, int iteration, FeatureCollection& feature_collection);
    void generate_sum_role_distance_numerical(const States& states, int iteration, FeatureCollection& feature_collection);

    void generate_and_role(const States& states, int iteration);
    void generate_compose_role(const States& states, int iteration);
    void generate_diff_role(const States& states, int iteration);
    void generate_identity_role(const States& states, int iteration);
    void generate_inverse_role(const States& states, int iteration);
    void generate_not_role(const States& states, int iteration);
    void generate_or_role(const States& states, int iteration);
    void generate_restrict_role(const States& states, int iteration);
    void generate_transitive_closure_role(const States& states, int iteration);
    void generate_transitive_reflexive_closure_role(const States& states, int iteration);

public:
    FeatureGeneratorImpl(std::shared_ptr<core::SyntacticElementFactory> factory, int complexity, int time_limit);

    /**
     * Exhaustively generates features with pairwise disjoint feature evaluations on the states.
     */
    FeatureCollection generate(const States& states);

    /**
     * Print some brief overview.
     */
    void print_brief_statistics() const;

    /**
     * Print complete statistics.
     */
    void print_overall_statistics() const;
};

}
}


#endif
