/**
* Copyright 2019-2020, AnonymousRookie. All rights reserved.
* https://github.com/AnonymousRookie/xx_chat
* Author: AnonymousRookie (357688981 at qq dot com)
* Description: 自定义按钮
*/

#pragma once

#include <memory>
#include <QWidget>
#include <QLabel>

enum BUTTONSTATUS
{
    BUTTON_STATUS_NORMAL,
    BUTTON_STATUS_HOVER,
    BUTTON_STATUS_PRESS,
    BUTTON_STATUS_DISABLE,
    BUTTON_STATUS_ALARM,
};

class ImageButton : public QWidget
{
    Q_OBJECT

public:
    explicit ImageButton(QWidget *parent = 0);
    explicit ImageButton(const QString &icon, int num = 4, QWidget *parent = 0);
    explicit ImageButton(const QString &icon, bool isDivision, int num = 4, QWidget *parent = 0);
    ~ImageButton();

    void setOneButtonInfo(const QString &icon, int num = 4);
    void setDivisionButtonInfo(const QString &icon, int num = 3);

    void setButtonInfo(BUTTONSTATUS status, const QString &icon);

    void setCursorEnable(bool cursorEnable);
    void setButtonStatus(BUTTONSTATUS status);
    void setText(const QString &text);
    void setTypeId(int id);
    void setPressed(bool pressed = true);
    void setIcon(const QString &icon);

signals:
    void clicked(int);

protected:
    void enterEvent(QEvent *e);  // 鼠标进入事件
    void leaveEvent(QEvent *e);  // 鼠标移出事件
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void paintEvent(QPaintEvent *e);
    void changeEvent(QEvent *e);

private:
    enum { ICON_MARGIN = 20 };

    QPixmap m_currentPix;

    QPixmap m_normalPix;
    QPixmap m_hoverPix;
    QPixmap m_pressPix;
    QPixmap m_disablePix;
    QPixmap m_alarmPix;

    bool m_pressed;

    QCursor m_currentCursor;
    bool m_cursorChangeAble;   // 控制鼠标进入的形状, 默认关闭
    std::shared_ptr<QLabel> m_text;
    std::shared_ptr<QLabel> m_icon;
    int m_typeId;
};
