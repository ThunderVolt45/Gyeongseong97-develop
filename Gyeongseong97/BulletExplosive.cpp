#include "BulletExplosive.h"
#include "ExplosionPool.h"
#include "GameManager.h"

BulletExplosive::BulletExplosive(int x, int y, float dx, float dy, bool isMine, int damage = 1)
{
    Reset(x, y, dx, dy, isMine, damage);
}

void BulletExplosive::Reset(int x, int y, float dx, float dy, bool isMine, int damage)
{
    this->x = x;
    this->y = y;
    this->speedX = dx;
    this->speedY = dy;
    this->isPlayer = isMine;
    this->damage = damage;

    ftxui::Color c = ftxui::Color::DarkRed;

    if (!isMine)
    {
        c = ftxui::Color::RosyBrown;
    }

    sprite = Sprite(8, 8, 
        {
            c, c, c, c, c, c, c, c,
            c, c, c, c, c, c, c, c,
            c, c, c, c, c, c, c, c,
            c, c, c, c, c, c, c, c,
            c, c, c, c, c, c, c, c,
            c, c, c, c, c, c, c, c,
            c, c, c, c, c, c, c, c,
            c, c, c, c, c, c, c, c
        });
}

void BulletExplosive::Destroy()
{
    GameManager& gameManager = GameManager::GetInstance();

    // 폭발 효과 생성
    // 폭발 위치를 총알의 중앙으로 설정
    std::shared_ptr<Explosion> explosion = ExplosionPool::GetInstance().GetExplosion(GetCenterX(), GetCenterY(), 50, 50);
    gameManager.CreateGameObject(explosion, false); // 폭발은 게임 오브젝트로 추가

    // 주변 오브젝트에 데미지 (선택 사항: 폭발 반경 내 적에게 데미지 로직 추가)
    // 이 부분은 GameManager나 CollisionManager에서 처리하는 것이 더 적합할 수 있습니다.
    // 예를 들어, explosion이 OnCollision을 오버라이드해서 주변 적들에게 데미지를 주거나
    // GameManager에서 GetGameObjects()를 순회하며 일정 반경 내 적에게 데미지를 줄 수 있습니다.
    
    // 자기 자신 (총알) 파괴
    // Bullet 클래스의 Destroy 로직을 호출하거나, 직접 제거
    GameObject::Destroy(); // GameObject의 Destroy를 호출하여 게임 월드에서 자신을 제거
}