#include "FileDescriptorWrapper.hpp"

FileDescriptorWrapper::FileDescriptorWrapper()
: fd(-1)
{
}

FileDescriptorWrapper::~FileDescriptorWrapper()
{
    if (fd > -1)
    {
        close(fd);
    }
}

void FileDescriptorWrapper::init(int fd)
{
    this->fd = fd;
}
