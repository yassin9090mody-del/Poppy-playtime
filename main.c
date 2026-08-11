#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include <pspgu.h>
#include <pspgum.h>
#include <stdlib.h>

PSP_MODULE_INFO("Vector PSP", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);

#define SCREEN_W 480
#define SCREEN_H 272

typedef struct {
    float x, y;
    unsigned int color;
} Vertex;

static unsigned int __attribute__((aligned(16))) list[262144];

static void drawRect(float x, float y, float w, float h, unsigned int color)
{
    Vertex *v = (Vertex *)sceGuGetMemory(2 * sizeof(Vertex));

    v[0].x = x;
    v[0].y = y;
    v[0].color = color;

    v[1].x = x + w;
    v[1].y = y + h;
    v[1].color = color;

    sceGuDrawArray(
        GU_SPRITES,
        GU_COLOR_8888 |
        GU_VERTEX_32BITF |
        GU_TRANSFORM_2D,
        2,
        0,
        v
    );
}

int main(void)
{
    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

    sceGuInit();

    sceGuStart(GU_DIRECT, list);

    sceGuDrawBuffer(
        GU_PSM_8888,
        (void *)0,
        512
    );

    sceGuDispBuffer(
        SCREEN_W,
        SCREEN_H,
        (void *)0x88000,
        512
    );

    sceGuDepthBuffer(
        (void *)0x110000,
        512
    );

    sceGuOffset(2048 - SCREEN_W / 2,
                2048 - SCREEN_H / 2);

    sceGuViewport(
        2048,
        2048,
        SCREEN_W,
        SCREEN_H
    );

    sceGuScissor(0, 0, SCREEN_W, SCREEN_H);
    sceGuEnable(GU_SCISSOR_TEST);

    sceGuDisable(GU_DEPTH_TEST);
    sceGuDisable(GU_TEXTURE_2D);

    sceGuFinish();
    sceGuSync(0, 0);

    sceDisplayWaitVblankStart();
    sceGuDisplay(GU_TRUE);

    float playerX = 70;
    float playerY = 200;

    float velocityY = 0;
    int onGround = 1;

    float obstacleX = 350;
    float obstacleY = 215;

    while (1)
    {
        SceCtrlData pad;
        sceCtrlReadBufferPositive(&pad, 1);

        /* القفز */
        if ((pad.Buttons & PSP_CTRL_CROSS) && onGround)
        {
            velocityY = -9.5f;
            onGround = 0;
        }

        /* الجاذبية */
        velocityY += 0.45f;
        playerY += velocityY;

        /* الأرض */
        if (playerY >= 200)
        {
            playerY = 200;
            velocityY = 0;
            onGround = 1;
        }

        /* العالم يتحرك ناحية اللاعب */
        obstacleX -= 3.0f;

        if (obstacleX < -40)
            obstacleX = 520;

        /* اصطدام بسيط */
        if (playerX + 20 > obstacleX &&
            playerX < obstacleX + 25 &&
            playerY + 40 > obstacleY)
        {
            playerX = 70;
            obstacleX = 350;
        }

        sceGuStart(GU_DIRECT, list);

        /* خلفية */
        sceGuClearColor(0xFFF2F2F2);
        sceGuClear(GU_COLOR_BUFFER_BIT);

        /*
         * الأرض
         */
        drawRect(
            0, 240,
            480, 32,
            0xFF202020
        );

        /*
         * عائق
         */
        drawRect(
            obstacleX,
            obstacleY,
            25,
            25,
            0xFF202020
        );

        /*
         * جسم اللاعب
         */
        drawRect(
            playerX,
            playerY,
            20,
            40,
            0xFF000000
        );

        /*
         * رأس اللاعب
         */
        drawRect(
            playerX + 2,
            playerY - 10,
            16,
            12,
            0xFF000000
        );

        sceGuFinish();
        sceGuSync(0, 0);

        sceDisplayWaitVblankStart();
        sceGuSwapBuffers();
    }

    sceGuTerm();

    sceKernelExitGame();
    return 0;
}
