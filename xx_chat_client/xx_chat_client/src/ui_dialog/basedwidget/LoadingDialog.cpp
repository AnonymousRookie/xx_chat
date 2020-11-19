/**
* Copyright 2019-2020, AnonymousRookie. All rights reserved.
* https://github.com/AnonymousRookie/xx_chat
* Author: AnonymousRookie (357688981 at qq dot com)
* Description: 等待提示窗口
*/

#include "LoadingDialog.h"
#include <QDesktopWidget>

LoadingDialog::LoadingDialog(QWidget *parent)
    : QDialog(parent)
{
    m_ui.setupUi(this);
    init();
}

LoadingDialog::~LoadingDialog()
{
}

LoadingDialog* LoadingDialog::instance()
{
    static LoadingDialog dlg;
    return &dlg;
}

void LoadingDialog::showDlg(int timeout/* = 1000*/)
{
    m_spLoadMsgTimer->start(timeout);
    m_movie->start();
    show();
}

void LoadingDialog::hideDlg()
{
    m_movie->stop();
    hide();
    m_spLoadMsgTimer->stop();
}

void LoadingDialog::onLoadMsgTimeout()
{
    m_movie->stop();
    hide();
    m_spLoadMsgTimer->stop();
}

void LoadingDialog::init()
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setWindowOpacity(0.3);
    setModal(true);

    QDesktopWidget *desktop = QApplication::desktop();
    QRect screen = desktop->screenGeometry();
    setFixedSize(screen.width(), screen.height());
    move(0, 0);

    m_ui.label_text->setProperty("parent", "LoadingDialog");
    m_movie = std::make_shared<QMovie>(":/SDM/loading");
    m_ui.label_gif->setMovie(m_movie.get());

    m_ui.label_gif->move(screen.width() / 2 - m_ui.label_gif->width() / 2, screen.height() / 3);
    m_ui.label_text->move(screen.width() / 2 - m_ui.label_text->width() / 2, screen.height() / 2);

    m_spLoadMsgTimer = std::make_shared<QTimer>(this);
    connect(m_spLoadMsgTimer.get(), SIGNAL(timeout()), this, SLOT(onLoadMsgTimeout()));
}

