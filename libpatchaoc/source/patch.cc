#include <patch.hh>

static u32 crc32(u8 *buf, u32 len)
{
    u32 ret = 0xffffffff;

    for(u32 i = 0; i < len; ++i)
        ret = crc32_lookup[(ret & 0xFF) ^ buf[i]] ^ (ret >> 8);

    return ~ret;
}

Result DLC::TitleExists(const DLC::PatchTitle &title)
{
	u64 title_ids[3] =
	{
		0x0004000000000000 | (u64)title.UniqueID, // base title
		0x0004008C00000000 | (u64)title.UniqueID, // dlc title
		0x0004000E00000000 | (u64)title.UniqueID  // update title
	};

	bool uses_dlc = (int)title.Flags & (int)PatchTitleFlags::HasDLC;
	bool uses_update = (int)title.Flags & (int)PatchTitleFlags::HasUpdate;

	AM_TitleEntry title_entries[3];

	u8 count = 1;

	if (uses_dlc)    count++;
	if (uses_update) count++;

	Result res;

	if (R_FAILED(res = AM_GetTitleInfo(MEDIATYPE_SD, count, title_ids, title_entries)))
		return res;

	if ((title_entries[0].version != title.TitleVersion) || (uses_dlc && title_entries[1].version != title.DLCTitleVersion) || (uses_update && title_entries[2].version != title.UpdateTitleVersion))
		return TITLE_VERSION_MISMATCH;

	DLC::SaveData savedata(title.UniqueID, (bool)((int)title.Flags & (int)PatchTitleFlags::UsesSaveData));

	if (R_FAILED(savedata.CurrentResult) ||
		R_FAILED(savedata.Close(true)))
		return savedata.CurrentResult;

	return 0;
}

Result DLC::TitlesExist(const PatchTitle *titles, u32 *out, int count)
{
	if (!out)
		return INVALID_POINTER;

	Result res = 0;
	u32 ret = 0;

	for (int i = 0; i < count; i++)
	{
		if (R_FAILED(res = DLC::TitleExists(titles[i])) && res != (Result)0xD8A083FA)
			return res;

		if (res == (Result)0xD8A083FA) // am title not found
			continue;

		ret |= (1 << i);
	}

	*out = ret;

	return 0;
}

Result DLC::RustysRealDealBaseball::Patch()
{
	Result res;
	u32 titles;
	std::vector<const DLC::PatchTitle *> patch_titles;

	if (R_FAILED(res = DLC::TitlesExist(DLC::RustysRealDealBaseball::Titles, &titles, 3)))
		return res;
	else if (titles == 0)
		return TITLES_NOT_FOUND;

	if (titles & 0x1) // USA
		patch_titles.push_back(&DLC::RustysRealDealBaseball::Titles[0]);

	if (titles & 0x2) // KOR
		patch_titles.push_back(&DLC::RustysRealDealBaseball::Titles[1]);

	if (titles & 0x4) // JPN
		patch_titles.push_back(&DLC::RustysRealDealBaseball::Titles[2]);

	for (const DLC::PatchTitle *&title : patch_titles)
	{
		DLC::SaveData savedata(title->UniqueID, true);

		if (R_FAILED(savedata.CurrentResult))
			return savedata.CurrentResult;

		u8 *buf;
		u64 size;

		if (R_FAILED(savedata.ReadFileIntoBuffer(buf, "/save00.bin", &size)))
		{
			res = savedata.CurrentResult;
			savedata.Close(true);
			return res;
		}

		for (u32 i = 0; i < 10; i++)
		{
			buf[0x3E4 + i] = i;
		}

		memset(buf + 0x3EE, 0x01, 10);
		*((u32 *)(buf + size - 4)) = crc32(buf, size - 4);

		if (R_FAILED(savedata.WriteFileFromBuffer(buf, "/save00.bin", (u32)size)))
		{
			res = savedata.CurrentResult;
			savedata.Close(true);
			return res;
		}

		if (R_FAILED(savedata.Close()))
		{
			delete [] buf;
			return savedata.CurrentResult;
		}

		FILE *f = fopen("/frii.bin", "w");
		fwrite(buf, 1, (u32)size, f);
		fclose(f);

		delete [] buf;
	}

	return 0;
}