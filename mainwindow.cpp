#include "mainwindow.h"
#include "ui_mainwindow.h"

int AN[4];
float Volt[4];
float amplitude=0.0;
float frequency=0.0,period=0.0;
float usramplitude=0.0;
float usrfrequency=0.0;
float first=0.0,last=0.0;
float oldUsrFreq=0.0,oldUsrAmp=0.0;
bool flag=true,ampFlag=false,freqFlag=false;
int speed=50;

#define MOTOR1_PWM 2    //pin 2 (13)
#define MOTOR1_ENABLE1 11   // pin 11 (26)
#define MOTOR1_ENABLE2 10   // pin 10 (24)

void zeroMotor();
void stopMotor();
void forward(int i);

void InitHW()
{
   wiringPiSetup();
   ads1115Setup(100,0x48);

   pinMode(MOTOR1_ENABLE1, OUTPUT); // in1
   pinMode(MOTOR1_ENABLE2, OUTPUT); // in2
   pinMode(MOTOR1_PWM, PWM_OUTPUT);

   if(softPwmCreate(MOTOR1_PWM, 0, 50)!=0 )
       exit(1);

   softPwmWrite(MOTOR1_PWM,20);

   //stopMotor();
   //zeroMotor();
   //delay(30);
}

void forward(int i){
        softPwmWrite(MOTOR1_PWM,i);
        digitalWrite(MOTOR1_ENABLE1,HIGH);
        digitalWrite(MOTOR1_ENABLE2,LOW);
        delay(100);
        softPwmWrite(MOTOR1_PWM,0);
}

void stopMotor(){
    digitalWrite(MOTOR1_ENABLE1, HIGH);
    digitalWrite(MOTOR1_ENABLE2, HIGH);
    delay(250);
    zeroMotor();
}

void zeroMotor(){
    digitalWrite(MOTOR1_ENABLE1, LOW);
    digitalWrite(MOTOR1_ENABLE2, LOW);
}

MainWindow::MainWindow(QWidget *parent) :QMainWindow(parent),ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setMinimumWidth( 700 );
    setMinimumHeight( 550 );

    InitHW();

    ui->customPlot->addGraph(); // blue line
    ui->customPlot->graph(0)->setPen(QPen(QColor(40, 110, 255)));
    ui->customPlot->addGraph(); // red line
    ui->customPlot->graph(1)->setPen(QPen(QColor(255, 110, 40)));
    ui->customPlot->addGraph(); // green line
    ui->customPlot->graph(2)->setPen(QPen(QColor(124, 252, 0)));

    ui->customPlot->yAxis->setRange(-5, 5);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(makePlot()));
    timer->start(timeInterval);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::makePlot()
{

    static QTime time(QTime::currentTime());
    double key = time.elapsed();
    static double lastPointKey = 0;

    for(int i=0;i<4;i++)
    {
        AN[i] = (int16_t) analogRead(100+i);
        Volt[i] = AN[i] * (4.096 / 32768);
    }

    ui->customPlot->graph(0)->addData(key, Volt[0]);
    ui->customPlot->graph(1)->addData(key, Volt[1]);
    ui->customPlot->graph(2)->addData(key, Volt[2]);

    if(amplitude < Volt[0])
        amplitude = Volt[0];

    if(Volt[0]==0.0){
        if(flag){
            first = time.elapsed();
            flag = false;
        }
        else{
            last = time.elapsed();
            flag = true;
        }
    }

    if(flag)
        period = ((last - first)*2);

    ui->label_3->setText(QString::number(frequency));
    ui->label_4->setText(QString::number(amplitude));

    usrfrequency = ui->frekansSlider->value();
    usramplitude = ui->genlikSlider->value();

    ui->speedBar->setValue(speed);

    if(oldUsrFreq != usrfrequency ){
        oldUsrFreq = usrfrequency;
        freqFlag=true;
        ampFlag=false;
    }
    if(oldUsrAmp != usramplitude ){
        oldUsrAmp = usramplitude;
        ampFlag=true;
        freqFlag=false;
    }


    if (key-lastPointKey > 1000.0) // at most add point every 2 ms
    {

      if(freqFlag){
          if(frequency > usrfrequency){
              if(speed<99){
                  speed+=2;
                  forward(speed);
              }
          }
          else{
              if(speed>1){
                  speed-=2;
                  forward(speed);
              }
          }
      }

      if(ampFlag){
          if(amplitude > usramplitude){
              if(speed>1){
                  speed-=2;
                  forward(speed);
              }
          }
          else{
              if(speed<99){
                  speed+=2;
                  forward(speed);
              }
          }
      }

      lastPointKey = key;
      amplitude = 0.0;
    }
    // make key axis range scroll with the data (at a constant range size of 8):
    ui->customPlot->xAxis->setRange(key, 8, Qt::AlignRight);
    ui->customPlot->replot();

    // calculate frames per second:
    static double lastFpsKey;
    static int frameCount;
    ++frameCount;
    if (key-lastFpsKey > 2) // average fps over 2 seconds
    {
      ui->statusBar->showMessage(
            QString("%1 FPS, Total Data points: %2")
            .arg(frameCount/(key-lastFpsKey), 0, 'f', 0)
            .arg(ui->customPlot->graph(0)->data()->size()+ui->customPlot->graph(1)->data()->size())
            , 0);
      lastFpsKey = key;
      frameCount = 0;
    }

}
