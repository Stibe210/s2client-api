#pragma once
#include <memory>
using namespace std;

template <class T>
class circular_buffer {
public:
    circular_buffer(size_t size) :
        buf_(std::unique_ptr<T[]>(new T[size])),
        max_size_(size)
    {  }

    void put(T item);
    T get();
    void reset();
    bool empty() const;
    bool full() const;
    size_t capacity() const;
    size_t size() const;
    double get_sum();
    double get_pow_sum();

private:
    std::unique_ptr<T[]> buf_;
    size_t head_ = 0;
    size_t tail_ = 0;
    const size_t max_size_;
    bool full_ = 0;
};

template <class T>
void circular_buffer<T>::reset()
{
    head_ = tail_;
    full_ = false;
}

template <class T>
bool circular_buffer<T>::empty() const
{
    //if head and tail are equal, we are empty
    return (!full_ && (head_ == tail_));
}

template <class T>
bool circular_buffer<T>::full() const
{
    //If tail is ahead the head by 1, we are full
    return full_;
}

template <class T>
size_t circular_buffer<T>::capacity() const
{
    return max_size_;
}

template <class T>
size_t circular_buffer<T>::size() const
{
    size_t size = max_size_;

    if (!full_)
    {
        if (head_ >= tail_)
        {
            size = head_ - tail_;
        }
        else
        {
            size = max_size_ + head_ - tail_;
        }
    }

    return size;
}

template <class T>
double circular_buffer<T>::get_sum()
{
    double sum = 0;
    auto current = tail_;
    auto end = full_ ? (head_ + max_size_ - 1) % max_size_ : head_;
    while (true)
    {
        sum += buf_[current];
        if (current == end)
        {
            break;
        }
        current = (current + 1) % max_size_;
        if (current == end && !full_)
        {
            break;
        }
    }
    return sum;
}

template <class T>
double circular_buffer<T>::get_pow_sum()
{
    double pow_sum = 0;
    auto current = tail_;
    auto end = full_ ? (head_ + max_size_ - 1) % max_size_ : head_;
    while (true)
    {
        pow_sum += pow(buf_[current], 2);
        if (current == end)
        {
            break;
        }
        current = (current + 1) % max_size_;
        if (current == end && !full_)
        {
            break;
        }
    }
    return pow_sum;
}

template <class T>
void circular_buffer<T>::put(T item)
{
    buf_[head_] = item;

    if (full_)
    {
        tail_ = (tail_ + 1) % max_size_;
    }

    head_ = (head_ + 1) % max_size_;

    full_ = head_ == tail_;
}

template <class T>
T circular_buffer<T>::get()
{
    if (empty())
    {
        return T();
    }
    //Read data and advance the tail (we now have a free space)
    auto val = buf_[tail_];
    full_ = false;
    tail_ = (tail_ + 1) % max_size_;

    return val;
}