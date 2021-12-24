/*******************************************************************************
* Copyright (c) 2011 by Gillen Daniel <gillen.dan@pinguin.lu>                  *
*                                                                              *
* This file contains macros with some very often used code pieces. It should   *
* primarily make my everyday life easier and improve code portability.         *
*                                                                              *
* This program is free software: you can redistribute it and/or modify it      *
* under the terms of the GNU General Public License as published by the Free   *
* Software Foundation, either version 3 of the License, or (at your option)    *
* any later version.                                                           *
*                                                                              *
* This program is distributed in the hope that it will be useful, but WITHOUT  *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for     *
* more details.                                                                *
*                                                                              *
* You should have received a copy of the GNU General Public License along with *
* this program. If not, see <http://www.gnu.org/licenses/>.                    *
*******************************************************************************/

// Available macro groups:
//
// MACROS_FILE, MACROS_STRING, MACROS_WSTRING, MACROS_ENDIANNESS, MACROS_MUTEX,
// MACROS_MEMORY, MACROS_LOGGING
//
// Define one or more of these before including this file to make them available

#ifndef MACROS_H
#define MACROS_H

/*
 * Macros for file access
 *
 * Will also include MACROS_LOGGING
 */
#ifdef MACROS_FILE
  #include <stdio.h>

  // These macros rely partly on logging macros, so those are needed too
  #define MACROS_LOGGING

  #undef FOPEN
  #ifndef __APPLE__
    #define FOPEN (FILE*)fopen64
  #else
    // Apple always uses fopen
    #define FOPEN (FILE*)fopen
  #endif // __APPLE__
  #undef FCLOSE
  #define FCLOSE(var,err_ret) { \
    if(fclose(var)!=0) { \
      LOG_ERROR("Couldn't close file!") \
      err_ret; \
    } \
  }
  #undef FSEEK
  #define FSEEK(hfile,off,whence,err_ret) { \
    if(fseeko(hfile,off,whence)!=0) { \
      LOG_ERROR("Couldn't seek to offset %u!",off); \
      err_ret; \
    } \
  }
  #undef FTELL
  #define FTELL(hfile,var,err_ret) { \
    if((var=ftello(hfile))==-1) { \
      LOG_ERROR("Unable to get file position!"); \
      err_ret; \
    } \
  }
  #undef FTELLSIZE
  #define FTELLSIZE(hfile,var,err_ret) { \
    FSEEK(hfile,0,SEEK_END,err_ret); \
    FTELL(hfile,var,err_ret); \
    rewind(hfile); \
  }
#endif

/*
 * Macros for string functions
 *
 * Will also include MACROS_MEMORY and MACROS_LOGGING
 */
#ifdef MACROS_STRING
  #include <string.h>

  // These macros rely partly on memory macros, so those are needed too
  #define MACROS_MEMORY

  #undef STRSET
  #define STRSET(dst,src,err_ret) { \
    MALLOC(dst,char*,(strlen(src)+1)*sizeof(char),err_ret) \
    strcpy(dst,src); \
  }
  #undef STRNSET
  #define STRNSET(dst,src,size,err_ret) { \
    MALLOC(dst,char*,((size)+1)*sizeof(char),err_ret) \
    strncpy(dst,src,size); \
    (dst)[size]='\0'; \
  }
  #undef STRAPP
  #define STRAPP(var1,var2,err_ret) { \
    REALLOC(var1,char*,(strlen(var1)+strlen(var2)+1)*sizeof(char),err_ret) \
    strcpy((var1)+strlen(var1),var2); \
  }
  #undef STRNAPP
  #define STRNAPP(var1,var2,size,err_ret) { \
    REALLOC(var1,char*,(strlen(var1)+(size)+1)*sizeof(char),err_ret) \
    (var1)[strlen(var1)+(size)]='\0'; \
    strncpy((var1)+strlen(var1),var2,size); \
  }
#endif

/*
 * Macros for wide string functions
 *
 * Will also include MACROS_MEMORY and MACROS_LOGGING
 */
#ifdef MACROS_WSTRING
  #include <string.h>

  // These macros rely partly on memory macros, so those are needed too
  #define MACROS_MEMORY

  #undef WSTRSET
  #define WSTRSET(dst,src,err_ret) { \
    MALLOC(dst,wchar_t*,(wcslen(src)+1)*sizeof(wchar_t),err_ret) \
    wcscpy(dst,src); \
  }
  #undef WSTRNSET
  #define WSTRNSET(dst,src,size,err_ret) { \
    MALLOC(dst,wchar_t*,((size)+1)*sizeof(wchar_t),err_ret) \
    wcsncpy(dst,src,size); \
    (dst)[size]=L'\0'; \
  }
#endif

/*
 * Macros for endianness conversion
 */
