
static PHP_METHOD(snowflake, __construct);
static PHP_METHOD(snowflake, __destruct);
static PHP_METHOD(snowflake, __debugInfo);
static PHP_METHOD(snowflake, getDataCenterIdBits);
static PHP_METHOD(snowflake, getWorkerIdBits);
static PHP_METHOD(snowflake, getSequenceBits);
static PHP_METHOD(snowflake, getStartTimestamp);
static PHP_METHOD(snowflake, getDataCenterId);
static PHP_METHOD(snowflake, getWorkerId);
static PHP_METHOD(snowflake, onCreateId);
static PHP_METHOD(snowflake, onMaxSequence);
static PHP_METHOD(snowflake, onTimeBackDial);
static PHP_METHOD(snowflake, getId);

void snowflake_minit(int data_center_id_bits, int worker_id_bits, int sequence_bits, long start_timestamp);