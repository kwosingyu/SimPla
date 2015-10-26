/**
 * @file block_iterator.h
 * @author salmon
 * @date 2015-10-26.
 */

#ifndef SIMPLA_BLOCK_ITERATOR_H
#define SIMPLA_BLOCK_ITERATOR_H

#include <cstdlib>
#include <cmath>
#include <iterator>
#include "../ntuple.h"
#include "../type_traits_ext.h"

namespace simpla
{


template<typename TV, int NDIMS>
struct block_iterator : public std::iterator<
        typename std::random_access_iterator_tag,
        nTuple<TV, NDIMS>, ptrdiff_t>
{
private:

    typedef block_iterator<TV, NDIMS> this_type;

    typedef nTuple<TV, NDIMS> value_type;

    typedef ptrdiff_t difference_type;

    static constexpr int ndims = NDIMS;

public:
    block_iterator(value_type const &self, value_type const &min, value_type const &max) :
            m_min_(min), m_max_(max), m_self_(self)
    {
    }

    block_iterator(value_type const &min, value_type const &max) :
            m_min_(min), m_max_(max), m_self_(min)
    {
    }

    block_iterator(this_type const &other) :
            m_min_(other.m_min_), m_max_(other.m_max_), m_self_(other.m_self_)
    {
    }

    block_iterator(this_type &&other) :
            m_min_(other.m_min_), m_max_(other.m_max_), m_self_(other.m_self_)
    {
    }

    ~block_iterator() { }

    this_type &operator=(this_type const &other)
    {
        this_type(other).swap(*this);
        return *this;
    }

    void swap(this_type &other)
    {
        std::swap(m_max_, other.m_max_);
        std::swap(m_min_, other.m_min_);
        std::swap(m_self_, other.m_self_);
    }

    bool operator==(this_type const &other) const { return m_self_ == other.m_self_; }

    bool operator!=(this_type const &other) const { return m_self_ != other.m_self_; }

    value_type const &operator*() const { return m_self_; }

    this_type &operator++()
    {
        advance(1);
        return *this;
    }

    this_type &operator--()
    {
        advance(-1);
        return *this;
    }

    this_type operator++(int) const
    {
        this_type res(*this);
        ++(*this);
        return std::move(res);
    }

    this_type operator--(int) const
    {
        this_type res(*this);
        --(*this);
        return std::move(res);
    }

    this_type &operator+=(difference_type const &n)
    {
        advance(n);
        return *this;
    }

    this_type &operator-=(difference_type const &n)
    {
        advance(-n);
        return *this;
    }


    this_type operator+(difference_type const &n) const
    {

        this_type res(*this);
        res += n;
        return std::move(res);
    }

    this_type operator-(difference_type const &n) const
    {

        this_type res(*this);
        res -= n;
        return std::move(res);
    }


    value_type operator[](difference_type const &n) const
    {
        this_type res(*this);
        res += n;
        return *res;
    }

    difference_type operator-(this_type const &other) const
    {
        return distance() - other.distance();
    }


    bool operator<(this_type const &other) const { return (*this - other) < 0; }

    bool operator>(this_type const &other) const { return (*this - other) > 0; }

    bool operator<=(this_type const &other) const { return (*this - other) <= 0; }

    bool operator>=(this_type const &other) const { return (*this - other) >= 0; }


private:

    value_type m_min_, m_max_, m_self_;


    difference_type advance(difference_type n)
    {
        for (int i = ndims - 1; i >= 0; --i)
        {
            auto d = div(m_self_[i] - m_min_[i] + n + m_max_[i] - m_min_[i], m_max_[i] - m_min_[i]);

            m_self_[i] = d.rem + m_min_[i];

            n = d.quot - 1;//- std::copysign(1, n);

        }
        return n;
    }

    difference_type distance() const
    {

        difference_type res = 0;


        for (int i = 0; i < ndims - 1; ++i)
        {
            res += m_self_[i] - m_min_[i];

            res *= m_max_[i + 1] - m_min_[i + 1];
        }

        res += m_self_[ndims - 1] - m_min_[ndims - 1];

        return res;
    }

};

}//namespace simpla

#endif //SIMPLA_BLOCK_ITERATOR_H
