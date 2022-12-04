/************************************************************************************************/
/* Copyright (C), 2016-2017, Allwinner Tech. Co., Ltd. */
/************************************************************************************************/
/**
 * @file common.h
 * @brief
 为其它模块提供公共服务,如网络服务,时间服务,配置文件操作等,目前由于功能不多,暂时共于一个文件,
          当功能逐渐增多时,可以合并拆分成几个不同的目录结构.
 * @author id: wangguixing
 * @version v0.1
 * @date 2017-04-14
 * @version v0.2
 * @date 2017-05-8
 * @brief add osd create rgb function. wangguixing
 */

#ifndef _COMMON_H_
#define _COMMON_H_

extern "C"{
#include "libavutil/log.h"
}


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
        printf("|------------> %s->%s (line:%d)]  " fmt, __FILE__,             \
               __FUNCTION__, __LINE__, ##args);                                \
    } while (0)
#else
#define DB_PRT(fmt, args...)                                                   \
    do {                                                                       \
    } while (0)
#endif

#if ERR_DEBUG
#define ERR_PRT(fmt, args...)                                                  \
    do {                                                                       \
        printf("|------------> \033[0;32;31m ERROR! [%s->%s (line:%d)  " fmt   \
               "\033[0m",                                                      \
               __FILE__, __FUNCTION__, __LINE__, ##args);                      \
    } while (0)
#else
#define ERR_PRT(fmt, args...)                                                  \
    do {                                                                       \
    } while (0)
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif


#define UC_LOG(fmt, args...)                                                   \
    do {                                                                       \
        printf("%s",fmt, ##args);            \
    } while (0)


// int avError(int errNum)
// {
// 	char buf[1024];
// 	//获取错误信息
// 	av_strerror(errNum, buf, sizeof(buf));
//     DB_PRT("failed! %s", buf);
// 	return -1;
// }


//获取错误信息
#define avError(errNum) \
        char buf[1024]; \
        av_strerror(errNum, buf, sizeof(buf)); \
        DB_PRT("failed! %s", buf); \


#endif /* _COMMON_H_ */
