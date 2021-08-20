
#ifndef PHP_SNOWFLAKE_H
#define PHP_SNOWFLAKE_H

extern zend_module_entry snowflake_module_entry;
#define phpext_snowflake_ptr &snowflake_module_entry

#define PHP_SNOWFLAKE_VERSION "1.0.0" /* Replace with version number for your extension */

#ifdef PHP_WIN32
#	define PHP_SNOWFLAKE_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_SNOWFLAKE_API __attribute__ ((visibility("default")))
#else
#	define PHP_SNOWFLAKE_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

// 全局变量
ZEND_BEGIN_MODULE_GLOBALS(snowflake)
	int  data_center_id_bits;
	int  worker_id_bits;
	int  sequence_bits;
	long start_timestamp;
	long *last_timestamp_ce;
	int *sequence_ce;
	int *run_lock;
ZEND_END_MODULE_GLOBALS(snowflake)


/* Always refer to the globals in your function as SNOWFLAKE_G(variable).
   You are encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/
#define SF_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(snowflake, v)

#if defined(ZTS) && defined(COMPILE_DL_SNOWFLAKE)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

#endif	/* PHP_SNOWFLAKE_H */

