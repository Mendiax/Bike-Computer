
// #------------------------------#
// |          includes            |
// #------------------------------#
// c/c++ includes

// my includes
#include "traces.h"
#include "file.hpp"
#include "string.h"
#include "sd_common.h"
#include "ff.h"

// #------------------------------#
// |           macros             |
// #------------------------------#

// #------------------------------#
// | global variables definitions |
// #------------------------------#

// #------------------------------#
// | static variables definitions |
// #------------------------------#

// #------------------------------#
// | static functions declarations|
// #------------------------------#

// #------------------------------#
// | global function definitions  |
// #------------------------------#

class FilePrivate {
public:
    FIL file;
    bool is_open = false;
};

File::File(const std::string_view& file_name, int flags)
    : flags(flags), file_name(file_name), pimpl(new FilePrivate())
{
    mount_drive();
    if(!sd_mounted)
    {
        TRACE_ABNORMAL(TRACE_SD, "Could not mount the drive\n");
    }
    // open file
    FRESULT res;
    BYTE mode = 0;
    if (flags & READ)
    {
        mode |= FA_READ;
    }
    if (flags & WRITE)
    {
        mode |= FA_WRITE;
    }
    if (flags & APPEND)
    {
        mode |= FA_OPEN_APPEND;
    }
    if (flags & CREATE)
    {
        mode |= FA_OPEN_ALWAYS;
    }
    if (flags & TRUNCATE)
    {
        mode |= FA_CREATE_ALWAYS;
    }

    TRACE_DEBUG(4, TRACE_SD, "File %s opened with flags %s(%s)\n", file_name.cbegin(), toBits(flags), toBits(mode));
    res = f_open(&pimpl->file, file_name.cbegin(), mode);
    if (res != FR_OK)
    {
        TRACE_ABNORMAL(TRACE_SD, "File::File: f_open error %d for file %s\n", res, file_name);
        pimpl->file = {};
    }
    else {
        pimpl->is_open = true;
    }
}
File::~File()
{
    this->close();
    delete pimpl;
}
void File::close()
{
    if (this->is_open())
    {
        f_close(&pimpl->file);
        pimpl->file = {};
        pimpl->is_open = false;
    }
}
bool File::is_open()
{
    return pimpl->is_open;
}
size_t File::read(char* buffer, size_t len)
{
    if (!this->is_open() || !(this->flags & READ))
    {
        return 0;
    }
    UINT bytesRead = 0;
    FRESULT res = f_read(&pimpl->file, buffer, len, &bytesRead);
    if (res != FR_OK)
    {
        TRACE_ABNORMAL(TRACE_SD, "File::read: f_read error %d for file %s\n", res, this->file_name.c_str());
        return 0;
    }
    return bytesRead;
}
size_t File::write(const std::string_view& buffer)
{
    if (!this->is_open() || !(this->flags & WRITE || this->flags & APPEND))
    {
        return 0;
    }
    UINT bytesWritten = 0;
    size_t len = buffer.length();
    FRESULT res = f_write(&pimpl->file, buffer.cbegin(), len, &bytesWritten);
    if (res != FR_OK)
    {
        TRACE_ABNORMAL(TRACE_SD, "File::write: f_write error %d for file %s\n", res, this->file_name.c_str());
        return 0;
    }
    return bytesWritten;
}
void File::flush()
{
    if (!this->is_open() || !(this->flags & WRITE || this->flags & APPEND))
    {
        return;
    }
    FRESULT res = f_sync(&pimpl->file);
    if (res != FR_OK)
    {
        TRACE_ABNORMAL(TRACE_SD, "File::flush: f_sync error %d for file %s\n", res, this->file_name.c_str());
        return;
    }
}
bool File::eof()
{
    if (!this->is_open() || !(this->flags & READ))
    {
        return true;
    }
    return f_eof(&pimpl->file);
}
bool File::seek(size_t pos)
{
    if (!this->is_open() || !(this->flags & READ || this->flags & WRITE || this->flags & APPEND))
    {
        return false;
    }
    FRESULT res = f_lseek(&pimpl->file, pos);
    if (res != FR_OK)
    {
        TRACE_ABNORMAL(TRACE_SD, "File::seek: f_lseek error %d for file %s\n", res, this->file_name.c_str());
        return false;
    }
    return true;
}
size_t File::tell()
{
    if (!this->is_open() || !(this->flags & READ || this->flags & WRITE || this->flags & APPEND))
    {
        return 0;
    }
    return f_tell(&pimpl->file);
}

// #------------------------------#
// | static functions definitions |
// #------------------------------#
