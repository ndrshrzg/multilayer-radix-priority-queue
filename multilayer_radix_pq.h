//
// Created by ndrshrzg on 10/18/17.
//

#include <stxxl/vector>
#include <limits>



#ifndef MULTILAYER_RADIX_PRIORITY_QUEUE_MULTILAYER_RADIX_PQ_H_H
#define MULTILAYER_RADIX_PRIORITY_QUEUE_MULTILAYER_RADIX_PQ_H_H


class Q
{
    using index_type = std::pair<int, int>;
private:
    std::priority_queue<int, std::vector<index_type>, std::greater<index_type>> pq_;
public:
    void push(index_type index)
    {
        pq_.push(index);
    }

    index_type pop()
    {
        index_type min = pq_.top();
        pq_.pop();
        return min;
    }

    bool empty() const
    {
        return pq_.empty();
    }

    index_type min()
    {
        return pq_.top();
    }

    unsigned long size()
    {
        return pq_.size();
    }

    void clear()
    {
        while(!pq_.empty())
        {
            pq_.pop();
        }
        std::cout << "q cleared" << std::endl;
    }

};


//TODO: Datastructure buckets, enclosing datastructure multilayer_radix_pq using Nqueue and buckets


template <typename KeyType, typename ValueType>
class bucket
{
public:
    using key_type = KeyType;
    using value_type = ValueType;
private:
    stxxl::vector<std::pair<key_type, value_type>> v_;


public:

    bool empty() const {
        return v_.empty();
    }

    void push(const key_type& key, const value_type& val)
    {
        v_.push_back(std::pair<key_type, value_type> (key, val));
    }

    void clear(){
        v_.clear();
    }

};



#endif //MULTILAYER_RADIX_PRIORITY_QUEUE_MULTILAYER_RADIX_PQ_H_H
