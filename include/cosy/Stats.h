#ifndef INCLUDE_COSY_STATS_H_
#define INCLUDE_COSY_STATS_H_

#include <utility>
#include <deque>
#include <vector>
#include <string>

#include "cosy/Printer.h"
#include "cosy/macros.h"

namespace cosy {

// Forward Declaration
class StatsGroup;


class Stat {
 public:
    Stat(const std::string& name);
    Stat(const std::string& name, StatsGroup *group);
    virtual ~Stat() {}

    std::string name() const { return _name;}
    virtual std::string valueString() const = 0;
    void print() const {
        Printer::printStat(_name.c_str(), valueString().c_str());
    }
 private:
    std::string _name;
};



class StatsGroup {
 public:

    explicit StatsGroup(const std::string& name) : _name(name), _stats() {}
    ~StatsGroup() {}

    void registerStat(Stat *stat) { _stats.push_back(stat); }
    void reset();

    void print() const;
 private:
    std::string _name;
    std::vector<Stat*> _stats;

    DISALLOW_COPY_AND_ASSIGN(StatsGroup);
};


class LinearStat : public Stat {
 public:
    explicit LinearStat(const std::string& name) : Stat(name) {}
    LinearStat(const std::string& name, StatsGroup *group) : Stat(name, group) {}
    ~LinearStat() {}

    void add (double x, double y, bool b) {
        _values.push_back(Info(x, y, b));
    }

    virtual std::string valueString() const {
        std::string str = "";
        for (const Info& info : _values)
            str += std::to_string(info.x) + " " +
                std::to_string(info.y) + " " +
                std::to_string(info.b) + "\n";
        return str;
    }
 private:
    struct Info {
        Info(double xx, double yy, bool bb) :
            x(xx), y(yy), b(bb) {}
        double x;
        double y;
        bool b;
    };
    std::vector< Info > _values;

};


class TemporalStat : public Stat {
 public:
    TemporalStat(const std::string& name, int window_size);
    TemporalStat(const std::string& name, int window_size, StatsGroup *group);
    ~TemporalStat() {}

    int sum() const { return _sum; }
    int min() const { return _min; }
    int max() const { return _max; }
    uint64_t num() const { return _num; }
    double average() const { return _average; }

    void add(int value);
    virtual std::string valueString() const;

 protected:
    std::deque<int> _window;
    int _window_size;

    int _sum;
    int _min;
    int _max;
    uint64_t _num;
    double _average;

};

class CumulativeStat : public Stat {
 public:
    explicit CumulativeStat(const std::string& name) : Stat(name), _value(0) {}
    CumulativeStat(const std::string& name, StatsGroup *group) :
        Stat(name, group), _value(0) {}
    ~CumulativeStat() {}

    void add(int value) { _value += value; }
    virtual std::string valueString() const { return std::to_string(_value); }
 private:
    int _value;

};


class DistributionStat : public Stat {
 public:
    explicit DistributionStat(const std::string& name);
    DistributionStat(const std::string& name, StatsGroup *group);
    virtual ~DistributionStat() override {}

    double sum() const { return _sum; }
    double min() const { return _min; }
    double max() const { return _max; }
    uint64_t num() const { return _num; }

    double average() const { return _average; }
    double stdDeviation() const;

    virtual std::string valueString() const override = 0;

 protected:
    double _sum;
    double _min;
    double _max;
    uint64_t _num;

    double _average;
    double _sum_squares_from_average;

    void addToDistribution(double value);
};


class IntegerDistribution : public DistributionStat {
 public:
    explicit IntegerDistribution(const std::string& name) :
        DistributionStat(name) {}
    IntegerDistribution(const std::string& name, StatsGroup* group)
        : DistributionStat(name, group) {}
    ~IntegerDistribution() {}
    void add(uint64_t value) { addToDistribution(static_cast<double>(value)); }
    virtual std::string valueString() const override;

};

}  // namespace cosy

#endif  // INCLUDE_COSY_STATS_H_
