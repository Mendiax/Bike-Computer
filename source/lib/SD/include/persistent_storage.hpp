#pragma once

#include <string>
#include <map>
#include <sstream>
#include "sd_file.h"

class PersistentStorage {
private:
    static constexpr const char* STORAGE_FILE = SD_PATH "_global.cfg";
    std::map<std::string, std::string> storage;

    void load_from_file();
    void save_to_file();

    template<typename T>
    T convert_value(const std::string& value) {
        T result;
        std::stringstream ss(value);
        ss >> result;
        return result;
    }

public:
    PersistentStorage();
    ~PersistentStorage();

    void set_value(const std::string& key, const std::string& value);

    template<typename T>
    void set_value(const std::string& key, const T& value) {
        std::stringstream ss;
        ss << value;
        set_value(key, ss.str());
    }

    std::string get_string(const std::string& key, const std::string& default_value = "");
    int get_int(const std::string& key, int default_value = 0);
    float get_float(const std::string& key, float default_value = 0.0f);
    bool get_bool(const std::string& key, bool default_value = false);

    bool has_key(const std::string& key) const;
};
