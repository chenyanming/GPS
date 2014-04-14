//: process.cpp
#include <QtGui>
#include <QDialog>
#include <QDebug>
#include "MyDialog.h"

//QString program0 = "/bin/testarm";
//QString program1 = "/bin/rmc_gsv_gsa";
//QString program2 = "/bin/print";

MyDialog::MyDialog(QWidget *parent, Qt::WFlags flags)
    : QWidget(parent, flags)
{
    //inputLabel = new QLabel(tr("请输入命令："));
    //inputEdit = new QLineEdit;

    runButton = new QPushButton(tr("启动GPS"));
    exitButton = new QPushButton(tr("关闭GPS"));
    haltButton = new QPushButton(tr("关机"));
    rebootButton = new QPushButton(tr("重启"));

    combo = new QComboBox();

    combo->addItem(QWidget::tr("默认"));
    combo->addItem(QWidget::tr("查看GPS数据"));
    combo->addItem(QWidget::tr("解码GPS数据"));
    //combo->insertItem(2, tr("Insert item"));
    //combo->insertSeparator(2);

    //outputLabel = new QLabel(tr("运行结果如下："));
    outputEdit = new QTextEdit;
    outputEdit->setReadOnly(true);

    QHBoxLayout* hlayout = new QHBoxLayout;
    hlayout->addWidget(runButton);
    hlayout->addWidget(exitButton);
    hlayout->addWidget(haltButton);
    hlayout->addWidget(rebootButton);
    hlayout->addWidget(combo);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addLayout(hlayout);
    //layout->addWidget(outputLabel);
    layout->addWidget(outputEdit);
    setLayout(layout);

    cmd = new QProcess(parent);
    //cmd1 = new QProcess(parent);
    //cmd2 = new QProcess(parent);
    //QProcess *cmd = new QProcess;

    connect(runButton, SIGNAL(clicked(bool)), this, SLOT(openGPS()));//点击该按钮，就启动openGPS
    connect(exitButton, SIGNAL(clicked(bool)), this, SLOT(closeGPS()));//点击该按钮，就启动closeGPS
    connect(haltButton, SIGNAL(clicked(bool)), this, SLOT(halt()));
    connect(rebootButton, SIGNAL(clicked(bool)), this, SLOT(reboot()));
    connect(combo, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(click_combo()));//点击combo的某个选项，就会触发click_combo

    connect(cmd, SIGNAL(readyRead()), this, SLOT(readOutput()));

    resize(400, 300);
    cmd->setReadChannel(QProcess::StandardOutput);

}
MyDialog::~MyDialog()
{
}

void MyDialog::openGPS()
{
    QString str;
    str = combo->currentText();
    if (str == "默认")
    {
        //QMessageBox::information(this, tr("Info"), "第0个");
        //cmd->start(program0);
        cmd->start("/bin/testarm");
    }
    else if (str == "查看GPS数据")
    {
        //QMessageBox::information(this, tr("Info"), "第1个");
        //cmd1->start(program1);
    }
    else if (str == "解码GPS数据")
    {
        //QMessageBox::information(this, tr("Info"), "第2个");
        //cmd2->start(program2);
    }
}

void MyDialog::closeGPS()
{
    cmd->kill();
    outputEdit->clear();
}

void MyDialog::readOutput()
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


void MyDialog::halt()
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

void MyDialog::reboot()
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

/*
 * 当选择了combo某个选项的时候，就会把进程关掉，并把文字区清0
 * 当然别忘记了，选项的名字会改变，我就是根据改变的名字来控制openGPS函数所打开的不同后台进程
 */
void MyDialog::click_combo()
{
    QString str;
    str = combo->currentText();
    if (str == "默认")
    {
        //QMessageBox::information(this, tr("Info"), "第0个");
        //cmd->kill();
        //outputEdit->clear();
    }
    else if (str == "查看GPS数据")
    {
        //QMessageBox::information(this, tr("Info"), "第1个");
        //cmd1->kill();
        //outputEdit->clear();
    }
    else if (str == "解码GPS数据")
    {
        //QMessageBox::information(this, tr("Info"), "第2个");
        //cmd2->kill();
        //outputEdit->clear();
    }
}

    //combo->setItemText(0, "我擦");
