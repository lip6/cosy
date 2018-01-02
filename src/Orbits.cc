#include "cosy/Orbits.h"

namespace cosy {

Orbits::Orbits() {
}

Orbits::Orbits(const std::vector< std::unique_ptr<Permutation> >& perms) {
    assign(perms);
}

Orbits::Orbits(const std::vector<Permutation*> perms) {
    assign(perms);
}

Orbits::~Orbits() {
}


void Orbits::assign(const std::vector< std::unique_ptr<Permutation> >& perms) {
    DisjointSets disjointSets;
    BooleanVariable element_var, image_var;
    std::unordered_set<BooleanVariable> symmetic_variables;

    clear();

    for (const std::unique_ptr<Permutation>& permutation : perms) {
        for (unsigned int c = 0; c < permutation->numberOfCycles(); ++c) {
            Literal element = permutation->lastElementInCycle(c);
            BooleanVariable element_var = element.variable();

            disjointSets.Add(element_var.value());
            for (const Literal& image : permutation->cycle(c)) {
                image_var = image.variable();

                symmetic_variables.insert(image_var);

                disjointSets.Add(image_var.value());
                disjointSets.Union(element_var.value(), image_var.value());

                element_var = image_var;
                element = image;
            }
        }
    }

    /* Build orbits */
    std::unordered_map<int, ITIVector<BooleanVariable, BooleanVariable>> clazz;

    for (const BooleanVariable& variable : symmetic_variables) {
        int representant = disjointSets.Find(variable.value());
        clazz[representant].push_back(variable);
    }

    for (const auto& pair : clazz)
        _orbits.emplace_back(std::move(pair.second));

}


void Orbits::assign(const std::vector<Permutation*> perms) {
    DisjointSets disjointSets;
    BooleanVariable element_var, image_var;
    std::unordered_set<BooleanVariable> symmetic_variables;

    clear();

    for (const Permutation* permutation : perms) {
        for (unsigned int c = 0; c < permutation->numberOfCycles(); ++c) {
            Literal element = permutation->lastElementInCycle(c);
            BooleanVariable element_var = element.variable();

            disjointSets.Add(element_var.value());
            for (const Literal& image : permutation->cycle(c)) {
                image_var = image.variable();

                symmetic_variables.insert(image_var);

                disjointSets.Add(image_var.value());
                disjointSets.Union(element_var.value(), image_var.value());

                element_var = image_var;
                element = image;
            }
        }
    }

    /* Build orbits */
    std::unordered_map<int, ITIVector<BooleanVariable, BooleanVariable>> clazz;

    for (const BooleanVariable& variable : symmetic_variables) {
        int representant = disjointSets.Find(variable.value());
        clazz[representant].push_back(variable);
    }

    for (const auto& pair : clazz)
        _orbits.emplace_back(std::move(pair.second));

}

void Orbits::clear() {
    _orbits.clear();
}


void Orbits::debugPrint() {
    for (const ITIVector<BooleanVariable, BooleanVariable>& orbit : _orbits) {
        for (const BooleanVariable& variable : orbit)
            LOG(INFO) << variable << " ";
        LOG(INFO) << std::endl;
    }
}


}  // namespace cosy
