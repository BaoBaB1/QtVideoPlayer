#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QWidget>
#include <filesystem>

class QVideoWidget;
class QMediaPlayer;
class QResizeEvent;
class QHBoxLayout;
class QVideoSink;
class QLabel;

class VideoPlayer : public QWidget
{
public:
    VideoPlayer();
    void setVideo(const std::filesystem::path& video);
private:
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void centerControlButtons();
private:
    QVideoWidget* m_videoWidget = nullptr;
    QMediaPlayer* m_mediaPlayer = nullptr;
    QWidget* m_videoControlBar = nullptr;
    QLabel* m_thumbnail = nullptr;
    QVideoSink* m_videoSink = nullptr;
    QHBoxLayout* m_buttonsLayout = nullptr;
    QSize m_size = {0, 0};
    std::filesystem::path m_video;
    //std::vector<std::filesystem::path> m_videos;
};

#endif // VIDEOPLAYER_H
