#ifndef SD_COMMON_H
#define SD_COMMON_H

#include "ff.h"
#include <string>
#include <vector>

#define SD_DRIVE_NAME "0:"

#define SD_FILE_NAME_SESSION "sessions.csv"

// global variable for checking if drive is mounted
extern bool sd_mounted;
extern FATFS fs;

/**
 * @brief mount drive if not mounted
 *
 */
void mount_drive();

/**
 * @brief unmount drive if not mounted
 *
 */
void unmount_drive();


namespace dir{
    /**
     * @brief delets directory with files
     *
     * @param path
     */
    void del(const char* path);

    /**
     * @brief Get all file names in folder
     *
     * @param path
     * @return std::vector<std::string>
     */
    std::vector<std::string> get_files(const char* path);
}


#endif