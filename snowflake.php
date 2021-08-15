<?php
$snowflake = new Snowflake(1,1);
var_dump($snowflake->getDataCenterIdBits());
var_dump($snowflake->getWorkerIdBits());
var_dump($snowflake->getDataCenterId());
var_dump($snowflake->getWorkerId());
var_dump($snowflake->getStartTimestamp());
var_dump($snowflake->getId());
?>
