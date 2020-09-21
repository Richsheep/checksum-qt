#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QLabel>
#include <QTextBrowser>
#include <QGridLayout>
#include <QPushButton>
#include <QDropEvent>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QFileInfo>
#include <QMessageBox>
#include <QTime>

#include <stdint.h>
#include <unistd.h>
#ifdef __WIN32

#else
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#endif

#include "md5.h"

#define TOOL_MD5_LENGTH 16
#define VERSION "1.0.2005.9131"

//#define DEBUG

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void md5sum_SingleFile(QString qstrFileName, QString& qstrMD5);
    std::string Hex2String(uint8_t ui8Hex);
    
private:
    QLabel          *m_pobjMD5ValueDisplay;
    QTextBrowser    *m_pobjHistory;

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

};

#endif // MAINWINDOW_H
