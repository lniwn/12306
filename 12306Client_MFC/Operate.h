#pragma once

int utf8ToUnicode(const char* src, int src_len, wchar_t* dst);

int unicodeToUtf8(const wchar_t* src, int src_len, char* dst);

HGLOBAL CreateStreamFromMemory(void* src, size_t len, IStream** ppStream);