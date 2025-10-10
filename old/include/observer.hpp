#ifndef OBSERVER_HPP
#define OBSERVER_HPP


#include <map>
#include <vector>
#include <functional>


namespace ft {


template <typename TEvent>
class Observer {
public:
void subscribe(const TEvent& event, const std::function<void()>& lambda) {
_observers[event].push_back(lambda);
}


void notify(const TEvent& event) {
if (_observers.count(event)) {
for (auto& func : _observers[event])
func();
}
}


private:
std::map<TEvent, std::vector<std::function<void()>>> _observers;
};


} // namespace ft


#endif // OBSERVER_HPP