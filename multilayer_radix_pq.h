//
// Created by ndrshrzg on 10/18/17.
//

#include <stxxl/vector>



#ifndef MULTILAYER_RADIX_PRIORITY_QUEUE_MULTILAYER_RADIX_PQ_H_H
#define MULTILAYER_RADIX_PRIORITY_QUEUE_MULTILAYER_RADIX_PQ_H_H



template<typename KeyType, typename ValueType>
struct CompareInputToPriorityqueue
{
public:
    using key_type = KeyType;
    using value_type = ValueType;
    bool operator()(std::pair<int, std::string> l, std::pair<int, std::string> r)
    {
        return l.first > r.first;
    }
};

template<typename KeyType, typename ValueType>
class N_queue
{
public:
    using key_type = KeyType;
    using value_type = ValueType;

private:
    int size_;
    //TODO fix instantiation of internal pq, use templated comparator
    //std::priority_queue<key_type, std::pair<key_type, value_type>, CompareInputToPriorityqueue<key_type, value_type>> q;
    //std::priority_queue<key_type, std::pair<key_type, value_type>> q;
    std::priority_queue<key_type> q;

public:

    N_queue() : size_(0){};

    int size() const
    {
        return size_;
    }

    bool empty() const
    {
        return (size_ <= 0);
    }

    void push(key_type key, value_type val)
    {
        //TODO implement last_ minimum functionality to ensure monotonicity
        ++size_;
        q.push(key);
    };

    void pop()
    {
        q.pop();
        --size_;
    }

    key_type min() const
    {
        return q.top();
    }

};






#endif //MULTILAYER_RADIX_PRIORITY_QUEUE_MULTILAYER_RADIX_PQ_H_H
