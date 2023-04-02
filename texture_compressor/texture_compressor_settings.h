#ifndef TEXTURE_COMPRESSOR_SETTINGS_H
#define TEXTURE_COMPRESSOR_SETTINGS_H

#include <QMainWindow>
#include <QOpenGLWindow>

namespace Ui {
class TextureCompressorSettings;
}

class TextureCompressorSettings : public QMainWindow
{
    Q_OBJECT

public:
    explicit TextureCompressorSettings(QOpenGLWindow* previewWindow);
    ~TextureCompressorSettings();

private:
    Ui::TextureCompressorSettings *ui;
    QOpenGLWindow *previewWindow;

    void ResizePreview(int width, int height);

private slots:
    void LoadImage();
    void CompressImage();
};

#endif // TEXTURE_COMPRESSOR_SETTINGS_H
