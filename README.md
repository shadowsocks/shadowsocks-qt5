Shadowsocks-Qt5
===============

[中文用户指南](https://github.com/librehat/shadowsocks-qt5/wiki/%E7%94%A8%E6%88%B7%E6%8C%87%E5%8D%97)

Introduction
------------

Shadowsocks-Qt5 is a fast, reliable and cross-platform GUI fronted for [Shadowsocks](http://shadowsocks.org).

Features
--------

- Shadowsocks-Qt5 is written in C++/Qt5.
- Intuitive and **native** UI. This is **not** a clumsy Web App.
- JUST a **GUI fronted**. Keep your favourite Shadowsocks port as backend.
- Easy-to-use and highly customisable.
- The `gui-config.json` file is partially compatible with [shadowsocks-gui](https://github.com/shadowsocks/shadowsocks-gui). In order to serve better, some new values have been added.
- `gui-config.json` is located under ~/.config/shadowsocks on UNIX platforms, or under the main programme's directory in Windows.

Note
----

- It is not a standalone programme. `ss-qt5` needs a shadowsocks backend such as [Shadowsocks-libev](https://github.com/clowwindy/shadowsocks) and [Shadowsocks-Python](ss-python).
- [Shadowsocks-Python](ss-python) is highly recommended to serve as backend for better performance and stability.
- Don't be panic if you encounter a bug. Please feel free to open [issues](https://github.com/librehat/shadowsocks-qt5/issues). Just remember to run from terminal or `cmd` and paste the output to the description of issue.

[ss-python]:https://github.com/clowwindy/shadowsocks

Installation
------------

### Windows ###

1. Download prebuilt binaries from [releases](https://github.com/librehat/shadowsocks-qt5/releases).
2. Install/update your favourite shadowsocks backend. (You may want to download prebuilt [ss-libev](http://dl.lazyzhu.com/file/Toolchain/ss-libev/)).

For those who want to build from source, follow the instructions below.

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

### Others ###

Mac OS X and *BSD are not tested and they're NOT supported officially. Well, I do hope you can help me mantain the compatibility if you have spare time.
