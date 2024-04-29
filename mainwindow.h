#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "gametextview.h"
#include <QLabel>
#include <QMainWindow>
#include "gamecontroller.h"

class QFrame;
class QTextEdit;
class QPushButton;
class QVBoxLayout;
class QGraphicsScene;
class QGraphicsView;
class QStackedWidget;
class GameGraphView;
class WelcomeScreen;


// Text representation class example
class TextTest : public QLabel
{
    Q_OBJECT
public:
    TextTest(QWidget* p = nullptr):
        QLabel(p)
    {
        setText("Text Representation");
    }

};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void closeEvent(QCloseEvent *event) override;

public slots:
    void startGame();
    void exitGame();
    void switchView();
    void handleHomeButton();
    void appendToConsole(const QString& text);
    void keyReleaseEvent(QKeyEvent *event) override;


signals:
    void keyPressed();
    void mousePressed();

private:
    void setupCentralWidget();
    QFrame* createGameFrame();
    QToolBar *createToolBar();
    QVBoxLayout* createRightLayout();
    QPushButton* createButton(const QString& text, const QString& iconPath);
    void setMainWindowProperties();

    GameController* _gameController;
    std::shared_ptr<GameGraphView> _currentView;
    std::shared_ptr<WelcomeScreen> _welcomeView;
    std::shared_ptr<GameTextView> _textView;
    std::unique_ptr<QStackedWidget> _stackedWidgets;

    QPushButton* _homeButton;
    QPushButton* _switchButton;
    QPushButton* _zoomInButton;
    QPushButton* _zoomOutButton;
    QPushButton* _exitButton;
    QVBoxLayout* _verticalLayout;


    QTextEdit* consoleTextEdit;
};
#endif // MAINWINDOW_H
