#include "sd_file.h"

#include "string.h"

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

void Sd_File::remove()
{
    f_unlink(file_name);
}



Sd_File::Result Sd_File::append(const char* string)
{
    // TODO return error
    FIL file_p;
    FRESULT res;
FILE_OPEN:
    res = f_open(&file_p, file_name,  FA_WRITE | FA_OPEN_APPEND);
    if (res != FR_OK)
    {
        //try to mount drive if it is not mounted
        mount_drive();
        return (last_result = F_ERROR);
    }
    auto ret = f_puts(string, &file_p);
    f_close(&file_p);
    if (ret < 0)
    {
        TRACE_ABNORMAL(TRACE_SD, "Could not write to file %s (%d)\n", string,  ret);
        return (last_result = F_ERROR);
    }
    return (last_result = F_OK);
}

bool Sd_File::is_empty()
{
    return get_size() == 0;
}

size_t Sd_File::get_size()
{
    FILINFO info;
    auto fres = f_stat(file_name, &info);
    if(fres == FR_NO_FILE)
    {
        TRACE_ABNORMAL(TRACE_SD, "no file \n");
        last_result = F_ERROR;
        return 0;
    }
    if(fres == FR_OK)
    {
        TRACE_DEBUG(1, TRACE_SD, "file size == %" PRIu64 " \n", info.fsize);
        last_result = F_OK;
        return info.fsize;
    }
    last_result = F_ERROR;
    return 0;
}


Sd_File::Result Sd_File::clear()
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
        return (last_result = F_ERROR);
    }

    auto ret = f_truncate(&file_p);
    f_close(&file_p);
    if (ret < 0)
    {
        TRACE_ABNORMAL(TRACE_SD, "Could not truncate (%d)\n",  ret);
        return (last_result = F_ERROR);
    }

    return (last_result = F_OK);
}

std::string Sd_File::read_all()
{
    // TODO return error
    // TODO tests
    FIL file_p;
    FRESULT res;
    res = f_open(&file_p, file_name,  FA_READ);
    if (res != FR_OK)
    {
        return "";
    }
    unsigned bytes_read;
    const size_t file_size = get_size();
    if(file_size == 0)
    {
        f_close(&file_p);
        last_result = F_ERROR;
        return "";
    }
    auto buffer = new char[file_size + 1];
    auto ret = f_read(&file_p, buffer, file_size, &bytes_read);
    f_close(&file_p);
    if (file_size != bytes_read)
    {
        TRACE_ABNORMAL(TRACE_SD, "Error while reading the file %s read %u/%zu\n",
                       file_name, bytes_read, file_size);
        last_result = F_ERROR;
        memset(buffer, 0, sizeof(buffer));
    }
    last_result = F_OK;
    buffer[file_size] = '\0';
    std::string file_content = buffer;
    delete buffer;
    return file_content;
}

Sd_File::Result Sd_File::overwrite(const char* string, size_t pos)
{
    FIL file_p;
    FRESULT res;
FILE_OPEN:
    res = f_open(&file_p, file_name,  FA_WRITE);
    if (res != FR_OK)
    {
        // failed
        //try to mount drive if it is not mounted
        mount_drive();
        return (last_result = F_ERROR);
    }
    f_lseek(&file_p, pos);
    auto ret = f_puts(string, &file_p);
    f_close(&file_p);
    if (ret < 0)
    {
        TRACE_ABNORMAL(TRACE_SD, "Could not write to file %s (%d)\n", string,  ret);
        return (last_result = F_ERROR);
    }
    return (last_result = F_OK);
}
