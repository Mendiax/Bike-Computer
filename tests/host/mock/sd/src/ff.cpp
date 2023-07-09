
// #------------------------------#
// |          includes            |
// #------------------------------#
// pico includes

// c/c++ includes
#include <cstdlib>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

// my includes
#include "ff.h"
#include <cstddef>
#include "traces.h"

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

FRESULT f_open(FIL *fp, const TCHAR *path, BYTE mode) {
    const char *fopen_mode;
    switch (mode) {
        case FA_READ:
            fopen_mode = "r";
            break;
        case FA_READ | FA_WRITE:
            fopen_mode = "r+";
            break;
        case FA_CREATE_ALWAYS | FA_WRITE:
            fopen_mode = "w";
            break;
        case FA_CREATE_ALWAYS | FA_WRITE | FA_READ:
            fopen_mode = "w+";
            break;
        case FA_OPEN_APPEND | FA_WRITE:
            fopen_mode = "a";
            break;
        case FA_OPEN_APPEND | FA_WRITE | FA_READ:
            fopen_mode = "a+";
            break;
        case FA_CREATE_NEW | FA_WRITE:
            fopen_mode = "wx";
            break;
        case FA_CREATE_NEW | FA_WRITE | FA_READ:
            fopen_mode = "w+x";
            break;
        default:
            TRACE_ABNORMAL(TRACE_HOST, "Invalid f_open mode\n");
            exit(-1);
            return FR_INVALID_PARAMETER;
    }

    FILE *file = fopen(path, fopen_mode);
    if (file == NULL) {
        TRACE_ABNORMAL(TRACE_HOST, "No file %s\n", path);
        exit(-1);
        return FR_NO_FILE;
    }

    fp->file = file;
    return FR_OK;
}

FRESULT f_close(FIL *fp) {
    if (fclose(fp->file) != 0) {
        return FR_INT_ERR;
    }
    return FR_OK;
}

FRESULT f_read(FIL *fp, void *buff, unsigned int btr, unsigned int *br) {
    *br = fread(buff, 1, btr, fp->file);
    if (*br < btr) {
        if (feof(fp->file)) {
            return FR_OK; // Reached end of file
        } else {
            return FR_DISK_ERR; // Read error
        }
    }
    return FR_OK;
}

FRESULT f_write(FIL *fp, const void *buff, unsigned int btw, unsigned int *bw) {
    *bw = fwrite(buff, 1, btw, fp->file);
    if (*bw < btw) {
        return FR_DISK_ERR; // Write error
    }
    return FR_OK;
}

FRESULT f_lseek(FIL *fp, FSIZE_t ofs) {
    if (fseek(fp->file, ofs, SEEK_SET) != 0) {
        return FR_DISK_ERR;
    }
    return FR_OK;
}

FRESULT f_truncate(FIL *fp) {
    if (fp == NULL || fp->file == NULL) {
        return FR_INVALID_OBJECT;
    }

    if (ftruncate(fileno(fp->file), ftell(fp->file)) != 0) {
        return FR_DISK_ERR;
    }
    return FR_OK;
}

FRESULT f_opendir(DIR **dp, const char *path) {
    // Implement directory opening logic
    *dp = opendir(path);
    if (*dp == NULL) {
        TRACE_ABNORMAL(TRACE_HOST, "Failed to open directory %s.\n", path);
        return FR_NO_PATH;
    }
    return FR_OK;
}

FRESULT f_closedir(DIR **dp) {
    // Implement directory closing logic
    return FR_OK;
}

FRESULT f_readdir(DIR **dp, FILINFO *fno) {

    struct dirent* entry;
    entry = readdir(*dp);

    if(entry == NULL)
    {
        return FR_NO_FILE;
    }
    // PRINT("d_type:" << (int)entry->d_type << "; name:" << entry->d_name);
    switch (entry->d_type) {
        case DT_DIR:
            fno->fattrib = AM_DIR;
        break;
        case DT_UNKNOWN:
        case DT_FIFO:
        case DT_CHR:
        case DT_BLK:
        case DT_REG:
        case DT_LNK:
        case DT_SOCK:
        default:
            fno->fattrib = AM_SYS;
            break;

    }
    strncpy(fno->fname, entry->d_name, 12);
    // Implement directory reading logic
    return FR_OK;
}
FRESULT f_mkdir(const TCHAR *path)
{
    if (mkdir(path, 0777) == 0) {
        return FR_OK;
    } else {
        TRACE_ABNORMAL(TRACE_HOST, "Failed to create directory %s.\n", path);
        return FR_DISK_ERR;
    }
}

FRESULT f_unlink(const char *path) {
    if (remove(path) != 0) {
        return FR_DISK_ERR;
    }
    return FR_OK;
}

static long get_file_size(const char* filename) {
    FILE* file = fopen(filename, "rb"); // Open the file in binary mode

    if (file == NULL) {
        // Error handling if the file cannot be opened
        return -1;
    }

    // Seek to the end of the file
    if (fseek(file, 0, SEEK_END) != 0) {
        // Error handling if fseek fails
        fclose(file);
        return -1;
    }

    // Get the position indicator which represents the file size
    long size = ftell(file);

    fclose(file);
    return size;
}

FRESULT f_stat(const char *path, FILINFO *fno) {
    fno->fsize = get_file_size(path);
    if (fno->fsize < 0){
        return FR_NO_FILE;
    }
    // Implement file/directory stat logic
    return FR_OK;
}

FRESULT f_mount(FATFS *fs, const char *path, unsigned char opt) {
    // Implement file system mounting logic
    return FR_OK;
}

int f_puts(const char *str, FIL *cp) {
    if (fputs(str, cp->file) == EOF) {
        return EOF;
    }
    return 0;
}

size_t f_tell(FIL *fp) {
    return ftell(fp->file);
}

void f_unmount(const char *path) {
    // do nothing
}

FRESULT f_rewind(FIL *fp) {
    if (fseek(fp->file, 0, SEEK_SET) != 0) {
        return FR_DISK_ERR;
    }
    return FR_OK;
}



// size_t f_tell(FIL *fp) { return 0; }
// void f_unmount(const char *path) {}

// #------------------------------#
// | static functions definitions |
// #------------------------------#
