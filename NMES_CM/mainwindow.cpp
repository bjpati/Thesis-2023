#include "mainwindow.h"
#include "pl1000Api.h"
#include <windows.h>
#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include <iostream>
#include <QDebug>
#include <QMessageBox>
#include <QApplication>
#include <QTimer>

#define TRUE		1
#define FALSE		0

#define PL1000_16_CHANNEL 16

int32_t		scale_to_mv = TRUE;
uint16_t	max_adc_value;
uint16_t  numDeviceChannels=16;
int16_t		g_handle;
int16_t		g_handle2;
int16_t		isReady;
PICO_STATUS status;
PICO_STATUS status2;
 QSerialPort serialPort;
 int row = 0;
 int col = 0;
 bool isPaused = false; // Add a boolean variable to track the pause state
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    resize(900, 900);
    //pico_init();
    createMatrixGrid();
    valueInput->installEventFilter(this);
    timer= new QTimer(this);
    connect(timer, &QTimer::timeout,this,&MainWindow::timeout);
    timer->start(500);
}

MainWindow::~MainWindow()
{
    timer->stop();
}

void MainWindow::createMatrixGrid()
{
    // Create the central widget
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Create a vertical layout for the central widget
    QVBoxLayout* centralLayout = new QVBoxLayout(centralWidget);
    // Set the background color for the central widget
      centralWidget->setStyleSheet("background-color: white;"); // Set the desired background color
    // Create the matrix layout
    matrixLayout1 = new QGridLayout();
    matrixLayout2 = new QGridLayout();

    // Create matrix 1 for picolog 1
    int matrix1[4][4] = {
        {1, 2, 3, 4},
        {5, 6, 7, 8},
        {9, 10, 11, 12},
        {13, 14, 15, 16}
    };
    // Create matrix 2
    int matrix2[4][4] = {
        {17, 18, 19, 20},
        {21, 22, 23, 24},
        {25, 26, 27, 28},
        {29, 30, 31, 32}
    };
    // Initialize the labels array
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            labels[row][col] = nullptr;
            labels2[row][col] = nullptr;
        }
    }
    QLabel* headlineLabel= new QLabel("Current monitoring");
    headlineLabel->setAlignment(Qt::AlignCenter);
    QFont font;
    font.setPointSize(18);
    font.setBold(true);
    headlineLabel->setFont(font);
    headlineLabel->setStyleSheet("color: black;");

    // Populate the grid with labels representing the matrix elements
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
           int value =0; // Generate a random value between 0 and 99

            QLabel* labelChannel = new QLabel(QString("Channel: %1").arg(matrix1[row][col]));
            QLabel* labelValue = new QLabel(QString("Voltage(mV): %1").arg(value));

            // Add a frame around each label
            QFrame* frame = new QFrame;
            frame->setFrameShape(QFrame::Box);
            frame->setLineWidth(2);
            //frame->setStyleSheet("border-radius: 5px; background-color: white;"); // Set the background color for the frames
            frame->setStyleSheet("border-radius: 5px; background-color: green; ");
            //frame->setStyleSheet("background-color: green;");
            // Increase the size of the frame
            QSize frameSize(200, 100);
            frame->setFixedSize(frameSize);
            labels[row][col] = labelValue;
            // Create a layout for the labels within the frame
            QVBoxLayout* frameLayout = new QVBoxLayout(frame);
            frameLayout->addWidget(labelChannel);
            frameLayout->addWidget(labels[row][col]);
            frameLayout->setAlignment(Qt::AlignCenter);
            // Store the frame in an array for later access
            frames[row][col] = frame;

            matrixLayout1->addWidget(frame, row, col);

        }
    }
    // Populate the grid with labels representing the matrix elements
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
           int value2 =0; // Generate a random value between 0 and 99

            QLabel* labelChannel = new QLabel(QString("Channel: %1").arg(matrix2[row][col]));
            QLabel* labelValue2 = new QLabel(QString("Voltage(mV): %1").arg(value2));

            // Add a frame around each label
            QFrame* frame = new QFrame;
            frame->setFrameShape(QFrame::Box);
            frame->setLineWidth(2);
