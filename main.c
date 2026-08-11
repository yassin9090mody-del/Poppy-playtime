#include <pspkernel.h>
#include <pspdebug.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include <stdlib.h>

PSP_MODULE_INFO("FactoryHorror", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

static int running = 1;
static int x = 240, y = 136;

int exit_callback(int arg1, int arg2, void *common) {
    running = 0;
    return 0;
}

int CallbackThread(SceSize args, void *argp) {
    int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
    sceKernelRegisterExitCallback(cbid);
    sceKernelSleepThreadCB();
    return 0;
}

int setup_callbacks(void) {
    int thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
    if (thid >= 0) sceKernelStartThread(thid, 0, NULL);
    return thid;
}

int main(void) {
    pspDebugScreenInit();
    setup_callbacks();
    pspDebugScreenSetXY(7, 3);
    pspDebugScreenPrintf("FACTORY HORROR - PSP Prototype\n");
    pspDebugScreenPrintf("\n");
    pspDebugScreenPrintf("Move with D-Pad. Find the red door.\n");
    pspDebugScreenPrintf("This is the first playable prototype.\n\n");

    SceCtrlData pad;
    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

    while (running) {
        sceCtrlReadBufferPositive(&pad, 1);

        if (pad.Buttons & PSP_CTRL_UP)    y -= 2;
        if (pad.Buttons & PSP_CTRL_DOWN)  y += 2;
        if (pad.Buttons & PSP_CTRL_LEFT)  x -= 2;
        if (pad.Buttons & PSP_CTRL_RIGHT) x += 2;

        if (x < 0) x = 0;
        if (x > 479) x = 479;
        if (y < 0) y = 0;
        if (y > 271) y = 271;

        pspDebugScreenSetXY(0, 10);
        pspDebugScreenPrintf("PLAYER: X=%03d Y=%03d   ", x, y);
        pspDebugScreenSetXY(0, 12);
        pspDebugScreenPrintf("Next: 3D factory, puzzles, enemy AI.\n");

        sceDisplayWaitVblankStart();
    }

    sceKernelExitGame();
    return 0;
}
