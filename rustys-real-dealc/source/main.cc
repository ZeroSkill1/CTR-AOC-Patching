#include <patch.hh>

int loop_3ds()
{
	// Main loop
	while (aptMainLoop())
	{
		gspWaitForVBlank();
		gfxSwapBuffers();
		hidScanInput();

		// Your code goes here
		u32 kDown = hidKeysDown();
		if (kDown & KEY_START)
			break; // break in order to return to hbmenu
	}

	gfxExit();
	amExit();
	fsExit();
	return 0;
}

int main(int argc, char* argv[])
{
	gfxInitDefault();
	amInit();
	fsInit();

	consoleInit(GFX_TOP, NULL);

	Result res = 0;

	if (R_FAILED(res = DLC::RustysRealDealBaseball::Patch()))
	{
		printf("patch fail: %08lX\n", res);
		return loop_3ds();
	}

	printf("patch success\n");

	return loop_3ds();
}
