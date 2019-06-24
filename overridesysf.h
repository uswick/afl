#ifndef __H_OVERRIDE_SYS_FUNCS_H__
#define __H_OVERRIDE_SYS_FUNCS_H__

#define _VMWARE_VMX_ 1

#ifndef _VMWARE_VMX_

#define gettimeofday 	gettimeofday
#define getenv 		getenv
#define fopen         	fopen
#define fdopen         	fdopen
#define open         	open
#define opendir         opendir
#define readdir         readdir
#define fgets    	fgets
#define lseek           lseek 
#define fclose 		fclose
#define symlink 	symlink
#define unlink 		unlink
#define link 		link
#define read 		read
#define mmap 		mmap
#define munmap 		munmap

#else

#include "posix.h"
#include "str.h"


static inline char *
AFLVMX_Str_Strcat(char *buf,       // IN/OUT
           const char *src)  // IN
{
   return  Str_Strcat(buf, src, 100000);
}


//#define AFLVMX_Str_Sprintf(buf, fmt, ...) 

static inline int
AFLVMX_Str_Sprintf(char *buf,       // OUT
            const char *fmt, // IN
            ...)             // IN
{
   va_list args;
   int i;

   va_start(args, fmt);
   i = Str_Vsnprintf(buf, 100000, fmt, args);
   va_end(args);
   if (i < 0) {
      Panic("%s:%d Buffer too small\n", __FILE__, __LINE__);
   }
   return i;
}


#define gettimeofday 	gettimeofday
#define getenv 		Posix_Getenv
#define fopen         	Posix_Fopen
#define fdopen         	fdopen
#define open         	Posix_Open
#define open64         	Posix_Open
#define opendir         Posix_OpenDir
#define rmdir 		Posix_Rmdir
#define mkdir 		Posix_Mkdir
#define rename          Posix_Rename
#define access  	Posix_Access
#define readdir         readdir
#define fgets    	fgets
#define lseek           lseek 
#define fclose 		fclose
#define symlink 	Posix_Symlink
#define unlink 		Posix_Unlink
#define link 		Posix_Link
#define read 		read
#define mmap 		mmap
#define munmap 		munmap
#define snprintf 	Str_Snprintf
#define sprintf 	AFLVMX_Str_Sprintf
#define strcat 		AFLVMX_Str_Strcat
#define lstat 		Posix_Lstat



#endif

#endif
