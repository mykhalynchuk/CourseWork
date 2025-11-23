#include "../include/Menu.h"
#include "../include/SeedDemo.h"  // або шлях, де ти покладеш файл

#include "../include/InputValidator.h"
#include "../include/ContractedPlayer.h"
#include "../include/FreeAgent.h"
#include "../include/Goalkeeper.h"
#include "../include/AuthManager.h"
#include "../include/FileManager.h"
#include "../include/Utils.h"

// using
#include <iostream>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <iomanip>

namespace FootballManagement
{
    Menu::Menu(std::shared_ptr<ClubManager> clubManager)
        : clubManager_(std::move(clubManager)), isRunning_(true)
    {
    }

    void Menu::LoadAllData() const
    {
        try {
            const auto userLines = fileManager_.LoadFromFile(USERS_FILE_NAME);
            AuthManager::GetInstance().DeserializeAllUsers(userLines);
            std::cout << "[ІНФО] Дані користувачів завантажено.\n";
        } catch (const std::exception& e) {
            std::cout << "[ПОМИЛКА] Не вдалося завантажити користувачів: " << e.what() << "\n";
        }

        try {
            const auto playerLines = fileManager_.LoadFromFile(PLAYERS_DATA_FILE);
            clubManager_->DeserializeAllPlayers(playerLines);
            std::cout << "[ІНФО] Дані гравців завантажено.\n";
        } catch (const std::exception& e) {
            std::cout << "[ПОМИЛКА] Не вдалося завантажити гравців: " << e.what() << "\n";
        }

        // === Якщо складу немає — засіємо демо-дані
        if (clubManager_->GetAll().empty()) {
            std::cout << "[ІНФО] Склад порожній — додаю демо-гравців.\n";
            SeedDemoData(*clubManager_);
            // одразу збережемо, щоб потім програма вже читала з файлу
            fileManager_.SaveToFile(PLAYERS_DATA_FILE, *clubManager_);
        }
        FootballManagement::SeedDemoData(*clubManager_);
    }


    void Menu::SaveAllData() const
    {
        std::cout << "[ІНФО] Збереження даних...\n";
        fileManager_.SaveToFile(USERS_FILE_NAME, AuthManager::GetInstance());
        fileManager_.SaveToFile(PLAYERS_DATA_FILE, *clubManager_);
        std::cout << "[УСПІХ] Усі дані збережено.\n";
    }

    void Menu::Run()
    {
        LoadAllData();
        authenticateUser();

        if (!AuthManager::GetInstance().IsLoggedIn())
        {
            std::cout << "\nСесію завершено. Програму закрито.\n";
            return;
        }

        while (isRunning_)
        {
            displayMainMenu();
            const int choice = InputValidator::GetIntInRange(
                "Ваш вибір: ", 1, 9);
            handleMainMenu(choice);
        }
    }

    void Menu::Stop()
    {
        isRunning_ = false;
        std::cout << "[ІНФО] Завершення роботи програми...\n";
    }

// Menu.cpp
void Menu::authenticateUser()
{
    std::cout << "========================================\n";
    std::cout << "     СИСТЕМА КЕРУВАННЯ ФУТБОЛЬНИМ КЛУБОМ\n";
    std::cout << "========================================\n";

    while (true)
    {
        std::cout << "\nОберіть дію:\n";
        std::cout << "1) Увійти\n";
        std::cout << "2) Зареєструватися\n";
        std::cout << "3) Увійти як гість\n";
        std::cout << "4) Вийти з програми\n";

        int cmd = InputValidator::GetIntInRange("Ваш вибір: ", 1, 4);

        if (cmd == 1)
        {
            for (int attempts = 0; attempts < 3; ++attempts)
            {
                const std::string login    = InputValidator::GetNonEmptyString("Логін: ");
                const std::string password = InputValidator::GetNonEmptyString("Пароль: ");
                if (AuthManager::GetInstance().Login(login, password))
                {
                    return; // успішний вхід
                }
                std::cout << "[ПОМИЛКА] Спробуйте ще раз.\n";
            }
            std::cout << "[ПОМИЛКА] Перевищено кількість спроб входу.\n";
            Stop();
            return;
        }
        else if (cmd == 2)
        {
            const std::string login    = InputValidator::GetNonEmptyString("Новий логін: ");
            const std::string password = InputValidator::GetNonEmptyString("Пароль (≥6 символів): ");
            const int roleInt = InputValidator::GetIntInRange("Роль (1=User, 0=Admin): ", 0, 1);
            const UserRole role = (roleInt == 0 ? UserRole::Admin : UserRole::StandardUser);

            if (AuthManager::GetInstance().Register(login, password, role))
            {
                fileManager_.SaveToFile(USERS_FILE_NAME, AuthManager::GetInstance());
                std::cout << "[УСПІХ] Обліковий запис створено. Тепер увійдіть.\n";
            }
        }
        else if (cmd == 3)
        {
            AuthManager::GetInstance().LoginAsGuest();
            return;
        }
        else // 4
        {
            Stop();
            return;
        }
    }
}