//            frame->setStyleSheet("border-radius: 5px; background-color: white;"); // Set the background color for the frames
            frame->setStyleSheet("border-radius: 5px; background-color: green;"); // Set the background color for the frames

            //frame->setStyleSheet("background-color: green;");
            // Increase the size of the frame
            QSize frameSize(200, 100);
            frame->setFixedSize(frameSize);
            labels2[row][col] = labelValue2;
            // Create a layout for the labels within the frame
            QVBoxLayout* frameLayout = new QVBoxLayout(frame);
            frameLayout->addWidget(labelChannel);
            frameLayout->addWidget(labels2[row][col]);
            frameLayout->setAlignment(Qt::AlignCenter);
            frames2[row][col] = frame;
            matrixLayout2->addWidget(frame, row, col);

        }
    }
    centralLayout->addWidget(headlineLabel);
    // Add the matrix layout to the central layout
    centralLayout->addLayout(matrixLayout1);
    centralLayout->addLayout(matrixLayout2);
    QHBoxLayout* inputButtonLayout = new QHBoxLayout();
    // Create the "Start Reading" button
    startButton = new QPushButton("Start Reading", centralWidget);
    valueInput = new QLineEdit(centralWidget);
    valueInput->setMaximumWidth(70);
    checkBox = new QCheckBox("Short Circuit", centralWidget);
    // Set the background color for the input field
    valueInput->setStyleSheet("background-color: lightgray;");
    // Create the label for "mA"
    QLabel* labelMA = new QLabel("Current (mA)");
    inputButtonLayout->addStretch();
    inputButtonLayout->addWidget(labelMA);
    inputButtonLayout->addWidget(valueInput);
    inputButtonLayout->addSpacing(10);
    inputButtonLayout->addWidget(startButton);
    inputButtonLayout->addWidget(checkBox);
    inputButtonLayout->addStretch();
    // Connect the button's clicked signal to a slot function
    connect(startButton, &QPushButton::clicked, this, &MainWindow::startReading);

    startButton->setStyleSheet("QPushButton { background-color: lightgray; }");
    // Set equal horizontal stretch for the input field and the button
    valueInput->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    startButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    // Add stretch to push the button to the bottom
    centralLayout->addStretch();
    centralLayout->addLayout(inputButtonLayout);
    centralWidget->setLayout(centralLayout);
    // Center-align the start button
    centralLayout->setAlignment(startButton, Qt::AlignHCenter);
    // Set the central layout as the layout for the central widget
    centralWidget->setLayout(centralLayout);

}
/****************************************************************************
 *
 * eventFilter
 *https://wiki.qt.io/How_to_catch_enter_key
 *
 * checks if the enter key is pressed
 *

 ****************************************************************************/
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type()==QEvent::KeyPress) {
        QKeyEvent* key = static_cast<QKeyEvent*>(event);
        if ( (key->key()==Qt::Key_Enter) || (key->key()==Qt::Key_Return) ) {
            //Enter or return was pressed
            handleInputValue();
        } else {
            return QObject::eventFilter(obj, event);
        }
        return true;
    } else {
        return QObject::eventFilter(obj, event);
    }
    return false;
}

void MainWindow::timeout()
{
    qDebug()<< "update..";
    //frames[row][col]->setStyleSheet("border-radius: 5px; background-color: red;");
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            frames[i][j]->setStyleSheet("border-radius: 5px; background-color: green;");
        }

    }
}
/****************************************************************************
 *
 * startReading
 *
 * when the user presses the start reading button this function is called
 * first checks if the picologs are available, changes the text on the button to stop reading
 * and then calls the picoInit function
 * else, if the button says stop reading and is pressed, we will call the stopReading function
 ****************************************************************************/
