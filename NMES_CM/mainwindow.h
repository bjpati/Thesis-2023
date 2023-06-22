#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QLineEdit>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QLineEdit>
#include <QObject>
#include <QEvent>
#include <QKeyEvent>
#include <QPalette>
#include <QColor>
#include <QCheckBox>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
protected:
bool eventFilter(QObject *obj, QEvent *event);//in header
public slots:
  void timeout();
private:
    QCheckBox* checkBox;
    QTimer* timer;
    void updateColor();
    QFrame* frames[4][4];
    QFrame* frames2[4][4];
    QWidget* centralWidget;
    QVBoxLayout* centralLayout;
    void createMatrixGrid();
    void startReading();
    void stopReading();
    int32_t adc_to_mv (int32_t raw);
    void collectIndividual();
    QGridLayout* matrixLayout1;
    QGridLayout* matrixLayout2;
    QPushButton* startButton;
    QLabel* labels[4][4];
    QLabel* labels2[4][4];
    QFrame* frame = new QFrame;
    QLineEdit* valueInput;
    void handleInputValue();
    float inputValue;
    QTimer* deviceCheckTimer;
    void referenceVoltage(int current);
    void serialCommunication(double ref);
    bool picologConnected;
    void PicoInit();
    void transmitDouble(double value);
    QByteArray doubleToByteArray(double value);
    void onCheckBoxStateChanged(int state);
};
#endif // MAINWINDOW_H
