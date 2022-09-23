#ifndef DLPLAN_SRC_CORE_ELEMENTS_CONCEPTS_BOT_H_
#define DLPLAN_SRC_CORE_ELEMENTS_CONCEPTS_BOT_H_

#include "../concept.h"


namespace dlplan::core::element {

class BotConcept : public Concept {
public:
    BotConcept(const VocabularyInfo& vocabulary)
    : Concept(vocabulary) { }

    ConceptDenotation evaluate(const State& state) const override {
        return ConceptDenotation(state.get_instance_info_ref().get_num_objects());
    }

    const ConceptDenotations& evaluate(const States& states, DenotationsCaches& caches) const override {
        // check if denotations is cached.
        auto cached = caches.m_c_denots_mapping.find(get_index());
        if (cached != caches.m_c_denots_mapping.end()) return cached->second;
        // allocate memory for new denotations
        ConceptDenotationsPtr denotations = std::make_unique<ConceptDenotations>();
        denotations->reserve(states.size());
        // get denotations of children
        for (size_t i = 0; i < states.size(); ++i) {
            const auto& state = states[i];
            int num_objects = state.get_instance_info_ref().get_num_objects();
            ConceptDenotationPtr denotation = std::make_unique<ConceptDenotation>(ConceptDenotation(num_objects));
            // register denotation and append it to denotations.
            denotations->push_back(std::cref(*caches.m_c_denot_cache.insert(std::move(denotation)).first->get()));
        }
        // register denotations and return it.
        auto result_denotations = std::cref(*caches.m_c_denots_cache.insert(std::move(denotations)).first->get());
        caches.m_c_denots_mapping.emplace(get_index(), result_denotations);
        return result_denotations;
    }

    int compute_complexity() const override {
        return 1;
    }

    void compute_repr(std::stringstream& out) const override {
        out << get_name();
    }

    static std::string get_name() {
        return "c_bot";
    }
};

}

#endif
