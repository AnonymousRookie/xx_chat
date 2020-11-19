/**
* Copyright 2019-2020, AnonymousRookie. All rights reserved.
* https://github.com/AnonymousRookie/xx_chat
* Author: AnonymousRookie (357688981 at qq dot com)
* Description: 自定义按钮
*/

#include <QPainter>
#include <QMouseEvent>
#include "imagebutton.h"

ImageButton::ImageButton(QWidget *parent)
    : QWidget(parent)
    , m_cursorChangeAble(false)
    , m_pressed(false)
{
    setAttribute(Qt::WA_TranslucentBackground);
    m_text = std::make_shared<QLabel>(this);
    m_text->setProperty("parent", "ImageButton");

    m_icon = std::make_shared<QLabel>(this);
    m_icon->setProperty("parent", "ImageButton");
}

ImageButton::ImageButton(const QString &icon, int num, QWidget *parent)
    : QWidget(parent)
    , m_cursorChangeAble(false)
    , m_pressed(false)
{
    setAttribute(Qt::WA_TranslucentBackground);

    QPixmap pix(icon);

    for (int i = 0; i != num; i++) {
        if (0 == i) {
            m_normalPix = pix.copy(i * pix.width() / num, 0, pix.width() / num, pix.height());
        }
        else if (1 == i) {
            m_hoverPix = pix.copy(i * pix.width() / num, 0, pix.width() / num, pix.height());
        }
        else if (2 == i) {
            m_pressPix = pix.copy(i * pix.width() / num, 0, pix.width() / num, pix.height());
        }
        else if (3 == i) {
            m_disablePix = pix.copy(i * pix.width() / num, 0, pix.width() / num, pix.height());
        }
    }

    m_currentPix = m_normalPix;
    setFixedSize(m_currentPix.size());
}

ImageButton::ImageButton(const QString &icon, bool isDivision, int num, QWidget *parent)
    : QWidget(parent)
    , m_cursorChangeAble(false)
    , m_pressed(false)
{
    setAttribute(Qt::WA_TranslucentBackground);

    if (isDivision) {
        setDivisionButtonInfo(icon, num);
    }
    else {
        setOneButtonInfo(icon, num);
    }
}

ImageButton::~ImageButton()
{

}

void ImageButton::setOneButtonInfo(const QString &icon, int num)
{
    QPixmap pix(icon);

    for (int i = 0; i != num; i++) {
        if (0 == i) {
            m_normalPix = pix.copy(i * pix.width() / num, 0, pix.width() / num, pix.height());
        }
        else if (1 == i) {
            m_hoverPix = pix.copy(i * pix.width() / num, 0, pix.width() / num, pix.height());
        }
        else if (2 == i) {
            m_pressPix = pix.copy(i * pix.width() / num, 0, pix.width() / num, pix.height());
        }
        else if (3 == i) {
            m_disablePix = pix.copy(i * pix.width() / num, 0, pix.width() / num, pix.height());
        }
    }

    m_currentPix = m_normalPix;
    setFixedSize(m_currentPix.size());
}

void ImageButton::setDivisionButtonInfo(const QString &icon, int num)
{
    if (num >= 1) {
        m_normalPix = QPixmap(icon + "_normal");
    }
    if (num >= 2) {
        m_hoverPix = QPixmap(icon + "_hover");
    }
    if (num >= 3) {
        m_pressPix = QPixmap(icon + "_press");
    }
    if (num >= 4) {
        m_normalPix = QPixmap(icon + "_disable");
    }
    if (num >= 5) {
        m_normalPix = QPixmap(icon + "_alarm");
    }

    m_currentPix = m_normalPix;
    setFixedSize(m_currentPix.size());
}

