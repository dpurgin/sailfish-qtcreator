/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** Commercial Usage
**
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
**
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://qt.nokia.com/contact.
**
**************************************************************************/

/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

#ifndef QPROXYLAYOUTITEM_H
#define QPROXYLAYOUTITEM_H

#include <qml.h>
#include <QGraphicsLayout>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QProxyLayout : public QObject, public QGraphicsLayout
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsLayout QGraphicsLayoutItem)
    Q_PROPERTY(QGraphicsLayout *layout READ layout WRITE setLayout)
public:
    QProxyLayout(QObject *parent=0);

    void setLayout(QGraphicsLayout *);
    QGraphicsLayout *layout() const;

    virtual void setGeometry(const QRectF &);
    virtual int count() const;
    virtual QGraphicsLayoutItem *itemAt(int) const;
    virtual void removeAt(int);
    virtual void updateGeometry();

protected:
    virtual QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint) const;

private:
    QRectF geometry;
    QGraphicsLayout *proxy;
};
QML_DECLARE_TYPE(QProxyLayout);

class QProxyLayoutItem : public QObject, public QGraphicsLayoutItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsLayoutItem)
    Q_PROPERTY(QGraphicsLayoutItem *widget READ item WRITE setItem)
public:
    QProxyLayoutItem(QGraphicsLayoutItem * = 0);

    virtual void setGeometry(const QRectF &);

    QGraphicsLayoutItem *item() const;
    void setItem(QGraphicsLayoutItem *);

    void setEnabled(bool);
protected:
    virtual QSizeF sizeHint(Qt::SizeHint, const QSizeF &) const;

private:
    bool enabled;
    QRectF geometry;
    QGraphicsLayoutItem *other;
};
QML_DECLARE_TYPE(QProxyLayoutItem);

#endif // QPROXYLAYOUTITEM_H

QT_END_NAMESPACE

QT_END_HEADER
