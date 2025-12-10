#pragma once
#include <vector>
#include <memory>
#include <mutex>
#include <algorithm>

template <typename T>
class ObjectPool
{
private:
    std::vector<std::shared_ptr<T>> pool;
    std::recursive_mutex poolMutex;

    ObjectPool() {}
    ~ObjectPool() {}

public:
    static ObjectPool& GetInstance()
    {
        static ObjectPool instance;
        return instance;
    }

    ObjectPool(const ObjectPool&) = delete;
    ObjectPool& operator=(const ObjectPool&) = delete;

    template <typename... Args>
    std::shared_ptr<T> Get(Args&&... args)
    {
        std::lock_guard<std::recursive_mutex> lock(poolMutex);

        // 비활성화된 오브젝트 찾기
        for (auto& obj : pool)
        {
            if (!obj->IsActive())
            {
                obj->Reset(std::forward<Args>(args)...);
                obj->SetActive(true);
                return obj;
            }
        }

        // 없으면 새로 생성
        auto newObj = std::make_shared<T>(std::forward<Args>(args)...);
        pool.push_back(newObj);
        return newObj;
    }

    void Return(std::shared_ptr<T> obj)
    {
        std::lock_guard<std::recursive_mutex> lock(poolMutex);
        if (obj)
        {
            obj->SetActive(false);
        }
    }
};
