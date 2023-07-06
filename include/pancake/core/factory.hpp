#pragma once

#include <unordered_map>
#include <functional>
#include <string>
#include <iostream>

template<class Base>
class Factory {

    private:
        std::unordered_map<std::string, std::function<Base*()>> constructors;

    public:

        static Factory<Base>& get() {
            static Factory<Base> instance;
            return instance;
        }

        template<class Derived>
        void add(std::string name) {
            this->constructors.insert({name, []() -> Base* { return new Derived(); }});
        }

        Base* create(std::string name) {
            const auto it = this->constructors.find(name);
            if (it == this->constructors.end()) {return nullptr;}
            return (it->second)();
        }

        void list() {
            for (const auto &creator : this->constructors) {
                std::cout << creator.first << '\n';
            }
        }

};

template<class Base, class Derived>
class Creator {

    public:

        explicit Creator(std::string name) {
            Factory<Base>::get().template add<Derived>(name);
        }

};

#define UNIQUE_VARIABLE_NAME() CONCATENATE(V, __COUNTER__)
#define CONCATENATE_IMPL(x, y) x##y
#define CONCATENATE(x, y) CONCATENATE_IMPL(x, y)

#define FACTORY(Base) Factory<Base>::get()
#define REGISTER(Base, Derived) namespace{Creator<Base, Derived> UNIQUE_VARIABLE_NAME()(#Derived);}