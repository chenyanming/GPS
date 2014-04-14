#ifndef PROCESS_H
#define PROCESS_H
//: process.h
#include <QWidget>
#include <QComboBox>
class QLabel;
class QLineEdit;
class QProcess;
class QPushButton;
class QTextEdit;
class MyDialog : public QWidget
{
    Q_OBJECT
public:
    MyDialog(QWidget *parent = 0, Qt::WFlags flags = 0);
    //QString program1;
    //QString program2;
    ~MyDialog();

private slots:
    void openGPS();
    void readOutput();
    void closeGPS();
    void halt();
    void reboot();
    void click_combo();
private:
    QProcess* cmd;
    QProcess* cmd1;
    QProcess* cmd2;
    QString output;
    QLabel* inputLabel;
    QLineEdit* inputEdit;
    QPushButton* runButton;
    QPushButton* exitButton;
    QPushButton* haltButton;
    QPushButton* rebootButton;
    QLabel* outputLabel;
    QTextEdit* outputEdit;
    QComboBox* combo;
};
#endif // PROCESS_H
