#ifndef __SD_FF_HPP__
#define __SD_FF_HPP__
// #-------------------------------#
// |           includes            |
// #-------------------------------#
// pico includes

// c/c++ includes
#include <cstddef>
#include <stdint.h>
#include <stdio.h>
#include <dirent.h>

// my includes

// #-------------------------------#
// |            macros             |
// #-------------------------------#

/*
FA_READ             Specifies read access to the file. Data can be read from the file.
FA_WRITE            Specifies write access to the file. Data can be written to the file. Combine with FA_READ for read-write access.
FA_OPEN_EXISTING    Opens a file. The function fails if the file is not existing. (Default)
FA_CREATE_NEW       Creates a new file. The function fails with FR_EXIST if the file is existing.
FA_CREATE_ALWAYS    Creates a new file. If the file is existing, it will be truncated and overwritten.
FA_OPEN_ALWAYS      Opens the file if it is existing. If not, a new file will be created.
FA_OPEN_APPEND      Same as FA_OPEN_ALWAYS except the read/write pointer is set end of the file.
*/

/* File access mode and open method flags (3rd argument of f_open) */
#define	FA_READ				0x01
#define	FA_WRITE			0x02
#define	FA_OPEN_EXISTING	0x00
#define	FA_CREATE_NEW		0x04
#define	FA_CREATE_ALWAYS	0x08
#define	FA_OPEN_ALWAYS		0x10
#define	FA_OPEN_APPEND		0x30

/* File attribute bits for directory entry (FILINFO.fattrib) */
#define	AM_RDO	0x01	/* Read only */
#define	AM_HID	0x02	/* Hidden */
#define	AM_SYS	0x04	/* System */
#define AM_DIR	0x10	/* Directory */
#define AM_ARC	0x20	/* Archive */

// #-------------------------------#
// | global types declarations     |
// #-------------------------------#
typedef unsigned int UINT;  /* int must be 16-bit or 32-bit */
typedef unsigned char BYTE; /* char must be 8-bit */
typedef uint16_t WORD;      /* 16-bit unsigned integer */
typedef uint32_t DWORD;     /* 32-bit unsigned integer */
typedef uint64_t QWORD;     /* 64-bit unsigned integer */
typedef WORD WCHAR;         /* UTF-16 character type */
typedef size_t FSIZE_t;
typedef char TCHAR;
typedef QWORD LBA_t;

typedef enum {
  FR_OK = 0,    /* (0) Succeeded */
  FR_DISK_ERR,  /* (1) A hard error occurred in the low level disk I/O layer */
  FR_INT_ERR,   /* (2) Assertion failed */
  FR_NOT_READY, /* (3) The physical drive cannot work */
  FR_NO_FILE,   /* (4) Could not find the file */
  FR_NO_PATH,   /* (5) Could not find the path */
  FR_INVALID_NAME, /* (6) The path name format is invalid */
  FR_DENIED, /* (7) Access denied due to prohibited access or directory full */
  FR_EXIST,  /* (8) Access denied due to prohibited access */
  FR_INVALID_OBJECT,  /* (9) The file/directory object is invalid */
  FR_WRITE_PROTECTED, /* (10) The physical drive is write protected */
  FR_INVALID_DRIVE,   /* (11) The logical drive number is invalid */
  FR_NOT_ENABLED,     /* (12) The volume has no work area */
  FR_NO_FILESYSTEM,   /* (13) There is no valid FAT volume */
  FR_MKFS_ABORTED,    /* (14) The f_mkfs() aborted due to any problem */
  FR_TIMEOUT, /* (15) Could not get a grant to access the volume within defined
                 period */
  FR_LOCKED,  /* (16) The operation is rejected according to the file sharing
                 policy */
  FR_NOT_ENOUGH_CORE,     /* (17) LFN working buffer could not be allocated */
  FR_TOO_MANY_OPEN_FILES, /* (18) Number of open files > FF_FS_LOCK */
  FR_INVALID_PARAMETER    /* (19) Given parameter is invalid */
} FRESULT;

typedef struct {
  FSIZE_t fsize; /* File size */
  WORD fdate;    /* Modified date */
  WORD ftime;    /* Modified time */
  BYTE fattrib;  /* File attribute */
#if FF_USE_LFN
  TCHAR altname[FF_SFN_BUF + 1]; /* Alternative file name */
  TCHAR fname[FF_LFN_BUF + 1];   /* Primary file name */
#else
  TCHAR fname[12 + 1]; /* File name */
#endif
} FILINFO;

typedef struct {
	FILE *file;
} FIL;

// typedef struct {
// 	void* p;
// } DIR;

typedef struct {
	void* p;
} FATFS;

