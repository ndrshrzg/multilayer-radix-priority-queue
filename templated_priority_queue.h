//
// Created by ndrshrzg on 10/26/17.
//

#ifndef MULTILAYER_RADIX_PRIORITY_QUEUE_TEMPLATED_PRIORITY_QUEUE_H
#define MULTILAYER_RADIX_PRIORITY_QUEUE_TEMPLATED_PRIORITY_QUEUE_H


/// DEPRECATED internal structure uses Bitvector instead of Priority Queue


#include <stxxl/vector>

template<typename KeyType, typename ValueType>
class templated_priority_queue {
public:
    using key_type = KeyType;
    using value_type = ValueType;
    using TPair = std::pair<key_type, value_type>;
    using VTPairs = std::vector<TPair>;

private:
    int size_;
    key_type min_ = std::numeric_limits<key_type>::max();
    //TODO: Clarify whether std::greater<std::pair<K, V>> sorts by first value or by combination? If latter, write custom Comparator.
    std::priority_queue<key_type, VTPairs, std::greater<TPair>> q;

public:

    templated_priority_queue() : size_(0){};

    int size() const
    {
        return size_;
    }

    bool empty() const
    {
        return (size_ <= 0);
    }
    //remove move semantics since stxxl doenst support it
    void push(const key_type& key, const value_type& val)
    {
        assert(key <= min_);
        ++size_;
        q.push(std::pair<key_type, value_type>(key, val));

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


#endif //MULTILAYER_RADIX_PRIORITY_QUEUE_TEMPLATED_PRIORITY_QUEUE_H
