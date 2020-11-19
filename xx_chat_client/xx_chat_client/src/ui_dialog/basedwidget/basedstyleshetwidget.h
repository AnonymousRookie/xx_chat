/**
* Copyright 2019-2020, AnonymousRookie. All rights reserved.
* https://github.com/AnonymousRookie/xx_chat
* Author: AnonymousRookie (357688981 at qq dot com)
* Description: BasedStyleShetWidget
*/

#pragma once

#include <QWidget>

class BasedStyleShetWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BasedStyleShetWidget(QWidget* parent = 0);
    ~BasedStyleShetWidget();

protected:
    void paintEvent(QPaintEvent* e);
};
