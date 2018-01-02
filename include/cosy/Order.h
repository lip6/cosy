#ifndef INCLUDE_COSY_ORDER_H_
#define INCLUDE_COSY_ORDER_H_

#include <algorithm>
#include <functional>
#include <vector>
#include <string>
#include <unordered_set>
#include <type_traits>

#include "cosy/int_type_indexed_vector.h"
#include "cosy/CNFModel.h"
#include "cosy/Orbits.h"
#include "cosy/PermutationGroup.h"
#include "cosy/Literal.h"

namespace cosy {

enum VariableOrderType {
    INCREASE,
    DECREASE,
    OCCURENCE,
    BREAKID,
    AUTO,
    EMPTY,
    CUSTOM,
};

enum ValueOrderType {
    TRUE_LESS_FALSE,
    FALSE_LESS_TRUE,
};

class Order {
 public:
    static Order* create(unsigned int num_vars,
                         VariableOrderType variable_order_type,
                         ValueOrderType value_order_type,
                         const CNFModel& model,
                         const PermutationGroup& group);

    static Order* create(unsigned int num_vars,
                                std::vector<Literal> literals,
                                ValueOrderType value_order_type);

    virtual void assign(unsigned int num_vars) final {
        _num_vars = num_vars;
    }

    std::vector<Literal>::const_iterator begin() const { return _order.begin(); }
    std::vector<Literal>::const_iterator end()   const { return _order.end();   }

    virtual const std::vector<Literal>& order() const final { return _order; }
    virtual unsigned int size() const final { return _order.size(); }

    virtual void addToOrder(const Literal& literal) final;
    virtual bool contains(const Literal& literal) const;

    virtual Literal leq(const Literal &l, const Literal &r) const final;
    virtual ValueOrderType valueOrderType() const { return _valueOrderType; }

    virtual void debugPrint() const final;
    virtual std::string variableInfo() const = 0;
    virtual std::string valueInfo() const {
        if (_valueOrderType == TRUE_LESS_FALSE) return "True < False";
        if (_valueOrderType == FALSE_LESS_TRUE) return "False < True";
        return "unknown";
    }

 protected:
    unsigned int _num_vars;
    std::vector<Literal> _order;
    std::unordered_map<Literal, unsigned int> _indexes;
    ValueOrderType _valueOrderType;
};

/* --------------------------------------------------------------------------- */
class EmptyOrder : public Order {
 public:
    EmptyOrder(unsigned int num_vars) { assign(num_vars); }
    virtual std::string variableInfo() const { return std::string("empty"); }
};

/* --------------------------------------------------------------------------- */

class IncreaseOrder : public Order {
 public:
    IncreaseOrder(unsigned int num_vars) {
        assign(num_vars);
        for (BooleanVariable var(0); var < num_vars; ++var)
            addToOrder(Literal(var, true));
        DCHECK(_order.size() == _num_vars);
    }

    virtual std::string variableInfo() const { return std::string("increase"); }
};

class DecreaseOrder : public Order {
 public:
    DecreaseOrder(unsigned int num_vars) {
        assign(num_vars);
        for (BooleanVariable var(num_vars-1); var >= 0; --var)
            addToOrder(Literal(var, true));
        DCHECK(_order.size() == _num_vars);
    }

    virtual std::string variableInfo() const { return std::string("decrease"); }
};

/* --------------------------------------------------------------------------- */
struct ValueOrderLt {
    const ITIVector<BooleanVariable, unsigned int>& values;
        bool operator() (int i, int j) {
            BooleanVariable x(i);
            BooleanVariable y(j);
            if (values[x] != values[y])
                return values[x] > values[y];
            return x < y;
        }
    explicit ValueOrderLt(const ITIVector<BooleanVariable, unsigned int>& v) :
        values(v) {}
};

class OccurenceOrder : public Order {
 public:
    OccurenceOrder(unsigned int num_vars, const CNFModel& model) {
        std::vector<unsigned int> indexes;
        assign(num_vars);

        indexes.resize(num_vars);
        for (unsigned int i=0; i < num_vars; ++i)
            indexes[i] = i;

        std::sort(indexes.begin(), indexes.end(),
                  ValueOrderLt(model.occurences()));

        for (unsigned int i=0; i < num_vars; ++i)
            addToOrder(Literal(BooleanVariable(indexes[i]), true));
        DCHECK(_order.size() == _num_vars);
    }

