#ifndef __TOOLS_RINGARRAY_HPP__
#define __TOOLS_RINGARRAY_HPP__
// #-------------------------------#
// |           includes            |
// #-------------------------------#
// c/c++ includes
#include <iterator>
#include <vector>

// my includes

// #-------------------------------#
// |            macros             |
// #-------------------------------#

// #-------------------------------#
// | global types declarations     |
// #-------------------------------#

template <typename T>
struct RingArray {
private:
    std::vector<T> array;
    size_t first_elem_pos = 0;

public:
    // RingArray(size_t length) : array(length), first_elem_pos(0) {}
    void set_length(size_t len) {
        array.resize(len);
    }
    /**
     * @brief overwrite first element in array and move first_elem_pos to next pos.
     *
     * @param element
     */
    void add_element(const T& element) {
        array[first_elem_pos] = element;
        first_elem_pos = (first_elem_pos + 1) % array.size();
    }

    const std::vector<T>& get_data() {
        return array;
    }

    /**
     * @brief returns iterator to data, where first element is at first_elem_pos,
     * and last is at first_elem_pos - 1.
     *
     */
    class Iterator {
    public:
        using iterator_category = std::output_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        Iterator(RingArray<T>& ring_array, size_t pos)
            : ring_array(ring_array), pos(pos) {}

        reference operator*() const {
            return ring_array.array[pos];
        }

        Iterator& operator++() {
            pos = (pos + 1) % ring_array.array.size();
            return *this;
        }

        bool operator!=(const Iterator& other) const {
            return pos != other.pos || &ring_array != &other.ring_array;
        }

    private:
        RingArray<T>& ring_array;
        size_t pos;
    };

    Iterator begin() {
        return Iterator(*this, first_elem_pos);
    }

    Iterator end() {
        return Iterator(*this, (first_elem_pos - 1 + array.size()) % array.size());
    }
};
// #-------------------------------#
// | global variables declarations |
// #-------------------------------#

// #-------------------------------#
// | global function declarations  |
// #-------------------------------#

// #-------------------------------#
// |  global function definitions  |
// #-------------------------------#

#endif
