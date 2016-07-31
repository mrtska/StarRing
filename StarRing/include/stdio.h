#pragma once

#include <stdarg.h>

//���Ȃ��݂̊֐� �J�[�l����
int kkprintf(const char *format, ...);
int kprintf(const char *format, ...);
int sprintf(char *data, const char *format, ...);

int vprintf(const char *format, va_list args);
int vvprintf(const char *format, void **args);
int vsprintf(char *buf, const char *format, va_list args);
int vvsprintf(char *buf, const char *format, void **args);
//�J�[�\�������ƂɈʒu�ɖ߂�
void resetPos(void);
