/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "dialog.h"

//#include <iostream>
using namespace std;


Dialog::Dialog(const vector<Connection>& connections):
    hostLabel(new QLabel(tr("服务器 IP:"))), portLabel(new QLabel(tr("服务器端口:"))),
    button(new QPushButton(tr("链接节点"))), portLineEdit(new QLineEdit()),
    hostList(new QTextBrowser()), listLabel(new QLabel("已连接服务器")), hostLineEdit(nullptr)
{
    // find out which IP to connect to
    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // use the first non-localhost IPv4 address
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
            ipAddressesList.at(i).toIPv4Address()) {
            ipAddress = ipAddressesList.at(i).toString();
            break;
        }
    }
    // if we did not find one, use IPv4 localhost
    if (ipAddress.isEmpty())
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();

//    QFont font("MS Shell Dlg", 9);
    setFont(QFont("MS Shell Dlg", 9));

    hostLineEdit = new QLineEdit(ipAddress);
    portLineEdit->setValidator(new QIntValidator(1, 65535, this));
    button->setEnabled(false);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(listLabel, 0, 0);
    mainLayout->addWidget(hostList, 1, 0, 2, 0);
    mainLayout->addWidget(hostLabel, 3, 0);
    mainLayout->addWidget(hostLineEdit, 3, 1);
    mainLayout->addWidget(portLabel, 4, 0);
    mainLayout->addWidget(portLineEdit, 4, 1);
    mainLayout->addWidget(button, 6, 0, 1, 2);
    setLayout(mainLayout);

    setWindowTitle(tr("连接对方计算机"));
    portLineEdit->setFocus();

    for(const Connection& c : connections) {
        hostList->append("<b>IP地址:</b> " + c.ipAddr + "<br><b>端口:</b> " + QString::number(c.portAddr) + "<br>");
    }

    connect(button, SIGNAL(clicked()), this, SLOT(requestBlockchain()));
    connect(hostLineEdit, SIGNAL(textChanged(QString)),
            this, SLOT(enableButton()));
    connect(portLineEdit, SIGNAL(textChanged(QString)),
            this, SLOT(enableButton()));
}

Dialog::~Dialog() {
//    // cerr << "In destructor..." << endl;
    delete hostLabel;
    delete portLabel;
    delete button;
    delete hostLineEdit;
    delete portLineEdit;
    delete hostList;
    delete listLabel;
    close();
}

void Dialog::requestBlockchain()
{
    button->setEnabled(false);
    emit selectedSettings(hostLineEdit->text(), portLineEdit->text().toInt());
}

void Dialog::enableButton()
{
    bool enable(!hostLineEdit->text().isEmpty() && !portLineEdit->text().isEmpty());
    button->setEnabled(enable);
}

void Dialog::updateServerLists(const std::vector<Connection>& hosts) {
    hostList->clear();
    for(const Connection& c : hosts) {
        hostList->append("<b>IP地址:</b> " + c.ipAddr + "<br><b>端口:</b> " + QString::number(c.portAddr) + "<br>");
    }
}
