Shadowsocks-Qt5
===============

Introduction
------------

Shadowsocks-Qt5 intended to be a fast and platform-independent GUI Client alternative for [Shadowsocks-libev](http://shadowsocks.org).

Latest Stable Version: 0.1.0

Features
--------

Shadowsocks-Qt5 is written in pure C++ with Qt5 magic. It is not a standalone programme. But only works along with ss-local binary from [shadowsocks-libev project](https://github.com/madeye/shadowsocks-libev).

Intuitive and native UI. You don't need to install node.js. And forget about the terminal emulator or command prompt.

The `gui-config.json` file is compatible with [shadowsocks-gui](https://github.com/shadowsocks/shadowsocks-gui). In order to serve better, a new value named `app` added to specify `ss-local` programme.

`gui-config.json` is located under ~/.config/shadowsocks for UNIX users, or under the main programme's directory for Windows users.

Note
----

Although it's recommended to use both 64-bit ss-local and ss-qt5 if you're running 64-bit OS. Feel free to use 32-bit ss-local together with 64-bit ss-qt5, and vice versa.

Installation
------------

### Windows ###

Downloading the bundle from releases is recommended. For those who want to build from source, follow the instructions below.

Open this project using Qt Creator and build it.

Or type the command in MSYS.

```bash
qmake INSTALL_PREFIX=../ss-qt5
make
make install
```

You will get `ss-qt5.exe` and `gui-config.json` in ../ss-qt5 directory.

### Linux ###

You should have installed Qt-5 and its development packages before building.

```bash
# Some distros don't have qmake but seperated qmake-qt4, qmake-qt5. In this situation, run `qmake-qt5`. You can specify INSTALL_PREFIX=/usr/local if needed. default is /usr
qmake INSTALL_PREFIX=/usr
make
make install
```

Usage
-----

You may have to specify the `ss-local` path if application didn't find it.

Minimise button would cause application hide. Close button would **close** the application. Systray icon is always visible.

Once you click the **Start** button, it'll become **Stop** button if `ss-local` executed successfully. All output of `ss-local` can be viewed by clicking **Log** button.

Once you click the **Save** button, the configuration would be written into file. Before that, you can click **Revert** button to restore the previous values.
