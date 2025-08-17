#include "sd_common.h"
#include "traces.h"
#include "sd_card.h"
#include <vector>


FATFS fs;
bool sd_mounted = 0;
bool sd_driver_inited = 0;

void mount_drive()
{
    if (!sd_driver_inited)
    {
        TRACE_DEBUG(2, TRACE_SD, "Inicializing sd driver\n");
        sd_driver_inited = sd_init_driver();
        TRACE_DEBUG(2, TRACE_SD, "Sd driver inicialized\n");
        if (!sd_driver_inited)
        {
            TRACE_ABNORMAL(TRACE_SD, "cannot init sd driver\n");
        }
    }

    if(sd_mounted == 1)
    {
        return;
    }
    TRACE_DEBUG(2, TRACE_SD, "Mounting drive " SD_DRIVE_NAME "\n");
    // Mount drive
    const auto fr = f_mount(&fs, SD_DRIVE_NAME, 1);
    sd_mounted = (fr == FR_OK);
    TRACE_DEBUG(2, TRACE_SD, "Drive mounted " SD_DRIVE_NAME "\n");

    if(!sd_mounted)
    {
        TRACE_ABNORMAL(TRACE_SD, "cannot mount drive " SD_DRIVE_NAME " Error %d\n", fr);
    }
}

void unmount_drive()
{
    if(sd_mounted == 0)
    {
        return;
    }
    // Mount drive
    f_unmount(SD_DRIVE_NAME);
    sd_mounted = 0;
}

void dir::del(const char* path)
{
    std::string full_path = path;
    #ifdef BUILD_FOR_PICO
    // building for pico we can allocate on stack
    // for host we are creating with f_opendir
    full_path = SD_DRIVE_NAME;
    full_path += path;
    #endif

    auto files_in_dir = get_files(path);
    for(auto& f : files_in_dir)
    {
        std::string file_path = full_path + std::string("/") + f;
        f_unlink(file_path.c_str());
    }
    f_unlink(full_path.c_str());
}


std::vector<std::string> dir::get_files(const char* path)
{
// #ifdef BUILD_FOR_HOST
//     std::vector<std::string> founded_files;
//     DIR* dir = opendir(path);
//     if (dir == NULL) {
//         TRACE_ABNORMAL(TRACE_HOST, "Failed to open directory %s.\n", path);
//         return founded_files;
//     }

//     struct dirent* entry;
//     while ((entry = readdir(dir)) != NULL) {
//         founded_files.emplace_back(entry->d_name);
//     }
//     closedir(dir);
//     return founded_files;
// #else
    static FILINFO fno;
    FRESULT res;
    mount_drive();
    std::vector<std::string> founded_files;

    std::string full_path = path;
    #ifdef BUILD_FOR_PICO
    // building for pico we can allocate on stack
    // for host we are creating with f_opendir
    DIR dir;
    full_path = SD_DRIVE_NAME;
    full_path += path;
    #else
    DIR* dir;
    #endif


    res = f_opendir(&dir, full_path.c_str());                       /* Open the directory */
    if (res == FR_OK) {
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
            if (fno.fattrib & AM_DIR) {                    /* It is a directory */
            } else {                                       /* It is a file. */
                founded_files.emplace_back(fno.fname);
            }
        }
        f_closedir(&dir);
    }
    else{
        TRACE_ABNORMAL(TRACE_HOST, "could not open file %s", path);
    }

    return founded_files;
// #endif
}