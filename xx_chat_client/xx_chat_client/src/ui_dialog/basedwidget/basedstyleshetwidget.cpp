/**
* Copyright 2019-2020, AnonymousRookie. All rights reserved.
* https://github.com/AnonymousRookie/xx_chat
* Author: AnonymousRookie (357688981 at qq dot com)
* Description: BasedStyleShetWidget
*/

#include <QPainter>
#include <QStyleOption>
#include "basedstyleshetwidget.h"

BasedStyleShetWidget::BasedStyleShetWidget(QWidget* parent/* = 0*/)
    : QWidget(parent)
{

}

BasedStyleShetWidget::~BasedStyleShetWidget()
{

}

void BasedStyleShetWidget::paintEvent(QPaintEvent* /*e*/)
{
    QPainter painter(this);
    QStyleOption opt;
    opt.init(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}
