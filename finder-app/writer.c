#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

int main(int argc, char* argv[])
{
    const char* filePath = argv[1];
    const char* writeStr = argv[2];

    if (filePath == NULL || writeStr == NULL)
    {
	syslog(LOG_ERR, "Failed to provide parameters");
	return 1;
    }

    openlog(NULL, 0, LOG_USER);

    int fd = open(filePath, O_CREAT | O_WRONLY, 0777);
    
    if (fd == -1)
    {
        syslog(LOG_ERR, "Failed to access file descriptor");
	return 1;
    }

    ssize_t sz = write(fd, writeStr, strlen(writeStr));
    if (sz == -1)
    {
	syslog(LOG_ERR, "Failed to write to file");
	return 1;
   }

    syslog(LOG_DEBUG, "Writing %s to %s", filePath, writeStr);

    int closeStatus = close(fd);

    if (closeStatus == -1)
    {
	syslog(LOG_ERR, "Failed to close file descriptor");
	return 1;
    }

    return 0;
}