void MainWindow::startReading()
{
   if (startButton->text() == "Start Reading") {

        status = pl1000OpenUnit(&g_handle);
        if (status != PICO_OK) {
            qDebug() << "Unable to open device";
            QMessageBox::information(this, "Error!", "Unable to find device1");
            return;
        }
        status2 = pl1000OpenUnit(&g_handle2);
        if (status2 != PICO_OK) {
            qDebug() << "Unable to open device";
            QMessageBox::information(this, "Error!", "Unable to find device2");
            return;
        }
        // Change the button text to "Stop Reading"
        startButton->setText("Stop Reading");

        // Disable the "Start Reading" button to prevent multiple clicks
        startButton->setEnabled(false);
        QMessageBox::information(this, "Start Button Clicked", "Data collection started!");
        // Call the collectIndividual() function to start the data collection
        PicoInit();
    } else if (startButton->text() == "Stop Reading") {
        // Change the button text back to "Start Reading"
        startButton->setText("Start Reading");

        // Enable the "Start Reading" button
        startButton->setEnabled(true);

        // Pause the data collection
        stopReading();
    }

 /*   if (startButton->text() == "Start Reading") {
            // Start reading
            status = pl1000OpenUnit(&g_handle);
            if (status != PICO_OK) {
                qDebug() << "Unable to open device";
                QMessageBox::information(this, "Error!", "Unable to find device 1");
                return;
            }
            status2 = pl1000OpenUnit(&g_handle2);
            if (status2 != PICO_OK) {
                qDebug() << "Unable to open device";
                QMessageBox::information(this, "Error!", "Unable to find device 2");
                return;
            }
            // Change the button text to "Pause Reading"
            startButton->setText("Pause Reading");
            QMessageBox::information(this, "Start Button Clicked", "Data collection started!");
            // Call the collectIndividual() function to start the data collection
            PicoInit();

    } else if (startButton->text() == "Pause Reading") {
        // Change the button text back to "Resume Reading"
        startButton->setText("Resume Reading");
        QMessageBox::information(this, "Pause Button Clicked", "Data collection paused!");
        pl1000CloseUnit(g_handle);
        pl1000CloseUnit(g_handle2);
    } else if (startButton->text() == "Resume Reading") {
        // Change the button text back to "Pause Reading"
        startButton->setText("Pause Reading");
        status = pl1000OpenUnit(&g_handle);
        if (status != PICO_OK) {
            qDebug() << "Unable to open device";
            QMessageBox::information(this, "Error!", "Unable to find device 1");
            return;
        }
        status2 = pl1000OpenUnit(&g_handle2);
        if (status2 != PICO_OK) {
            qDebug() << "Unable to open device";
            QMessageBox::information(this, "Error!", "Unable to find device 2");
            return;
        }
        QMessageBox::information(this, "Resume Button Clicked", "Data collection resumed!");
        // Perform any necessary actions to resume data collection here
        PicoInit();
    }*/

}
/****************************************************************************
 *
 * stopReading
 *
 * If we want to stop reading from the picologs,
 * Should maybe implement so that it doesnt stop but only pauses!
 *
 ****************************************************************************/
void MainWindow::stopReading(){
qDebug() << "Collection stopped" ;
QMessageBox::information(this, "Stop Button Clicked", "Data collection ended!");
}
/****************************************************************************
 *
 * handleInputValue
 *
 * Value taken from the text box from the user
 * taken as a string and then converted to a float value
 *
 ****************************************************************************/
void MainWindow::handleInputValue()
{
    QString text = valueInput->text();
    bool conversionOk;
    inputValue = text.toFloat(&conversionOk);

    if (!conversionOk){
        inputValue = 0; // Set a default value if the conversion fails
    QMessageBox::information(this, "Error", "Enter a numerical value");
    }

    // Example: Display the value in a message box
    QMessageBox::information(this, "Input Value", QString("The entered value is: %1").arg(inputValue));
    referenceVoltage(inputValue);
}
/****************************************************************************
 *
 * referenceVoltage
 *
 * Depending on the selected current value
 * Calculate the reference voltage
 *
 ****************************************************************************/
