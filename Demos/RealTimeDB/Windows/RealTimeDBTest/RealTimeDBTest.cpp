#include <string>
#include <time.h>

#ifdef WIN32
#include "../../../../Include/RealTimeDatabase.h"
#define REAL_TIME_DATABASE_LIBPATH "..\\..\\..\\..\\Libs\\Windows\\RealTimeDatabase.dll"
#else
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <RealTimeDatabase.h>
#define REAL_TIME_DATABASE_LIBPATH  "../../../Libs/Linux/RealTimeDatabase.so"
#endif //WIN32

#define FIELD_COUNT 80
#define RECORD_COUNT 10000*10

struct RealTimeDatabaseRecord
{
	int field[FIELD_COUNT + 1];
};

std::string generate_createsql(int n)
{
	std::string sql = "CREATE TABLE t(x INTEGER,";
	char buf[64];
	for ( int i = 0 ; i < n ; i++)
	{
		sprintf(buf, "f%d INTEGER,", i+1);
		sql = sql + buf;
	}
	sql = sql + "PRIMARY KEY(x ASC));";
	printf("%s\n", sql.c_str());
	return sql;
}

std::string generate_insertsql(int n)
{
	std::string sql = "INSERT INTO t(x,";
	char buf[64];
	for ( int i = 0 ; i < n ; i++)
	{
		sprintf(buf, "f%d", i+1);
		sql = sql + buf;
		sql = (i == n-1) ? sql + ") VALUES(?," : sql + ",";
	}
	for ( int i = 0 ; i < n ; i++)
	{
		sql = (i == n-1) ? sql + "?)" : sql + "?,";
	}
	printf("%s\n", sql.c_str());
	return sql;
}

bool query_callback_test(const sql_handle handle, long record, int column_count, void *parameter)
{
	struct RealTimeDB *rtdb = (struct RealTimeDB *)parameter;

	if( IS_COLUMN_HEADER(record) )
	{
		for(int i = 0 ; i < column_count; i++)
		{
			const char* val= rtdb->column_name(handle, i);
			printf("Real Time Database Column Head : %d, %s\n", i, val);
		}
	}
	else
	{
		for(int i = 0 ; i < column_count; i++)
		{
			int val= rtdb->column_int(handle, i);
			printf("Real Time Database Record : %d, %d\n", i, val);
		}
	}

	return true;
}

RealTimeBuffer generate_buffer(struct RealTimeDB *rtdb)
{
	// Alloc Real Time Buffer
	struct RealTimeField fields[FIELD_COUNT + 1];
	memset(fields, 0, sizeof(fields));
	for(int i = 0 ; i < FIELD_COUNT + 1 ; i ++)
	{
		fields[i].type = RTDB_INT32;
	}
	RealTimeBuffer buffer = rtdb->alloc(fields, sizeof(fields), RECORD_COUNT);

	//Generate Test Data and save in Real Time Buffer
	RealTimeDatabaseRecord record;
	memset(&record, 0, sizeof(record));
	for(int x = 0 ; x < RECORD_COUNT ; x ++)
	{
		record.field[0] = x + 1;
		for(int y = 1 ; y < FIELD_COUNT + 1 ; y ++)
		{
			record.field[y] = 31000000 + 123456 + (x+1) * 111 + y * 111;
		}
		rtdb->add_record(buffer, &record, sizeof(RealTimeDatabaseRecord));
	}

	return buffer;
}

bool RealTimeDatabase_Test(struct RealTimeDB *rtdb)
{
	if(rtdb == NULL)
	{
		return false;
	}

	//Open Real Time Database
	db_handle db;
	int rc = rtdb->open("RealTimeDatabase_Test.rdb", &db);
	if(rc == RTDB_OK)
	{
		//CREATE TABLE
		std::string sql = generate_createsql(FIELD_COUNT);
		if(rtdb->exec(db, sql.c_str()))
		{
			printf("success create table\r\n\n");
		}

		//Alloc Real Time Database Buffer for Insert
		RealTimeBuffer rtdb_buffer = generate_buffer(rtdb);

		//INSERT DATA INTO Real Time Database
		std::string sql2 = generate_insertsql(FIELD_COUNT);
		time_t start3 = time(NULL);
		long count = rtdb->insert_v2(db, sql2.c_str(), rtdb_buffer);
		time_t stop3 = time(NULL);
		printf("[Insert]Start at : %llu, Stop at : %llu, Elapsed : %u seconds\n",   start3, stop3, stop3-start3);
		printf("success insert : %ld records into Real Time Database\r\n\n", count);

		//Free Real Time Database Buffer
		rtdb->free(&rtdb_buffer);

		//Query DATA From Real Time Database
		start3 = time(NULL);
		long record_count = rtdb->query(db, "SELECT count(*) FROM t", query_callback_test, rtdb);
		stop3 = time(NULL);
		printf("[Query]Start at : %llu, Stop at : %llu, Elapsed : %u seconds\n",   start3, stop3, stop3-start3);
		printf("success query : %ld records from Real Time Database\r\n\n", record_count);

		// DROP TABLE
		if(rtdb->exec(db, "DROP TABLE t"))
		{
			printf("success drop table\r\n");
		}

		//Close Real Time Database
		rtdb->close(db);
	}
	return true;		
}

void RealTimeDatabase_Initialize()
{
	lib_handle handle = LoadLib(REAL_TIME_DATABASE_LIBPATH);
	if(handle == 0)
	{	
		printf("Load failed.\n");
		return;
	}
	 _RealTimeDB_Initialize RealTimeDB_Initialize = (_RealTimeDB_Initialize)GetAPI(handle, "Initialize");
	if(RealTimeDB_Initialize == 0)
	{
		printf("Get API error\n");
		return;
	}

 	struct RealTimeDB rtdb;
	if(RealTimeDB_Initialize(&rtdb))
	{
		RealTimeDatabase_Test(&rtdb);
	}

	FreeLib(handle);
}

int main(int argc, char* argv[])
{
	printf("Begin Real Time Database test\r\n\n");

	RealTimeDatabase_Initialize();

	printf("\nPress Any key to Exit.");

	char c;
	scanf("%c", &c);
	return 0;
}



