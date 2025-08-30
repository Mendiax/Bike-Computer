#include "persistent_storage.hpp"
#include "traces.h"
#include "file.hpp"
#include "line_reader.hpp"

PersistentStorage::PersistentStorage() {
    load_from_file();
}

PersistentStorage::~PersistentStorage() {
    save_to_file();
}

void PersistentStorage::load_from_file() {
    TRACE_DEBUG(0, TRACE_SD, "Loading persistent storage from %s\n", STORAGE_FILE);
    File file(STORAGE_FILE, File::Flags::READ);
    if (!file.is_open()) {
        TRACE_DEBUG(2, TRACE_CORE_1, "Could not open storage file %s\n", STORAGE_FILE);
        return;
    }

    for (const auto& line : LineReader(file)) {
        size_t delimiter_pos = line.find('=');
        if (delimiter_pos != std::string::npos) {
            std::string key = line.substr(0, delimiter_pos);
            std::string value = line.substr(delimiter_pos + 1);
            storage[key] = value;
        }
    }
}

void PersistentStorage::save_to_file() {
    TRACE_DEBUG(0, TRACE_SD, "Saving persistent storage from %s\n", STORAGE_FILE);
    File file(STORAGE_FILE, File::Flags::WRITE | File::Flags::TRUNCATE);
    if (!file.is_open()) {
        TRACE_DEBUG(2, TRACE_CORE_1, "Could not open storage file %s for writing\n", STORAGE_FILE);
        return;
    }

    for (const auto& pair : storage) {
        std::string line = pair.first + "=" + pair.second + "\n";
        file.write(line);
    }
}

void PersistentStorage::set_value(const std::string& key, const std::string& value) {
    storage[key] = value;
    save_to_file();
}

std::string PersistentStorage::get_string(const std::string& key, const std::string& default_value) {
    auto it = storage.find(key);
    return (it != storage.end()) ? it->second : default_value;
}

int PersistentStorage::get_int(const std::string& key, int default_value) {
    auto it = storage.find(key);
    if (it == storage.end()) return default_value;
    return convert_value<int>(it->second);
}

float PersistentStorage::get_float(const std::string& key, float default_value) {
    auto it = storage.find(key);
    if (it == storage.end()) return default_value;
    return convert_value<float>(it->second);
}

bool PersistentStorage::get_bool(const std::string& key, bool default_value) {
    auto it = storage.find(key);
    if (it == storage.end()) return default_value;
    return convert_value<bool>(it->second);
}

bool PersistentStorage::has_key(const std::string& key) const {
    return storage.find(key) != storage.end();
}
