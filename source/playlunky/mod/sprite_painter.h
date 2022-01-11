#pragma once

#include <filesystem>
#include <memory>
#include <mutex>
#include <vector>

#include "util/color.h"
#include "util/image.h"

class PlaylunkySettings;
class SpriteSheetMerger;
class VirtualFilesystem;

class SpritePainter
{
  public:
    SpritePainter(SpriteSheetMerger& merger, VirtualFilesystem& vfs, const PlaylunkySettings& settings);
    SpritePainter(const SpritePainter&) = delete;
    SpritePainter(SpritePainter&&) = delete;
    SpritePainter& operator=(const SpritePainter&) = delete;
    SpritePainter& operator=(SpritePainter&&) = delete;
    ~SpritePainter();

    void RegisterSheet(std::filesystem::path full_path, std::filesystem::path db_destination, bool outdated, bool deleted);

    void FinalizeSetup(const std::filesystem::path& source_folder, const std::filesystem::path& destination_folder);

    bool NeedsWindowDraw();
    void WindowDraw();

    void Update(const std::filesystem::path& source_folder, const std::filesystem::path& destination_folder);

  private:
    struct RegisteredColorModSheet;
    void SetupSheet(RegisteredColorModSheet& sheet);
    bool RepaintImage(const std::filesystem::path& full_path, const std::filesystem::path& db_destination);

    struct FilePair
    {
        std::filesystem::path relative_path;
        std::filesystem::path db_destination;
    };
    FilePair ConvertToRealFilePair(const std::filesystem::path& full_path, const std::filesystem::path& db_destination);
    std::optional<std::filesystem::path> GetSourcePath(const std::filesystem::path& relative_path);
    std::filesystem::path ReplaceColExtension(std::filesystem::path path, std::string_view replacement = "");

    struct RegisteredColorModSheet
    {
        std::filesystem::path full_path;
        std::filesystem::path db_destination;
        bool outdated;

        Image source_image;
        Image color_mod_image;

        std::vector<Image> source_sprites;
        std::vector<Image> color_mod_sprites;
        std::vector<Image> preview_sprites;

        std::vector<struct ID3D11Texture2D*> textures;
        std::vector<struct ID3D11ShaderResourceView*> shader_resource_views;

        std::vector<ColorRGB8> unique_colors;
        std::vector<ColorRGB8> chosen_colors;
        std::vector<bool> color_picker_hovered;
    };
    struct PendingRepaint
    {
        const RegisteredColorModSheet* sheet;
    };

    SpriteSheetMerger& m_Merger;
    VirtualFilesystem& m_Vfs;

    std::vector<RegisteredColorModSheet> m_RegisteredColorModSheets;

    std::vector<PendingRepaint> m_PendingRepaints;
    std::size_t m_RepaintTimestamp{ 0 };
    bool m_HasPendingRepaints{ false };
};