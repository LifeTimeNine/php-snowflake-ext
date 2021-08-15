# 获取当前目录
ext_path=$(pwd)
# 编译目录
compile_path="/php-ext"
# phpize 目录
phpize_path="/www/server/php/71/bin/phpize"
# php-config 目录
phpconfig_path="/www/server/php/71/bin/php-config"

# 拷贝文件到编译目录
cp -rf $ext_path $compile_path
# 切换到编译目录
cd $compile_path
# 编译
$phpize_path
./configure --with-php-config=$phpconfig_path
make && make install
# PHP服务重载
/etc/init.d/php-fpm-71 reload
# 切换至当前目录
cd $ext_path
# 删除编译目录
rm -rf $compile_path

# 运行PHP
echo ""
echo ""
echo "-----------------------------------------------------"
php snowflake.php
echo "-----------------------------------------------------"