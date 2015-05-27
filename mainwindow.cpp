#include <QSettings>

#include "dataprovider.h"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    qDebug() << "MainWindow::MainWindow";

    QCoreApplication::setOrganizationName("somafm-qt");
    //QCoreApplication::setOrganizationDomain("somafm.com");
    QCoreApplication::setApplicationName("somafm-qt");
    setWindowTitle(tr("SomaFM Radio Player"));
    readSettings();

    setFont(QFont(fontInfo().family(), -1));

    m_topView = new QLabel(this);
    //m_topView->setFixedSize(QSize(468, 60));
    //m_topView->setWidth(m_topView->maximumWidth());
    m_topView->setScaledContents(true);
    QPixmap banner = QPixmap::fromImage(QImage(":/banner.gif"));
    m_topView->setPixmap(banner);

    m_tabWidget = new QTabWidget(this);
    m_channelsView = new ChannelsView(this);
    m_channelsView->setFocus();

    m_tabWidget->addTab(m_channelsView, tr("Channels"));

    m_playerView = new PlayerView(this);

    m_mainLayout = new QVBoxLayout;
    m_mainLayout->addWidget(m_topView);
    m_mainLayout->addWidget(m_tabWidget);
    m_mainLayout->addWidget(m_playerView);

    m_mainWidget = new QFrame(this);
    m_mainWidget->setLayout(m_mainLayout);

    setCentralWidget(m_mainWidget);

    m_dataProvider = new DataProvider;
    connect(m_channelsView, SIGNAL(requestChannelList()),
            m_dataProvider, SLOT(provideChannelList()));

    connect(m_dataProvider, SIGNAL(channelListProvided(ChannelList)),
            m_channelsView, SLOT(updateChannelList(ChannelList)));

    connect(m_channelsView, SIGNAL(requestChannelIcon(QString)),
            m_dataProvider, SLOT(provideChannelIcon(QString)));

    connect(m_dataProvider, SIGNAL(channelIconProvided(QString,QImage)),
            m_channelsView, SLOT(updateChannelIcon(QString,QImage)));

    connect(m_dataProvider, SIGNAL(channelImageProvided(QString,QImage)),
            m_playerView, SLOT(updateChannelImage(QString,QImage)));

    connect(m_channelsView, SIGNAL(channelActivated(QString)),
            this, SLOT(startPlayer(QString)));

    connect(m_playerView, SIGNAL(requestChannelImage(QString)),
            m_dataProvider, SLOT(provideChannelImage(QString)));

    connect(m_playerView, SIGNAL(playerStarted(QString)),
            m_channelsView, SLOT(playingState(QString)));
    connect(m_playerView, SIGNAL(playerStopped()),
            m_channelsView, SLOT(stoppedState()));

    m_dataProvider->provideChannelList();
}

void MainWindow::startPlayer(QString id) {
    qDebug() << "MainWindow::startPlayer";

    QUrl url = m_dataProvider->getPlaylistUrl(id);
    m_playerView->play(id, url);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    writeSettings();
    event->accept();
}

MainWindow::~MainWindow()
{
}

void MainWindow::writeSettings()
{
    QSettings settings;

    settings.beginGroup("MainWindow");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.endGroup();
}

void MainWindow::readSettings()
{
    QSettings settings;
    settings.beginGroup("MainWindow");
    resize(settings.value("size", QSize(520, 800)).toSize());
    move(settings.value("pos", QPoint(0, 0)).toPoint());
    settings.endGroup();
}
