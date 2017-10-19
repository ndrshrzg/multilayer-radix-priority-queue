//
// Created by ndrshrzg on 10/18/17.
//

#include <stxxl/vector>
#include <limits>



#ifndef MULTILAYER_RADIX_PRIORITY_QUEUE_MULTILAYER_RADIX_PQ_H_H
#define MULTILAYER_RADIX_PRIORITY_QUEUE_MULTILAYER_RADIX_PQ_H_H

template<typename KeyType, typename ValueType>
class N_queue
{
public:
    using key_type = KeyType;
    using value_type = ValueType;
    using TPair = std::pair<key_type, value_type>;
    using VTPairs = std::vector<TPair>;

private:
    int size_;
    key_type min_ = std::numeric_limits<key_type>::max();
    //TODO: Clarify whether std::greater<std::pair<K, V>> sorts by first value or by combination? Of latter, write custom Comparator.
    std::priority_queue<key_type, VTPairs, std::greater<TPair>> q;

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
    //TODO: find out if pass by reference is actually more efficient
    void push(const key_type& key, const value_type& val)
    {
        assert(key <= min_);
        ++size_;
        q.push(std::move(std::pair<key_type, value_type>(key, val)));

    };

    void pop()
    {
        min_ = q.top().first;
        q.pop();
        --size_;
    }

    key_type min() const
    {
        return q.top().first;
    }

    value_type minVale() const {return q.top().second;}

};


//TODO: Datastructure buckets, enclosing datastructure multilayer_radix_pq using Nqueue and buckets



#endif //MULTILAYER_RADIX_PRIORITY_QUEUE_MULTILAYER_RADIX_PQ_H_H
