#ifndef COMMON_H
#define COMMON_H



/************************************************************************************************/
/*                                      Include Files */
/************************************************************************************************/
/* None */

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/************************************************************************************************/
/*                                     Macros & Typedefs */
/************************************************************************************************/
#define MAX_COLOR_SIZE 8

#define INFO_DEBUG 1
#define ERR_DEBUG 1

#if INFO_DEBUG
#define DB_PRT(fmt, args...)                                                   \
        do {                                                                       \
            printf("|--> %s->%s (line:%d)]  " fmt, __FILE__,             \
                   __FUNCTION__, __LINE__, ##args);                                \
        } while (0)
#endif

#if ERR_DEBUG
#define ERR_PRT(fmt, args...)                                                  \
    do {                                                                       \
        printf("|--> \033[0;32;31m ERROR! [%s->%s (line:%d)  " fmt   \
               "\033[0m",                                                      \
               __FILE__, __FUNCTION__, __LINE__, ##args);                      \
    } while (0)
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif


#endif // COMMON_H