typedef struct {
	BYTE fmt;			/* Format option (FM_FAT, FM_FAT32, FM_EXFAT and FM_SFD) */
	BYTE n_fat;			/* Number of FATs */
	UINT align;			/* Data area alignment (sector) */
	UINT n_root;		/* Number of root directory entries */
	DWORD au_size;		/* Cluster size (byte) */
} MKFS_PARM;


// #-------------------------------#
// | global variables declarations |
// #-------------------------------#

// #-------------------------------#
// | global function declarations  |
// #-------------------------------#
/*--------------------------------------------------------------*/
/* FatFs Module Application Interface                           */
/*--------------------------------------------------------------*/

FRESULT f_open(FIL *fp, const TCHAR *path,
               BYTE mode); /* Open or create a file */
FRESULT f_close(FIL *fp);  /* Close an open file object */
FRESULT f_read(FIL *fp, void *buff, UINT btr,
               UINT *br); /* Read data from the file */
FRESULT f_write(FIL *fp, const void *buff, UINT btw,
                UINT *bw); /* Write data to the file */
FRESULT f_lseek(FIL *fp,
                FSIZE_t ofs); /* Move file pointer of the file object */
FRESULT f_truncate(FIL *fp);  /* Truncate the file */
FRESULT f_sync(FIL *fp);      /* Flush cached data of the writing file */

FRESULT f_opendir(DIR **dp, const TCHAR *path); /* Open a directory */
FRESULT f_closedir(DIR **dp);                   /* Close an open directory */
FRESULT f_readdir(DIR **dp, FILINFO *fno);      /* Read a directory item */

// FRESULT f_findfirst(DIR *dp, FILINFO *fno, const TCHAR *path,
//                     const TCHAR *pattern); /* Find first file */
// FRESULT f_findnext(DIR *dp, FILINFO *fno); /* Find next file */
FRESULT f_mkdir(const TCHAR *path);        /* Create a sub directory */
FRESULT f_unlink(const TCHAR *path); /* Delete an existing file or directory */
// FRESULT f_rename(const TCHAR *path_old,
//                  const TCHAR *path_new); /* Rename/Move a file or directory */
FRESULT f_stat(const TCHAR *path, FILINFO *fno); /* Get file status */
// FRESULT f_chmod(const TCHAR *path, BYTE attr,
//                 BYTE mask); /* Change attribute of a file/dir */
// FRESULT f_utime(const TCHAR *path,
//                 const FILINFO *fno);     /* Change timestamp of a file/dir */
// FRESULT f_chdir(const TCHAR *path);      /* Change current directory */
// FRESULT f_chdrive(const TCHAR *path);    /* Change current drive */
// FRESULT f_getcwd(TCHAR *buff, UINT len); /* Get current directory */
// FRESULT f_getfree(const TCHAR *path, DWORD *nclst,
//                   FATFS **fatfs); /* Get number of free clusters on the drive */
// FRESULT f_getlabel(const TCHAR *path, TCHAR *label,
//                    DWORD *vsn);         /* Get volume label */
// FRESULT f_setlabel(const TCHAR *label); /* Set volume label */
// FRESULT f_forward(FIL *fp, UINT (*func)(const BYTE *, UINT), UINT btf,
//                   UINT *bf); /* Forward data to the stream */
// FRESULT f_expand(FIL *fp, FSIZE_t fsz,
//                  BYTE opt); /* Allocate a contiguous block to the file */
FRESULT f_mount(FATFS *fs, const TCHAR *path,
                BYTE opt); /* Mount/Unmount a logical drive */
// FRESULT f_mkfs(const TCHAR *path, const MKFS_PARM *opt, void *work,
//                UINT len); /* Create a FAT volume */
// FRESULT f_fdisk(BYTE pdrv, const LBA_t ptbl[],
//                 void *work);  /* Divide a physical drive into some partitions */
// FRESULT f_setcp(WORD cp);     /* Set current code page */
// int f_putc(TCHAR c, FIL *fp); /* Put a character to the file */
int f_puts(const TCHAR *str, FIL *cp); /* Put a string to the file */
// int f_printf(FIL *fp, const TCHAR *str,
//              ...); /* Put a formatted string to the file */
// TCHAR *f_gets(TCHAR *buff, int len, FIL *fp); /* Get a string from the file */

/* Some API fucntions are implemented as macro */

size_t f_tell(FIL* fp);
void f_unmount(const char* path);
FRESULT f_rewind(FIL* fp);

// #define f_eof(fp) ((int)((fp)->fptr == (fp)->obj.objsize))
// #define f_error(fp) ((fp)->err)
// #define f_tell(fp) ((fp)->fptr) // pos in file
// #define f_size(fp) ((fp)->obj.objsize)
// #define f_rewind(fp) f_lseek((fp), 0)
// #define f_rewinddir(dp) f_readdir((dp), 0)
// #define f_rmdir(path) f_unlink(path)
// #define f_unmount(path)


#endif