void ImageButton::setButtonInfo(BUTTONSTATUS status, const QString &icon)
{
    switch (status)
    {
    case BUTTON_STATUS_NORMAL:
        m_normalPix = QPixmap(icon);
        break;
    case BUTTON_STATUS_HOVER:
        m_hoverPix = QPixmap(icon);
        break;
    case BUTTON_STATUS_PRESS:
        m_pressPix = QPixmap(icon);
        break;
    case BUTTON_STATUS_DISABLE:
        m_disablePix = QPixmap(icon);
        break;
    case BUTTON_STATUS_ALARM:
        m_alarmPix = QPixmap(icon);
        break;
    default:
        break;
    }
}

void ImageButton::setButtonStatus(BUTTONSTATUS status)
{
    if (isEnabled())
    {
        switch (status)
        {
        case BUTTON_STATUS_PRESS :
            m_currentPix = m_pressPix;
            break;
        case BUTTON_STATUS_NORMAL :
            m_currentPix = m_normalPix;
            break;
        case BUTTON_STATUS_HOVER :
            m_currentPix = m_hoverPix;
            break;
        case BUTTON_STATUS_ALARM:
            m_currentPix = m_alarmPix;
            break;
        default :
            break;
        }
    }
    else
    {
        if (BUTTON_STATUS_DISABLE == status) {
            m_currentPix = m_disablePix;
        }
    }
    setFixedSize(m_currentPix.size());
    update();
}

void ImageButton::setIcon(const QString &icon)
{
    QPixmap pix = QPixmap(icon);
    m_icon->setPixmap(pix);
    m_icon->setFixedSize(pix.size());
    m_icon->move(ICON_MARGIN, (this->height() - m_icon->height()) / 2);
}

void ImageButton::setText(const QString &text)
{
    m_text->resize((this->width() - ICON_MARGIN - m_icon->width()), this->height());
    m_text->move(ICON_MARGIN + m_icon->width(), 0);
    m_text->setText(text);
    m_text->setAlignment(Qt::AlignCenter);
}

void ImageButton::setTypeId(int id)
{
    m_typeId = id;
}

void ImageButton::setCursorEnable(bool cursorEnable)
{
    m_cursorChangeAble = cursorEnable;
}

void ImageButton::paintEvent(QPaintEvent * /*e*/)
{
    QPainter painter(this);
    painter.drawPixmap(rect(), m_currentPix);
}

void ImageButton::mousePressEvent(QMouseEvent *e)
{
    if (e->button() != Qt::LeftButton) {
        e->ignore();
    } else {
        setButtonStatus(BUTTON_STATUS_PRESS);
    }
}

void ImageButton::mouseReleaseEvent(QMouseEvent *e)
{
    if (m_pressed) {
        e->ignore();
        return;
    }
    if (e->button() != Qt::LeftButton) {
        e->ignore();
        return;
    }
    if (rect().contains(e->pos())) {
        if (isEnabled()) {
            emit clicked(m_typeId);
        }
        setButtonStatus(BUTTON_STATUS_HOVER);
        e->accept();
    }
    else {
        setButtonStatus(BUTTON_STATUS_NORMAL);
        e->ignore();
    }
}

void ImageButton::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::EnabledChange) {
        if (!isEnabled()) {
            setButtonStatus(BUTTON_STATUS_DISABLE);
        }
    }
}

void ImageButton::enterEvent(QEvent * /*e*/)
{
    if (!m_pressed) {
        setButtonStatus(BUTTON_STATUS_HOVER);
    }
    if (m_cursorChangeAble) {
        m_currentCursor = cursor();
        setCursor(Qt::PointingHandCursor);
    }
}

void ImageButton::leaveEvent(QEvent * /*e*/)
{
    if (!m_pressed) {
        setButtonStatus(BUTTON_STATUS_NORMAL);
    }
    if (m_cursorChangeAble) {
        setCursor(m_currentCursor);
    }
}

void ImageButton::setPressed(bool pressed/* = true*/)
{
    m_pressed = pressed;
    if (!pressed) {
        setButtonStatus(BUTTON_STATUS_NORMAL);
    }
    update();
}