void MainWindow::referenceVoltage(int current){

    //double reference = ((((current * 0.001)*100)/100)*13)+0.48;
    double reference = ((((current * 0.001)*100)/100)*9.0)+0.46;

    // Check if the checkbox is checked
    if (checkBox && checkBox->isChecked()) {
        reference /= 2.0;
    }
    QMessageBox::information(this, "Ref Voltage", QString("The calculated reference voltage is: %1").arg(reference));

    //send this value through serial port communication
    serialCommunication(reference);
}

QByteArray MainWindow::doubleToByteArray(double value)
{
    QByteArray byteArray;
    quint16 dac_val = (quint16)(value/3.3*4096);
    byteArray.append((quint8)(dac_val & 0xff));
    byteArray.append((quint8)((dac_val >> 8) & 0xff));

    return byteArray;



}

// Example function to transmit the double value
void MainWindow::transmitDouble(double value)
{
    QByteArray byteArray = doubleToByteArray(value);
    qDebug() << "data: "<<byteArray;
    // Assuming uart is the QSerialPort instance for UART communication
    serialPort.write(byteArray);
}

/****************************************************************************
 *
 * serialCommunication
 *
 * The calculated reference voltage will be sent to stm32 through serial port communication
 *
 *
 ****************************************************************************/
void MainWindow::serialCommunication(double ref){

    // Set the name of the serial port
    serialPort.setPortName("COM5");

    // Set the baud rate and other settings
    serialPort.setBaudRate(QSerialPort::Baud9600);
    serialPort.setDataBits(QSerialPort::Data8);
    serialPort.setParity(QSerialPort::NoParity);
    serialPort.setStopBits(QSerialPort::OneStop);
    serialPort.setFlowControl(QSerialPort::NoFlowControl);


    // Open the serial port for reading and writing
      if (!serialPort.open(QIODevice::ReadWrite)) {
        qCritical() << "Failed to open serial port:" << serialPort.errorString();
    }
    qDebug() << "Serial port opened";


    //double value = 2.14;
    transmitDouble(ref);
    qDebug() << "Transmission succeeded! Value sent:" << ref;


    qDebug() << "Waiting for response";
    // Wait for the response
            // Wait for data to be available
            if (serialPort.waitForReadyRead(5000)) {
                QByteArray responseData = serialPort.readAll();
                qDebug() << "Received response:" << responseData;

                // Check if the received byte is '0'
                if (responseData == "0") {
                    qDebug() << "Response received: Success!";
                }
            } else {
                qDebug() << "Timeout: No response received.";
            }

    serialPort.close();
}
void MainWindow::PicoInit(){
    int8_t	info[80];
    int16_t	requiredSize = 0;
    if (status != PICO_OK)
    {
        printf ("Unable to open device\nPress any key\n");
        //qDebug() << "Unable to open device\nPress any key\n";
        _getch();
        exit(99);
    }
    else{

        // Set all digital outputs to zero
        pl1000SetDo(g_handle, 0, PL1000_DO_CHANNEL_0);
        pl1000SetDo(g_handle, 0, PL1000_DO_CHANNEL_1);
        pl1000SetDo(g_handle, 0, PL1000_DO_CHANNEL_2);
        pl1000SetDo(g_handle, 0, PL1000_DO_CHANNEL_3);

        //printf ("Device opened successfully\n\n");
        qDebug() << "Device opened successfully\n\n";
        status = pl1000GetUnitInfo(g_handle, info, 80, &requiredSize, PICO_VARIANT_INFO);
        //printf("Model:\t\t\t %s\n", info);
        qDebug() << "Model:\t\t\t %s\n" << info;

        status = pl1000GetUnitInfo(g_handle, info, 80, &requiredSize, PICO_BATCH_AND_SERIAL);
        printf("Serial Number:\t\t %s\n", info);
        //qDebug() << "Serial Number:\t\t %s\n"<< info;

        status = pl1000GetUnitInfo(g_handle, info, 80, &requiredSize, PICO_CAL_DATE);
        printf("Calibration Date:\t %s\n", info);
        //qDebug() << "Calibration Date:\t %s\n" << info;

        status = pl1000GetUnitInfo(g_handle, info, 80, &requiredSize, PICO_USB_VERSION);
        printf("USB version:\t\t %s\n", info);
        //qDebug() << "USB version:\t\t %s\n" << info;

        status = pl1000GetUnitInfo(g_handle, info, 80, &requiredSize, PICO_HARDWARE_VERSION);
        printf("Hardware version:\t %s\n", info);
        //qDebug() << "Hardware version:\t %s\n"<< info;

        status = pl1000GetUnitInfo(g_handle, info, 80, &requiredSize, PICO_DRIVER_VERSION);
        printf("pl1000.dll version:\t %s\n", info);
        //qDebug() << "pl1000.dll version:\t %s\n" << info;

        status = pl1000GetUnitInfo(g_handle, info, 80, &requiredSize, PICO_KERNEL_VERSION);
        printf("Kernel version:\t\t %s\n", info);
        //qDebug() << "Kernel version:\t\t %s\n"<< info;

        status = pl1000GetUnitInfo(g_handle, info, 80, &requiredSize, PICO_FIRMWARE_VERSION_1);
        printf("Firmware:\t\t %s\n", info);
        //qDebug() << "Firmware:\t\t %s\n"<< info;

        // Find the maximum ADC count for the device

        status = pl1000MaxValue(g_handle, &max_adc_value);
    }

    if (status2 != PICO_OK)
    {
        printf ("Unable to open device\nPress any key\n");
        //qDebug() << "Unable to open device\nPress any key\n";
        _getch();
        exit(99);
    }
    else{

        // Set all digital outputs to zero
        pl1000SetDo(g_handle2, 0, PL1000_DO_CHANNEL_0);
        pl1000SetDo(g_handle2, 0, PL1000_DO_CHANNEL_1);
        pl1000SetDo(g_handle2, 0, PL1000_DO_CHANNEL_2);
        pl1000SetDo(g_handle2, 0, PL1000_DO_CHANNEL_3);

        //printf ("Device opened successfully\n\n");
        qDebug() << "Device opened successfully\n\n";
        status2 = pl1000GetUnitInfo(g_handle2, info, 80, &requiredSize, PICO_VARIANT_INFO);
        //printf("Model:\t\t\t %s\n", info);
        qDebug() << "Model:\t\t\t %s\n" << info;
        status2 = pl1000GetUnitInfo(g_handle2, info, 80, &requiredSize, PICO_BATCH_AND_SERIAL);
        printf("Serial Number:\t\t %s\n", info);
        //qDebug() << "Serial Number:\t\t %s\n"<< info;

        status2 = pl1000GetUnitInfo(g_handle2, info, 80, &requiredSize, PICO_CAL_DATE);
        printf("Calibration Date:\t %s\n", info);
        //qDebug() << "Calibration Date:\t %s\n" << info;

        status2 = pl1000GetUnitInfo(g_handle2, info, 80, &requiredSize, PICO_USB_VERSION);
        printf("USB version:\t\t %s\n", info);
        //qDebug() << "USB version:\t\t %s\n" << info;

        status2 = pl1000GetUnitInfo(g_handle2, info, 80, &requiredSize, PICO_HARDWARE_VERSION);
        printf("Hardware version:\t %s\n", info);
        //qDebug() << "Hardware version:\t %s\n"<< info;

        status2 = pl1000GetUnitInfo(g_handle2, info, 80, &requiredSize, PICO_DRIVER_VERSION);
        printf("pl1000.dll version:\t %s\n", info);
        //qDebug() << "pl1000.dll version:\t %s\n" << info;

        status2 = pl1000GetUnitInfo(g_handle2, info, 80, &requiredSize, PICO_KERNEL_VERSION);
        printf("Kernel version:\t\t %s\n", info);
        //qDebug() << "Kernel version:\t\t %s\n"<< info;

        status2 = pl1000GetUnitInfo(g_handle2, info, 80, &requiredSize, PICO_FIRMWARE_VERSION_1);
        printf("Firmware:\t\t %s\n", info);
        //qDebug() << "Firmware:\t\t %s\n"<< info;

        // Find the maximum ADC count for the device

        status2 = pl1000MaxValue(g_handle2, &max_adc_value);
    }
    collectIndividual();
}

