#include <3ds.h>
#include <stdio.h>

Handle mcuhwcHandle;

Result mcuInit()
{
    return srvGetServiceHandle(&mcuhwcHandle, "mcu::HWC");
}

Result mcuExit()
{
    return svcCloseHandle(mcuhwcHandle);
}

//https://www.3dbrew.org/wiki/MCUHWC:SetPowerLEDPattern
Result mcuSetPowerLEDPattern(u8 pattern)
{
    u32* ipc = getThreadCommandBuffer();
    ipc[0] = 0x60040;
    ipc[1] = pattern;
    Result ret = svcSendSyncRequest(mcuhwcHandle);
    if(ret < 0) return ret;
    return ipc[1];
}

//https://www.3dbrew.org/wiki/MCUHWC:SetWifiLEDState
Result mcuSetWifiLEDState(u8 state)
{
    u32* ipc = getThreadCommandBuffer();
    ipc[0] = 0x70040;
    ipc[1] = state;
    Result ret = svcSendSyncRequest(mcuhwcHandle);
    if(ret < 0) return ret;
    return ipc[1];
}

Result mcuGetBatteryLevel(u8* out)
{
    u32* ipc = getThreadCommandBuffer();
    ipc[0] = 0x50000;
    Result ret = svcSendSyncRequest(mcuhwcHandle);
    if(ret < 0) return ret;
	*out = ipc[2];
    return ipc[1];
}

int main()
{

	// Initialize services
	gfxInitDefault();

	//Initialize console on top screen. Using NULL as the second argument tells the console library to use the internal console structure as current one
	consoleInit(GFX_TOP, NULL);

	u8 batteryPercentage;
	mcuInit();
	mcuGetBatteryLevel(&batteryPercentage);
	mcuExit();

	puts("====BiPoLAR v0.01 by Spaqin====\nProgram will automatically quit after an action was taken");
	puts("Press X to disable Power LED.");
	puts("Press Y to enable it.");
  puts("Press A to disable Wifi LED.");
  puts("Press B to enable it.");
	puts("Press START to exit.\n");
	printf("btw your battery %%: %d%%\n", batteryPercentage);

	// Main loop
	while (aptMainLoop())
	{
		//Scan all the inputs. This should be done once for each frame
		hidScanInput();

		u32 kDown = hidKeysDown();

		if (kDown & KEY_START) break; // break in order to return to hbmenu

		if (kDown & KEY_X)
		{
			mcuInit();
			mcuSetPowerLEDPattern(3);
			mcuExit();
			break;
		}
		if (kDown & KEY_Y)
		{
			mcuInit();
			mcuSetPowerLEDPattern(0);
			mcuExit();
			break;
		}
    if (kDown & KEY_A)
    {
      mcuInit();
      mcuSetWifiLEDState(0);
      mcuExit();
      break;
    }
    if (kDown & KEY_B)
    {
      mcuInit();
      mcuSetWifiLEDState(1);
      mcuExit();
      break;
    }
	}

	// Exit services
	gfxExit();
	return 0;
}
