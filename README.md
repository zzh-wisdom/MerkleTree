# MerkleTree

This is the code for the final assignment of the blockchain course, which implements the Merkle Tree and is optimized in conjunction with the Bloom filter. In addition, it also contains some tests

## mac

找不到头文件 `openssl/sha.h`

```shell
openssl version # 查看是否安装openssl
brew upgrade openssl # 更新openssl
brew install openssl  # 安装openssl
brew link openssl --force
```

运行最后一条命令得到以下结果：

```shell
brew link openssl --force
Warning: Refusing to link macOS provided/shadowed software: openssl@1.1
If you need to have openssl@1.1 first in your PATH run:
  echo 'export PATH="/usr/local/opt/openssl@1.1/bin:$PATH"' >> ~/.zshrc

For compilers to find openssl@1.1 you may need to set:
  export LDFLAGS="-L/usr/local/opt/openssl@1.1/lib"
  export CPPFLAGS="-I/usr/local/opt/openssl@1.1/include"

For pkg-config to find openssl@1.1 you may need to set:
  export PKG_CONFIG_PATH="/usr/local/opt/openssl@1.1/lib/pkgconfig"
```

只要添加其中的环境变量 `LDFLAGS` 和 `CPPFLAGS`，或者用g++编译器时添加对应的选项。

但在vscode IDE中扔提示该头文件搜索不到，需要配置IntelliSense。

扩展程序会根据当前系统环境配置基本信息，因此有可能配置不完整，这时需要通过生成c_cpp_properties.json文件来配置缺少的信息：

ctrl+shift+P打开Command Palette,运行C/Cpp: Edit configurations...生成c_cpp_properties.json：

在 `includePath` 中添加对应的路径 `/usr/local/opt/openssl@1.1/include`

```json
            "includePath": [
                "${workspaceFolder}/**",
                "/usr/local/opt/openssl@1.1/include/"
            ],
```

## build

```shell
g++ -I/usr/local/opt/openssl@1.1/include -L/usr/local/opt/openssl@1.1/lib -o main main.cpp ../util/hash.cpp ../util/coding.cpp -lssl -lcrypto
```

```shell
g++ -I/usr/local/opt/openssl@1.1/include -L/usr/local/opt/openssl@1.1/lib -o unit_test unit_test.cpp ../src/merkletree.cpp ../util/hash.cpp ../util/coding.cpp -lssl -lcrypto -g

sudo gdb ./unit_test
```
