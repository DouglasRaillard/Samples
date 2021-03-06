#include <stdio.h>
#include "_LoggingFieldNamesEnum.h"

#define DUMP_PERIOD_MICRO 1E6
#define FIELDS_PRECISION_DECIMAL 5
#define FIELDS_PRECISION_INTEGRAL 3
#define SEPARATOR_STRING ","
typedef double FIELD_TYPE;

int dumpLog();
// Recommended mode to overwrite the log each time the file is opened: "wb"
int initLogging(const char *path, const char *mode);
int shutdownLogging();
int addValueForNextLogEntry(FIELD_NAME name, FIELD_TYPE field);
char *getLogFields();
