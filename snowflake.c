
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_snowflake.h"

#include "unistd.h"

ZEND_DECLARE_MODULE_GLOBALS(snowflake)

/* True global resources - no need for thread safety here */
// static int le_snowflake;

/* 获取毫秒时间戳
 */
long sf_get_timestamp()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

/* {{{ PHP_INI
 */
PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("snowflake.data_center_id_bits", "5", PHP_INI_SYSTEM, OnUpdateLong, data_center_id_bits, zend_snowflake_globals,snowflake_globals)
	STD_PHP_INI_ENTRY("snowflake.worker_id_bits", "5", PHP_INI_SYSTEM, OnUpdateLong, worker_id_bits, zend_snowflake_globals,snowflake_globals)
	STD_PHP_INI_ENTRY("snowflake.sequence_bits", "12", PHP_INI_SYSTEM, OnUpdateLong, sequence_bits, zend_snowflake_globals,snowflake_globals)
	STD_PHP_INI_ENTRY("snowflake.start_timestamp", "0", PHP_INI_SYSTEM, OnUpdateLong, start_timestamp, zend_snowflake_globals,snowflake_globals)
PHP_INI_END()
/* }}} */


/* {{{ php_snowflake_init_globals
 */
static void php_snowflake_init_globals(zend_snowflake_globals *snowflake_globals)
{
	snowflake_globals->data_center_id_bits = 5;
	snowflake_globals->worker_id_bits = 5;
	snowflake_globals->sequence_bits = 12;
	snowflake_globals->start_timestamp = 0;
}
/* }}} */

#include "ext-src/snowflake.c"

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(snowflake)
{
	REGISTER_INI_ENTRIES();

	SF_G(last_timestamp_ce) = (long *)pemalloc(sizeof(long), 1);
	*SF_G(last_timestamp_ce) = 0;
	SF_G(sequence_ce) = (int *)pemalloc(sizeof(int), 1);
	*SF_G(sequence_ce) = 0;
	SF_G(run_lock) = (int *)pemalloc(sizeof(1), 1);
	*SF_G(run_lock) = 0;

	snowflake_minit(
		SF_G(data_center_id_bits),
		SF_G(worker_id_bits),
		SF_G(sequence_bits),
		SF_G(start_timestamp)
		);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(snowflake)
{
	pefree(SF_G(last_timestamp_ce), 1);
	SF_G(last_timestamp_ce) = NULL;
	pefree(SF_G(sequence_ce), 1);
	SF_G(sequence_ce) = NULL;
	pefree(SF_G(run_lock), 1);
	SF_G(run_lock) = NULL;

	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(snowflake)
{
#if defined(COMPILE_DL_SNOWFLAKE) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(snowflake)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(snowflake)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "snowflake", "enabled");
	php_info_print_table_row(2, "Version", PHP_SNOWFLAKE_VERSION);
	php_info_print_table_row(2, "Author", "LifetimeNine <2390904403@qq.com>");
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */

PHP_FUNCTION(snowflake_test)
{
	zval value;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l",&value) == FAILURE){
			RETURN_NULL();
	}
	RETURN_LONG(Z_LVAL_P(&value));
}
ZEND_BEGIN_ARG_INFO_EX(arg_snowflake_test, 0, 0, 1)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()
/* {{{ snowflake_functions[]
 *
 * Every user visible function must have an entry in snowflake_functions[].
 */
const zend_function_entry snowflake_functions[] = {
	PHP_FE(snowflake_test, NULL)
	PHP_FE_END	/* Must be the last line in snowflake_functions[] */
};
/* }}} */

/* {{{ snowflake_module_entry
 */
zend_module_entry snowflake_module_entry = {
	STANDARD_MODULE_HEADER,
	"snowflake",
	snowflake_functions,
	PHP_MINIT(snowflake),
	PHP_MSHUTDOWN(snowflake),
	PHP_RINIT(snowflake),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(snowflake),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(snowflake),
	PHP_SNOWFLAKE_VERSION,
	PHP_MODULE_GLOBALS(snowflake),
	NULL,
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

#ifdef COMPILE_DL_SNOWFLAKE
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(snowflake)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
