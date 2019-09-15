Nao PinTool
====

Based on pin-3.11-97998-g7ecce2dac-gcc-linux


How to build
----
```shell
cd source/tools
git clone THIS_REPOSITORY
cd NaoPinTool
make
```


How to use
-----
```shell
../../../pin -t obj-intel64/branchtrace.so -- /bin/echo test
cat branchtrace.out
```