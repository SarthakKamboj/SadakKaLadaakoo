#pragma once

#include <cstdio>
#include <stdio.h>

#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <WindowsX.h>

#include "skl_d3d12.h"
#include "defines.h"
#include "../app.h"

LRESULT CALLBACK window_procedure(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

struct win32_init_ctx_t {
  HINSTANCE h_instance;
  PWSTR p_cmd_line_args;
  int n_show_state;
};

struct win32_ctx_t {
  HWND hwd;
};

void convert_char_to_wchar(const char* input, int inputLen, __SKL_OUT__ wchar_t* output, int wchar_buffer_len);

void app_run_platform_specific(const window_info_t& window_info, const win32_init_ctx_t& context);
