#include <cmath>
#include <sstream>

#include "cosy/Stats.h"

namespace cosy {

Stat::Stat(const std::string& name) : _name(name) {
}

Stat::Stat(const std::string& name, StatsGroup *group) : _name(name) {
    group->registerStat(this);
}

void StatsGroup::print() const {
    Printer::printSection(_name.c_str());
    for (const Stat* stat : _stats)
        stat->print();
}

DistributionStat::DistributionStat(const std::string& name) :
    Stat(name),
    _sum(0.0),
    _min(0.0),
    _max(0.0),
    _num(0),
    _average(0.0),
    _sum_squares_from_average(0.0) {}

DistributionStat::DistributionStat(const std::string& name, StatsGroup *group) :
    Stat(name, group),
    _sum(0.0),
    _min(0.0),
    _max(0.0),
    _num(0),
    _average(0.0),
    _sum_squares_from_average(0.0) {}



void DistributionStat::addToDistribution(double value) {
    if (_num == 0) {
        _sum = value;
        _min = value;
        _max = value;
        _average = value;
        _num = 1;
        return;
    }

    _sum += value;
    _min = std::min(_min, value);
    _max = std::max(_max, value);
    _num++;

    const double delta = value - _average;
    _average = _sum / _num;
    _sum_squares_from_average += delta * (value - _average);
}


double DistributionStat::stdDeviation() const {
    if (_num == 0) return 0.0;
    return sqrt(_sum_squares_from_average / _num);
}


std::string IntegerDistribution::valueString() const {
    std::stringstream ss;
    ss << static_cast<int>(_num) << " [" << _min << ", " << _max << "] " <<
        average() << " " << stdDeviation();
    return ss.str();
}



TemporalStat::TemporalStat(const std::string& name, int window_size) :
    Stat(name),
    _window_size(window_size),
    _sum(0.0),
    _min(0.0),
    _max(0.0),
    _num(0),
    _average(0.0) {}

TemporalStat::TemporalStat(const std::string& name, int window_size,
                           StatsGroup *group) :
    Stat(name, group),
    _window_size(window_size),
    _sum(0.0),
    _min(0.0),
    _max(0.0),
    _num(0),
    _average(0.0) {}

void TemporalStat::add(int value) {
    if (_num == 0) {
        _sum = value;
        _min = value;
        _max = value;
        _average = value;
        _num = 1;
        return;
    }
    _sum += value;
    _min = std::min(_min, value);
    _max = std::max(_max, value);
    _num++;

    _window.push_back(value);
    if (_window.size() >_window_size) {
        _sum -= _window.front();
        _window.pop_front();
    }
    _average = _sum / _num;
}

std::string TemporalStat::valueString() const {
    std::stringstream ss;
    ss << _window.back() << "[" << _min << "," << _max << "] " <<
        average();
    return ss.str();
}


}  // namespace cosy
