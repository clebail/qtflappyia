#ifndef COMMON_H
#define COMMON_H

#include <QtDebug>
#include <QImage>

#define PI  3.14159

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
private:
    Common();

    QImage spritesImage;
    QMap<ESpriteType, SSprite> sprites;
};

#endif // COMMON_H
