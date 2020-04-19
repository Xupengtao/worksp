/*
 * stl_iterartor.h
 *
 *  Created on: Apr 8, 2019
 *      Author: xupengtao
 */

#ifndef STL_ITERARTOR_H_
#define STL_ITERARTOR_H_

struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};

/******************************** iterator_traits *******************************/
template <class Category ,class T,class Distance = ptrdiff_t, class Pointer = T*,class Reference = T&>
struct iterator
{
	typedef Category 	iterartor_category;
	typedef T 			value_type;
	typedef Distance	difference_type;
	typedef Pointer		pointer;
	typedef Reference	reference;
};

template <class Iterator>
struct iterator_traits
{
	typedef typename Iterator::iterartor_category	iterartor_category;
	typedef typename Iterator::value_type			value_type;
	typedef typename Iterator::difference_type		difference_type;
	typedef typename Iterator::pointer				pointer;
	typedef typename Iterator::reference			reference;
};

template <class T>
struct iterator_traits<T*>
{
	typedef random_access_iterator_tag				iterartor_category;
	typedef T										value_type;
	typedef ptrdiff_t								difference_type;
	typedef T*										pointer;
	typedef T&										reference;
};

template <class T>
struct iterator_traits<const T*>
{
	typedef random_access_iterator_tag				iterartor_category;
	typedef T										value_type;
	typedef ptrdiff_t								difference_type;
	typedef const T*								pointer;
	typedef const T&								reference;
};
/******************************* iterator_traits ********************************/

/***************************** iterator_traits_func *****************************/
template<class Iterator>
inline typename iterator_traits<Iterator>::iterartor_category
iterator_category(const Iterator &)
{
	typedef typename iterator_traits<Iterator>::iterartor_category category;
	return category();
}

template<class Iterator>
inline typename iterator_traits<Iterator>::difference_type*
distance_type(const Iterator &)
{
	return static_cast<typename iterator_traits<Iterator>::difference_type *>(0);
}

template<class Iterator>
inline typename iterator_traits<Iterator>::value_type*
value_type(const Iterator &)
{
	return static_cast<typename iterator_traits<Iterator>::value_type *>(0);
}
/***************************** iterator_traits_func *****************************/

/********************************* type_traits **********************************/
struct __true_type {};
struct __false_type {};

template<class type>
struct __type_traits
{
	typedef __true_type			this_dummy_member_must_be_first;
	typedef __false_type		has_trivial_default_constructor;
	typedef __false_type		has_trivial_copy_constructor;
	typedef __false_type		has_trivial_assignment_operator;
	typedef __false_type		has_trivial_destructor;
	typedef __false_type		is_POD_type;
};

template<>
struct __type_traits<char>
{
	typedef __true_type			has_trivial_default_constructor;
	typedef __true_type			has_trivial_copy_constructor;
	typedef __true_type			has_trivial_assignment_operator;
	typedef __true_type			has_trivial_destructor;
	typedef __true_type			is_POD_type;
};

template<>
struct __type_traits<signed char>
{
	typedef __true_type			has_trivial_default_constructor;
	typedef __true_type			has_trivial_copy_constructor;
	typedef __true_type			has_trivial_assignment_operator;
	typedef __true_type			has_trivial_destructor;
	typedef __true_type			is_POD_type;
};

template<>
struct __type_traits<unsigned char>
{
	typedef __true_type			has_trivial_default_constructor;
	typedef __true_type			has_trivial_copy_constructor;
	typedef __true_type			has_trivial_assignment_operator;
	typedef __true_type			has_trivial_destructor;
	typedef __true_type			is_POD_type;
};

template<>
struct __type_traits<short>
{
	typedef __true_type			has_trivial_default_constructor;
	typedef __true_type			has_trivial_copy_constructor;
	typedef __true_type			has_trivial_assignment_operator;
	typedef __true_type			has_trivial_destructor;
	typedef __true_type			is_POD_type;
};

template<>
struct __type_traits<unsigned short>
{
	typedef __true_type			has_trivial_default_constructor;
	typedef __true_type			has_trivial_copy_constructor;
	typedef __true_type			has_trivial_assignment_operator;
	typedef __true_type			has_trivial_destructor;
	typedef __true_type			is_POD_type;
};

template<>
struct __type_traits<int>
{
	typedef __true_type			has_trivial_default_constructor;
	typedef __true_type			has_trivial_copy_constructor;
	typedef __true_type			has_trivial_assignment_operator;
	typedef __true_type			has_trivial_destructor;
	typedef __true_type			is_POD_type;
};

template<>
struct __type_traits<unsigned int>
{
	typedef __true_type			has_trivial_default_constructor;
	typedef __true_type			has_trivial_copy_constructor;
	typedef __true_type			has_trivial_assignment_operator;
	typedef __true_type			has_trivial_destructor;
	typedef __true_type			is_POD_type;
};

template<>
struct __type_traits<long>
{
	typedef __true_type			has_trivial_default_constructor;
	typedef __true_type			has_trivial_copy_constructor;
	typedef __true_type			has_trivial_assignment_operator;
	typedef __true_type			has_trivial_destructor;
	typedef __true_type			is_POD_type;
};

template<>
struct __type_traits<unsigned long>
{
	typedef __true_type			has_trivial_default_constructor;
	typedef __true_type			has_trivial_copy_constructor;
	typedef __true_type			has_trivial_assignment_operator;
	typedef __true_type			has_trivial_destructor;
	typedef __true_type			is_POD_type;
};

template<>
struct __type_traits<float>
{
	typedef __true_type			has_trivial_default_constructor;
	typedef __true_type			has_trivial_copy_constructor;
	typedef __true_type			has_trivial_assignment_operator;
	typedef __true_type			has_trivial_destructor;
	typedef __true_type			is_POD_type;
};

template<>
struct __type_traits<double>
{
	typedef __true_type			has_trivial_default_constructor;
	typedef __true_type			has_trivial_copy_constructor;
	typedef __true_type			has_trivial_assignment_operator;
	typedef __true_type			has_trivial_destructor;
	typedef __true_type			is_POD_type;
};

template<>
struct __type_traits<long double>
{
	typedef __true_type			has_trivial_default_constructor;
	typedef __true_type			has_trivial_copy_constructor;
	typedef __true_type			has_trivial_assignment_operator;
	typedef __true_type			has_trivial_destructor;
	typedef __true_type			is_POD_type;
};

template<class T>
struct __type_traits<T*>
{
	typedef __true_type			has_trivial_default_constructor;
	typedef __true_type			has_trivial_copy_constructor;
	typedef __true_type			has_trivial_assignment_operator;
	typedef __true_type			has_trivial_destructor;
	typedef __true_type			is_POD_type;
};
/********************************* type_traits **********************************/

/********************************* type_traits **********************************/
#endif /* STL_ITERARTOR_H_ */
