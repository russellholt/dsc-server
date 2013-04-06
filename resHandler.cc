#include "resHandler.h"

resHandler::resHandler(void)
{
	Sock = NULL;
}

resHandler::resHandler(Socket *sock)
{
	Sock = sock;
}

int resHandler::GetFileInfo(String& path, struct stat& file_info, String& ext)
{
int ext_ind=0;
int err = stat(path.chars(), &file_info);

	if (err)
		return 1;

	ext_ind = path.index(".", -1);

	if (ext_ind >= 0)
		ext = path.after(ext_ind);

	return 0;
}
