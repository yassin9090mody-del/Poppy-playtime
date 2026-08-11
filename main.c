#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include <pspgu.h>
#include <pspgum.h>

PSP_MODULE_INFO("Vector PSP", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);

#define SCREEN_W 480
#define SCREEN_H 272

typedef struct {
    unsigned int color;
    float x;
    float y;
    float z;
} Vertex;

static unsigned int __attribute__((aligned(16))) list[262144];

/* رسم مستطيل */
static void drawRect(
    float x,
    float y,
    float w,
    float h,
    unsigned int color
)
{
    Vertex *v = (Vertex *)sceGuGetMemory(2 * sizeof(Vertex));

    v[0].color = color;
    v[0].x = x;
    v[0].y = y;
    v[0].z = 0.0f;

    v[1].color = color;
    v[1].x = x + w;
    v[1].y = y + h;
    v[1].z = 0.0f;

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
    /* التحكم */
    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

    /* تشغيل GU */
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

    /*
     * إعداد الشاشة 480x272
     */
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

    sceGuScissor(
        0,
        0,
        SCREEN_W,
        SCREEN_H
    );

    sceGuEnable(GU_SCISSOR_TEST);

    sceGuDisable(GU_DEPTH_TEST);
    sceGuDisable(GU_TEXTURE_2D);

    sceGuFinish();
    sceGuSync(0, 0);

    sceDisplayWaitVblankStart();
    sceGuDisplay(GU_TRUE);

    /* اللاعب */
    float playerX = 70.0f;
    float playerY = 200.0f;

    float velocityY = 0.0f;
    int onGround = 1;

    /* العائق */
    float obstacleX = 350.0f;
    float obstacleY = 215.0f;

    while (1)
    {
        SceCtrlData pad;

        sceCtrlReadBufferPositive(&pad, 1);

        /*
         * القفز
         */
        if ((pad.Buttons & PSP_CTRL_CROSS) && onGround)
        {
            velocityY = -9.5f;
            onGround = 0;
        }

        /*
         * الجاذبية
         */
        velocityY += 0.45f;
        playerY += velocityY;

        /*
         * الأرض
         */
        if (playerY >= 200.0f)
        {
            playerY = 200.0f;
            velocityY = 0.0f;
            onGround = 1;
        }

        /*
         * تحريك العائق
         */
        obstacleX -= 3.0f;

        if (obstacleX < -40.0f)
        {
            obstacleX = 520.0f;
        }

        /*
         * الاصطدام
         */
        if (
            playerX + 20.0f > obstacleX &&
            playerX < obstacleX + 25.0f &&
            playerY + 40.0f > obstacleY
        )
        {
            playerX = 70.0f;
            obstacleX = 350.0f;
        }

        /*
         * بداية الرسم
         */
        sceGuStart(GU_DIRECT, list);

        /*
         * الخلفية
         */
        sceGuClearColor(0xFFF2F2F2);
        sceGuClear(GU_COLOR_BUFFER_BIT);

        /*
         * الأرض
         */
        drawRect(
            0.0f,
            240.0f,
            480.0f,
            32.0f,
            0xFF202020
        );

        /*
         * العائق
         */
        drawRect(
            obstacleX,
            obstacleY,
            25.0f,
            25.0f,
            0xFF202020
        );

        /*
         * جسم اللاعب
         */
        drawRect(
            playerX,
            playerY,
            20.0f,
            40.0f,
            0xFF000000
        );

        /*
         * رأس اللاعب
         */
        drawRect(
            playerX + 2.0f,
            playerY - 10.0f,
            16.0f,
            12.0f,
            0xFF000000
        );

        /*
         * إنهاء الرسم
         */
        sceGuFinish();
        sceGuSync(0, 0);

        sceDisplayWaitVblankStart();

        sceGuSwapBuffers();
    }

    sceGuTerm();

    sceKernelExitGame();

    return 0;
}
