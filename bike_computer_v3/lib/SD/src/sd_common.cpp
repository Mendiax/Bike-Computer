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