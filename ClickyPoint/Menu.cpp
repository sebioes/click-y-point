#include "Menu.h"

// MenuItem implementation
MenuItem::MenuItem(std::string label, std::function<void()> act)
    : staticLabel(std::move(label)), action(std::move(act)), isDynamic(false) {}

MenuItem::MenuItem(std::function<std::string()> labelFunc, std::function<void()> act)
    : dynamicLabel(std::move(labelFunc)), action(std::move(act)), isDynamic(true) {}

std::string MenuItem::getLabel() const {
    return isDynamic ? dynamicLabel() : staticLabel;
}

void MenuItem::select() const {
    if (action) action();
}

// Menu implementation
Menu::Menu() : currentIndex(0) {}

void Menu::addItem(const std::string& label, std::function<void()> action) {
    items.emplace_back(label, action);
}

void Menu::addItem(std::function<std::string()> labelProvider, std::function<void()> action) {
    items.emplace_back(labelProvider, action);
}

void Menu::next() {
    if (!items.empty()) {
        currentIndex = (currentIndex + 1) % items.size();
    }
}

void Menu::previous() {
    if (!items.empty()) {
        currentIndex = (currentIndex + items.size() - 1) % items.size();
    }
}

void Menu::select() {
    if (!items.empty()) {
        items[currentIndex].select();
    }
}

std::string Menu::getCurrentLabel() const {
    return !items.empty() ? items[currentIndex].getLabel() : "";
}

size_t Menu::size() const {
    return items.size();
}

std::vector<std::string> Menu::getAllLabels() const {
    std::vector<std::string> labels;
    labels.reserve(items.size());
    for (const auto& item : items) {
        labels.push_back(item.getLabel());
    }
    return labels;
}

size_t Menu::getCurrentIndex() const {
    return currentIndex;
} 