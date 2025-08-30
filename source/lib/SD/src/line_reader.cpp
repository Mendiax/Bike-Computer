#include "line_reader.hpp"


void LineReader::iterator::read_next() {
    if (!file || file->eof()) {
        at_end = true;
        file = nullptr;
        return;
    }

    current_line.clear();
    char c;
    while (!file->eof()) {
        if (file->read(&c, 1) != 1)
            break;

        if (c == '\n')
            break;
        if (c != '\r')
            current_line.push_back(c);
    }
}

LineReader::iterator::iterator(File* f, bool end) : file(f), at_end(end) {
    if (file && !end) {
        read_next();
        if (current_line.empty() && (!file || file->eof())) {
            at_end = true;
            file = nullptr;
        }
    }
}

const std::string& LineReader::iterator::operator*() const { return current_line; }
const std::string* LineReader::iterator::operator->() const { return &current_line; }

LineReader::iterator& LineReader::iterator::operator++() {
    read_next();
    return *this;
}

LineReader::iterator LineReader::iterator::operator++(int) {
    iterator tmp = *this;
    ++(*this);
    return tmp;
}

bool LineReader::iterator::operator==(const LineReader::iterator& other) const {
    return (at_end && other.at_end) || (file == other.file && at_end == other.at_end);
}

bool LineReader::iterator::operator!=(const LineReader::iterator& other) const {
    return !(*this == other);
}


LineReader::LineReader(File& f) : file(f) {}

LineReader::iterator LineReader::begin() { return LineReader::iterator(&file, false); }
LineReader::iterator LineReader::end() { return LineReader::iterator(nullptr, true); }
