#include "../include/FileManager.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <filesystem>

namespace fs = std::filesystem;

namespace FootballManagement
{
    std::string FileManager::GetFullPath(const std::string& fileName) const
    {
        fs::path base(directoryPath_);
        fs::path file(fileName);
        fs::path full = base / file;
        return full.string();
    }

    bool FileManager::EnsureDirectoryExists() const
    {
        try
        {
            fs::path base(directoryPath_);
            if (!fs::exists(base))
            {
                fs::create_directories(base);
                std::cout << "[ІНФО] Створено каталог даних: "
                    << base.string() << "\n";
            }
            return true;
        }
        catch (const fs::filesystem_error& e)
        {
            std::cout <<
                "[КРИТИЧНА ПОМИЛКА] Не вдалося забезпечити каталог даних: "
                << e.what() << "\n";
            return false;
        }
    }

    bool FileManager::FileExists(const std::string& fileName) const
    {
        try
        {
            return fs::exists(GetFullPath(fileName));
        }
        catch (const fs::filesystem_error&)
        {
            return false;
        }
    }

    bool FileManager::SaveToFile(const std::string& fileName,
                                 const IFileHandler& serializableObject) const
    {
        if (!EnsureDirectoryExists()) return false;

        const std::string fullPath = GetFullPath(fileName);

        try
        {
            std::ofstream file(fullPath, std::ios::out | std::ios::trunc);
            if (!file.is_open())
                throw std::runtime_error(
                    "Не вдалося відкрити файл для запису.");

            const std::string data = serializableObject.Serialize();
            file << data;
            if (!data.empty() && data.back() != '\n')
                file << '\n';
            file.flush();
            file.close();

            std::cout << "[ІНФО] Дані збережено у файл: " << fileName << "\n";
            return true;
        }
        catch (const std::exception& e)
        {
            std::cout << "[ПОМИЛКА] Не вдалося зберегти дані у файл \""
                << fileName << "\": " << e.what() << "\n";
            return false;
        }
    }

    std::vector<std::string>
    FileManager::LoadFromFile(const std::string& fileName) const
    {
        std::vector<std::string> lines;

        if (!FileExists(fileName))
        {
            std::cout << "[ПОПЕРЕДЖЕННЯ] Файл не знайдено: " << fileName
                << ". Повертається порожній набір даних.\n";
            return lines;
        }

        const std::string fullPath = GetFullPath(fileName);

        try
        {
            std::ifstream file(fullPath);
            if (!file.is_open())
                throw std::runtime_error(
                    "Не вдалося відкрити файл для читання.");

            std::string line;
            while (std::getline(file, line))
            {
                if (!line.empty())
                    lines.push_back(line);
            }
            file.close();
        }
        catch (const std::exception& e)
        {
            std::cout << "[ПОМИЛКА] Не вдалося прочитати дані з \""
                << fileName << "\": " << e.what() << "\n";
            lines.clear();
        }

        return lines;
    }

    void FileManager::DisplayFileContent(const std::string& fileName) const
    {
        if (!FileExists(fileName))
        {
            std::cout << "[ПОМИЛКА] Неможливо відобразити: файл \""
                << fileName << "\" не знайдено.\n";
            return;
        }

        std::cout << "\n--- ВМІСТ ФАЙЛУ " << fileName << " ---\n";
        const auto lines = LoadFromFile(fileName);
        for (const auto& line : lines)
            std::cout << line << "\n";
        std::cout << "----------------------------------------\n\n";
    }

    bool FileManager::DeleteFile(const std::string& fileName) const
    {
        const std::string fullPath = GetFullPath(fileName);
        try
        {
            if (fs::remove(fullPath))
            {
                std::cout << "[ІНФО] Файл успішно видалено: " << fileName <<
                    "\n";
                return true;
            }

            std::cout << "[ПОПЕРЕДЖЕННЯ] Файл не видалено (можливо, не існує): "
                << fileName << "\n";
            return false;
        }
        catch (const fs::filesystem_error& e)
        {
            std::cout << "[ПОМИЛКА] Не вдалося видалити файл \""
                << fileName << "\": " << e.what() << "\n";
            return false;
        }
    }
}
