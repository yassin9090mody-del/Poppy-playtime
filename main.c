#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include <pspgu.h>
#include <stdlib.h>

PSP_MODULE_INFO("Shadow Runner", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);

#define SCREEN_W 480
#define SCREEN_H 272

static unsigned int __attribute__((aligned(16))) list[262144];

typedef struct
{
    float x;
    float y;
    float w;
    float h;
    unsigned int color;
} RectVertex;

static void drawRect(float x, float y, float w, float h, unsigned int color)
{
    RectVertex *v = (RectVertex *)sceGuGetMemory(sizeof(RectVertex) * 2);

    v[0].x = x;
    v[0].y = y;
    v[0].w = 0;
    v[0].h = 0;
    v[0].color = color;

    v[1].x = x + w;
    v[1].y = y + h;
    v[1].w = 0;
    v[1].h = 0;
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

static void drawPlayer(float x, float y)
{
    /* الرأس */
    drawRect(x + 6, y - 10, 14, 14, 0xFF050505);

    /* الجسم */
    drawRect(x + 8, y + 4, 10, 25, 0xFF050505);

    /* الذراع */
    drawRect(x + 1, y + 8, 10, 5, 0xFF050505);

    /* الرجلان */
    drawRect(x + 6, y + 27, 6, 18, 0xFF050505);
    drawRect(x + 14, y + 27, 6, 18, 0xFF050505);
}

static void drawChaser(float x, float y)
{
    /* رأس المطارد */
    drawRect(x + 6, y - 10, 14, 14, 0xFF303030);

    /* جسم المطارد */
    drawRect(x + 8, y + 4, 10, 25, 0xFF303030);

    /* الذراعان */
    drawRect(x + 1, y + 8, 10, 5, 0xFF303030);
    drawRect(x + 17, y + 8, 10, 5, 0xFF303030);

    /* الرجلان */
    drawRect(x + 6, y + 27, 6, 18, 0xFF303030);
    drawRect(x + 14, y + 27, 6, 18, 0xFF303030);
}

int main(void)
{
    SceCtrlData pad;

    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_DIGITAL);

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

    sceGuOffset(
        2048 - SCREEN_W / 2,
        2048 - SCREEN_H / 2
    );

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

    /* اللاعب */
    float playerX = 120.0f;
    float playerY = 190.0f;

    float playerVelocityY = 0.0f;
    int playerGrounded = 1;

    /* المطارد */
    float chaserX = 35.0f;
    float chaserY = 190.0f;

    /* العوائق */
    float obstacle1X = 380.0f;
    float obstacle2X = 650.0f;

    int gameOver = 0;
    int won = 0;

    int distance = 0;

    while (1)
    {
        sceCtrlReadBufferPositive(&pad, 1);

        /*
         * إعادة اللعبة
         * START
         */
        if (gameOver || won)
        {
            if (pad.Buttons & PSP_CTRL_START)
            {
                playerX = 120.0f;
                playerY = 190.0f;
                playerVelocityY = 0.0f;
                playerGrounded = 1;

                chaserX = 35.0f;
                chaserY = 190.0f;

                obstacle1X = 380.0f;
                obstacle2X = 650.0f;

                distance = 0;

                gameOver = 0;
                won = 0;
            }
        }
        else
        {
            /*
             * القفز
             * X
             */
            if ((pad.Buttons & PSP_CTRL_CROSS) &&
                playerGrounded)
            {
                playerVelocityY = -10.5f;
                playerGrounded = 0;
            }

            /*
             * الجاذبية
             */
            playerVelocityY += 0.5f;
            playerY += playerVelocityY;

            /*
             * الأرض
             */
            if (playerY >= 190.0f)
            {
                playerY = 190.0f;
                playerVelocityY = 0.0f;
                playerGrounded = 1;
            }

            /*
             * سرعة العالم
             */
            obstacle1X -= 4.0f;
            obstacle2X -= 4.0f;

            /*
             * إعادة العوائق
             */
            if (obstacle1X < -40.0f)
            {
                obstacle1X = 520.0f + rand() % 100;
                distance++;
            }

            if (obstacle2X < -40.0f)
            {
                obstacle2X = 650.0f + rand() % 150;
                distance++;
            }

            /*
             * المطارد يجري خلف اللاعب
             */
            if (chaserX < playerX - 55.0f)
            {
                chaserX += 1.0f;
            }

            /*
             * لو المطارد قرب جدًا
             */
            if (chaserX + 25.0f >= playerX)
            {
                gameOver = 1;
            }

            /*
             * اصطدام اللاعب بالعائق الأول
             */
            if (playerX + 20.0f > obstacle1X &&
                playerX < obstacle1X + 28.0f &&
                playerY + 45.0f > 210.0f)
            {
                gameOver = 1;
            }

            /*
             * اصطدام اللاعب بالعائق الثاني
             */
            if (playerX + 20.0f > obstacle2X &&
                playerX < obstacle2X + 28.0f &&
                playerY + 45.0f > 200.0f)
            {
                gameOver = 1;
            }

            /*
             * الوصول لنهاية المرحلة
             */
            if (distance >= 20)
            {
                won = 1;
            }
        }

        sceGuStart(GU_DIRECT, list);

        /*
         * الخلفية
         */
        sceGuClearColor(0xFFE8E8E8);
        sceGuClear(GU_COLOR_BUFFER_BIT);

        /*
         * مباني الخلفية
         */
        drawRect(0, 120, 55, 115, 0xFFD0D0D0);
        drawRect(60, 90, 70, 145, 0xFFC8C8C8);
        drawRect(135, 135, 60, 100, 0xFFD5D5D5);
        drawRect(200, 105, 75, 130, 0xFFCCCCCC);
        drawRect(280, 135, 65, 100, 0xFFD2D2D2);
        drawRect(350, 95, 70, 140, 0xFFC5C5C5);
        drawRect(425, 125, 55, 110, 0xFFD0D0D0);

        /*
         * الأرض
         */
        drawRect(
            0,
            235,
            480,
            37,
            0xFF181818
        );

        /*
         * خط الأرض
         */
        drawRect(
            0,
            231,
            480,
            4,
            0xFF000000
        );

        /*
         * العوائق
         */
        drawRect(
            obstacle1X,
            210,
            28,
            25,
            0xFF101010
        );

        drawRect(
            obstacle2X,
            200,
            28,
            35,
            0xFF101010
        );

        /*
         * المطارد
         */
        drawChaser(chaserX, chaserY);

        /*
         * اللاعب
         */
        drawPlayer(playerX, playerY);

        /*
         * شريط التقدم
         */
        drawRect(
            20,
            18,
            160,
            6,
            0xFFAAAAAA
        );

        drawRect(
            20,
            18,
            (distance % 21) * 8,
            6,
            0xFF111111
        );

        /*
         * Game Over
         */
        if (gameOver)
        {
            drawRect(
                80,
                85,
                320,
                100,
                0xFFEEEEEE
            );

            drawRect(
                120,
                105,
                240,
                10,
                0xFF111111
            );

            drawRect(
                150,
                130,
                180,
                8,
                0xFF555555
            );

            drawRect(
                180,
                155,
                120,
                6,
                0xFF777777
            );
        }

        /*
         * الفوز
         */
        if (won)
        {
            drawRect(
                80,
                85,
                320,
                100,
                0xFFEAEAEA
            );

            drawRect(
                120,
                105,
                240,
                10,
                0xFF111111
            );

            drawRect(
                145,
                130,
                190,
                8,
                0xFF555555
            );

            drawRect(
                170,
                155,
                140,
                6,
                0xFF777777
            );
        }

        sceGuFinish();
        sceGuSync(0, 0);

        sceDisplayWaitVblankStart();
        sceGuSwapBuffers();
    }

    sceGuTerm();

    sceKernelExitGame();

    return 0;
}
