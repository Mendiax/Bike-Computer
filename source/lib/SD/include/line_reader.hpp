#include <string>
#include <vector>
#include <iterator>
#include <stdexcept>
#include <iostream>
#include "file.hpp"

class LineReader {
public:
    class iterator {
        File* file;
        std::string current_line;
        bool at_end;

        void read_next();

    public:
        iterator(File* f = nullptr, bool end = false);

        const std::string& operator*() const;
        const std::string* operator->() const;

        iterator& operator++();

        iterator operator++(int);

        bool operator==(const iterator& other) const;

        bool operator!=(const iterator& other) const;
    };

    explicit LineReader(File& f);

    iterator begin();
    iterator end();

private:
    File& file;
};

// helper function
inline LineReader read_lines(File& f) {
    return LineReader(f);
}
