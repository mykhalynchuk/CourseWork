#include "../include/Menu.h"
#include "../include/SeedDemo.h"

#include "../include/ContractedPlayer.h"
#include "../include/FreeAgent.h"
#include "../include/Goalkeeper.h"
#include "../include/AuthManager.h"
#include "../include/FileManager.h"
#include "../include/Utils.h"
#include "../include/InputValidator.h"

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

        if (clubManager_->GetAll().empty())
        {
            std::cout << "[ІНФО] Склад порожній — додаю демо-гравців.\n";
            SeedDemoData(*clubManager_);
            fileManager_.SaveToFile(PLAYERS_DATA_FILE, *clubManager_);
        }
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
            const int choice = InputValidator::GetIntInRange("Ваш вибір: ", 1, 9);
            handleMainMenu(choice);
        }
    }

    void Menu::Stop()
    {
        isRunning_ = false;
        std::cout << "[ІНФО] Завершення роботи програми...\n";
    }

    // ======= АВТЕНТИФІКАЦІЯ (без підказок про 0) =======
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
                    const std::string login = InputValidator::GetNonEmptyString("Логін: ");
                    const std::string password = InputValidator::GetNonEmptyString("Пароль: ");

                    if (AuthManager::GetInstance().Login(login, password))
                        return;

                    std::cout << "[ПОМИЛКА] Спробуйте ще раз.\n";
                }
                Stop(); return;
            }
            else if (cmd == 2)
            {
                const std::string login = InputValidator::GetNonEmptyString("Новий логін: ");
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
                Stop(); return;
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
        case 1: displayPlayerMenu(); break;
        case 2: viewPlayersFlow(); break;
        case 3: searchPlayerFlow(); break;
        case 4: transferFlow(); break;
        case 5:
            if (isAdmin) manageUsersFlow();
            else std::cout << "[ВІДМОВА] Необхідні права адміністратора.\n";
            break;
        case 7: displayHelp(); break;
        case 8: SaveAllData(); break;
        case 9: saveAndExit(); break;
        default: std::cout << "[ПОМИЛКА] Некоректний вибір.\n"; break;
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
        std::cout << "Ця програма дозволяє керувати складом футбольної команди:\n";
        std::cout << "• Додавати або видаляти гравців.\n";
        std::cout << "• Пошук, сортування, фільтрація.\n";
        std::cout << "• Підписання вільних агентів.\n";
        std::cout << "-----------------------------------\n";
    }

    // ======= ПІДМЕНЮ ГРАВЦІВ (додано 0. Назад) =======
    void Menu::displayPlayerMenu() const
    {
        while (true) {
            std::cout << "\n==== МЕНЮ ГРАВЦІВ ====\n";
            std::cout << "1. Додати нового гравця\n";
            std::cout << "2. Видалити гравця за ID\n";
            std::cout << "0. Назад\n";

            int choice = InputValidator::GetIntInRange("Ваш вибір: ", 0, 2);
            if (choice == 0) return;

            if (choice == 1) const_cast<Menu*>(this)->addPlayerFlow();
            if (choice == 2) const_cast<Menu*>(this)->deletePlayerFlow();
        }
    }

    void Menu::deletePlayerFlow() const
    {
        std::string raw = InputValidator::GetNonEmptyString("ID гравця (0 - назад): ");
        if (raw == "0") return;

        try {
            int id = std::stoi(raw);
            clubManager_->RemovePlayers(id);
            std::cout << "[УСПІХ] Якщо гравець з ID " << id << " існував — видалено.\n";
        } catch (...) {
            std::cout << "[ПОМИЛКА] Невірний формат ID.\n";
        }
    }

    void Menu::addPlayerFlow()
    {
        std::cout << "\n--- ДОДАВАННЯ ГРАВЦЯ ---\n";

        std::string name = InputValidator::GetNonEmptyString("Ім'я (0 - назад): ");
        if (name == "0") return;
        int age = InputValidator::GetIntInRange("Вік: ", 16, 45);
        std::string nationality = InputValidator::GetNonEmptyString("Національність (0 - назад): ");
        if (nationality == "0") return;
        std::string origin = InputValidator::GetNonEmptyString("Походження (0 - назад): ");
        if (origin == "0") return;
        double height = InputValidator::GetDoubleInput("Зріст (см): ");
        double weight = InputValidator::GetDoubleInput("Вага (кг): ");
        double value = InputValidator::GetDoubleInput("Ринкова вартість: ");

        int pos = InputValidator::GetIntInRange("Позиція (0=GK, 1=DEF, 2=MID, 3=FWD): ", 0, 3);
        const Position position = static_cast<Position>(pos);

        if (position == Position::Goalkeeper)
        {
            auto gk = std::make_shared<Goalkeeper>(
                name, age, nationality, origin, height, weight, value);
            clubManager_->AddPlayer(gk);
            std::cout << "[УСПІХ] Воротаря додано.\n";
            return;
        }

        int type = InputValidator::GetIntInRange("Тип (1=Контрактний, 2=Вільний агент, 0=Назад): ", 0, 2);
        if (type == 0) return;

        if (type == 1)
        {
            double salary = InputValidator::GetDoubleInput("Річна зарплата: ");
            std::string contractUntil = InputValidator::GetNonEmptyString("Контракт до (YYYY-MM-DD) (0 - назад): ");
            if (contractUntil == "0") return;

            auto player = std::make_shared<ContractedPlayer>(
                name, age, nationality, origin, height, weight, value, position,
                salary, contractUntil);
            clubManager_->AddPlayer(player);
            std::cout << "[УСПІХ] Контрактного гравця додано.\n";
        }
        else // type == 2
        {
            double expectedSalary = InputValidator::GetDoubleInput("Очікувана зарплата: ");
            std::string lastClub = InputValidator::GetNonEmptyString("Останній клуб (0 - назад): ");
            if (lastClub == "0") return;

            auto agent = std::make_shared<FreeAgent>(
                name, age, nationality, origin, height, weight, value, position,
                expectedSalary, lastClub);
            clubManager_->AddPlayer(agent);
            std::cout << "[УСПІХ] Вільного агента додано.\n";
        }
    }

    // ======= ПЕРЕГЛЯД/ПОШУК/СОРТУВАННЯ (додано 0. Назад у меню) =======
    void Menu::viewPlayersFlow() const
    {
        clubManager_->ViewAllPlayers();
        // повернення до меню відбувається після показу списку
    }

    void Menu::searchPlayerFlow() const
    {
        while (true) {
            std::cout << "\n--- ПОШУК ТА ФІЛЬТРАЦІЯ ---\n";
            std::cout << "1. Пошук за ім’ям\n";
            std::cout << "2. Сортування за рейтингом\n";
            std::cout << "3. Фільтрація за статусом\n";
            std::cout << "0. Назад\n";

            int choice = InputValidator::GetIntInRange("Ваш вибір: ", 0, 3);
            if (choice == 0) return;

            if (choice == 1)
            {
                std::string query = InputValidator::GetNonEmptyString("Введіть ім’я або частину (0 - назад): ");
                if (query == "0") continue;

                const auto results = clubManager_->SearchByName(query);
                for (const auto& p : results) {
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
                std::string status = InputValidator::GetNonEmptyString("Статус (0 - назад): ");
                if (status == "0") continue;

                const auto results = clubManager_->FilterByStatus(status);
                for (const auto& p : results) {
                    p->ShowInfo();
                    std::cout << "------------------------\n";
                }
            }
        }
    }

    // ======= ТРАНСФЕРНИЙ РИНОК =======
    void Menu::transferFlow() const
    {
        std::cout << "\n--- ТРАНСФЕРНИЙ РИНОК ---\n";
        std::string nameQuery = InputValidator::GetNonEmptyString("Пошук агента за ім’ям (0 - назад): ");
        if (nameQuery == "0") return;

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
        double offer = InputValidator::GetDoubleInput("Запропонована зарплата: ");
        std::string until = InputValidator::GetNonEmptyString("Контракт до (YYYY-MM-DD) (0 - назад): ");
        if (until == "0") return;

        if (clubManager_->SignFreeAgent(agent, offer, until))
            std::cout << "[УСПІХ] Контракт підписано.\н";
        else
            std::cout << "[ПОМИЛКА] Не вдалося підписати контракт.\n";
    }

    // ======= КОРИСТУВАЧІ (ADMIN) =======
    void Menu::manageUsersFlow()
    {
        while (true) {
            std::cout << "\n--- КЕРУВАННЯ КОРИСТУВАЧАМИ ---\n";
            AuthManager::GetInstance().ViewAllUsers();

            std::cout << "1. Створити користувача\n";
            std::cout << "2. Видалити користувача\n";
            std::cout << "0. Назад\n";

            int choice = InputValidator::GetIntInRange("Ваш вибір: ", 0, 2);
            if (choice == 0) return;

            if (choice == 1)
            {
                std::string login = InputValidator::GetNonEmptyString("Новий логін (0 - назад): ");
                if (login == "0") continue;
                std::string password = InputValidator::GetNonEmptyString("Пароль (0 - назад): ");
                if (password == "0") continue;
                int role = InputValidator::GetIntInRange("Роль (1=Admin, 2=User, 0=Назад): ", 0, 2);
                if (role == 0) continue;

                if (AuthManager::GetInstance().Register(
                        login, password,
                        (role == 1 ? UserRole::Admin : UserRole::StandardUser))) {
                    std::cout << "[УСПІХ] Користувача створено.\n";
                } else {
                    std::cout << "[ПОМИЛКА] Не вдалося створити користувача (логін може існувати).\n";
                }
            }
            else // 2
            {
                std::string login = InputValidator::GetNonEmptyString("Логін користувача для видалення (0 - назад): ");
                if (login == "0") continue;

                if (InputValidator::GetYesNoInput("Підтвердити видалення?"))
                {
                    AuthManager::GetInstance().DeleteUser(login);
                    std::cout << "[УСПІХ] Якщо користувач існував — видалено.\n";
                }
                else
                {
                    std::cout << "[СКАСОВАНО] Видалення перервано.\n";
                }
            }
        }
    }

    void Menu::adminCreateUser()
    {
        // (Не використовується напряму — логіка є у manageUsersFlow)
        std::string login = InputValidator::GetNonEmptyString("Новий логін (0 - назад): ");
        if (login == "0") return;
        std::string password = InputValidator::GetNonEmptyString("Пароль (0 - назад): ");
        if (password == "0") return;
        int role = InputValidator::GetIntInRange("Роль (1=Admin, 2=User, 0=Назад): ", 0, 2);
        if (role == 0) return;

        AuthManager::GetInstance().Register(
            login, password,
            (role == 1 ? UserRole::Admin : UserRole::StandardUser)
        );
        std::cout << "[УСПІХ] Користувача створено.\n";
    }
} // namespace FootballManagement
