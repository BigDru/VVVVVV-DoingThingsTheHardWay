#ifndef MAIN_H
#define MAIN_H

#pragma once

#define VK_A (0x41)
#define VK_D (0x44)

#include <vector>

const char * title = "VVVVVV";
const char * command_file_path = R"(C:\commands.txt)";

HWND hwnd_console = nullptr;
HWND hwnd_game = nullptr;
bool first_run = true;
std::vector<std::tuple<WORD, long>> commands;

bool handle_input();

void setup_if_first_run();
HWND find_game_hwnd();
HWND find_notepad_hwnd();

void switch_window(const HWND hwnd);
void switch_to_game();
void switch_to_console();

void fetch_commands();
void send_commands();
void sleep(long duration);
void send_key_for_milliseconds(WORD input, long duration);

#endif // MAIN_H
