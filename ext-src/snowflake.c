
#include "snowflake.h"

zend_class_entry *class_snowflake_ce;

static PHP_METHOD(snowflake, __construct) {
	zend_long dataCenterId = 0;
	zend_long workerId = 0;
	zval *dataCenterIdBits, *workerIdBits;
	zval *rv; // php >= 7.0
	zend_class_entry *ce;
	zval *this = getThis();
	ce = Z_OBJCE_P(this);

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|ll", &dataCenterId, &workerId) == FAILURE) {
		RETURN_FALSE;
	}
#else
	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 0, 2)
    Z_PARAM_OPTIONAL
    Z_PARAM_LONG(dataCenterId)
		Z_PARAM_LONG(workerId)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
#endif

	dataCenterIdBits = zend_read_property(ce, this, "dataCenterIdBits", sizeof("dataCenterIdBits")-1, 1, rv TSRMLS_DC);
	if (dataCenterId > (-1 ^ (-1 << Z_LVAL_P(dataCenterIdBits)))) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "data center ID exceeds the maximum value");
	}
	workerIdBits = zend_read_property(ce, this, "workerIdBits", sizeof("workerIdBits")-1, 1, rv TSRMLS_DC);
	if (workerId > (-1 ^ (-1 << Z_LVAL_P(workerIdBits)))) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "worker ID exceeds the maximum value");
	}
	zend_update_property_long(ce, this, "dataCenterId", sizeof("dataCenterId")-1, dataCenterId TSRMLS_CC);
	zend_update_property_long(ce, this, "workerId", sizeof("workerId")-1, workerId TSRMLS_CC);
}

static PHP_METHOD(snowflake, __destruct) {}

static PHP_METHOD(snowflake, getDataCenterIdBits)
{
	zval *dataCenterIdBits;
	zval *rv; // php >= 7.0
	zend_class_entry *ce;
	zval *this = getThis();
	ce = Z_OBJCE_P(this);
	dataCenterIdBits = zend_read_property(ce, this, "dataCenterIdBits", sizeof("dataCenterIdBits")-1, 1, rv TSRMLS_DC);
	RETURN_LONG(Z_LVAL_P(dataCenterIdBits));
}

static PHP_METHOD(snowflake, getWorkerIdBits)
{
	zval *workerIdBits;
	zval *rv; // php >= 7.0
	zend_class_entry *ce;
	zval *this = getThis();
	ce = Z_OBJCE_P(this);
	workerIdBits = zend_read_property(ce, this, "workerIdBits", sizeof("workerIdBits")-1, 1, rv TSRMLS_DC);
	RETURN_LONG(Z_LVAL_P(workerIdBits));
}

static PHP_METHOD(snowflake, getSequenceBits)
{
	zval *sequenceBits;
	zval *rv; // php >= 7.0
	zend_class_entry *ce;
	zval *this = getThis();
	ce = Z_OBJCE_P(this);
	sequenceBits = zend_read_property(ce, this, "sequenceBits", sizeof("sequenceBits")-1, 1, rv TSRMLS_DC);
	RETURN_LONG(Z_LVAL_P(sequenceBits));
}

static PHP_METHOD(snowflake, getStartTimestamp)
{
	zval *startTimestamp;
	zval *rv; // php >= 7.0
	zend_class_entry *ce;
	zval *this = getThis();
	ce = Z_OBJCE_P(this);
	startTimestamp = zend_read_property(ce, this, "startTimestamp", sizeof("startTimestamp")-1, 1, rv TSRMLS_DC);
	RETURN_LONG(Z_LVAL_P(startTimestamp));
}

static PHP_METHOD(snowflake, getDataCenterId)
{
	zval *dataCenterId;
	zval *rv; // php >= 7.0
	zend_class_entry *ce;
	zval *this = getThis();
	ce = Z_OBJCE_P(this);
	dataCenterId = zend_read_property(ce, this, "dataCenterId", sizeof("dataCenterId")-1, 1, rv TSRMLS_DC);
	RETURN_LONG(Z_LVAL_P(dataCenterId));
}

static PHP_METHOD(snowflake, getWorkerId)
{
	zval *workerId;
	zval *rv; // php >= 7.0
	zend_class_entry *ce;
	zval *this = getThis();
	ce = Z_OBJCE_P(this);
	workerId = zend_read_property(ce, this, "workerId", sizeof("workerId")-1, 1, rv TSRMLS_DC);
	RETURN_LONG(Z_LVAL_P(workerId));
}

