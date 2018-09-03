#pragma once

#include <limits.h>
#include <cctype>
#include <regex>
#include <algorithm>
#include <string>
#include <ostream>
#include <iterator>

template <typename T>
class elem_traits
{
  public:
    typedef T value_type;
};

template <typename T, typename U>
bool is_numeric(const T &t)
{
    std::string tmp = t;
    const std::regex re{"^[1-9][0-9]*$"};
    if (std::regex_match(tmp.begin(), tmp.end(), re))
        return true;
    std::cout << "Параметр должен быть числом в интервале от 1 до " << std::numeric_limits<U>::max() << std::endl;
    return false;
}

template <typename T>
class TypeID
{
    static std::size_t counter;

  public:
    static T value()
    {
        counter++;
        if (counter > std::numeric_limits<T>::max())
        {
            counter = 1;
        }
        return counter;
    }
};

template <class T>
std::size_t TypeID<T>::counter = 0;

template <typename T>
struct Property
{
    T value;
    Property(const T initial_value)
    {
        *this = initial_value;
    }
    operator T()
    {
        // perform some getter action
        return value;
    }
    T operator=(T new_value)
    {
        // perform some setter action
        return value = new_value;
    }
};

// infix_iterator.h
//
// Lifted from Jerry Coffin's 's prefix_ostream_iterator
template <class T,
          class charT = char,
          class traits = std::char_traits<charT>>
class infix_ostream_iterator : public std::iterator<std::output_iterator_tag, void, void, void, void>
{
    std::basic_ostream<charT, traits> *os;
    charT const *delimiter;
    bool first_elem;

  public:
    typedef charT char_type;
    typedef traits traits_type;
    typedef std::basic_ostream<charT, traits> ostream_type;
    infix_ostream_iterator(ostream_type &s)
        : os(&s), delimiter(0), first_elem(true)
    {
    }
    infix_ostream_iterator(ostream_type &s, charT const *d)
        : os(&s), delimiter(d), first_elem(true)
    {
    }
    infix_ostream_iterator<T, charT, traits> &operator=(T const &item)
    {
        // Here's the only real change from ostream_iterator:
        // Normally, the '*os << item;' would come before the 'if'.
        if (!first_elem && delimiter != 0)
            *os << delimiter;
        *os << item;
        first_elem = false;
        return *this;
    }
    infix_ostream_iterator<T, charT, traits> &operator*()
    {
        return *this;
    }
    infix_ostream_iterator<T, charT, traits> &operator++()
    {
        return *this;
    }
    infix_ostream_iterator<T, charT, traits> &operator++(int)
    {
        return *this;
    }
};
