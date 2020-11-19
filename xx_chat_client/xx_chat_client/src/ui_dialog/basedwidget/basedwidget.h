/**
* Copyright 2019-2020, AnonymousRookie. All rights reserved.
* https://github.com/AnonymousRookie/xx_chat
* Author: AnonymousRookie (357688981 at qq dot com)
* Description: BasedWidget
*/

#pragma once

#include <QWidget>

class QPoint;

class BasedWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BasedWidget(QWidget* parent = 0);
    ~BasedWidget();

protected:
    void mousePressEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);

private:
    QPoint m_pos;
    bool m_isPressed;
};
