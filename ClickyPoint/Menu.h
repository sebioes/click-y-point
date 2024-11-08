#ifndef MENU_H
#define MENU_H

#include <functional>
#include <vector>
#include <string>

class IMenu {
public:
    virtual ~IMenu() = default;
    virtual void next() = 0;
    virtual void previous() = 0;
    virtual void select() = 0;
    virtual std::string getCurrentLabel() const = 0;
    virtual size_t size() const = 0;
    virtual std::vector<std::string> getAllLabels() const = 0;
    virtual size_t getCurrentIndex() const = 0;
};

class MenuItem {
public:
    MenuItem(std::string label, std::function<void()> act);
    MenuItem(std::function<std::string()> labelFunc, std::function<void()> act);
    
    std::string getLabel() const;
    void select() const;

private:
    std::string staticLabel;
    std::function<std::string()> dynamicLabel;
    std::function<void()> action;
    bool isDynamic;
};

class Menu : public IMenu {
public:
    Menu();
    
    void addItem(const std::string& label, std::function<void()> action = nullptr);
    void addItem(std::function<std::string()> labelProvider, std::function<void()> action = nullptr);
    
    // IMenu implementation
    void next() override;
    void previous() override;
    void select() override;
    std::string getCurrentLabel() const override;
    size_t size() const override;
    std::vector<std::string> getAllLabels() const override;
    size_t getCurrentIndex() const override;

private:
    std::vector<MenuItem> items;
    size_t currentIndex;
};

#endif 