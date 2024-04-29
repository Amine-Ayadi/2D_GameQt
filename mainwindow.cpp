#include "mainwindow.h"
#include "gamegraphview.h"
#include "welcomescreen.h"
#include "worldInstance.h"
#include "gametextview.h"
#include "Config.h"
#include <QFontDatabase>
#include <QFrame>
#include <QGraphicsView>
#include <QGraphicsProxyWidget>
#include <QLabel>
#include <QTextEdit>
#include <QMenu>
#include <QMenuBar>
#include <QStandardItemModel>
#include <QTreeView>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QThread>
#include <QToolBar>
#include <QStackedWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), _gameController(new GameController(this))
{
    qDebug() << "MainWindow constructor ";
    auto w_instance = WorldInstance::getInstance();
    connect(w_instance.get(), &WorldInstance::startPressed, this, &MainWindow::startGame);
    connect(w_instance.get(), &WorldInstance::exitPressed, this, &MainWindow::exitGame);
    connect(_gameController, &GameController::showOptionsRequested, this,  &MainWindow::switchView);
    connect(_gameController, &GameController::addTextConsole, this, &MainWindow::appendToConsole);
    connect(_gameController, &GameController::keyPressRequested, this, &MainWindow::keyPressEvent);
    connect(this, &MainWindow::mousePressed, _gameController, &GameController::handleSelectedTile);
    // Init view
    try {
        _stackedWidgets = std::make_unique<QStackedWidget>(this);
        _welcomeView = std::make_shared<WelcomeScreen>("Welcome to GAME_NAME");
        _stackedWidgets->addWidget(_welcomeView->view());
        setupCentralWidget();
    } catch (const std::exception& e) {
        qDebug() << "Exception caught during MainWindow initialization: " << e.what();
    } catch (...) {
        qDebug() << "Unknown exception caught during MainWindow initialization.";
    }
    // _stackedWidgets->setCurrentIndex(1);
    _stackedWidgets->setCurrentIndex(0);
    // showNormal();
}

MainWindow::~MainWindow()
{
    delete _gameController;
    qDebug() << "MainWindow Destroyed";
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMainWindow::closeEvent(event);
    exitGame();
}

void MainWindow::setupCentralWidget()
{
    // Create main widget
    QWidget* mainWidget = new QWidget(this);
    mainWidget->setStyleSheet("background-color:  #0E3441");
    mainWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setCentralWidget(mainWidget);

    // Create layouts
    QHBoxLayout* mainLayout = new QHBoxLayout(mainWidget);
    mainLayout->addWidget(createToolBar());
    mainLayout->addLayout(createRightLayout());
    mainLayout->addWidget(_stackedWidgets.get());

    qDebug() << "MainWindow: layout created";
}

QPushButton* MainWindow::createButton(const QString& text, const QString& iconPath)
{
    // QFont font("Bitter", 12);
    QPushButton* button = new QPushButton(text);
    button->setIcon(QIcon(iconPath));
    button->setIconSize(QSize(40, 40));  // Set the icon size as needed
    QString fontPath = ":/other/AGoblinAppears-o2aV.ttf";
    int fontId = QFontDatabase::addApplicationFont(fontPath);

    QFont customFont;
    if (fontId != -1) {
        QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
        if (!fontFamilies.isEmpty()) {
            QString fontFamily = fontFamilies.at(0);
            customFont = QFont(fontFamily, 12);
        }
    } else {
        qDebug() << "Failed to load the font!";
        customFont = QFont("Bitter", 18); ;
    }
    button->setFont(customFont);
    button->setFlat(false);
    button->setFixedSize(120, 40);
    // Set button styles using stylesheets
    button->setStyleSheet("background-color: #FFA07A;"
                          "opacity: 50;"   // background
                          "border: 1px #3E4095;"            // border
                          "border-radius: 6px;"         // rounded corners
                          "padding: 5px;"               // padding
                          "color: #000000;"             // Text color
                          "font-weight: bold;");
    return button;
}

QToolBar *MainWindow::createToolBar()
{

    QToolBar* leftToolBar = new QToolBar("Left Toolbar", this);
    addToolBar(Qt::LeftToolBarArea, leftToolBar);
    QWidget* verticalLayoutContainer = new QWidget(leftToolBar);
    _verticalLayout = new QVBoxLayout(verticalLayoutContainer);
    _homeButton = createButton("HOME","d");
    _switchButton = createButton("SWITCH","d");
    _exitButton = createButton("EXIT","d");

    connect(_homeButton, &QPushButton::clicked, this, &MainWindow::handleHomeButton);
    connect(_switchButton, &QPushButton::clicked, _gameController, &GameController::handleShowOptions);
    connect(_exitButton, &QPushButton::clicked, this, &MainWindow::exitGame);

    _verticalLayout->addWidget(_homeButton);
    _verticalLayout->addWidget(_switchButton);
    _verticalLayout->addWidget(_exitButton);
    _verticalLayout->setSpacing(15);

    // toolbar container
    leftToolBar->addWidget(verticalLayoutContainer);
    return leftToolBar;
}

