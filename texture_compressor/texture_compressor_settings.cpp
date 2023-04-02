#include "texture_compressor_settings.h"
#include "ui_texture_compressor_settings.h"

#include <QFileDialog>
#include <QString>
#include <iostream>
#include <vector>
#include <filesystem>

#include "texture_compressor_backend.h"

std::string m_LoadedTexturePath;
bool m_TextureLoaded;

std::vector<int> m_InputFormats;
std::vector<int> m_CompressedFormats;

TextureCompressorSettings::TextureCompressorSettings(QOpenGLWindow *previewWindow) :
    QMainWindow(nullptr),
    ui(new Ui::TextureCompressorSettings),
    previewWindow(previewWindow)
{
    ui->setupUi(this);

    auto &inputFormats = TextureCompressorBackend::GetInputFormats();
    m_InputFormats.reserve(inputFormats.size());
    for (int i = 0; i < inputFormats.size(); ++i)
    {
        auto &pair = inputFormats[i];
        ui->inputFormatsComboBox->addItem(QString::fromStdString(pair.second));
        m_InputFormats.push_back(pair.first);
    }

    auto &compressedFormats = TextureCompressorBackend::GetCompressedFormats();
    m_CompressedFormats.reserve(compressedFormats.size());
    for (int i = 0; i < compressedFormats.size(); ++i)
    {
        auto &pair = compressedFormats[i];
        ui->compressedFormatsComboBox->addItem(QString::fromStdString(pair.second));
        m_CompressedFormats.push_back(pair.first);
    }

    connect(ui->loadButton, &QPushButton::released, this, &TextureCompressorSettings::LoadImage);
    connect(ui->compressButton, &QPushButton::released, this, &TextureCompressorSettings::CompressImage);
}

TextureCompressorSettings::~TextureCompressorSettings()
{
    delete ui;
}

void TextureCompressorSettings::ResizePreview(int width, int height)
{
    if (previewWindow)
    {
        previewWindow->resize(width, height);
    }
}

void TextureCompressorSettings::LoadImage()
{
    auto filename = QFileDialog::getOpenFileName(this, tr("Open Image"), "/", tr("Image Files (*.png)")).toStdString();
    auto textureInfo = TextureCompressorBackend::LoadTexture(filename);

    ResizePreview(textureInfo.Width, textureInfo.Height);

    ui->textureNameLabel->setText(QString::fromStdString("Texture Name: " + std::filesystem::path(filename).filename().string()));
    ui->dimensionsLabel->setText(QString::fromStdString("Dimensions: " + std::to_string(textureInfo.Width) + "x" + std::to_string(textureInfo.Height)));
    ui->formatLabel->setText(QString::fromStdString("Format: " + textureInfo.Format));
    ui->sizeLabel->setText(QString::fromStdString("Size: " + std::to_string(textureInfo.Size) + "KB"));

    m_LoadedTexturePath = filename;
    m_TextureLoaded = true;
}

void TextureCompressorSettings::CompressImage()
{
    if (m_TextureLoaded)
    {
        TextureCompressorBackend::CompressTexture(
            m_LoadedTexturePath, 
            m_InputFormats[ui->inputFormatsComboBox->currentIndex()], 
            m_CompressedFormats[ui->compressedFormatsComboBox->currentIndex()]
        );
        TextureCompressorBackend::LoadTexture(m_LoadedTexturePath);
    }
}