    void Menu::displayMainMenu() const
    {
        std::cout << "\n=========== ГОЛОВНЕ МЕНЮ ===========\n";
        std::cout << "1. Керування гравцями (Додати / Видалити)\n";
        std::cout << "2. Переглянути всіх гравців\n";
        std::cout << "3. Пошук / Сортування / Фільтрація\n";
        std::cout << "4. Трансферний ринок (Підписати агента)\n";

        if (AuthManager::GetInstance().GetCurrentUser()->IsAdmin())
            std::cout << "5. Керування користувачами (тільки Admin)\n";

        std::cout << "7. Довідка\n";
        std::cout << "8. Зберегти всі дані\n";
        std::cout << "9. Вихід\n";
        std::cout << "====================================\n";
    }

    void Menu::handleMainMenu(int choice)
    {
        const auto user = AuthManager::GetInstance().GetCurrentUser();
        const bool isAdmin = user->IsAdmin();

        switch (choice)
        {
        case 1: displayPlayerMenu();
            break;
        case 2: viewPlayersFlow();
            break;
        case 3: searchPlayerFlow();
            break;
        case 4: transferFlow();
            break;
        case 5:
            if (isAdmin) manageUsersFlow();
            else std::cout << "[ВІДМОВА] Необхідні права адміністратора.\n";
            break;
        case 7: displayHelp();
            break;
        case 8: SaveAllData();
            break;
        case 9: saveAndExit();
            break;
        default: std::cout << "[ПОМИЛКА] Некоректний вибір.\n";
            break;
        }
    }

    void Menu::saveAndExit()
    {
        if (InputValidator::GetYesNoInput("Зберегти зміни перед виходом?"))
            SaveAllData();

        AuthManager::GetInstance().Logout();
        Stop();
    }

    void Menu::displayHelp() const
    {
        std::cout << "\n--- ДОВІДКА ---\n";
        std::cout <<
            "Ця програма дозволяє керувати складом футбольної команди:\n";
        std::cout << "• Додавати або видаляти гравців.\n";
        std::cout << "• Пошук, сортування, фільтрація.\n";
        std::cout << "• Підписання вільних агентів.\n";
        std::cout << "-----------------------------------\n";
    }

    void Menu::displayPlayerMenu() const
    {
        std::cout << "\n==== МЕНЮ ГРАВЦІВ ====\n";
        std::cout << "1. Додати нового гравця\n";
        std::cout << "2. Видалити гравця за ID\n";
        std::cout << "3. Назад\n";

        const int choice = InputValidator::GetIntInRange("Ваш вибір: ", 1, 3);
        if (choice == 1) const_cast<Menu*>(this)->addPlayerFlow();
        if (choice == 2) const_cast<Menu*>(this)->deletePlayerFlow();
    }

    void Menu::deletePlayerFlow() const
    {
        const int id = InputValidator::GetIntInput(
            "Введіть ID гравця для видалення: ");
        clubManager_->RemovePlayers(id);
    }

    void Menu::addPlayerFlow()
    {
        std::cout << "\n--- ДОДАВАННЯ ГРАВЦЯ ---\n";

        const std::string name = InputValidator::GetNonEmptyString("Ім'я: ");
        const int age = InputValidator::GetIntInRange("Вік: ", 16, 45);
        const std::string nationality = InputValidator::GetNonEmptyString(
            "Національність: ");
        const std::string origin = InputValidator::GetNonEmptyString(
            "Походження: ");
        const double height = InputValidator::GetDoubleInput("Зріст (см): ");
        const double weight = InputValidator::GetDoubleInput("Вага (кг): ");
        const double value = InputValidator::GetDoubleInput(
            "Ринкова вартість: ");

        const int pos = InputValidator::GetIntInRange(
            "Позиція (0=GK, 1=DEF, 2=MID, 3=FWD): ", 0, 3);
        const Position position = static_cast<Position>(pos);

        // Особливий випадок: воротар — окремий клас Goalkeeper.
        if (position == Position::Goalkeeper)
        {
            auto gk = std::make_shared<Goalkeeper>(
                name, age, nationality, origin, height, weight, value);
            clubManager_->AddPlayer(gk);
            return;
        }

        // Для польових гравців (DEF/MID/FWD) пропонуємо Контрактний/Вільний агент:
        const int type = InputValidator::GetIntInRange(
            "Тип (1=Контрактний, 2=Вільний агент): ", 1, 2);

        if (type == 1)
        {
            const double salary = InputValidator::GetDoubleInput(
                "Річна зарплата: ");
            const std::string contractUntil =
                InputValidator::GetNonEmptyString("Контракт до (YYYY-MM-DD): ");

            auto player = std::make_shared<ContractedPlayer>(
                name, age, nationality, origin, height, weight, value, position,
                salary, contractUntil);
            clubManager_->AddPlayer(player);
        }
        else
        {
            const double expectedSalary = InputValidator::GetDoubleInput(
                "Очікувана зарплата: ");
            const std::string lastClub = InputValidator::GetNonEmptyString(
                "Останній клуб: ");

            auto agent = std::make_shared<FreeAgent>(
                name, age, nationality, origin, height, weight, value, position,
                expectedSalary, lastClub);
            clubManager_->AddPlayer(agent);
        }
    }

