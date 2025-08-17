#include "sd_file.h"

#include "string.h"

#include "sd_common.h"
#include "ff.h"
#include "traces.h"

Sd_File::Sd_File(const std::string& file_name)
    :file_name{file_name}, correct_lines_pos{false}
{
    mount_drive();
    if(!sd_mounted)
    {
        TRACE_ABNORMAL(TRACE_SD, "Could not mount the drive\n");
    }
    FIL file_p;
    FRESULT res;
    FILINFO fno;

    while(true)
    {
        res = f_stat(file_name.c_str(), &fno);
        if (res == FR_OK)
        {
            // file exists
            TRACE_DEBUG(1, TRACE_SD, "File %s already exists\n", file_name.c_str());
            last_result = F_OK;
            return;
        }
        else {
            // failed
            TRACE_ABNORMAL(TRACE_SD, "File %s does not exist!\n", file_name.c_str());
            auto all_files = dir::get_files(".");
            std::string all_files_str;
            for (const auto& f : all_files) {
                all_files_str += f + " ";
            }
            TRACE_ABNORMAL(TRACE_SD, "Files in the current directory: %s\n", all_files_str.c_str());
        }
        res = f_open(&file_p, file_name.c_str(),  FA_READ | FA_WRITE | FA_OPEN_APPEND);
        if (res != FR_OK)
        {
            // failed
            //try to mount drive if it is not mounted
            mount_drive();
            TRACE_ABNORMAL(TRACE_SD, "Could not open the file %s\n", file_name.c_str());

            sleep_ms(10000);
        }
        else {
            f_close(&file_p);
            break;
        }
    }
}

Sd_File::~Sd_File()
{
    // PRINTF("Unmount\n");
    // unmount_drive();
}

void Sd_File::remove()
{
    f_unlink(file_name.c_str());
}



