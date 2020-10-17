#include "gif.h"
#include "utils.h"
#include "appsettings.h"
#include <QPainter>
#include <QBitmap>
#include <QFileInfo>
#include <QDir>
#include <array>

#include <QGraphicsView>
#include <QGraphicsScene>

static const constexpr std::array<const char *, 41> characters = {
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
    "a", "b", "c", "d", "e", "f", "g", "h", "i", "j",
    "k", "l", "m", "n", "o", "p", "q", "r", "s", "t",
    "u", "v", "w", "x", "y", "z", "zz0exclam",
    "zz1quest", "zz2apost", "zz3colon", "zz4quote"
};


Gif::Gif()
{
    setFlags(ItemIsMovable | ItemIsSelectable);
}

void Gif::addFrame(QString filename)
{
    QPixmap pix(filename);
    originalFrames.push_back(pix);
    frames.push_back(pix);

    if (originalFrames.size() == 1)
        timerEvent(nullptr);
}

void Gif::setSpeed(int speed)
{
    if (timerId != -1)
        killTimer(timerId);
    fps = speed;
    if (speed > 0)
        timerId = startTimer(1000 / speed);
    else
        timerId = -1;
}

void Gif::setHSL(int h, int s, int l)
{
    H = h;
    S = s;
    L = l;

    frames.clear();
    for (auto pix : originalFrames)
    {
        frames.push_back(Utils::ChangeHSL(pix, h / 100.0f, s / 100.0f, l / 100.0f));
    }

    if (frames.size() == 1)
        timerEvent(nullptr);
}

void Gif::setFrameOffset(int f)
{
    offsetFrame = f;
    refresh();
}

void Gif::mirror(bool active)
{
    mirrored = active;

    if (frames.size() == 1)
        timerEvent(nullptr);
}

void Gif::flip(bool active)
{
    flipped = active;

    if (frames.size() == 1)
        timerEvent(nullptr);
}

void Gif::setSwingOrSpin(int animation)
{
    swingOrSpin = animation;
}

void Gif::setSwingOrSpinSpeed(int speed)
{
    swingOrSpinSpeed = speed;
}

void Gif::set3DFlipX(bool b)
{
    flip3DX = b;
}

void Gif::set3DFlipXSpeed(int speed)
{
    flip3DXSpeed = speed;
}

void Gif::set3DFlipY(bool b)
{
    flip3DY = b;
}

void Gif::set3DFlipYSpeed(int speed)
{
    flip3DYSpeed = speed;
}

void Gif::setFade(bool b)
{
    fade = b;
}

void Gif::setFadeSpeed(int speed)
{
    fadeSpeed = speed;
}

void Gif::setSync(bool b)
{
    sync = b;
}

void Gif::setGifAnimation(int animation)
{
    gifAnimation = animation;
}

void Gif::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)

    currentFrame = (currentFrame + 1) % frames.size();
    QPixmap frame = frames[currentFrame];
    QTransform transform;
    if (mirrored || flipped)
    {
        transform.scale(mirrored ? -1 : 1, flipped ? -1 : 1);
    }

    frame = frame.transformed(transform);

    setPixmap(frame);
    setOffset(-frame.width() / 2, -frame.height() / 2);
    update();
}

void Gif::refresh()
{
    originalFrames.clear();
    frames.clear();
    setSpeed(0);

    auto searchPaths = AppSettings::GetSearchPaths();
    for (auto path : searchPaths)
    {
        if (QFileInfo fi(path + "/images/gifs/" + nameOf); fi.isDir())
        {
            QDir dir(fi.absoluteFilePath());
            for (auto entry : dir.entryInfoList(QDir::Files, QDir::Name))
            {
                if (entry.suffix() == "speed")
                {
                    auto spd = entry.baseName().toInt();
                    setSpeed(spd);
                }
                else
                {
                    addFrame(entry.absoluteFilePath());
                }
            }
            break;
        }
        else if (QFile(path + "/images/static/" + nameOf + ".png").exists())
        {
            addFrame(path + "/images/static/" + nameOf + ".png");
            break;
        }
        else if (QFile(path + "/images/shapes/" + nameOf + ".png").exists())
        {
            addFrame(path + "/images/shapes/" + nameOf + ".png");
            break;
        }
        else if (QFileInfo fi(path + "/images/wordart/" + nameOf); fi.isDir())
        {
            auto letter = "0";
            if (offsetFrame > 0 && offsetFrame < static_cast<int>(characters.size()))
            {
                letter = characters[offsetFrame];
                if (!QFile(QString("%1/%2.png").arg(fi.absoluteFilePath()).arg(letter)).exists())
                {
                    letter = "0";
                }
            }
            addFrame(QString("%1/%2.png").arg(fi.absoluteFilePath()).arg(letter));
            break;
        }
    }

    // disable the timer if there is
    // a sole image with a speed set.
    if (originalFrames.size() == 1)
    {
        setSpeed(0);
    }

    setHSL(H, S, L);
}
