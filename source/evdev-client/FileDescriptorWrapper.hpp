#pragma once

#include <unistd.h>

struct FileDescriptorWrapper
{
    int fd;

    FileDescriptorWrapper();
    ~FileDescriptorWrapper();

    void init(int fd);
};
