## 雪花算法 PHP扩展

### 编译
1. 进入扩展目录进行编译
```
{your phpize}
./configure --enable-maintainer-zts
make && make install
```
2. 在配置中加入扩展
```
[snowflake]
extension = {your snowflake.so path}
```
3. 重启PHP服务

### 简单示例
```php
<?php

$snowfalke = new Snowflake();
$id = $snowflake->getId();
>
```

### 方法

#### `__construct` 构造函数

* 参数
  - dataCenterId  
  数据中心标识(int), 默认值 0。
  - workerId  
  机器标识(int), 默认值 0。

* 返回值  
Snowflake 实例

* 示例  
```php
<?php
new Snowflake(1, 1);
>
```
* 注意  
  最大值计算方法为 2的n次方，n为位数；如果设置的值超过最大值，会抛出异常。
#### `getId` 获取ID

* 参数  
  无
* 返回值  
  ID(int)

* 示例  
```php
<?php
$sf = new Snowflake(1, 1);
$sf->getId();
>
```
#### `getDataCenterIdBits` 获取数据中心标识的位数（值来自于配置）

* 参数  
  无
* 返回值  
  数据中心标识的位数(int)

* 示例  
```php
<?php
$sf = new Snowflake(1, 1);
$sf->getDataCenterIdBits();
>
```

#### `getWorkerIdBits` 获取机器标识的位数（值来自于配置）

* 参数  
  无
* 返回值  
  机器标识的位数(int)

* 示例  
```php
<?php
$sf = new Snowflake(1, 1);
$sf->getWorkerIdBits();
>
```

#### `getSequenceBits` 获取序列号的位数（值来自于配置）

* 参数  
  无
* 返回值  
  序列号的位数(int)

* 示例  
```php
<?php
$sf = new Snowflake(1, 1);
$sf->getSequenceBits();
>
```

#### `getStartTimestamp` 获取系统开始运行的毫秒时间戳（值来自于配置）

* 参数  
  无
* 返回值  
  系统开始运行的毫秒时间戳(int)

* 示例  
```php
<?php
$sf = new Snowflake(1, 1);
$sf->getStartTimestamp();
>
```

#### `getDataCenterId` 获取当前数据中心标识

* 参数  
  无
* 返回值  
  当前数据中心标识(int)

* 示例  
```php
<?php
$sf = new Snowflake(1, 1);
$sf->getDataCenterId();
>
```

#### `getWorkerId` 获取当前机器标识

* 参数  
  无
* 返回值  
  当前机器标识(int)

* 示例  
```php
<?php
$sf = new Snowflake(1, 1);
$sf->getWorkerId();
>
```

#### `onCreateId` 注册创建ID时的回调
* 参数  
  回调函数
* 返回值  
  true
* 示例  
```php
<?php
$sf = new Snowflake(1, 1);
$sf->onCreateId(function($id) {
  echo "最新创建的ID是: {$id}\n";
});
echo "id:" . $sf->getId();
>
```
* 示例输出
```
最新创建的ID是: 52142313378048
id: 52142313378048
```
* 注意  
  此回调函数是在生成ID之后，返回结果之间执行，如果回调函数中发生异常，`getId`将直接抛出，序列号仍然加1。

#### `onMaxSequence` 注册当前毫秒内序列号到达最大值时的回调
* 参数  
  回调函数
* 返回值  
  true
* 示例  
```php
<?php
$sf = new Snowflake(1, 1);
$sf->onMaxSequence(function($sequence) {
  echo "当前的序列号是: {$sequence}\n";
});
echo "id:" . $sf->getId();
>
```
* 注意  
  当毫秒内序列号到达最大值时，会阻塞程序，直到下一毫秒再重新获取ID；可根据此回调函数触发的频率适当调整序列号位数
#### `onTimeBackDial` 注册发生时间回拨时的回调
* 参数  
  回调函数
* 返回值  
  true
* 示例  
```php
<?php
$sf = new Snowflake(1, 1);
$sf->onTimeBackDial(function($lastTimestamp) {
  echo "最后生成ID的时间戳是: {$lastTimestamp}\n";
});
echo "id:" . $sf->getId();
>
```
* 注意  
  当时间发生回拨时，会阻塞程序，直到时间追上上次生成ID的时间。
  判断依据为最后生成ID的时间。

### 配置参数

* `snowflake.data_center_id_bits` 数据中心标识位数(int)
* `snowflake.worker_id_bits` 机器标识位数(int)
* `snowflake.sequence_bits` 序列号标识位数(int)
* `snowflake.start_timestamp` 系统开始运行的毫秒时间戳(int)