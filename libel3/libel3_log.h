#ifndef __LIBEL3_LOG_H__
#define __LIBEL3_LOG_H__

#define ErrPrintf(format, ...) fprintf(stdout, format, ##__VA_ARGS__)
#define DbgPrintf(format, ...) fprintf(stdout, format, ##__VA_ARGS__)
#define LogData(data, size)

#else

#endif