QVBoxLayout* MainWindow::createRightLayout()
{

    QVBoxLayout* rightLayout = new QVBoxLayout();

    // Add text to Console
    QTextEdit* consoleTextEdit = new QTextEdit();
    consoleTextEdit->setReadOnly(true);
    consoleTextEdit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    consoleTextEdit->setMaximumWidth(200);
    consoleTextEdit->setStyleSheet("background-color: #FFDAB9; color: #000000;");
    this->consoleTextEdit = consoleTextEdit;
    return rightLayout;
}

void MainWindow::switchView()
{
    if(_stackedWidgets->currentIndex() == 1) {
        if (_stackedWidgets->widget(1) != nullptr) {
            _stackedWidgets->setCurrentIndex(2);
            qDebug() << "Text Representation";
        } else {
            _stackedWidgets->setCurrentIndex(0);
            qDebug() << "Return Home";
        }
    }
    else {
        if(_stackedWidgets->widget(2) != nullptr){
            _stackedWidgets->setCurrentIndex(1);
            qDebug() << "Graphical Representation";
        } else {
            _stackedWidgets->setCurrentIndex(0);
            qDebug() << "Return Home";
        }
    }
}

void MainWindow::handleHomeButton()
{
    if (_currentView)
    {
        // _currentView = nullptr;
        qDebug() << "homebutton deleting currentview";
        delete(_zoomInButton);
        delete(_zoomOutButton);
    }
    else qDebug() << "homebutton currentview null";
    _stackedWidgets->setCurrentIndex(0);
    if(_stackedWidgets->widget(2) ||_stackedWidgets->widget(1)){
        // _stackedWidgets->removeWidget(_stackedWidgets->widget(2));
        // _stackedWidgets->removeWidget(_stackedWidgets->widget(1));
        // delete(_zoomInButton);
        // delete(_zoomOutButton);
        qDebug() << "homebutton deleting either widget";
    }
}

void MainWindow::startGame()
{
    // Create World + GraphicsView
    auto w_instance = WorldInstance::getInstance();
    _currentView = std::make_shared<GameGraphView>("GraphicalRepresentation", this);
    _textView = std::make_shared<GameTextView>("TextRepresentation", this);

    std::unique_ptr<GraphicsManager> graphicsManager = std::make_unique<GraphicsManager>();
    w_instance->createWorld(w_instance->getMap(),
                            Config::ENEMY_NB,
                            Config::POTION_NB, 0.325);

    // Create GraphicsManager
    if (graphicsManager) std::cout << "Success: graphicsManager instance in mainwindow";
    else std::cout << "Error: graphicsManager instance in mainwindow";
    graphicsManager->setScene(_currentView->getScene());
    graphicsManager->init();
    w_instance->setGraphicsManager(graphicsManager);
    // _currentView->populateScene();
    w_instance->startGameLoop();

    // Create Text Rep
    // std::shared_ptr<TextTest> textTestPtr = std::make_shared<TextTest>(_stackedWidgets.get());
    _stackedWidgets->addWidget(_textView.get());
    _stackedWidgets->addWidget(_currentView->view());
    _stackedWidgets->setCurrentIndex(2);

    // Add Zoom Buttons
    _zoomInButton = createButton("Zoom IN","d");
    _zoomOutButton = createButton("Zoom OUT","d");
    _verticalLayout->addWidget(_zoomInButton);
    _verticalLayout->addWidget(_zoomOutButton);
    connect(_zoomInButton, &QPushButton::clicked, _currentView.get(), &GameGraphView::handleZoomIn);
    connect(_zoomOutButton, &QPushButton::clicked, _currentView.get(), &GameGraphView::handleZoomOut);
}

void MainWindow::exitGame()
{
    auto w_instance = WorldInstance::getInstance();
    w_instance->setAtomicIsRunning(false);
    delete _gameController;
    qDebug() << "Game Exited: Everything cleaned";
}

void MainWindow::appendToConsole(const QString& text)
{
    if (consoleTextEdit) {
        consoleTextEdit->append(text);
    }
}

void MainWindow::setMainWindowProperties()
{
    setWindowTitle("2D Graphical Representation");
    resize(1200, 800);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) {
        // Ignore auto-repeat events
        return;
    }

    switch (event->key()) {
        WorldInstance::getInstance()->setAtomicKeyAction(true);
    case Qt::Key_W:
        WorldInstance::getInstance()->pushActionToStack(WorldInstance::ActionType::UP);
        break;
    case Qt::Key_S:
        WorldInstance::getInstance()->pushActionToStack(WorldInstance::ActionType::DOWN);
        break;
    case Qt::Key_A:
        WorldInstance::getInstance()->pushActionToStack(WorldInstance::ActionType::LEFT);
        break;
    case Qt::Key_D:
        WorldInstance::getInstance()->pushActionToStack(WorldInstance::ActionType::RIGHT);
        break;
    case Qt::Key_R:
        WorldInstance::getInstance()->pushActionToStack(WorldInstance::ActionType::ATTACK);
        break;
    default:
        break;
    }
    WorldInstance::getInstance()->setAtomicKeyAction(true);
    QString keyText = QString("Key pressed: %1").arg(event->text());
    appendToConsole(keyText);
}


