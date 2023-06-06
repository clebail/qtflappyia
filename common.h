#ifndef COMMON_H
#define COMMON_H

#include <QtDebug>
#include <QImage>

#define PI                  3.14159
#define SCENE_WIDTH         563
#define SCENE_HEIGHT        510
#define TUYAU_HEIGHT        389
#define SOL_HEIGHT          110
#define FLAPPY_WIDTH        35
#define FLAPPY_HEIGHT       25
#define FLAPPY_WIDTH2       (FLAPPY_WIDTH/2.0)
#define FLAPPY_HEIGHT2      (FLAPPY_HEIGHT/2.0)

class Common {
public:
    typedef enum { estFond, estSol, estFlappy, estTuyauHaut, estTuyauBas } ESpriteType;

    typedef struct _SSprite {
        QList<QRect> rects;
    }SSprite;

    static Common * getInstance();
    QImage getSpritesImage();
    QImage getSpriteImage(const ESpriteType& type, int idx = 0) const;
    int getNbSpriteImage(const ESpriteType& type) const;
    QSize getSpriteSize(const ESpriteType& type) const;
    float getFlappyHypo() const;
    float getFlappyBaseAngle() const;
private:
    Common();

    QImage spritesImage;
    QMap<ESpriteType, SSprite> sprites;
    float flappyHypo;
    float flappyBaseAngle;
};

#endif // COMMON_H
