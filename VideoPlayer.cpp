#include "VideoPlayer.h"

#include <QVideoWidget>
#include <QMediaPlayer>
#include <QSlider>
#include <QResizeEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QTime>
#include <QImage>
#include <QVideoSink>
#include <QVideoFrame>
#include <QAudioOutput>

VideoPlayer::VideoPlayer()
{
    m_videoWidget = new QVideoWidget(this);
    m_mediaPlayer = new QMediaPlayer(this);
    m_videoSink = new QVideoSink(this);
    m_videoControlBar = new QWidget(this);
    m_thumbnail = new QLabel(this);
    m_thumbnail->setScaledContents(true);

    auto audio = new QAudioOutput(this);
    auto vLayout = new QVBoxLayout(this);
    m_mediaPlayer->setAudioOutput(audio);
    audio->setVolume(0.f);

    m_videoControlBar->setMinimumHeight(45);
    m_videoControlBar->setMaximumHeight(60);

    auto vControlPanelLayout = new QVBoxLayout(m_videoControlBar);
    m_buttonsLayout = new QHBoxLayout(m_videoControlBar);
    auto playbackLayout = new QHBoxLayout(m_videoControlBar);

    auto slider = new QSlider(Qt::Orientation::Horizontal);
    auto volumeSlider = new QSlider(Qt::Orientation::Horizontal);
    volumeSlider->setMinimumWidth(50);
    volumeSlider->setMaximumWidth(250);
    //volumeSlider->setFixedWidth(150);
    volumeSlider->setEnabled(false);
    volumeSlider->setRange(-1, 101);
    slider->setEnabled(false);
    auto playButton = new QPushButton("Play");
    auto nextButton = new QPushButton("Next");
    auto prevButton = new QPushButton("Prev");
    auto plus15SButton = new QPushButton("+15");
    auto minus15SButton = new QPushButton("-15");
    // TODO
    nextButton->setEnabled(false);
    prevButton->setEnabled(false);
    auto currentTimeLabel = new QLabel("00:00:00");
    auto endTimeLabel = new QLabel("00:00:00");

    static bool isThumbnailRetrieved = false;

    QObject::connect(playButton, &QPushButton::pressed, this,
    [=]
    {
        if (!m_mediaPlayer->hasVideo())
            return;
        if (m_mediaPlayer->isPlaying())
        {
            m_mediaPlayer->pause();
            playButton->setText("Play");
        }
        else
        {
            m_thumbnail->hide();
            m_videoWidget->show();
            m_mediaPlayer->play();
            playButton->setText("Stop");
        }
    });

    QObject::connect(minus15SButton, &QPushButton::pressed, this,
    [=]
    {
        if (!m_mediaPlayer->hasVideo())
            return;
        m_mediaPlayer->setPosition(std::max<qint64>(0, m_mediaPlayer->position() - 15'000));
    });

    QObject::connect(plus15SButton, &QPushButton::pressed, this,
    [=]
    {
        if (!m_mediaPlayer->hasVideo())
            return;
        m_mediaPlayer->setPosition(std::min<qint64>(m_mediaPlayer->duration(), m_mediaPlayer->position() + 15'000));;
    });

    QObject::connect(m_mediaPlayer, &QMediaPlayer::sourceChanged, this,
    [=]
    {
        if (m_mediaPlayer->isPlaying())
        {
            m_mediaPlayer->stop();
        }
        isThumbnailRetrieved = false;
        slider->setEnabled(!m_mediaPlayer->source().isEmpty());
        volumeSlider->setEnabled(!m_mediaPlayer->source().isEmpty());
        currentTimeLabel->setText("00:00:00");
        playButton->setText("Play");
    });

    QObject::connect(m_mediaPlayer, &QMediaPlayer::durationChanged, this,
    [=]
    {
        slider->setMaximum(m_mediaPlayer->duration());
        QTime totalTime = QTime(0, 0).addMSecs(m_mediaPlayer->duration());
        endTimeLabel->setText(totalTime.toString("hh:mm:ss"));
    });

    QObject::connect(m_mediaPlayer, &QMediaPlayer::positionChanged, this,
    [=]
    {
        slider->setValue(m_mediaPlayer->position());
        QTime currentTime = QTime(0, 0).addMSecs(m_mediaPlayer->position());
        currentTimeLabel->setText(currentTime.toString("hh:mm:ss"));
    });

    QObject::connect(m_mediaPlayer, &QMediaPlayer::mediaStatusChanged, this,
    [=]
    {
        if (m_mediaPlayer->mediaStatus() == QMediaPlayer::EndOfMedia)
        {
            playButton->setText("Play");
            m_videoWidget->hide();
            m_thumbnail->show();
        }
    });

    QObject::connect(slider, &QSlider::sliderMoved, this,
    [=]
    {
        m_mediaPlayer->setPosition(slider->value());
    });

    QObject::connect(slider, &QSlider::sliderPressed, this,
    [=]
    {
        m_mediaPlayer->setPosition(slider->value());
    });

    QObject::connect(m_videoSink, &QVideoSink::videoFrameChanged, this,
    [=]
    {
        if (!isThumbnailRetrieved)
        {
            const QVideoFrame frame = m_videoSink->videoFrame();
            if (!frame.isValid())
                return;
            m_thumbnail->setGeometry(m_videoWidget->geometry());
            m_thumbnail->resize(m_videoWidget->size());
            m_thumbnail->setPixmap(QPixmap::fromImage(frame.toImage()));
            m_mediaPlayer->setVideoSink(nullptr);
            m_mediaPlayer->setVideoOutput(m_videoWidget);
            m_mediaPlayer->stop();
            m_videoWidget->hide();
            m_thumbnail->show();
            audio->setVolume(1.f);
            volumeSlider->setValue(volumeSlider->maximum());
            isThumbnailRetrieved = true;
        }
    });

    QObject::connect(volumeSlider, &QSlider::sliderMoved, this,
    [=]
    {
        m_mediaPlayer->audioOutput()->setVolume(float(volumeSlider->value()) / volumeSlider->maximum());
    });

    QObject::connect(volumeSlider, &QSlider::sliderPressed, this,
    [=]
    {
        m_mediaPlayer->audioOutput()->setVolume(float(volumeSlider->value()) / volumeSlider->maximum());
    });

    // left spacer
    m_buttonsLayout->addStretch();
    m_buttonsLayout->addWidget(minus15SButton, 0, Qt::AlignCenter);
    m_buttonsLayout->addWidget(prevButton, 0, Qt::AlignCenter);
    m_buttonsLayout->addWidget(playButton, 0, Qt::AlignCenter);
    m_buttonsLayout->addWidget(nextButton, 0, Qt::AlignCenter);
    m_buttonsLayout->addWidget(plus15SButton, 0, Qt::AlignCenter);
    // right spacer
    m_buttonsLayout->addStretch();

    QHBoxLayout* controlsLayout = new QHBoxLayout();
    controlsLayout->addLayout(m_buttonsLayout);
    controlsLayout->addStretch();
    controlsLayout->addWidget(new QLabel("Volume"), 0, Qt::AlignVCenter);
    controlsLayout->addWidget(volumeSlider, 0, Qt::AlignVCenter);
    controlsLayout->addStretch();

    playbackLayout->addWidget(currentTimeLabel, 0, Qt::AlignVCenter);
    playbackLayout->addWidget(slider, 0, Qt::AlignVCenter);
    playbackLayout->addWidget(endTimeLabel, 0, Qt::AlignVCenter);

    vControlPanelLayout->setContentsMargins(0, 0, 0, 0);
    vControlPanelLayout->addLayout(playbackLayout);
    vControlPanelLayout->addLayout(controlsLayout);
    m_videoControlBar->setLayout(vControlPanelLayout);

    QSizePolicy policy = m_videoWidget->sizePolicy();
    policy.setRetainSizeWhenHidden(true);
    m_videoWidget->setSizePolicy(policy);

    vLayout->addWidget(m_videoWidget, 10);
    vLayout->addWidget(m_videoControlBar, 1);
}

void VideoPlayer::setVideo(const std::filesystem::path& video)
{
    m_video = video;
    m_mediaPlayer->setSource(QString::fromStdString(video.string()));
    m_mediaPlayer->setVideoOutput(nullptr);
    m_mediaPlayer->setVideoSink(m_videoSink);
    // get video's frame and use it as thumbnail
    m_mediaPlayer->play();
}

void VideoPlayer::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    m_thumbnail->setGeometry(m_videoWidget->geometry());
    m_thumbnail->resize(m_videoWidget->size());
    m_size = event->size();
    centerControlButtons();
}

void VideoPlayer::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    centerControlButtons();
}

void VideoPlayer::centerControlButtons()
{
    // since control buttons belong to same horizontal layout just like volume slider,
    // here we adjust spacer position in a way that control buttons will be centered in the middle of the window,
    // instead of being centered automatically by layout (that shifts buttons to the left)
    const auto spacer = static_cast<QSpacerItem*>(m_buttonsLayout->itemAt(0));
    const auto button1 = static_cast<QPushButton*>(m_buttonsLayout->itemAt(1)->widget());
    const auto button5 = static_cast<QPushButton*>(m_buttonsLayout->itemAt(5)->widget());
    const int buttonsWidth = (button5->geometry().x() + button5->geometry().width()) - button1->geometry().x();
    spacer->changeSize((m_size.width() / 2) - (buttonsWidth / 2), spacer->geometry().height());
    m_buttonsLayout->update();
}
