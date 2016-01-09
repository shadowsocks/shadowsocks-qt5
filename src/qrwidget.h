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
 */

#ifndef QRWIDGET_H
#define QRWIDGET_H

#include <QWidget>
#include <QPaintEvent>

class QRWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QRWidget(QWidget *parent = 0);
    void setQRData(const QByteArray &data);
    const QImage& getQRImage() const;

private:
    QImage qrImage;

protected:
    void paintEvent(QPaintEvent *);
};

#endif // QRWIDGET_H
