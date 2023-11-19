+ Need -std=c++11 to compile.
+ Need to set the rpath.
```
install_name_tool -change @rpath/libqpdf.29.dylib /usr/local/lib/libqpdf.29.dylib src/Rqpdf.so
```
