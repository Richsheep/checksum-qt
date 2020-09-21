#include "mainwindow.h"

/**
 * @brief MainWindow::MainWindow
 * @param parent
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    QWidget *pMainWidget = new QWidget;
    QVBoxLayout *pobjVBoxLayout = new QVBoxLayout(this);

    m_pobjHistory = new QTextBrowser(this);
    m_pobjMD5ValueDisplay = new QLabel(this);

    m_pobjMD5ValueDisplay->setText("Filename & MD5");

    // Vertical box for vertical layout
    pobjVBoxLayout->addWidget(m_pobjMD5ValueDisplay);
    pobjVBoxLayout->addWidget(m_pobjHistory);

    // Vbox into main widget
    pMainWidget->setLayout(pobjVBoxLayout);

    // set Central avaliable widget
    setCentralWidget(pMainWidget);

    // Make mainwindow accept drag drop
    this->setAcceptDrops(true);
    this->resize(QSize(480, 200));
    this->setWindowTitle(QString("md5sum ") + QString(VERSION));
}

/**
 * @brief MainWindow::~MainWindow
 */
MainWindow::~MainWindow()
{
}

/**
 * @brief MainWindow::dragEnterEvent
 * @param event
 */
void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

/**
 * @brief MainWindow::dropEvent
 * @param event
 */
void MainWindow::dropEvent(QDropEvent *event)
{
    // Get all urls
    QList<QUrl> qListUrl = event->mimeData()->urls();

    for (QUrl qUrl : qListUrl) {
        QString qstrUrlName = qUrl.toLocalFile();

        QFileInfo qFileInfo(qstrUrlName);
        if (qFileInfo.isFile())
        {
            QString qstrAbsFileName = qstrUrlName;
            QString qstrMD5;
            md5sum_SingleFile(qstrAbsFileName, qstrMD5);
            m_pobjHistory->append((qstrAbsFileName.append(" ")
                                                  .append(qstrMD5)));
        }
        else if (qFileInfo.isDir())
        {
            QDir qDir(qstrUrlName);
            QStringList qstrlistFiles = qDir.entryList(QDir::Files | QDir::Readable, QDir::Name);

            for (QString qstrFileName : qstrlistFiles)
            {
                QString qstrMD5;
                QString qstrAbsFileName = QString(qstrUrlName + "/" + qstrFileName);
                md5sum_SingleFile(qstrAbsFileName, qstrMD5);
                m_pobjHistory->append((qstrAbsFileName.append(" ")
                                                      .append(qstrMD5)));
            }
        }
        else
        {
            QMessageBox::warning(this, "warning", "not supported");
        }
    }
    m_pobjHistory->append("*****************************");
}

/**
 * @brief MainWindow::md5sum_SingleFile
 * @param qstrFileName  absolute file name
 * @param qstrMD5       result md5 string
 */
void MainWindow::md5sum_SingleFile(QString qstrAbsFileName, QString& qstrMD5)
{
    MD5_CTX stMD5;
    unsigned char charrMD5[TOOL_MD5_LENGTH];
    MD5Init(&stMD5);

#ifdef DEBUG
    QTime time;
    time.start();
#endif

#if 1
    // Step 1: Open file
    QFile qobjFile(qstrAbsFileName);
    qobjFile.open(QIODevice::ReadOnly);

    // Step 2: Streaming read and calculate MD5
    while (1) {
        // Read 64 * MD5_BLOCK_SIZE(64) = 4KB at a time(), break after read entire file
        QByteArray qBytedata = qobjFile.read(MD5_BLOCK_SIZE * 64);
        if (qBytedata.size() == 0) break;

        MD5Update(&stMD5, (unsigned char*)qBytedata.data(), qBytedata.size());
    }
#else
    // mmap way.
    struct stat sb;
    int fd = open(qstrAbsFileName.toLocal8Bit(), O_RDONLY);
    fstat(fd, &sb);
    unsigned char* mapped = (unsigned char *)mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
    unsigned char* tmp = mapped;
    uint32_t block_count = sb.st_size / (2u * 1024 * 1024 * 1024);
    uint32_t last_block_size = sb.st_size % (2u * 1024 * 1024 * 1024);
    for (uint32_t i = 0; i < block_count; i++)
    {
        MD5Update(&stMD5, (unsigned char*)tmp, (2u * 1024 * 1024 * 1024));
        tmp += (2u * 1024 * 1024 * 1024);
    }
    MD5Update(&stMD5, (unsigned char*)tmp, last_block_size);
    munmap((void *)mapped, sb.st_size);
    ::close(fd);
#endif

    // Step 3: Get MD5 result
    MD5Final(&stMD5, charrMD5);

#ifdef DEBUG
    qDebug() << "time elapsed: " <<time.elapsed() / 1000.0 << "s";
#endif

    // Step 4: transform result to string
    for (int i = 0; i < TOOL_MD5_LENGTH; i++)
    {
        std::string str = Hex2String(charrMD5[i]);
        qstrMD5.append(str.c_str());
    }
}

/**
 * @brief MainWindow::Hex2String
 * @param ui8Hex
 * @return
 */
std::string MainWindow::Hex2String(uint8_t ui8Hex)
{
    char chHex[3];

    snprintf(chHex, sizeof(chHex), "%02x", ui8Hex);

    return std::string(chHex);
}

