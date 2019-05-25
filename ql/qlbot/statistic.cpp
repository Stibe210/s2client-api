#include "qlbot/statistic.h"

void Statistic::copy(Statistic& other)
{
    node_counter = other.node_counter;
    sum = other.sum;
    pow_sum = other.pow_sum;
    count_ = other.count_;
    save_after = other.save_after;
    is_cont = other.is_cont;
}

void Statistic::copy(const Statistic& other)
{
    node_counter = other.node_counter;
    sum = other.sum;
    pow_sum = other.pow_sum;
    count_ = other.count_;
    save_after = other.save_after;
    is_cont = other.is_cont;
}

Statistic::Statistic(int save_after, bool is_cont, int batch_size)
{
    batch = new circular_buffer<double>(batch_size);
    this->save_after = save_after;
    this->is_cont = is_cont;
}

Statistic::~Statistic()
{
    delete batch;
    batch = nullptr;
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
    batch->reset();
    node_counter = 0;
    sum = 0;
    pow_sum = 0;
    count_ = 0;
}

double Statistic::mean()
{
    if (is_cont)
    {
        return batch->get_sum() / batch->size();
    }
    return sum / count_;
}

double Statistic::variance()
{
    if (is_cont)
    {
        return (batch->get_pow_sum() / batch->size()) - pow(batch->get_sum() / batch->size(), 2);
    }
    return (pow_sum / count_) - pow(sum / count_, 2);
}

double Statistic::std()
{
    return sqrt(variance());
}

void Statistic::add(double value)
{
    if (is_cont)
    {
        batch->put(value);
        return;
    }
    count_++;
    sum += value;
    pow_sum += pow(value, 2);
    node_counter++;
    if (node_counter == save_after)
    {
        node_counter = 0;
    }
}

int Statistic::count()
{
    if (is_cont)
    {
        return batch->size();
    }
    return count_;
}

string Statistic::to_csv_string()
{
    if (is_cont)
    {
        return to_string(mean()) + ";" + to_string(std()) + ";\n";
    }
    return to_string(count_) + ";" + to_string(mean()) + ";" + to_string(std()) + ";\n";
}

string Statistic::GetLastGamesResult() {
	return to_string(mean()) + ";";
}

