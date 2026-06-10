#ifndef COMMON_H
#define COMMON_H

#include <QtDebug>
#include <QImage>

#define PI                  3.14159

// IA
#define FLAPPY_START_X      150
#define FLAPPY_START_Y      255
#define PENTE_NEURONE       0.01
#define TAILLE_POPULATION   100
#define TAUX_MUTATION       25
#define FLAPPY_NB_FRONT     9
#define FLAPPY_NB_REAR      2
#define FLAPPY_NB_INPUTS    (FLAPPY_NB_FRONT + FLAPPY_NB_REAR + 1)
#define FLAPPY_NB_HIDDEN    4
#define SCENE_WIDTH         563
#define SCENE_HEIGHT        510
#define TUYAU_HEIGHT        389
#define TUYAU_GAP           120
#define SOL_HEIGHT          110
#define FLAPPY_WIDTH        35
#define FLAPPY_HEIGHT       25
#define FLAPPY_WIDTH2       (FLAPPY_WIDTH/2.0)
#define FLAPPY_HEIGHT2      (FLAPPY_HEIGHT/2.0)

// RL
#define RL_GAMMA            0.99
#define RL_REWARD_TICK      1.0
#define RL_REWARD_DEAD      -10.0
#define RL_EPSILON_START    1.0
#define RL_EPSILON_MIN      0.05
#define RL_EPSILON_DECAY    0.995
#define RL_NB_HIDDEN        16
#define RL_ETA              0.0003
#define RL_BUFFER_SIZE      10000
#define RL_TARGET_UPDATE    500
#define RL_REWARD_PIPE      10.0

class Common {
public:
    typedef enum { estFond, estSol, estFlappy, estFlappyRL, estTuyauHaut, estTuyauBas } ESpriteType;

    typedef struct _SSprite {
        QList<QRect> rects;
    }SSprite;

    static Common * getInstance(void);
    QImage getSpritesImage(void) const;
    QImage getSpriteImage(const ESpriteType& type, int idx = 0) const;
    int getNbSpriteImage(const ESpriteType& type) const;
    QSize getSpriteSize(const ESpriteType& type) const;
    float getFlappyHypo(void) const;
    float getFlappyBaseAngle(void) const;
private:
    Common(void);

    QImage spritesImage;
    QMap<ESpriteType, SSprite> sprites;
    float flappyHypo;
    float flappyBaseAngle;
};

#endif // COMMON_H