#ifdef MACROS_ENDIANNESS
  #include <inttypes.h>
  #include <endian.h>

  #undef LE16TOH
  #define LE16TOH(var) le16toh(var)
  #undef BE16TOH
  #define BE16TOH(var) be16toh(var)
  #undef LE32TOH
  #define LE32TOH(var) le32toh(var)
  #undef BE32TOH
  #define BE32TOH(var) be32toh(var)
  #undef LE64TOH
  #define LE64TOH(var) le64toh(var)
  #undef BE64TOH
  #define BE64TOH(var) be64toh(var)
  #undef HTOLE16
  #define HTOLE16(var) htole16(var)
  #undef HTOBE16
  #define HTOBE16(var) htobe16(var)
  #undef HTOLE32
  #define HTOLE32(var) htole32(var)
  #undef HTOBE32
  #define HTOBE32(var) htobe32(var)
  #undef HTOLE64
  #define HTOLE64(var) htole64(var)
  #undef HTOBE64
  #define HTOBE64(var) htobe64(var)
  #undef UTF16LETOH
  #define UTF16LETOH(buf,buf_len) {                                         \
    for(int buf_off=0;buf_off<((buf_len)-1);buf_off+=2) {                   \
      *((uint16_t*)((buf)+buf_off))=LE16TOH(*((uint16_t*)((buf)+buf_off))); \
    }                                                                       \
  }
  #undef UTF16BETOH
  #define UTF16BETOH(buf,buf_len) {                                         \
    for(int buf_off=0;buf_off<((buf_len)-1);buf_off+=2) {                   \
      *((uint16_t*)((buf)+buf_off))=BE16TOH(*((uint16_t*)((buf)+buf_off))); \
    }                                                                       \
  }
  #undef HTOUTF16LE
  #define HTOUTF16LE(buf,buf_len) {                                         \
    for(int buf_off=0;buf_off<((buf_len)-1);buf_off+=2) {                   \
      *((uint16_t*)((buf)+buf_off))=HTOLE16(*((uint16_t*)((buf)+buf_off))); \
    }                                                                       \
  }
  #undef HTOUTF16BE
  #define HTOUTF16BE(buf,buf_len) {                                         \
    for(int buf_off=0;buf_off<((buf_len)-1);buf_off+=2) {                   \
      *((uint16_t*)((buf)+buf_off))=HTOBE16(*((uint16_t*)((buf)+buf_off))); \
    }                                                                       \
  }
#endif

/*
 * Macros for mutex access
 */
#ifdef MACROS_MUTEX
  #include <pthread.h>

  #undef MUTEX_INIT
  #define MUTEX_INIT(var) { \
    pthread_mutex_init(&(var),NULL); \
  }
  #undef MUTEX_DESTROY
  #define MUTEX_DESTROY(var) { \
    pthread_mutex_destroy(&(var)); \
  }
  #undef MUTEX_LOCK
  #define MUTEX_LOCK(var) { \
    pthread_mutex_lock(&(var)); \
  }
  #undef MUTEX_UNLOCK
  #define MUTEX_UNLOCK(var) { \
    pthread_mutex_unlock(&(var)); \
  }
#endif

/*
 * Macros for memory management
 *
 * Will also include MACROS_LOGGING!
 */
#ifdef MACROS_MEMORY
  #include <stdlib.h>

  // These macros rely partly on logging macros, so those are needed too
  #define MACROS_LOGGING

  #undef MALLOC
  #define MALLOC(var,var_type,size,err_ret) { \
    (var)=(var_type)malloc(size); \
    if((var)==NULL) { \
      LOG_ERROR("Couldn't allocate memmory!\n"); \
      err_ret; \
    } \
  }
  #undef REALLOC
  #define REALLOC(var,var_type,size,err_ret) { \
    (var)=(var_type)realloc((var),size); \
    if((var)==NULL) { \
      LOG_ERROR("Couldn't allocate memmory!\n"); \
      err_ret; \
    } \
  }
  #undef FREE
  #define FREE(var) free(var)
#endif

/*
 * Macros to ease debugging and error reporting
 *
 * These require the following function to be implemented somewhere:
 *
 * #include <stdio.h>
 * #include <stdarg.h>
 *
 * static void LogMessage(char *p_message_type,
 *                        char *p_calling_function,
 *                        int line,
 *                        char *p_message,
 *                        ...)
 * {
 *   va_list VaList;
 *   // Print message "header"
 *   printf("%s: %s@%u : ",
 *          p_message_type,
 *          p_calling_function,
 *          line);
 *   // Print message with variable parameters
 *   va_start(VaList,p_message);
 *   vprintf(p_message,VaList);
 *   va_end(VaList);
 *   printf("\n");
 * }
 */
#ifdef MACROS_LOGGING
  #undef LOG_ERROR
  #define LOG_ERROR(...) \
    LogMessage("ERROR",(char*)__FUNCTION__,__LINE__,__VA_ARGS__);
  #undef LOG_DEBUG
  #define LOG_DEBUG(...) { \
    LogMessage("DEBUG",(char*)__FUNCTION__,__LINE__,__VA_ARGS__); \
  }
#endif

#endif // MACROS_H

/*
  ----- Change history -----
  20130611: * Added ability to only include specific macro groups.
  20110428: * Initial release.
*/

