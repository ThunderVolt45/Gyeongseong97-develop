#pragma once
#include "Bullet.h"

class BulletExplosive : public Bullet
{
public:
    BulletExplosive(int x, int y, float speedX, float speedY, bool isMine, int damage);

    void Reset(int x, int y, float speedX, float speedY, bool isMine, int damage = 1) override;
    void Destroy() override;
};