Shadowsocks-Qt5
===============

Please check [project's wiki](https://github.com/librehat/shadowsocks-qt5/wiki) for "how-tos".

V2.0
----

`master` branch is used as a development branch for upcoming version 2.0. For old-stable code, please check `0.x` branch.

### TODO ###

- Rewrite UI
- Enable multiple profiles at the same time
- Server latency test
- INI format configuration file
- ~~Remove all other backends except for `libQtShadowsocks`~~
- New flat icon

Introduction
------------

Shadowsocks-Qt5 is a native and cross-platform GUI client for [Shadowsocks](http://shadowsocks.org).

Features
--------

- Shadowsocks-Qt5 is written in C++/Qt5.
- Intuitive and **native** UI. This is **not** a clumsy Web App.
- Keep your favourite Shadowsocks port as backend if you want.
- Easy-to-use and highly customisable.
- The `gui-config.json` file is extended from [shadowsocks-gui](https://github.com/shadowsocks/shadowsocks-gui).
- `gui-config.json` is located under ~/.config/shadowsocks on UNIX platforms, or under the main programme's directory on Windows.

Note
----

- By default, `ss-qt5` works with `libQtShadowsocks` which is considered as a reliable and lightweight alternative. While you can still use other shadowsocks backends such as [Shadowsocks-libev] [ss-libev] and [Shadowsocks-Python] [ss-python].
- If `ss-qt5` crashes and the single-instance mode is on, you may need to manually delete `/tmp/qipc_sharedmemory_shadowsocksqt*` and `/tmp/qipc_systemsem_shadowsocksqt*`. Otherwise, `ss-qt5` will complain that another instance is already running.
- Don't be panic if you encounter a bug. Please feel free to open [issues](https://github.com/librehat/shadowsocks-qt5/issues). Just remember to run from terminal or `cmd` and paste the output to the description of issue.


[ss-python]: https://github.com/clowwindy/shadowsocks
[ss-libev]: https://github.com/shadowsocks/shadowsocks-libev

LICENSE
-------

Copyright © 20014-2015 Symeon Huang

This project is licensed under version 3 of the GNU Lesser General Public License.
