#pragma once

int os_fd_set_cloexec(int fd);
int set_cloexec_or_close(int fd);
int create_tmpfile_cloexec(char *tmpname);
int os_create_anonymous_file(off_t size);
