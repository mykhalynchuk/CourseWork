#include "../include/InputValidator.h"

// using
#include <iostream>
#include <limits>
#include <algorithm>
#include <cctype>

namespace FootballManagement
{
    void InputValidator::ClearInputBuffer()
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    int InputValidator::GetIntInput(const std::string& prompt)
    {
        int value;
        while (true)
        {
            std::cout << prompt;
            if (std::cin >> value)
            {
                ClearInputBuffer();
                return value;
            }
            std::cout <<
                "[ПОМИЛКА] Введено некоректне значення. Введіть ціле число.\n";
            ClearInputBuffer();
        }
    }

    int InputValidator::GetIntInRange(const std::string& prompt, int min,
                                      int max)
    {
        while (true)
        {
            const int value = GetIntInput(
                prompt + " (" + std::to_string(min) + " - " +
                std::to_string(max) + "): "
            );

            if (value >= min && value <= max)
                return value;

            std::cout << "[ПОМИЛКА] Значення має бути між " << min << " і " <<
                max << ".\n";
        }
    }

    double InputValidator::GetDoubleInput(const std::string& prompt)
    {
        double value;
        while (true)
        {
            std::cout << prompt;
            if (std::cin >> value)
            {
                ClearInputBuffer();
                return value;
            }
            std::cout <<
                "[ПОМИЛКА] Введіть коректне число (наприклад, 12.5).\n";
            ClearInputBuffer();
        }
    }

    std::string InputValidator::GetNonEmptyString(const std::string& prompt)
    {
        std::string input;
        while (true)
        {
            std::cout << prompt;
            std::getline(std::cin, input);

            if (!input.empty())
                return input;

            std::cout <<
                "[ПОМИЛКА] Рядок не може бути порожнім. Спробуйте ще раз.\n";
        }
    }

    bool InputValidator::GetYesNoInput(const std::string& prompt)
    {
        std::string input;
        while (true)
        {
            std::cout << prompt << " (Y/N): ";
            std::getline(std::cin, input);
            if (input.empty()) continue;

            const char answer = static_cast<char>(std::tolower(
                static_cast<unsigned char>(input[0])));
            if (answer == 'y') return true;
            if (answer == 'n') return false;

            std::cout << "[ПОМИЛКА] Введіть 'Y' або 'N'.\n";
        }
    }
} // namespace FootballManagement
