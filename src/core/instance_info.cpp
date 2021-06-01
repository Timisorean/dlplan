#include "instance_info.h"
#include "state.h"

#include <algorithm>
#include <iostream>
#include <sstream>


namespace dlp {
namespace core {

static bool exists(const std::string& name, std::unordered_map<std::string, unsigned>& mapping) {
    auto f = mapping.find(name);
    return (f != mapping.end());
}

static unsigned insert_or_retrieve(const std::string& name, std::unordered_map<std::string, unsigned>& mapping) {
    auto f = mapping.find(name);
    if (f == mapping.end()) {
        mapping.emplace(std::make_pair(name, mapping.size()));
        return mapping.size() - 1;
    }
    return f->second;
}

InstanceInfoImpl::InstanceInfoImpl(std::shared_ptr<VocabularyInfoImpl> vocabulary_info)
    : m_vocabulary_info(vocabulary_info) {}

AtomImpl InstanceInfoImpl::add_atom(const std::string &predicate_name, const Name_Vec &object_names) {
    if (!m_vocabulary_info->exists_predicate_name(predicate_name)) {
        throw std::runtime_error("InstanceInfoImpl::add_atom - name of predicate missing in vocabulary ("s + predicate_name + ")");
    } else if (m_vocabulary_info->get_predicate(m_vocabulary_info->get_predicate_idx(predicate_name)).m_arity != object_names.size()) {
        throw std::runtime_error("InstanceInfoImpl::add_atom - arity of predicate in vocabulary does not match with atom ("s + std::to_string(m_vocabulary_info->get_predicate(m_vocabulary_info->get_predicate_idx(predicate_name)).m_arity) + " != " + std::to_string(object_names.size()));
    }
    bool predicate_exists = exists(predicate_name, m_predicate_name_to_predicate_idx);
    if (!predicate_exists) {
        unsigned predicate_idx = m_predicates.size();
        m_predicates.push_back(PredicateImpl(predicate_name, predicate_idx, object_names.size()));
        m_predicate_name_to_predicate_idx.insert(std::make_pair(predicate_name, predicate_idx));
    }
    int atom_idx = m_atoms.size();
    std::stringstream ss;
    ss << predicate_name << "(";
    Index_Vec object_idxs;
    for (unsigned i = 0; i < object_names.size(); ++i) {
        const std::string& object_name = object_names[i];
        object_idxs.push_back(insert_or_retrieve(object_name, m_object_name_to_object_idx));
        ss << object_name;
        if (i < object_names.size() - 1) {
            ss << ",";
        }
    }
    ss << ")";
    std::string atom_name = ss.str();
    if (m_atom_name_to_atom_idx.find(atom_name) != m_atom_name_to_atom_idx.end()) {
        throw std::runtime_error("InstanceInfoImpl::add_atom - adding duplicate atom with name ("s + atom_name + ") is not allowed.");
    }
    m_atom_name_to_atom_idx.insert(std::make_pair(atom_name, atom_idx));
    int predicate_idx = insert_or_retrieve(predicate_name, m_predicate_name_to_predicate_idx);

    m_atoms.push_back(AtomImpl(atom_name, atom_idx, predicate_name, predicate_idx, object_names, object_idxs, false));
    return m_atoms.back();
}

AtomImpl InstanceInfoImpl::add_static_atom(const std::string& predicate_name, const Name_Vec& object_names) {
    const AtomImpl& atom = add_atom(predicate_name, object_names);
    m_atoms[atom.m_atom_idx].m_is_static = true;
    m_static_atom_idxs.push_back(atom.m_atom_idx);
    return atom;
}

StateImpl InstanceInfoImpl::parse_state(std::shared_ptr<InstanceInfoImpl> info, const Name_Vec& atom_names) const {
    Index_Vec atoms;
    atoms.reserve(atom_names.size() + m_static_atom_idxs.size());
    for (const auto& atom_name : atom_names) {
        auto p = m_atom_name_to_atom_idx.find(atom_name);
        if (p == m_atom_name_to_atom_idx.end()) {
            throw std::runtime_error("InstanceInfoImpl::parse_state - atom name ("s + atom_name + ") not found in instance.");
        }
        atoms.push_back(p->second);
    }
    atoms.insert(atoms.end(), m_static_atom_idxs.begin(), m_static_atom_idxs.end());
    return StateImpl(info, std::move(atoms));
}

StateImpl InstanceInfoImpl::convert_state(std::shared_ptr<InstanceInfoImpl> info, const std::vector<AtomImpl>& atoms) const {
    Index_Vec atom_indices;
    atom_indices.reserve(atoms.size() + m_static_atom_idxs.size());
    for (const auto& atom : atoms) {
        auto p = m_atom_name_to_atom_idx.find(atom.m_atom_name);
        if (p == m_atom_name_to_atom_idx.end()) {
            throw std::runtime_error("InstanceInfoImpl::parse_state - atom name ("s + atom.m_atom_name + ") not found in instance.");
        }
        atom_indices.push_back(p->second);
    }
    atom_indices.insert(atom_indices.end(), m_static_atom_idxs.begin(), m_static_atom_idxs.end());
    return StateImpl(info, std::move(atom_indices));
}

StateImpl InstanceInfoImpl::convert_state(std::shared_ptr<InstanceInfoImpl> info, const Index_Vec& atom_idxs) const {
    Index_Vec atom_indices;
    atom_indices.reserve(atom_idxs.size() + m_static_atom_idxs.size());
    atom_indices.insert(atom_indices.end(), atom_idxs.begin(), atom_idxs.end());
    atom_indices.insert(atom_indices.end(), m_static_atom_idxs.begin(), m_static_atom_idxs.end());
    return StateImpl(info, std::move(atom_indices));
}

const AtomImpl& InstanceInfoImpl::get_atom(unsigned atom_idx) const {
    return m_atoms[atom_idx];
}

const std::vector<AtomImpl>& InstanceInfoImpl::get_atoms() const {
    return m_atoms;
}

unsigned InstanceInfoImpl::get_num_objects() const {
    return m_object_name_to_object_idx.size();
}

}
}
