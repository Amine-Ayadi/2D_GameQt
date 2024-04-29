#ifndef GAMETEXTVIEW_H
#define GAMETEXTVIEW_H

#include "worldInstance.h"
#include <QWidget>
#include <QTextEdit>

class GameTextView : public QWidget
{
    Q_OBJECT

public:
    explicit GameTextView(const QString &name, QWidget *parent = nullptr);
    ~GameTextView() override;

    QWidget *view() {return this;}
    //virtual QWidget *view() const;

public slots:
    void updateTextScene(); // Slot to update the text scene

private:
    QTextEdit *_textView; // Text view to display the game scene
    QString getTileCharacter(int x, int y, const std::shared_ptr<WorldInstance>& w_instance); // Helper function to get character representation of a tile

    // You can add more private methods or variables here as needed
};

#endif // GAMETEXTVIEW_H
