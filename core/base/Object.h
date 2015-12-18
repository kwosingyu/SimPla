/**
 * @file object.h
 * @author salmon
 * @date 2015-12-16.
 */

#ifndef SIMPLA_OBJECT_H
#define SIMPLA_OBJECT_H

#include <typeinfo>
#include <mutex>
#include <stddef.h> //for size_t
#include <memory>

namespace simpla { namespace base
{


/** @ingroup task_flow
 *  @addtogroup sp_object SIMPla object
 *  @{
 *  ## Summary
 *   - Particle distribution function is a @ref physical_object;
 *   - Electric field is a @ref physical_object
 *   - Magnetic field is a @ref physical_object;
 *   - Plasma density field is a @ref physical_object;
 *   - @ref physical_object is a geometry defined on a domain in configuration space;
 *   - @ref physical_object has properties;
 *   - @ref physical_object can be saved or loaded as DataSet;
 *   - @ref physical_object may be decomposed and sync between mpi processes;
 *   - The element value of PhysicalObject may be accessed through a index of discrete grid point in the domain
 *
 *
 *  ## Member types
 *   Member type	 			| Semantics
 *   ---------------------------|--------------
 *   domain_type				| Domain
 *   iterator_type				| iterator of element value
 *   range_type					| range of element value
 *
 *
 *
 *  ## Member functions
 *
 *  ### Constructor
 *
 *   Pseudo-Signature 	 			| Semantics
 *   -------------------------------|--------------
 *   `PhysicalObject()`						| Default constructor
 *   `~PhysicalObject() `					| destructor.
 *   `PhysicalObject( const PhysicalObject& ) `	| copy constructor.
 *   `PhysicalObject( PhysicalObject && ) `			| move constructor.
 *   `PhysicalObject( Domain & D ) `			| Construct a PhysicalObject on domain \f$D\f$.
 *
 *  ### Swap
 *    `swap(PhysicalObject & )`					| swap
 *
 *  ###  Fuctions
 *   Pseudo-Signature 	 			| Semantics
 *   -------------------------------|--------------
 *   `bool is_valid() `  			| _true_ if PhysicalObject is valid for accessing
 *   `update()`					| allocate memory
 *   `DataModel()`					| return the data set of PhysicalObject
 *   `clear()`						| set value to zero, allocate memory if empty() is _true_
 *   `T properties(std::string name)const` | get properties[name]
 *   `properties(std::string name,T const & v) ` | set properties[name]
 *   `std::ostream& print(std::ostream & os) const` | print description to `os`
 *
 *  ### Element access
 *   Pseudo-Signature 				| Semantics
 *   -------------------------------|--------------
 *   `value_type & at(index_type s)`   			| access element on the grid points _s_ with bounds checking
 *   `value_type & operator[](index_type s) `  | access element on the grid points _s_as
 *
 *  @}
 **/


class SpObject
{
public:
    SpObject();

    SpObject(SpObject &&other);

    SpObject(SpObject const &);

    SpObject &operator=(SpObject const &other);

    virtual  ~SpObject();

    void swap(SpObject &other);

    virtual bool is_a(std::type_info const &info) const;

    virtual std::string get_class_name() const;

    virtual bool is_same(SpObject const &other) const;

    virtual std::ostream &print(std::ostream &os, int indent) const;

    /**
     *  @name concept lockable
     *  @{
     */
public:
    inline void lock() { m_mutex_.lock(); }

    inline void unlock() { m_mutex_.unlock(); }

    inline bool try_lock() { return m_mutex_.try_lock(); }

private:
    std::mutex m_mutex_;
    /** @} */
public:
    /**
     *  @name concept touchable
     *  @{
     */
    inline void touch()
    {
        m_mutex_.lock();
        ++m_click_;
        m_mutex_.unlock();
    }

    inline size_t get_click() const { return m_click_; }
    /** @} */
private:

    size_t m_click_ = 0;
};

#define SP_OBJECT_HEAD(_CLASS_NAME_, _BASE_CLASS_NAME_)                       \
virtual bool is_a(std::type_info const &info)const                            \
  { return typeid(_CLASS_NAME_) == info || _BASE_CLASS_NAME_::is_a(info); }   \
virtual std::string get_class_name() const { return __STRING(_CLASS_NAME_); }  \
private:                                                                      \
    typedef _BASE_CLASS_NAME_ base_type;                                      \
public:

//virtual std::shared_ptr<SpObject> clone_object()const { return std::dynamic_pointer_cast<SpObject>(this->clone()); }


}}//namespace simpla { namespace base


#endif //SIMPLA_OBJECT_H