Sd_File::Result Sd_File::append(const char* string)
{
    FIL file_p;
    FRESULT res;
    res = f_open(&file_p, file_name.c_str(),  FA_WRITE | FA_OPEN_APPEND);
    if (res != FR_OK)
    {
        //try to mount drive if it is not mounted
        mount_drive();
        return (last_result = F_ERROR);
    }
    correct_lines_pos = false;
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
    auto fres = f_stat(file_name.c_str(), &info);
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
    res = f_open(&file_p, file_name.c_str(),  FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
    if (res != FR_OK)
    {
        // failed
        //try to mount drive if it is not mounted
        mount_drive();
        TRACE_ABNORMAL(TRACE_SD, "Could not open a file (%d)\n",  res);
        return (last_result = F_ERROR);
    }
    correct_lines_pos = true;
    lines_pos.clear();
    auto ret = f_truncate(&file_p);
    f_close(&file_p);
    if (ret != FR_OK)
    {
        TRACE_ABNORMAL(TRACE_SD, "Could not truncate (%d)\n",  ret);
        return (last_result = F_ERROR);
    }

    return (last_result = F_OK);
}

std::string Sd_File::read_all()
{
    FIL file_p;
    FRESULT res;
    res = f_open(&file_p, file_name.c_str(),  FA_READ);
    if (res != FR_OK)
    {
        PRINT("file_name.c_str() = " << this->file_name.c_str());
        TRACE_ABNORMAL(TRACE_SD, " %zu Read file failed (%d)\n", strlen(this->file_name.c_str()), res);
        return "";
    }
    unsigned bytes_read;
    const size_t file_size = get_size();
    if(file_size == 0)
    {
        f_close(&file_p);
        last_result = F_ERROR;
        TRACE_ABNORMAL(TRACE_SD, "File size 0 \n");
        return "";
    }
    auto buffer = new char[file_size + 1];
    memset(buffer, 0, file_size + 1);
    if(buffer == nullptr)
    {
        f_close(&file_p);
        last_result = F_ERROR;
        TRACE_ABNORMAL(TRACE_SD, "couldnt allocate buffer \n");
        return "";
    }
    f_read(&file_p, buffer, file_size, &bytes_read);
    f_close(&file_p);
    if (file_size != bytes_read)
    {
        TRACE_ABNORMAL(TRACE_SD, "Error while reading the file %s read %u/%zu\n",
                       file_name.c_str(), bytes_read, file_size);
        last_result = F_ERROR;
        memset(buffer, 0, file_size + 1);
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
    res = f_open(&file_p, file_name.c_str(),  FA_READ | FA_WRITE);
    if (res != FR_OK)
    {
        // failed
        //try to mount drive if it is not mounted
        mount_drive();
        return (last_result = F_ERROR);
    }
    f_lseek(&file_p, pos);
    auto ret = f_puts(string, &file_p);
    correct_lines_pos = false;

    f_close(&file_p);
    if (ret < 0)
    {
        TRACE_ABNORMAL(TRACE_SD, "Could not write to file %s (%d)\n", string,  ret);
        return (last_result = F_ERROR);
    }
    return (last_result = F_OK);
}

size_t Sd_File::get_no_of_lines()
{
    if(correct_lines_pos == true)
    {
        return lines_pos.size();
    }
    FIL file_p;
    FRESULT res;
    res = f_open(&file_p, file_name.c_str(),  FA_READ);
    if (res != FR_OK)
    {
        // failed
        //try to mount drive if it is not mounted
        mount_drive();
        return (last_result = F_ERROR);
    }
    // f_lseek(&file_p, pos);
    // auto ret = f_puts(string, &file_p);
    enum {BUFFER_SIZE=64};
    char buffer[BUFFER_SIZE + 1];
    UINT bytes_read;
    size_t no_of_lines = 0;
    lines_pos.clear();
    correct_lines_pos = true;

    do{
        size_t pos = f_tell(&file_p);
        auto res = f_read(&file_p, buffer, BUFFER_SIZE, &bytes_read);
        if(res != FR_OK){return no_of_lines;}
        // stary of reading
        if(pos == 0 && bytes_read > 0){
            lines_pos.emplace_back(pos);
            no_of_lines = 1;
        }
        buffer[bytes_read] = '\0'; // add end of str
        PRINTF("read line %s\n", buffer);


        char* new_line_char = strchr(buffer, '\n');
        while(new_line_char != NULL)
        {
            size_t offset = new_line_char - buffer + 1;
            lines_pos.emplace_back(pos + offset);
            no_of_lines++;
            PRINTF("found line %zu \n", no_of_lines);
            new_line_char++;
            new_line_char = strchr(new_line_char, '\n');
        }
    } while(bytes_read == BUFFER_SIZE); // read while there is data



    auto ret = f_close(&file_p);
    if (res != FR_OK)
    {
        TRACE_ABNORMAL(TRACE_SD, "Could not count no of files to file (%d)\n",  ret);
        last_result = F_ERROR;
    }
    else
    {
        last_result = F_OK;
    }
    return no_of_lines;
}
std::string Sd_File::read_last_line(size_t max_len)
{
    size_t no_lines = get_no_of_lines();
    if(no_lines > 0)
        return read_line(no_lines - 1, max_len);
    else
        return "";
}

std::vector<std::string> Sd_File::read_all_lines(size_t max_len)
{
    std::vector<std::string> all_lines;
    size_t no_lines = get_no_of_lines();
    all_lines.reserve(no_lines);

    FIL file_p;
    FRESULT res;
    res = f_open(&file_p, file_name.c_str(),  FA_READ);
    if (res != FR_OK)
    {
        return all_lines;
    }
    unsigned bytes_read;

    auto buffer = new char[max_len + 1];
    for(size_t line_no = 0; line_no < no_lines; line_no++)
    {
        size_t line_length = max_len;
        if(line_no < no_lines - 1)
        {
            line_length = lines_pos.at(line_no + 1) - lines_pos.at(line_no) - 1; // -1 for \n
        }

        const size_t bytes_to_read = line_length < max_len ? line_length : max_len;

        // TODO check if buffer != null
        f_lseek(&file_p, lines_pos.at(line_no));
        f_read(&file_p, buffer, bytes_to_read, &bytes_read);
        buffer[bytes_read] = '\0';

        if (bytes_to_read != bytes_read)
        {
            TRACE_ABNORMAL(TRACE_SD, "Error while reading the file %s read %u/%zu\n",
                        file_name.c_str(), bytes_read, bytes_to_read);
            last_result = F_ERROR;
        }
        last_result = F_OK;
        all_lines.emplace_back(buffer);
    }
    f_close(&file_p);

    delete buffer;
    return all_lines;
}


std::string Sd_File::read_line(size_t line_no, size_t max_len)
{
    size_t no_lines = get_no_of_lines();
    if(line_no >= no_lines)
    {
        TRACE_ABNORMAL(TRACE_SD, "Error while reading the file:%s line:%zu no_lines:%zu\n",
                file_name.c_str(), line_no, no_lines);
        return "";
    }
    FIL file_p;
    FRESULT res;
    res = f_open(&file_p, file_name.c_str(),  FA_READ);
    if (res != FR_OK)
    {
        return "";
    }
    unsigned bytes_read;

    size_t line_length = max_len;
    if(line_no < no_lines - 1)
    {
        line_length = lines_pos.at(line_no + 1) - lines_pos.at(line_no) - 1; // -1 for \n
    }
    else
    {
        line_length = get_size() - lines_pos.at(line_no);
    }

    const size_t bytes_to_read = line_length < max_len ? line_length : max_len;

    auto buffer = new char[bytes_to_read + 1];
    f_lseek(&file_p, lines_pos.at(line_no));
    f_read(&file_p, buffer, bytes_to_read, &bytes_read);
    buffer[bytes_read] = '\0';

    f_close(&file_p);
    if (bytes_to_read != bytes_read)
    {
        TRACE_ABNORMAL(TRACE_SD, "Error while reading the file:%s line:%zu no_lines:%zu read %u/%zu\n",
                       file_name.c_str(), line_no, no_lines, bytes_read, bytes_to_read);
        last_result = F_ERROR;
    }
    else
    {
        last_result = F_OK;
    }
    std::string file_content = buffer;
    delete buffer;
    return file_content;
}