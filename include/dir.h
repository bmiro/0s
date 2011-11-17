#ifndef __DIR_H__
#define __DIR_H__

#define VIRTUAL_KEYBOARD_PATH "KEYBOARD"
#define VIRTUAL_DISPLAY_PATH "DISPLAY"

#define DIR_SIZE 10

#define FILE_NAME_SIZE 10

struct dir_entry {
  char name[FILE_NAME_SIZE];
  int size;
  int file;
}

/* Inicialitzes KEYBOARD and DISPLAY */
void init_dir();

int find_path(const char *path);

#endif