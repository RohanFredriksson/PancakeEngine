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
        void add(const std::string& name) {
            this->constructors.insert({name, []() -> Base* { return new Derived(); }});
        }

        Base* create(const std::string& name) {
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

#define FACTORY(Class) Factory<Class>::get()