static PHP_METHOD(snowflake, getId) {
	while(*SF_G(run_lock) == 1){}
	*SF_G(run_lock) = 1;
	zval *dataCenterIdBits, *workerIdBits, *sequenceBits, *startTimestamp, *dataCenterId, *workerId;
	zval *createIdCallable, *maxSequceCallable, *timeBackDialCallable;
	zval retval;
	zend_fcall_info fci;
	zend_fcall_info_cache fci_cache;
	zval *rv; // php >= 7.0
	zend_class_entry *ce;

	zval *this = getThis();
	ce = Z_OBJCE_P(this);

	long timestamp = sf_get_timestamp();

	// 如果发生时间回拨
	if (timestamp < *SF_G(last_timestamp_ce)) {
		timeBackDialCallable = zend_read_property(ce, this, "timeBackDialCallable", sizeof("timeBackDialCallable")-1, 1, rv TSRMLS_DC);
		if (Z_TYPE_P(timeBackDialCallable) != IS_NULL) {
			zval timeBackDialCallableParam;
			ZVAL_LONG(&timeBackDialCallableParam, *SF_G(last_timestamp_ce));
			zend_fcall_info_init(timeBackDialCallable, 0, &fci, &fci_cache, NULL, NULL);
			zend_fcall_info_argn(&fci, 1, &timeBackDialCallableParam);
			zend_fcall_info_call(&fci, &fci_cache, &retval, NULL);
			zval_ptr_dtor(&timeBackDialCallableParam);
			zval_ptr_dtor(&retval);
		}

		while(timestamp < *SF_G(last_timestamp_ce)) {
			timestamp = sf_get_timestamp();
		}
	}

	sequenceBits = zend_read_property(ce, this, "sequenceBits", sizeof("sequenceBits")-1, 1, rv TSRMLS_DC);

	if (timestamp == *SF_G(last_timestamp_ce)) {

		// 如果当前毫秒的序列号用完
		if (*SF_G(sequence_ce) >= (-1 ^ (-1 << Z_LVAL_P(sequenceBits)))) {
			maxSequceCallable = zend_read_property(ce, this, "maxSequceCallable", sizeof("maxSequceCallable")-1, 1, rv TSRMLS_DC);
			if (Z_TYPE_P(maxSequceCallable) != IS_NULL) {
				zval maxSequceCallableParam;
				ZVAL_LONG(&maxSequceCallableParam, *SF_G(sequence_ce));
				zend_fcall_info_init(maxSequceCallable, 0, &fci, &fci_cache, NULL, NULL);
				zend_fcall_info_argn(&fci, 1, &maxSequceCallableParam);
				zend_fcall_info_call(&fci, &fci_cache, &retval, NULL);
				zval_ptr_dtor(&maxSequceCallableParam);
				zval_ptr_dtor(&retval);
			}

			while(timestamp > *SF_G(last_timestamp_ce)) {
				timestamp = sf_get_timestamp();
			}

			*SF_G(sequence_ce) = 0;
		} else {
			*SF_G(sequence_ce) += 1;
		}
	} else {
		*SF_G(sequence_ce) = 0;
	}

	*SF_G(last_timestamp_ce) = timestamp;
	int sequence = *SF_G(sequence_ce);
	*SF_G(run_lock) = 0;

	dataCenterIdBits = zend_read_property(ce, this, "dataCenterIdBits", sizeof("dataCenterIdBits")-1, 1, rv TSRMLS_DC);
	workerIdBits = zend_read_property(ce, this, "workerIdBits", sizeof("workerIdBits")-1, 1, rv TSRMLS_DC);
	dataCenterId = zend_read_property(ce, this, "dataCenterId", sizeof("dataCenterId")-1, 1, rv TSRMLS_DC);
	workerId = zend_read_property(ce, this, "workerId", sizeof("workerId")-1, 1, rv TSRMLS_DC);
	startTimestamp = zend_read_property(ce, this, "startTimestamp", sizeof("startTimestamp")-1, 1, rv TSRMLS_DC);
	
	long res = (
		((timestamp - Z_LVAL_P(startTimestamp)) << (Z_LVAL_P(dataCenterIdBits) + Z_LVAL_P(workerIdBits) + Z_LVAL_P(sequenceBits))) |
		(Z_LVAL_P(dataCenterId) << (Z_LVAL_P(workerIdBits) + Z_LVAL_P(sequenceBits))) |
		(Z_LVAL_P(workerId) << Z_LVAL_P(sequenceBits)) |
		sequence
	);

	createIdCallable = zend_read_property(ce, this, "createIdCallable", sizeof("createIdCallable")-1, 1, rv TSRMLS_DC);

	if (Z_TYPE_P(createIdCallable) != IS_NULL) {
		zval createIdCallableParam;
		ZVAL_LONG(&createIdCallableParam, res);
		zend_fcall_info_init(createIdCallable, 0, &fci, &fci_cache, NULL, NULL);
		zend_fcall_info_argn(&fci, 1, &createIdCallableParam);
		zend_fcall_info_call(&fci, &fci_cache, &retval, NULL);
		zval_ptr_dtor(&createIdCallableParam);
		zval_ptr_dtor(&retval);
	}
	
	RETURN_LONG(res);
}

static PHP_METHOD(snowflake, onCreateId)
{
	zval *callable;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", callable) == FAILURE) {
		RETURN_FALSE;
	}
#else
	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 1, 1)
    Z_PARAM_ZVAL(callable)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
