#include "MainWindow.h"

#include <QResizeEvent>
#include <QMenu>
#include <QMenuBar>
#include <QFileDialog>

MainWindow::MainWindow()
{
    m_videoPlayer = new VideoPlayer();
    QMenu* menu = menuBar()->addMenu("File");
    QAction* openFileAction = menu->addAction("Open");
    QObject::connect(openFileAction, &QAction::triggered, this,
    [=]
    {
        QString filename = QFileDialog::getOpenFileName(this, "Select video file", "", "*.mp4");
        if (filename.isEmpty() || filename.isNull())
        {
            return;
        }
        m_videoPlayer->setVideo(filename.toStdString());
    });
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    m_videoPlayer->setParent(centralWidget);
    setMinimumSize({640, 480});
    resize({1280, 720});
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
    const QSize windowSize = event->size();
    const QSize menuBarSize = menuBar()->size();
    m_videoPlayer->setGeometry(0, 0, windowSize.width(), windowSize.height() - menuBarSize.height());
}
