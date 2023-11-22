#include "../../include/dlplan/core.h"


namespace dlplan::core {

Role::Role(std::shared_ptr<VocabularyInfo> vocabulary_info, ElementIndex index, bool is_static)
    : BaseElement(index, vocabulary_info, is_static) {
}

Role::Role(const Role& other) = default;

Role& Role::operator=(const Role& other) = default;

Role::Role(Role&& other) = default;

Role& Role::operator=(Role&& other) = default;

Role::~Role() = default;

std::shared_ptr<const RoleDenotation> Role::evaluate(const State& state, DenotationsCaches& caches) const {
    auto cached = caches.role_denotation_cache.get_denotation(
        get_index(),
        state.get_instance_info()->get_index(),
        is_static() ? -1 : state.get_index());
    if (cached) return cached;
    auto denotation = caches.role_denotation_cache.insert_denotation(evaluate_impl(state, caches));
    caches.role_denotation_cache.insert_denotation(
        get_index(),
        state.get_instance_info()->get_index(),
        is_static() ? -1 : state.get_index(),
        denotation);
    return denotation;
}

std::shared_ptr<const RoleDenotations> Role::evaluate(const States& states, DenotationsCaches& caches) const {
    auto cached = caches.role_denotations_cache.get_denotation(get_index(), -1, -1);
    if (cached) return cached;
    auto result_denotations = caches.role_denotations_cache.insert_denotation(evaluate_impl(states, caches));
    caches.role_denotations_cache.insert_denotation(get_index(), -1, -1, result_denotations);
    return result_denotations;
}

}