    void Menu::viewPlayersFlow() const
    {
        clubManager_->ViewAllPlayers();
    }

    void Menu::searchPlayerFlow() const
    {
        std::cout << "\n--- ПОШУК ТА ФІЛЬТРАЦІЯ ---\n";
        std::cout << "1. Пошук за ім’ям\n";
        std::cout << "2. Сортування за рейтингом\n";
        std::cout << "3. Фільтрація за статусом\n";

        const int choice = InputValidator::GetIntInRange("Ваш вибір: ", 1, 3);

        if (choice == 1)
        {
            const std::string query = InputValidator::GetNonEmptyString(
                "Введіть ім’я або частину: ");
            const auto results = clubManager_->SearchByName(query);
            for (const auto& p : results)
            {
                p->ShowInfo();
                std::cout << "------------------------\n";
            }
        }
        else if (choice == 2)
        {
            clubManager_->SortByPerformanceRating();
            clubManager_->ViewAllPlayers();
        }
        else if (choice == 3)
        {
            const std::string status = InputValidator::GetNonEmptyString(
                "Введіть статус (наприклад: Активний, Травмований, В оренді): ");
            const auto results = clubManager_->FilterByStatus(status);
            for (const auto& p : results)
            {
                p->ShowInfo();
                std::cout << "------------------------\n";
            }
        }
    }

    void Menu::transferFlow() const
    {
        std::cout << "\n--- ТРАНСФЕРНИЙ РИНОК ---\n";
        const std::string nameQuery = InputValidator::GetNonEmptyString(
            "Пошук агента за ім’ям: ");
        const auto found = clubManager_->SearchByName(nameQuery);

        std::shared_ptr<FreeAgent> agent = nullptr;
        for (const auto& p : found)
        {
            agent = std::dynamic_pointer_cast<FreeAgent>(p);
            if (agent && agent->IsAvailableForNegotiation())
                break;
        }

        if (!agent)
        {
            std::cout << "[ПОМИЛКА] Доступного вільного агента не знайдено.\n";
            return;
        }

        std::cout << "[ІНФО] Знайдено агента: " << agent->GetName() << "\n";
        const double offer = InputValidator::GetDoubleInput(
            "Запропонована зарплата: ");
        const std::string until = InputValidator::GetNonEmptyString(
            "Контракт до (YYYY-MM-DD): ");

        if (clubManager_->SignFreeAgent(agent, offer, until))
            std::cout << "[УСПІХ] Контракт підписано.\n";
        else
            std::cout << "[ПОМИЛКА] Не вдалося підписати контракт.\n";
    }

    void Menu::manageUsersFlow()
    {
        std::cout << "\n--- КЕРУВАННЯ КОРИСТУВАЧАМИ ---\n";
        AuthManager::GetInstance().ViewAllUsers();

        std::cout << "1. Створити користувача\n";
        std::cout << "2. Видалити користувача\n";
        const int choice = InputValidator::GetIntInRange("Ваш вибір: ", 1, 2);

        if (choice == 1)
        {
            adminCreateUser();
        }
        else
        {
            const std::string login =
                InputValidator::GetNonEmptyString(
                    "Логін користувача для видалення: ");
            AuthManager::GetInstance().DeleteUser(login);
        }
    }

    void Menu::adminCreateUser()
    {
        const std::string login = InputValidator::GetNonEmptyString(
            "Новий логін: ");
        const std::string password = InputValidator::GetNonEmptyString(
            "Пароль: ");
        const int role = InputValidator::GetIntInRange(
            "Роль (1=Admin, 2=User): ", 1, 2);

        AuthManager::GetInstance().Register(
            login, password,
            (role == 1 ? UserRole::Admin : UserRole::StandardUser)
        );
    }
} // namespace FootballManagement
