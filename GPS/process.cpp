//: process.cpp
#include <QtGui>
#include <QDialog>
#include <QDebug>
#include "process.h"

Process::Process(QWidget *parent, Qt::WFlags flags)
    : QWidget(parent, flags)
{
    //inputLabel = new QLabel(tr("请输入命令："));
    //inputEdit = new QLineEdit;
    runButton = new QPushButton(tr("启动GPS"));
    exitButton = new QPushButton(tr("关闭GPS"));
    haltButton = new QPushButton(tr("关机"));
    rebootButton = new QPushButton(tr("重启"));
    //outputLabel = new QLabel(tr("运行结果如下："));
    outputEdit = new QTextEdit;
    outputEdit->setReadOnly(true);

    QHBoxLayout* hlayout = new QHBoxLayout;
    hlayout->addWidget(runButton);
    hlayout->addWidget(exitButton);
    hlayout->addWidget(haltButton);
    hlayout->addWidget(rebootButton);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addLayout(hlayout);
    //layout->addWidget(outputLabel);
    layout->addWidget(outputEdit);
    setLayout(layout);

    cmd = new QProcess;
    cmd1 = new QProcess;
    cmd2 = new QProcess;
    //QProcess *cmd = new QProcess;

    connect(runButton, SIGNAL(clicked(bool)), this, SLOT(openGPS()));//点击该按钮，就启动openGPS
    connect(exitButton, SIGNAL(clicked(bool)), this, SLOT(closeGPS()));//点击该按钮，就启动closeGPS
    connect(haltButton, SIGNAL(clicked(bool)), this, SLOT(halt()));
    connect(rebootButton, SIGNAL(clicked(bool)), this, SLOT(reboot()));



    connect(cmd, SIGNAL(readyRead()), this, SLOT(readOutput()));

    resize(400, 300);
    cmd->setReadChannel(QProcess::StandardOutput);

}
Process::~Process()
{
}
void Process::openGPS()
{
    //cmd->start("ping 192.168.1.8");
    cmd->start("/bin/testarm");
    //QString input = inputEdit->text();
    //QString input = "/mnt/2.serial/testarm";
    //QString input = "/root/testarm";
    //QString input = "/bin/ls";
    //cmd->start(input);
    //output = tr("");
    //outputEdit->setText(output);
}

void Process::closeGPS()
{
    cmd->kill();
    outputEdit->clear();
}

void Process::readOutput()
{
    QString output(cmd->readAll());
    //textEdit.append(appendText);
    //output += cmd->readAll();
    //output = cmd->readAllStandardOutput();
    //output = cmd->readAll();
    //outputEdit->clear();
    //outputEdit->setText(output);
    outputEdit->append(output);
}


void Process::halt()
{
    QMessageBox msgBox;
    msgBox.setText("准备关机...");
    msgBox.setInformativeText("是否关机？");
    msgBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int ret = msgBox.exec();
    if (ret == QMessageBox::Yes)
    {
        cmd1->start("/sbin/halt");
    }


}

void Process::reboot()
{
    QMessageBox msgBox;
    msgBox.setText("准备重启...");
    msgBox.setInformativeText("是否重启？");
    msgBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int ret = msgBox.exec();
    if (ret == QMessageBox::Yes)
    {
        cmd2->start("/sbin/reboot");
    }
}
