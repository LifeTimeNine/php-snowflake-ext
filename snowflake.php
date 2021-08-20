<?php
// $snowflake = new \Snowflake();
// var_dump($snowflake->getDataCenterIdBits());
// var_dump($snowflake->getWorkerIdBits());
// var_dump($snowflake->getDataCenterId());
// var_dump($snowflake->getWorkerId());
// var_dump($snowflake->getStartTimestamp());
// var_dump($snowflake->getId());

function dump($data)
{
    var_dump($data) . "\n";
}

$snowflake = new \Snowflake();
dump($snowflake);
$snowflake->onMaxSequence(function ($sequence) {
    dump($sequence);
});
$snowflake->onCreateId(function ($id) {
    dump($id);
});
for ($i = 0; $i < 1; $i++) {
    ($snowflake->getId());
}

dump("success");
?>
