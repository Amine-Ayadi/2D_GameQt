#include "gametextview.h"
#include "Config.h"

#include <QStringBuilder>
#include <QTextEdit>
#include <QVBoxLayout>

GameTextView::GameTextView(const QString &name, QWidget *parent)
    : QWidget(parent)
{
    qDebug() << "GameTextView constructor";
    _textView = new QTextEdit(this);
    _textView->setReadOnly(true); // Make text view read-only
    _textView->setStyleSheet("QTextEdit {"
                             "    color: white;"
                             "    font-size: 24pt;"
                             "    font-family: Courier;"
                             "}");

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(_textView);

    auto w_instance = WorldInstance::getInstance();
    connect(w_instance.get(), &WorldInstance::positionUpdated, this, &GameTextView::updateTextScene);
    // Other necessary connections
}

GameTextView::~GameTextView()
{
    // Resource cleanup if necessary
}

/*QWidget *GameTextView::view() const
{
    return static_cast<QWidget *>(_textView);
}*/

void GameTextView::updateTextScene()
{
    auto w_instance = WorldInstance::getInstance();
    if (!w_instance)
    {
        qDebug() << "WorldInstance is empty: Cannot update text scene!";
        return;
    }

    QString textScene;
    int rows = w_instance->getRows();
    int cols = w_instance->getCols();
    // Calculate offsets similar to GameGraphView
    int startOffsetX = std::max(0, w_instance->getProtagonist()->getXPos() - Config::RENDER_START_OFFSET);
    int startOffsetY = std::max(0, w_instance->getProtagonist()->getYPos() - Config::RENDER_START_OFFSET);
    int endOffsetX = std::min(cols, w_instance->getProtagonist()->getXPos() + Config::RENDER_END_OFFSET);
    int endOffsetY = std::min(rows, w_instance->getProtagonist()->getYPos() + Config::RENDER_END_OFFSET);

    for (int y = startOffsetY; y < endOffsetY; ++y)
    {
        for (int x = startOffsetX; x < endOffsetX; ++x)
        {
            // Build the string for each tile
            textScene += getTileCharacter(x, y, w_instance);
        }
        textScene += "\n"; // New line at the end of each row
    }

    _textView->setText(textScene);
}

QString GameTextView::getTileCharacter(int x, int y, const std::shared_ptr<WorldInstance>& w_instance)
{
    // Define the size of the square for each tile
    const int tileSize = 3; // 3x3 square for example
    QString tileRepresentation;

    // Create the top and bottom rows of the square
    QString edgeRow = QString(tileSize, '.');

    // Determine the center character
    char centerChar = '.';
    if (x == w_instance->getProtagonist()->getXPos() && y == w_instance->getProtagonist()->getYPos()) {
        centerChar = 'H'; // Protagonist
    } else {
        for (const auto& potion : w_instance->getHealthPotions()) {
            if (potion->getXPos() == x && potion->getYPos() == y) {
                centerChar = 'P'; // Potion
                break;
            }
        }
        for (const auto& enemy : w_instance->getEnemies()) {
            if (enemy->getXPos() == x && enemy->getYPos() == y) {
                centerChar = 'E'; // Enemy
                break;
            }
        }
    }

    // Create the middle row of the square
    QString middleRow = QString(".") + centerChar + QString(".");

    // Build the square representation
    tileRepresentation = edgeRow + "\n" + middleRow + "\n" + edgeRow + "\n";

    return tileRepresentation;
}

