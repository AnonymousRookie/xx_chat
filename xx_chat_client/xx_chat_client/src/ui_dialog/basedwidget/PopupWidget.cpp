/**
* Copyright 2019-2020, AnonymousRookie. All rights reserved.
* https://github.com/AnonymousRookie/xx_chat
* Author: AnonymousRookie (357688981 at qq dot com)
* Description: 弹出式窗口
*/

#include <QAction>
#include <QHBoxLayout>
#include "PopupWidget.h"
#include "imagebutton.h"

PopupWidgetTitle::PopupWidgetTitle(QWidget *parent)
    : BasedStyleShetWidget(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setObjectName("PopupWidgetTitle");
    initUi();
}

PopupWidgetTitle::~PopupWidgetTitle()
{

}

void PopupWidgetTitle::initUi()
{
    QLabel *titleLabel = new QLabel("");
    titleLabel->setObjectName("TitleLabel");

    QWidget *titleWidget = new QWidget(this);
    QHBoxLayout *titleLayout = new QHBoxLayout(titleWidget);
    titleLayout->addWidget(titleLabel, 0, Qt::AlignVCenter);
    titleLayout->setContentsMargins(6, 6, 6, 6);

    ImageButton *closeButton = new ImageButton(":/SDM/close");
    closeButton->setCursorEnable(true);
    connect(closeButton, SIGNAL(clicked(int)), this, SIGNAL(closeClicked()));

    QWidget *buttonWidget = new QWidget;
    QHBoxLayout *buttnLayout = new QHBoxLayout(buttonWidget);
    buttnLayout->addWidget(closeButton, 0, Qt::AlignVCenter);
    buttnLayout->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(titleWidget, 0, Qt::AlignLeft | Qt::AlignTop);
    mainLayout->addStretch();
    mainLayout->addWidget(buttonWidget, 0, Qt::AlignRight | Qt::AlignVCenter);
    mainLayout->setContentsMargins(5, 5, 5, 5);
}

PopupWidget::PopupWidget(QWidget *parent)
    : BasedWidget(parent)
{
    // 窗口最前显示
    setWindowFlags(Qt::Widget 
        | Qt::FramelessWindowHint 
        | Qt::WindowSystemMenuHint 
        | Qt::WindowStaysOnTopHint);
    setProperty("type", "PopUpWidget");
    initUi();
    initConnect();
}

PopupWidget::~PopupWidget()
{

}

void PopupWidget::onCloseBtnClicked()
{
    close();
}

void PopupWidget::closeEvent(QCloseEvent*)
{

}

void PopupWidget::initUi()
{
    // 标题widget
    m_spTitleWidget = std::make_shared<PopupWidgetTitle>(this);
    m_spTitleWidget->show();
    m_spTitleWidget->move(0, 0);
}

void PopupWidget::initConnect()
{
    connect(m_spTitleWidget.get(), SIGNAL(closeClicked()), this, SLOT(onCloseBtnClicked()));
}

void PopupWidget::setLabel(const QString& content)
{
 
}

void PopupWidget::setTitleWidgetWidth(int width)
{
    m_spTitleWidget->setFixedWidth(width);
}

void PopupWidget::setTitleWidgetHeight(int height)
{
    m_spTitleWidget->setFixedHeight(height);
}
