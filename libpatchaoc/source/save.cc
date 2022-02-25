#include <save.hh>

Result DLC::SaveData::OpenExtDataArchive(u32 unique_id)
{
	const u32 path[3] = { MEDIATYPE_SD, unique_id, 0 };
	return FSUSER_OpenArchive(&this->Archive, ARCHIVE_EXTDATA, { PATH_BINARY, 12, path });
}

Result DLC::SaveData::OpenSaveDataArchive(u32 unique_id)
{
	const u32 path[3] = { MEDIATYPE_SD, unique_id, 0x00040000 };
	return FSUSER_OpenArchive(&this->Archive, ARCHIVE_USER_SAVEDATA, { PATH_BINARY, 12, path });
}

DLC::SaveData::SaveData(u32 unique_id, bool is_save)
{
	this->CurrentResult = (this->IsSave = is_save) ?
		this->OpenSaveDataArchive(unique_id) :
		this->OpenExtDataArchive(unique_id);
	this->IsClosed = false;
}

DLC::SaveData::~SaveData()
{
	if (!this->IsClosed)
	{
		if (R_FAILED(this->Close(true)) && this->CurrentResult == CANNOT_CLOSE_ARCHIVE_FILES_OPEN)
		{
			for (const Handle& handle : this->OpenFiles)
				this->CloseFile(handle);

			this->Close(true);
		}
	}
}

Result DLC::SaveData::OpenFile(Handle *out_handle, const char *path, u32 open_flags, u32 attributes)
{
	if (this->IsClosed)
		return ARCHIVE_CLOSED;
	else if (!out_handle)
		return INVALID_POINTER;

	Handle file;

	char16_t filename[256];

	utf8_to_utf16((u16 *)filename, (const uint8_t *)path, 256);

	if (R_FAILED(this->CurrentResult = FSUSER_OpenFile(&file, this->Archive, fsMakePath(PATH_UTF16, filename), open_flags, attributes)))
		return this->CurrentResult;

	this->OpenFiles.insert(file);
	*out_handle = file;
	return 0;
}

Result DLC::SaveData::ReadFileIntoBuffer(u8 *&out_buf, Handle file, u64 *out_size, bool close_file)
{
	if (this->IsClosed)
		return ARCHIVE_CLOSED;
	else if (!file)
		return INVALID_HANDLE;
	else if (this->OpenFiles.find(file) == this->OpenFiles.end())
		return FILE_NOT_FOUND;
	
	u64 size;

	if (R_FAILED(this->CurrentResult = FSFILE_GetSize(file, &size)))
		return this->CurrentResult;

	u8 *buf = new u8[size];
	u32 read;

	if ((R_FAILED(this->CurrentResult = FSFILE_Read(file, &read, 0, buf, (u32)size))) ||
		(close_file && R_FAILED(this->CloseFile(file))))
	{
		delete [] buf;
		return this->CurrentResult;
	}

	out_buf = buf;
	if (out_size) *out_size = size;
	return 0;
}

Result DLC::SaveData::ReadFileIntoBuffer(u8 *&out_buf, const char *path, u64 *out_size)
{
	Handle file;

	if (R_FAILED(this->CurrentResult = this->OpenFile(&file, path, FS_OPEN_READ)) ||
		R_FAILED(this->CurrentResult = this->ReadFileIntoBuffer(out_buf, file, out_size, true)))
		return this->CurrentResult;

	return 0;
}

Result DLC::SaveData::WriteFileFromBuffer(u8 *buf, Handle file, u32 size, bool close_file)
{
	if (!buf)
		return INVALID_POINTER;
	else if (!file)
		return INVALID_HANDLE;
	else if (this->OpenFiles.find(file) == this->OpenFiles.end())
		return FILE_NOT_FOUND;

	u32 written;

	if (R_FAILED(this->CurrentResult = FSFILE_SetSize(file, size)) ||
		R_FAILED(this->CurrentResult = FSFILE_Write(file, &written, 0LL, buf, size, 0)) ||
		(close_file && R_FAILED(this->CurrentResult = this->CloseFile(file))))
		return this->CurrentResult;

	if (written != size)
		return INCOMPLETE_WRITE;

	return 0;
}

Result DLC::SaveData::WriteFileFromBuffer(u8 *buf, const char *path, u32 size)
{
	Handle file;

	if (R_FAILED(this->CurrentResult = this->OpenFile(&file, path, FS_OPEN_WRITE)) ||
		R_FAILED(this->CurrentResult = this->WriteFileFromBuffer(buf, file, size, true)))
		return this->CurrentResult;

	return 0;
}

Result DLC::SaveData::CloseFile(Handle file)
{
	if (!file)
		return INVALID_HANDLE;
	else if (this->OpenFiles.find(file) == this->OpenFiles.end())
		return FILE_NOT_FOUND;

	if (R_FAILED(this->CurrentResult = FSFILE_Close(file)))
		return this->CurrentResult;

	this->OpenFiles.erase(file);
	return 0;
}

Result DLC::SaveData::Close(bool discard_changes)
{
	if (this->IsClosed)
		return ARCHIVE_CLOSED;
	else if (this->OpenFiles.size() > 0)
		return CANNOT_CLOSE_ARCHIVE_FILES_OPEN;

	if ((this->IsSave && !discard_changes && R_FAILED(this->CurrentResult = FSUSER_ControlArchive(this->Archive, ARCHIVE_ACTION_COMMIT_SAVE_DATA, nullptr, 0, nullptr, 0))) ||
		R_FAILED(this->CurrentResult = FSUSER_CloseArchive(this->Archive)))
		return this->CurrentResult;

	this->IsClosed = true;
	return 0;
}