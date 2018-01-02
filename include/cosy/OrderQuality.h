#ifndef INCLUDE_COSY_ORDERQUALITY_H_
#define INCLUDE_COSY_ORDERQUALITY_H_

#include <unordered_set>
#include <memory>
#include <string>
#include <iostream>

#include "cosy/Literal.h"

namespace cosy {

class Observe {
 public:
    explicit Observe(const std::string& name) : _name(name) {}
    virtual ~Observe() {}

    virtual void
    add(const std::vector< std::unique_ptr<PermutationStatus> >& statuses) = 0;

    virtual int value() const = 0;
    virtual std::string name() const { return _name; }

 protected:
    std::string _name;
};

class FullObserve : public Observe {
 public:
    explicit FullObserve(const std::string& name) : Observe(name) {}
    virtual ~FullObserve() {}

    virtual void
    add(const std::vector< std::unique_ptr<PermutationStatus> >& statuses) {
        _observed.clear();

        for (const std::unique_ptr<PermutationStatus>& status : statuses)
            status->observed(&_observed);
    }

    virtual int value() const { return _observed.size(); }

 private:
    std::unordered_set<BooleanVariable> _observed;
};

class LocalObserve : public Observe {
 public:
    explicit LocalObserve(const std::string& name) : Observe(name) {}
    ~LocalObserve() {}

    void add(const std::vector<std::unique_ptr<PermutationStatus>>& statuses) {
        _observed.clear();

        BooleanVariable var;
        for (const std::unique_ptr<PermutationStatus>& status : statuses) {
            if (status->isEndOfOrder())
                continue;
            var = status->lookupVariable();
            if (var != kNoBooleanVariable)
                _observed.insert(var);
            var = status->lookupVariableInverse();
            if (var != kNoBooleanVariable)
                _observed.insert(var);
        }
    }
    virtual int value() const { return _observed.size(); }

 private:
    std::unordered_set<BooleanVariable> _observed;
};


class AssignmentTracker {
 public:
    explicit AssignmentTracker(const std::string& name) : _name(name) {}
    ~AssignmentTracker() {}

    void registerVariable(const BooleanVariable& variable) {
        _track.insert(variable);
    }

    void regitserVariables(const std::vector<BooleanVariable>& variables) {
        for (const BooleanVariable& variable : variables)
            registerVariable(variable);
    }

    void add(const Assignment& assignment) {
        _affected = 0;
        for (const BooleanVariable& var : _track)
            if (assignment.variableIsAssigned(var))
                _affected++;
    }

    int value() const { return _affected; }
    std::string name() const { return _name; }


 private:
    std::string _name;
    std::unordered_set<BooleanVariable> _track;
    uint64_t _affected;

};

class OrderQuality {

 public:
    OrderQuality() : _is_header_printed(false) {}
    ~OrderQuality() {}

    void registerObserve(std::unique_ptr<Observe>&& observe) {
        _observed.emplace_back(observe.release());
    }

    void registerAssignmentTracker(std::unique_ptr<AssignmentTracker>&& track) {
        _trackers.emplace_back(track.release());
    }


    void update(const Assignment& assignment,
               const std::vector<std::unique_ptr<PermutationStatus>>& statuses) {

        for (const std::unique_ptr<Observe>& observe : _observed)
            observe->add(statuses);
        for (const std::unique_ptr<AssignmentTracker>& tracker : _trackers)
            tracker->add(assignment);

        dump();
    }

    void dump() {
        if (!_is_header_printed) {
            for (const std::unique_ptr<AssignmentTracker>& tracker : _trackers)
                std::cout << tracker->name() << ";";
            for (const std::unique_ptr<Observe>& observe : _observed)
                std::cout << observe->name() << ";";
            std::cout << std::endl;

            _is_header_printed = true;
        }
        for (const std::unique_ptr<AssignmentTracker>& tracker : _trackers)
            std::cout << tracker->value() << ";";
        for (const std::unique_ptr<Observe>& observe : _observed)
            std::cout << observe->value() << ";";
        std::cout << std::endl;
    }
 private:
    bool _is_header_printed;
    std::vector<std::unique_ptr<Observe>> _observed;
    std::vector<std::unique_ptr<AssignmentTracker>> _trackers;
};

}  // namespace cosy

#endif  // INCLUDE_COSY_ORDERQUALITY_H_
