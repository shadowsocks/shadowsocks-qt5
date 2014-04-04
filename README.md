Shadowsocks-Qt5 is a GUI Wrapper for shadowsocks-libev
------

Shadowsocks-Qt5 intended to be a fast and platform-independent GUI Client alternative of shadowsocks.

The odd minor version number (0.**X**) indicates development version. And the even one suggests a stable version.

## Note ##

- This is not a standalone programme. It works along with ss-local binary from [shadowsocks-libev project](https://github.com/madeye/shadowsocks-libev).
- The gui-config.json file is compatible with [shadowsocks-gui](https://github.com/shadowsocks/shadowsocks-gui). But add a new value named "app" to specify `ss-local` programme.
- gui-config.json is located under ~/.config/shadowsocks for UNIX users, but under this app's directory for Windows users.
- Although it's recommended to use both 64-bit ss-local and ss-qt5 if you're running 64-bit OS. Feel free to use 32-bit ss-local together with 64-bit ss-qt5, vice versa.

## BUILD FROM SOURCE ##

You should have installed Qt-5 and its development packages before building.

`qmake
make
make install`

_Building using Qt Creator is also viable._

## INSTALLATION ##

There is prebuilt tarball including ss-local in **releases** for Windows.

For Linux/*BSD/Mac OS X users, please refer to **BUILD FROM SOURCE** to install it.
