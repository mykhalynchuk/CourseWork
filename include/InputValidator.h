#pragma once

#include <string>

namespace FootballManagement
{
    /**
     * @file InputValidator.h
     * @brief Утилітарний клас для безпечного введення даних з консолі.
     *
     * Надає статичні методи для зчитування цілих/дійсних чисел, рядків
     * та відповідей типу «так/ні» з повідомленнями українською мовою.
     */
    class InputValidator
    {
    private:

    public:
        InputValidator() = delete; ///< Заборонено створювати екземпляри.
        ~InputValidator() = delete; ///< Заборонено знищувати екземпляри.

    public:
        /**
         * @brief Зчитує ціле число з підказкою.
         * @param prompt Підказка користувачу.
         * @return Введене ціле число.
         */
        [[nodiscard]] static int GetIntInput(const std::string& prompt);

        /**
         * @brief Зчитує ціле число у вказаному діапазоні.
         * @param prompt Підказка користувачу.
         * @param min Мінімальне значення.
         * @param max Максимальне значення.
         * @return Валідне ціле число у діапазоні [min; max].
         */
        [[nodiscard]] static int GetIntInRange(const std::string& prompt,
                                               int min, int max);

        /**
         * @brief Зчитує дійсне число (double) з підказкою.
         * @param prompt Підказка користувачу.
         * @return Введене дійсне число.
         */
        [[nodiscard]] static double GetDoubleInput(const std::string& prompt);

        /**
         * @brief Зчитує непорожній рядок.
         * @param prompt Підказка користувачу.
         * @return Непорожній рядок.
         */
        [[nodiscard]] static std::string GetNonEmptyString(
            const std::string& prompt);

        /**
         * @brief Зчитує відповідь типу «так/ні».
         * @param prompt Підказка користувачу (додасться «(Y/N)»).
         * @return true, якщо відповідь «Y/y», інакше false (для «N/n»).
         */
        [[nodiscard]] static bool GetYesNoInput(const std::string& prompt);

    private:
        /// @brief Очищає стан потоку вводу та скидає буфер до кінця рядка.
        static void ClearInputBuffer();
    };
}