/****************************************************************************
 *
 * collectIndividual
 *
 * If start reading button is pressed
 * Data collection from the picolog will begin
 *
 ****************************************************************************/
void MainWindow::collectIndividual()
{
    startButton->setEnabled(true);
    bool stop = false;
    int32_t		c;
    uint16_t		value;
    uint16_t		value2;

    while (!stop)
    {
       // Sleep (100);
     /*   printf("Channels 1-%d: \n", numDeviceChannels);
        for (c = (int32_t)PL1000_CHANNEL_1; c <= numDeviceChannels; c++)
        {
            printf("ch%02d  ", c);
        }*/
       // printf("\n");
        for (c = (int32_t)PL1000_CHANNEL_1; c <= numDeviceChannels; c++)
        {
            value = 0;
            pl1000GetSingle(g_handle, static_cast<PL1000_INPUTS>(c), &value);

            int intValue = adc_to_mv(value);
            //printf("%5d ", adc_to_mv(value));
            labels[row][col]->setText(QString("Voltage(mV): %1").arg(adc_to_mv (value)));
            //frames[row][col]->setStyleSheet("border-radius: 5px; background-color: green;");
            if (intValue > 2000){
                frames[row][col]->setStyleSheet("border-radius: 5px; background-color: red;");
                //timer->start(50);
            }
            else{
               // frames[row][col]->setStyleSheet("border-radius: 5px; background-color: green;");
            }
            if (++col >= 4) {
                col = 0;
                if (++row >= 4) {
                    row = 0;
                }
            }
        }
        /*printf("\n");
        printf("Channels 17-32: \n");
        for (c = (int32_t)PL1000_CHANNEL_1; c <= numDeviceChannels; c++)
        {
            printf("ch%02d  ", c + numDeviceChannels);
        }
        printf("\n");*/
        row = 0;
        col = 0;
        for (c = (int32_t)PL1000_CHANNEL_1; c <= numDeviceChannels; c++)
        {
            value2 = 0;
            pl1000GetSingle(g_handle2, static_cast<PL1000_INPUTS>(c), &value2);

            int intValue2 = adc_to_mv(value2);
            labels2[row][col]->setText(QString("Voltage(mV): %1").arg(adc_to_mv (value2)));

            if (intValue2 > 2000)
                frames2[row][col]->setStyleSheet("border-radius: 5px; background-color: red;");
            else
               // frames2[row][col]->setStyleSheet("border-radius: 5px; background-color: green;");

            if (++col >= 4) {
                col = 0;
                if (++row >= 4) {
                    row = 0;
                }
            }
            //printf("%5d ", adc_to_mv(value2));

        }
        //printf("\n");
        //qDebug() << "\n";
        QApplication::processEvents();
               if (startButton->text() == "Start Reading")
                   stop = true;

    }

    _getch ();

}
/****************************************************************************
 *
 * adc_to_mv
 *
 * If the user selects scaling to millivolts,
 * Convert a 10- or 12-bit ADC count into millivolts
 *
 ****************************************************************************/
int32_t MainWindow::adc_to_mv (int32_t raw)
{
    int32_t	scaled;

    if (scale_to_mv)
    {
        scaled = raw * 2500 / max_adc_value;
    }
    else
    {
        scaled = raw;
    }

    return scaled;
}