#endif

	zend_class_entry *ce;
	zval *this = getThis();
	ce = Z_OBJCE_P(this);

	zend_update_property(ce, this, "createIdCallable", sizeof("createIdCallable")-1, callable TSRMLS_CC);

	RETURN_TRUE;
}

static PHP_METHOD(snowflake, onMaxSequence)
{
	zval *callable;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", callable) == FAILURE) {
		RETURN_FALSE;
	}
#else
	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 1, 1)
    Z_PARAM_ZVAL(callable)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
#endif

	zend_class_entry *ce;
	zval *this = getThis();
	ce = Z_OBJCE_P(this);

	zend_update_property(ce, this, "maxSequceCallable", sizeof("maxSequceCallable")-1, callable TSRMLS_CC);

	RETURN_TRUE;
}

static PHP_METHOD(snowflake, onTimeBackDial)
{
	zval *callable;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", callable) == FAILURE) {
		RETURN_FALSE;
	}
#else
	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 1, 1)
    Z_PARAM_ZVAL(callable)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
#endif

	zend_class_entry *ce;
	zval *this = getThis();
	ce = Z_OBJCE_P(this);

	zend_update_property(ce, this, "timeBackDialCallable", sizeof("timeBackDialCallable")-1, callable TSRMLS_CC);

	RETURN_TRUE;
}

static PHP_METHOD(snowflake, __debugInfo)
{
	zend_class_entry *ce;
	zval *this = getThis();
	ce = Z_OBJCE_P(this);
	zval *rv;
	zval *dataCenterId, *workerId;
	dataCenterId = zend_read_property(ce, this, "dataCenterId", sizeof("dataCenterId")-1, 1, rv TSRMLS_DC);
	workerId = zend_read_property(ce, this, "workerId", sizeof("workerId")-1, 1, rv TSRMLS_DC);
	array_init(return_value);
	add_assoc_zval(return_value, "dataCenterId", dataCenterId);
	add_assoc_zval(return_value, "workerId", workerId);
	return;
}

ZEND_BEGIN_ARG_INFO_EX(arg_void, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arg_snowflake_construct, 0, 0, 0)
	ZEND_ARG_INFO(0, dataCenterId)
	ZEND_ARG_INFO(0, workerId)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arg_snowfalke_on, 0, 0, 1)
    ZEND_ARG_CALLABLE_INFO(0, callback, 0)
ZEND_END_ARG_INFO()

static zend_function_entry snowflake_method[] = {
	PHP_ME(snowflake, __construct, arg_snowflake_construct, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(snowflake, __destruct, arg_void, ZEND_ACC_PUBLIC|ZEND_ACC_DTOR)
	PHP_ME(snowflake, getDataCenterIdBits, arg_void, ZEND_ACC_PUBLIC)
	PHP_ME(snowflake, getWorkerIdBits, arg_void, ZEND_ACC_PUBLIC)
	PHP_ME(snowflake, getStartTimestamp, arg_void, ZEND_ACC_PUBLIC)
	PHP_ME(snowflake, getDataCenterId, arg_void, ZEND_ACC_PUBLIC)
	PHP_ME(snowflake, getWorkerId, arg_void, ZEND_ACC_PUBLIC)
	PHP_ME(snowflake, getId, arg_void, ZEND_ACC_PUBLIC)
	PHP_ME(snowflake, onCreateId, arg_snowfalke_on, ZEND_ACC_PUBLIC)
	PHP_ME(snowflake, onMaxSequence, arg_snowfalke_on, ZEND_ACC_PUBLIC)
	PHP_ME(snowflake, onTimeBackDial, arg_snowfalke_on, ZEND_ACC_PUBLIC)
	PHP_ME(snowflake, __debugInfo, arg_void, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

void snowflake_minit(int data_center_id_bits, int worker_id_bits, int sequence_bits, long start_timestamp) {
	// 定义类
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "Snowflake", snowflake_method);
	class_snowflake_ce = zend_register_internal_class(&ce TSRMLS_CC);
	// 定义类属性
	zend_declare_property_long(class_snowflake_ce, "dataCenterIdBits", strlen("dataCenterIdBits"), data_center_id_bits, ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_long(class_snowflake_ce, "workerIdBits", strlen("workerIdBits"), worker_id_bits, ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_long(class_snowflake_ce, "sequenceBits", strlen("sequenceBits"), sequence_bits, ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_long(class_snowflake_ce, "startTimestamp", strlen("startTimestamp"), start_timestamp, ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_long(class_snowflake_ce, "dataCenterId", strlen("dataCenterId"), 0, ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_long(class_snowflake_ce, "workerId", strlen("workerId"), 0, ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(class_snowflake_ce, "createIdCallable", strlen("createIdCallable"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(class_snowflake_ce, "maxSequceCallable", strlen("maxSequceCallable"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(class_snowflake_ce, "timeBackDialCallable", strlen("timeBackDialCallable"), ZEND_ACC_PROTECTED TSRMLS_CC);
}