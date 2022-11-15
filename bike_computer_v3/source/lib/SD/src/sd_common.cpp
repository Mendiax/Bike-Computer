#include "sd_common.h"
#include "traces.h"
#include "sd_card.h"


FATFS fs;
bool sd_mounted = 0;
bool sd_driver_inited = 0;

void mount_drive()
{
    if (!sd_driver_inited)
    {
        sd_driver_inited = sd_init_driver();
        if (!sd_driver_inited)
        {
            TRACE_ABNORMAL(TRACE_SD, "cannot init sd driver\n");
        }
    }

    if(sd_mounted == 1)
    {
        return;
    }
    // Mount drive
    auto fr = f_mount(&fs, SD_DRIVE_NAME, 1);
    sd_mounted = (fr == FR_OK);
    if(!sd_mounted)
    {
        TRACE_ABNORMAL(TRACE_SD, "cannot mount drive " SD_DRIVE_NAME " \n");
    }
}

void unmount_drive()
{
    if(sd_mounted == 0)
    {
        return;
    }
    // Mount drive
    auto fr = f_unmount(SD_DRIVE_NAME);
    sd_mounted = 0;
}

void dir::del(const char* path)
{
    std::string full_path = SD_DRIVE_NAME;
    full_path += path;

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
    FRESULT res;
    DIR dir;
    UINT i;
    static FILINFO fno;

    mount_drive();
    std::string full_path = SD_DRIVE_NAME;
    full_path += path;

    std::vector<std::string> founded_files;

    res = f_opendir(&dir, full_path.c_str());                       /* Open the directory */
    if (res == FR_OK) {
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
            if (fno.fattrib & AM_DIR) {                    /* It is a directory */
            } else {                                       /* It is a file. */
                // printf("%s/%s\n", fno.fname); TODO TRACE
                founded_files.emplace_back(fno.fname);
            }
        }
        f_closedir(&dir);
    }

    return founded_files;
}