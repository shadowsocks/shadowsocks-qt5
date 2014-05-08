Shadowsocks-Qt5
===============

[中文用户指南](https://github.com/librehat/shadowsocks-qt5/wiki/%E7%94%A8%E6%88%B7%E6%8C%87%E5%8D%97)

Introduction
------------

Shadowsocks-Qt5 is a fast, reliable and platform-cross GUI fronted for [Shadowsocks](http://shadowsocks.org).

Latest Stable Version: 0.4.0

Features
--------

- Shadowsocks-Qt5 is written in C++/Qt5.
- An intuitive and native UI. And JUST a **GUI fronted**. Keep your favourite Shadowsocks port as backend.
- The `gui-config.json` file is partially compatible with [shadowsocks-gui](https://github.com/shadowsocks/shadowsocks-gui). In order to serve better, some new values have been added.
- `gui-config.json` is located under ~/.config/shadowsocks for UNIX users, or under the main programme's directory for Windows users.

Note
----

- It is not a standalone programme. By default, `ss-qt5` works along with `ss-local` from [shadowsocks-libev project](https://github.com/madeye/shadowsocks-libev). While it also supports using other shadowsocks ports as backend such as [Shadowsocks-Nodejs](https://github.com/clowwindy/shadowsocks-nodejs).
- Shadowsocks-Nodejs port is highly recommended to serve as backend for better performance.
- Don't be panic if you encounter a bug. Feel free to open [issues](https://github.com/librehat/shadowsocks-qt5/issues). Remember to run from terminal or `cmd` and paste the output.

Installation
------------

### Windows ###

Downloading the prebuilt bundle (only **32-bit**) from [releases](https://github.com/librehat/shadowsocks-qt5/releases) is recommended because the bundles include `ss-local.exe`. For those who want to build from source, follow the instructions below.

Open this project using Qt Creator and build it.

Or type the command in MSYS.

```bash
qmake INSTALL_PREFIX=../ss-qt5
make
make install
```

You will get `ss-qt5.exe` and `gui-config.json` in ../ss-qt5 directory.

### Linux ###

You should have installed Qt 5 and its development packages before building.

```bash
# Some distros use seperated qmake-qt4, qmake-qt5. Then, just run `qmake-qt5`. You can specify INSTALL_PREFIX=/usr/local if needed. default is /usr
qmake INSTALL_PREFIX=/usr
make
make install
```
