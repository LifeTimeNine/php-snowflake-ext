
static PHP_METHOD(snowflake_class, __construct);
static PHP_METHOD(snowflake_class, __destruct);
static PHP_METHOD(snowflake_class, getId);

void snowflake_minit(int data_center_id_bits, int worker_id_bits, int sequence_bits, long start_timestamp);