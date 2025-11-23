#include <iostream>
#include <memory>

#include "../include/Menu.h"
#include "../include/ClubManager.h"

int main()
{
    using namespace FootballManagement;

    // пришвидшуємо консольний ввід/вивід
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    try
    {
        // Ініціалізуємо клуб і меню
        auto club = std::make_shared<ClubManager>("Динамо Київ", 50'000'000.0);

        Menu menu(club);
        menu.Run();

        return 0; // акуратне завершення
    }
    catch (const std::exception& e)
    {
        std::cerr << "[КРИТИЧНА ПОМИЛКА] " << e.what() << "\n";
        return 1;
    }
    catch (...)
    {
        std::cerr << "[КРИТИЧНА ПОМИЛКА] Невідома помилка.\n";
        return 2;
    }
}
