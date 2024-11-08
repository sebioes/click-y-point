#ifndef MENU_H
#define MENU_H

#include <functional>
#include <vector>

class MenuItem {
public:
    MenuItem(std::function<const char*()> labelFunc, std::function<void()> selectFunc)
        : dynamicLabel(labelFunc), selectCallback(selectFunc), staticLabel(nullptr) {}
    
    MenuItem(const char* label, std::function<void()> selectFunc)
        : staticLabel(label), selectCallback(selectFunc) {}

    const char* getLabel() const {
        return dynamicLabel ? dynamicLabel() : staticLabel;
    }

    void select() {
        if (selectCallback) selectCallback();
    }

private:
    std::function<const char*()> dynamicLabel;
    std::function<void()> selectCallback;
    const char* staticLabel;
};

class Menu {
public:
    Menu() : currentIndex(0) {}

    void addItem(std::function<const char*()> labelFunc, std::function<void()> selectFunc) {
        items.emplace_back(labelFunc, selectFunc);
    }

    void addItem(const char* label, std::function<void()> selectFunc) {
        items.emplace_back(label, selectFunc);
    }

    void next() {
        if (!items.empty()) {
            currentIndex = (currentIndex + 1) % items.size();
        }
    }

    void previous() {
        if (!items.empty()) {
            currentIndex = (currentIndex + items.size() - 1) % items.size();
        }
    }

    void select() {
        if (!items.empty()) {
            items[currentIndex].select();
        }
    }

    const char** getAllLabels(size_t& count) const {
        count = items.size();
        if (count == 0) return nullptr;

        static std::vector<const char*> labels;
        labels.resize(count);
        
        for (size_t i = 0; i < count; i++) {
            labels[i] = items[i].getLabel();
        }
        
        return labels.data();
    }

    size_t getCurrentIndex() const {
        return currentIndex;
    }

private:
    std::vector<MenuItem> items;
    size_t currentIndex;
};

#endif 