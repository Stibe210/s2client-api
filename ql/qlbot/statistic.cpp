#include "qlbot/statistic.h"

void Statistic::copy(Statistic& other)
{
    node_counter = other.node_counter;
    sum = other.sum;
    pow_sum = other.pow_sum;
    count_ = other.count_;
    values = other.values;
    save_after = other.save_after;
}

void Statistic::copy(const Statistic& other)
{
    node_counter = other.node_counter;
    sum = other.sum;
    pow_sum = other.pow_sum;
    count_ = other.count_;
    values = other.values;
    save_after = other.save_after;
}

Statistic::Statistic(int save_after)
{
    this->save_after = save_after;
}

Statistic::~Statistic()
{
}

Statistic::Statistic(Statistic& other)
{
    copy(other);
}

Statistic::Statistic(const Statistic& other)
{
    copy(other);
}

Statistic& Statistic::operator=(Statistic& other)
{
    copy(other);
    return *this;
}

Statistic& Statistic::operator=(const Statistic& other)
{
    copy(other);
    return *this;
}

void Statistic::reset()
{
    node_counter = 0;
    sum = 0;
    pow_sum = 0;
    count_ = 0;
    values.clear();
}

double Statistic::mean()
{
    return sum / count_;
}

double Statistic::variance()
{
    return (pow_sum / count_) - pow(sum / count_, 2);
}

double Statistic::std()
{
    return sqrt(variance());
}

void Statistic::add(double value)
{
    count_++;
    sum += value;
    pow_sum += pow(value, 2);
    node_counter++;
    if (node_counter == save_after)
    {
        values.push_back(new Statistic(*this));
        node_counter = 0;
    }
}

int Statistic::count()
{
    return count_;
}

string Statistic::to_csv_string()
{
    return to_string(count_) + ";" + to_string(mean()) + ";" + to_string(std()) + ";\n";
}

