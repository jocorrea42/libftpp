#ifndef SINGLETON_HPP
#define SINGLETON_HPP

#include <memory>
#include <stdexcept>
#include <utility>

namespace ft {

template <typename TType>
class Singleton {
public:
    template <typename... TArgs>
    static void instantiate(TArgs&&... args) {
        if (_instance)
            throw std::runtime_error("Instance already exists");
        _instance.reset(new TType(std::forward<TArgs>(args)...));
    }

    static TType* instance() {
        if (!_instance)
            throw std::runtime_error("Instance not yet created");
        return _instance.get();
    }

private:
    static std::unique_ptr<TType> _instance;
};

template <typename TType>
std::unique_ptr<TType> Singleton<TType>::_instance = nullptr;

} // namespace ft

#endif