    virtual std::string variableInfo() const { return std::string("occurence"); }
};
/* --------------------------------------------------------------------------- */
class BreakIDOrder : public Order {
 public:
    BreakIDOrder(unsigned int num_vars, const PermutationGroup& group) {

        Orbits orbits;
        std::vector<Permutation*> Q, filter;
        std::unordered_map<BooleanVariable, int> occ_generators;
        std::unordered_set<BooleanVariable> used;

        unsigned int largestOrbitSize = 0;
        BooleanVariable next, var;

        assign(num_vars);

        for (const std::unique_ptr<Permutation>& p : group)
            Q.push_back(p.get());


        while (Q.size() > 0) {
            /* Compute occurence in generators */
            occ_generators.clear();
            for (const Permutation* p : Q)
                for (const Literal& lit : p->support())
                    if (lit.isPositive())
                        occ_generators[lit.variable()]++;

            orbits.assign(Q);

            largestOrbitSize = 0;
            next = kNoBooleanVariable;

            /* Find next variable with less occurences in the largest orbit */
            for (const ITIVector<BooleanVariable, BooleanVariable>& orbit :
                     orbits.orbits()) {
                if (orbit.size() < largestOrbitSize)
                    continue;
                for (const BooleanVariable& var : orbit) {
                    if (occ_generators[var] == 0)
                        continue;
                    if (next == kNoBooleanVariable ||
                        (occ_generators[var] < occ_generators[next]) ||
                        (occ_generators[var] ==  occ_generators[next] &&
                         var < next)) {
                        next = var;
                        largestOrbitSize = orbit.size();
                    }
                }
            }

            if (next == kNoBooleanVariable) {
                LOG(WARNING) << "generator seems not consistent";
                break;
            }

            addToOrder(Literal(next, true));
            used.insert(next);

            /* Filter Permutation */
            filter.clear();
            for (Permutation* p : Q) {
                bool found = false;
                for (const Literal& lit : p->support()) {
                    var = lit.variable();
                    if (var == next) {
                        found = true;
                        break;
                    }
                }
                if (!found)
                    filter.push_back(p);
            }
            Q = filter;
        }

        /* Complete order */


        for (unsigned int i=0; i<_num_vars; i++)
            if (used.find(BooleanVariable(i)) == used.end())
                addToOrder(Literal(BooleanVariable(i), true));
    }

    virtual std::string variableInfo() const { return std::string("breakid"); }

};

/* --------------------------------------------------------------------------- */

class CustomOrder : public Order {
 public:
    CustomOrder(unsigned int num_vars, const std::vector<Literal>& order) {
        assign(num_vars);
        for (const Literal& literal : order)
            addToOrder(literal);
    }
    virtual std::string variableInfo() const { return std::string("custom"); }
};

/* --------------------------------------------------------------------------- */


// static
inline Order* Order::create(unsigned int num_vars,
                            VariableOrderType variable_order_type,
                            ValueOrderType value_order_type,
                            const CNFModel& model,
                            const PermutationGroup& group) {

    Order *order = nullptr;

    switch (variable_order_type) {
    case INCREASE: order = new IncreaseOrder(num_vars);            break;
    case DECREASE: order = new DecreaseOrder(num_vars);            break;
    case OCCURENCE: order = new OccurenceOrder(num_vars, model);   break;
    case BREAKID : order = new BreakIDOrder(num_vars, group);      break;
    case EMPTY : order = new EmptyOrder(num_vars);                 break;
    case AUTO :
        if (group.numberOfInvertingVars() /
            static_cast<double>(model.numberOfVariables()) > 0.25)
            order = new BreakIDOrder(num_vars, group);
        else
            order =  new OccurenceOrder(num_vars, model);
        break;

    default: return nullptr;
    }

    order->_valueOrderType = value_order_type;

    return order;
}

inline Order* Order::create(unsigned int num_vars,
                            std::vector<Literal> literals,
                            ValueOrderType value_order_type) {

    Order *order = new CustomOrder(num_vars, literals);
    order->_valueOrderType = value_order_type;
    return order;
}


}  // namespace cosy
#endif  // INCLUDE_COSY_ORDER_H_
