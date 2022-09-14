#include "sd_file.h"

#include "sd_common.h"
#include "ff.h"
#include "traces.h"

Sd_File::Sd_File(const char* file_name)
{
    mount_drive();
    if(!sd_mounted)
    {
        TRACE_ABNORMAL(TRACE_SD, "Could not mount the drive\n");
    }
    FIL file_p;
    FRESULT res;
FILE_OPEN:
    res = f_open(&file_p, file_name,  FA_READ | FA_WRITE | FA_OPEN_APPEND);
    if (res != FR_OK)
    {
        // failed
        //try to mount drive if it is not mounted
        mount_drive();
        TRACE_ABNORMAL(TRACE_SD, "Could not open the file\n");

        sleep_ms(10000);
        goto FILE_OPEN;
    }
    f_close(&file_p);
    this->file_name = file_name;
}

Sd_File::~Sd_File()
{
    PRINTF("Unmount\n");
    unmount_drive();
}


void Sd_File::append(const char* string)
{
    // TODO return error
    FIL file_p;
    FRESULT res;
FILE_OPEN:
    res = f_open(&file_p, file_name,  FA_WRITE | FA_OPEN_APPEND);
    if (res != FR_OK)
    {
        // failed

        //try to mount drive if it is not mounted
        mount_drive();

        sleep_ms(10000);
        goto FILE_OPEN;
    }
    auto ret = f_puts(string, &file_p);
    if (ret < 0)
    {
        TRACE_ABNORMAL(TRACE_SD, "Could not write to file %s (%d)\n", string,  ret);
    }
    f_close(&file_p);
}

bool Sd_File::is_empty()
{
    FILINFO info;
    auto fres = f_stat(file_name, &info);
    if(fres == FR_NO_FILE)
    {
        TRACE_ABNORMAL(TRACE_SD, "no file \n");
        return true;
    }
    if(fres == FR_OK)
    {
        TRACE_ABNORMAL(TRACE_SD, "file size == %" PRIu64 " \n", info.fsize);
        bool file_has_bytes = info.fsize;
        return !file_has_bytes;
    }
    TRACE_ABNORMAL(TRACE_SD, "fres %d \n", fres);

    return false;

}

bool Sd_File::clear()
{
    FIL file_p;
    FRESULT res;
FILE_OPEN:
    res = f_open(&file_p, file_name,  FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
    if (res != FR_OK)
    {
        // failed

        //try to mount drive if it is not mounted
        mount_drive();
        TRACE_ABNORMAL(TRACE_SD, "Could not open a file (%d)\n",  res);


        sleep_ms(10000);
        goto FILE_OPEN;
    }

    auto ret = f_truncate(&file_p);
    if (ret < 0)
    {
        TRACE_ABNORMAL(TRACE_SD, "Could not truncate (%d)\n",  ret);
    }
    f_close(&file_p);

    return true;
}


