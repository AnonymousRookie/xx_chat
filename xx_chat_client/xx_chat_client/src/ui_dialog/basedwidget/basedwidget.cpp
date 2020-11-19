/**
* Copyright 2019-2020, AnonymousRookie. All rights reserved.
* https://github.com/AnonymousRookie/xx_chat
* Author: AnonymousRookie (357688981 at qq dot com)
* Description: BasedWidget
*/

#include <QMouseEvent>
#include "basedwidget.h"

BasedWidget::BasedWidget(QWidget* parent/* = 0*/)
    : QWidget(parent)
    , m_isPressed(false)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
}

BasedWidget::~BasedWidget()
{

}

void BasedWidget::mousePressEvent(QMouseEvent* e)
{
    if (e->button() != Qt::LeftButton) {
        e->ignore();
    } else {
        m_isPressed = true;
        m_pos = e->globalPos() - pos();
    }
}

void BasedWidget::mouseReleaseEvent(QMouseEvent* /*e*/)
{
    m_isPressed = false;
}

void BasedWidget::mouseMoveEvent(QMouseEvent* e)
{
    if (m_isPressed) {
        move(e->globalPos() - m_pos);
    }
    QWidget::mouseMoveEvent(e);
}
