#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <windows.h>

#include "main.h"

int
main()
{
    while (true)
    {
        system("cls");
        if (handle_input()) break;

        setup_if_first_run();

        if (hwnd_game != nullptr)
        {
            fetch_commands();

            std::cout << "Switching to game" << std::endl;
            switch_to_game();

            std::cout << "Running commands" << std::endl;
            send_commands();

            std::cout << "Switching to console" << std::endl;
            switch_to_console();
        }
    }

    return 0;
}

bool
handle_input()
{
    std::cout << "q: quit \nenter: run\n\nEnter a command: ";

    const char input = std::cin.get();

    if (input == 'q') return true;
    return false;
}

void
setup_if_first_run()
{
    system("cls");
    if (first_run)
    {
        hwnd_console = GetConsoleWindow();

        std::cout << "Locating " << title << "\n";
        hwnd_game = find_game_hwnd();
        std::cout << title << " HWND: " << hwnd_game << std::endl;
        std::cout << "Console HWND: " << hwnd_console << std::endl << std::endl;
        std::cout << "Press return to continue";
        std::cin.get();

        if (hwnd_game != nullptr) first_run = false;
    }
}

HWND
find_game_hwnd()
{
    return FindWindowA(
        nullptr,
        static_cast<LPCSTR>(title));
}

void
switch_window(const HWND hwnd)
{
    SetForegroundWindow(hwnd);
    SetFocus(hwnd);
}

void
switch_to_game()
{
    switch_window(hwnd_game);
}

void
switch_to_console()
{
    switch_window(hwnd_console);
}

void
fetch_commands()
{
    commands.clear();

    std::ifstream file(command_file_path);
    std::string line;
    while (std::getline(file, line))
    {
        if (line[0] == '#') continue;

        std::istringstream iss(line);
        char input;
        long duration;

        if (!(iss >> input >> duration)) continue;

        WORD input_code;
        switch (input)
        {
        case 'a':
        case 'A':
            input_code = VK_A;
            break;
        case 'd':
        case 'D':
            input_code = VK_D;
            break;
        case 's':
        case 'S':
            input_code = VK_SPACE;
            break;
        default:
            input_code = 0xFF;
        }

        commands.emplace_back(input_code, duration);
    }
}

void
send_commands()
{
    for (std::tuple<WORD, long> command : commands)
    {
        switch (std::get<0>(command))
        {
        case VK_A:
        case VK_D:
        case VK_SPACE:
            send_key_for_milliseconds(
                std::get<0>(command),
                std::get<1>(command));
            break;
        default:
            sleep(std::get<1>(command));
            break;
        }
    }
}

void
sleep(long duration)
{
    std::cout << "Sleeping for " << duration << " ms" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(duration));
}

void
send_key_for_milliseconds(WORD input, long duration)
{
    WPARAM key_code = input;
    uint32_t scan_code = MapVirtualKeyA(input, MAPVK_VK_TO_VSC);

    std::cout << "-------------------" << std::endl;
    std::cout << "Sending '" << input << "'" << std::endl;
    PostMessageA(hwnd_game, WM_KEYDOWN, key_code, 1 | (scan_code << 16));

    sleep(duration);

    std::cout << "Stopping '" << input << "'" << std::endl;
    std::cout << "-------------------" << std::endl;
    PostMessageA(hwnd_game, WM_KEYUP, key_code, 1 | (scan_code << 16) | (1 << 30));
}
