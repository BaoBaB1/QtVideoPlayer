#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "VideoPlayer.h"

#include <QMainWindow>

class QResizeEvent;

class MainWindow : public QMainWindow
{
public:
    MainWindow();
    //void setVideoPlayer(VideoPlayer* player);
    VideoPlayer* getVideoPlayer() { return m_videoPlayer; }
    const VideoPlayer* getVideoPlayer() const { return m_videoPlayer; }
private:
    void resizeEvent(QResizeEvent* event) override;
private:
    VideoPlayer* m_videoPlayer = nullptr;
};

#endif // MAINWINDOW_H
