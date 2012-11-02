#ifndef _REAL_TIME_DATABASE_H_
#define _REAL_TIME_DATABASE_H_

#ifndef _TYPE_DEF_H_
#define _TYPE_DEF_H_

#include <stdint.h> /* int64_t */
#include <errno.h>

#ifdef WIN32
#include <windows.h>
typedef HINSTANCE lib_handle;
#define LoadLib LoadLibraryA
#define GetAPI GetProcAddress
#define FreeLib FreeLibrary
#else //WIN32
#include <dlfcn.h>
typedef void *lib_handle;
#define LoadLib(x) dlopen(x, RTLD_LAZY)
#define GetAPI dlsym
#define FreeLib dlclose
#endif //WIN32

#ifndef __cplusplus
typedef enum {false = 0, true = 1} bool;
#endif //__cplusplus

#ifdef WIN32
typedef __int64 int64;
typedef unsigned __int64 uint64;
#else //WIN32
typedef long long int64;
typedef unsigned long long uint64;
#endif //WIN32

#if !defined(_SSIZE_T_) && !defined(_SSIZE_T_DEFINED)
typedef intptr_t ssize_t;
# define _SSIZE_T_
# define _SSIZE_T_DEFINED
#endif

#include <stddef.h>
#define container_of(ptr, type, member) \
  ((type *) ((char *) (ptr) - offsetof(type, member)))

#endif //_TYPE_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

#define RTDB_OK           0   /* Successful result */
#define RTDB_ERROR        1   /* SQL error or missing database */

typedef void *db_handle;
typedef void *sql_handle;
typedef void *RealTimeBuffer;

struct RealTimeField
{
	unsigned char type		:4;	//4 bits	0-15
	unsigned char length	:4;	//4 bits	0-15
};

typedef enum {
	RTDB_PADDING = 0, 
	RTDB_INT8 = 1, 
	RTDB_UINT8 = 2, 
	RTDB_INT16 = 3, 
	RTDB_UINT16 = 4, 
	RTDB_INT32 = 5, 
	RTDB_UINT32 = 6, 
	RTDB_INT64 = 7, 
	RTDB_UINT64 = 8, 
	RTDB_FLOAT = 9, 
	RTDB_DOUBLE = 10, 
	RTDB_STRING = 11, 
	RTDB_BINARY = 12, 
} RTDB_TYPE;

typedef void (*_reserved)(void);
typedef int (*_open)(const char *filename, db_handle *ppDb);
typedef int (*_close)(db_handle);
typedef int (*_column_count)(sql_handle pStmt);
typedef const char *(*_column_name)(sql_handle, int N);
typedef const void *(*_column_blob)(sql_handle, int iCol);
typedef int (*_column_bytes)(sql_handle, int iCol);
typedef double (*_column_double)(sql_handle, int iCol);
typedef int (*_column_int)(sql_handle, int iCol);
typedef int64 (*_column_int64)(sql_handle, int iCol);
typedef const unsigned char *(*_column_text)(sql_handle, int iCol);
typedef int (*_column_type)(sql_handle, int iCol);
typedef bool (*_exec)(db_handle, const char *sql);
typedef bool (*query_callback)(const sql_handle, long record, int column_count, void *parameter);
typedef long (*_query)(db_handle, const char *sql, query_callback callback, void *parameter);
typedef RealTimeBuffer (*_alloc)(struct RealTimeField *field, unsigned char field_count, unsigned long max_record_count);
typedef void (*_free)(RealTimeBuffer *);
typedef bool (*_reset)(RealTimeBuffer buffer);
typedef long (*_add_record)(RealTimeBuffer buffer, void *record, int record_length);
typedef long (*_get_record_count)(RealTimeBuffer buffer);
typedef bool (*_get_record)(RealTimeBuffer buffer, long index, void *record, int record_length);
typedef long (*_insert_v2)(db_handle, const char *sql, RealTimeBuffer buffer);

struct RealTimeDB
{
	_open open;
	_reserved reserved1;
	_close close;
	_reserved reserved2;
	_reserved reserved3;
	_reserved reserved4;
	_reserved reserved5;
	_reserved reserved6;
	_reserved reserved7;
	_reserved reserved8;
	_reserved reserved9;
	_reserved reserved10;
	_reserved reserved11;
	_reserved reserved12;
	_reserved reserved13;
	_reserved reserved14;
	_reserved reserved15;
	_column_count column_count;
	_column_name column_name;
	_column_type column_type;
	_column_bytes column_bytes;
	_column_blob column_blob;
	_column_double column_double;
	_column_int column_int;
	_column_int64 column_int64;
	_column_text column_text;
	_exec exec;
	_query query;
	_reserved reserved16;
	_alloc alloc;
	_free free;
	_reset reset;
	_add_record add_record;
	_get_record_count get_record_count;
	_get_record get_record;
	_insert_v2 insert_v2;
};

typedef bool (*_RealTimeDB_Initialize)(struct RealTimeDB *rtdb);

#define IS_COLUMN_HEADER(record) (record == 0)

#ifdef __cplusplus
}  /* end of the 'extern "C"' block */
#endif

#endif //_REAL_TIME_DATABASE_H_