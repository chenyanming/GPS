#include "process.h"
#include <QApplication>
#include <QTextCodec>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));       //支持Tr中文
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));//支持中文文件名显示
    QFont font;
    font.setStyleStrategy(QFont::PreferDefault);
    //font.setPointSize(160);//实际上是16的字号，但设成16却不行
    //font.setFamily(("wenquanyi"));
    //font.setBold(false);
    a.setFont(font);

    Process *w = new Process;
    w->showMaximized();//最大化显示
    return a.exec();
}
