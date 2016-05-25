/** 
 * @file RangeAdapter.h
 * @author salmon
 * @date 16-5-25 - 上午11:45
 *  */

#ifndef SIMPLA_RANGEADAPTER_H
#define SIMPLA_RANGEADAPTER_H

#include <memory>
#include "Range.h"

namespace simpla
{

template<typename TIterator>
class RangeAdapter
{
    typedef RangeAdapter<TIterator> this_type;

    class HolderBase;

    template<typename> struct Holder;
public :
    typedef TIterator iterator;

    RangeAdapter() { }

    //****************************************************************************
    // TBB Range Concept Begin
    template<typename TOtherRange>
    RangeAdapter(TOtherRange const &other) :
            m_holder_(std::dynamic_pointer_cast<HolderBase>(std::make_shared<Holder<TOtherRange>>(other)))
    {
    }

    template<typename TOtherRange, typename ...Args>
    RangeAdapter(TOtherRange &other, Args &&...args) :
            m_holder_(std::dynamic_pointer_cast<HolderBase>(
                    std::make_shared<Holder<TOtherRange>>(other, std::forward<Args>(args)...)))
    {
    }

    RangeAdapter(this_type const &other) : m_holder_(other.m_holder_->clone()) { }

    RangeAdapter(this_type &&other) : m_holder_(other.m_holder_) { }

    ~RangeAdapter() { }

    static const bool is_splittable_in_proportion = true;

    bool is_divisible() const { return m_holder_->is_divisible(); }

    bool empty() const { return m_holder_ == nullptr || m_holder_->empty(); }

    void swap(this_type &other) { std::swap(m_holder_, other.m_holder_); }

    iterator begin() { return m_holder_->begin(); }

    iterator end() { return m_holder_->end(); }



    // TBB Range Concept End
    //****************************************************************************
private:

    std::shared_ptr<HolderBase> m_holder_;

    struct HolderBase
    {
        virtual std::shared_ptr<HolderBase> clone() const = 0;

        virtual bool is_a(std::type_info const &) const = 0;

        virtual bool is_divisible() const = 0;

        virtual bool empty() const = 0;

//        virtual void swap(this_type &other) = 0;

        virtual iterator begin() const = 0;

        virtual iterator end() const = 0;


    };

    template<typename Tit>
    struct Holder : public HolderBase
    {
        typedef Holder<Tit> this_type;
        Tit m_iter_;
    public:

        template<typename ...Args>
        Holder(Args &&... args) : m_iter_(std::forward<Args>(args)...) { }

        virtual  ~Holder() final { }

        virtual std::shared_ptr<HolderBase> clone() const
        {
            return std::dynamic_pointer_cast<HolderBase>(std::make_shared<this_type>(m_iter_));
        }


        virtual bool is_a(std::type_info const &t_info) const final { return typeid(Tit) == t_info; }

        virtual bool is_divisible() const { return m_iter_.is_divisible(); }

        virtual bool empty() const { return m_iter_.empty(); }

        virtual void swap(this_type &other) { other.m_iter_.swap(m_iter_); }

        virtual iterator begin() const { return iterator(m_iter_.begin()); }

        virtual iterator end() const { return iterator(m_iter_.end()); }


    };
};

template<typename Tp> using AdaptRange = RangeAdapter<typename Tp::iterator>;

}//namespace simpla//namespace detail
#endif //SIMPLA_RANGEADAPTER_H