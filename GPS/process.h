#ifndef PROCESS_H
#define PROCESS_H
//: process.h
#include <QWidget>
class QLabel;
class QLineEdit;
class QProcess;
class QPushButton;
class QTextEdit;
class Process : public QWidget
{
    Q_OBJECT
public:
    Process(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~Process();
private slots:
    void openGPS();
    void readOutput();
    void closeGPS();
    void halt();
    void reboot();
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
};
#endif // PROCESS_H
