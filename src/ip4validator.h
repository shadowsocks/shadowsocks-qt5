/*
 * Copyright (C) 2014-2016 Symeon Huang <hzwhuang@gmail.com>
 *
 * shadowsocks-qt5 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * shadowsocks-qt5 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libQtShadowsocks; see the file LICENSE. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * This class is based on a piece of code posted on Qt Centre.
 * http://www.qtcentre.org/threads/6228-Ip-Address-Validation?p=32368#post32368
 */

#ifndef IP4VALIDATOR_H
#define IP4VALIDATOR_H

#include <QValidator>

class IP4Validator : public QValidator
{
public:
    IP4Validator(QObject *parent = 0);
    State validate(QString &input, int &) const;
};

#endif // IP4VALIDATOR_H
