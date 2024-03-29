## About QextSerialPort

QextSerialPort provides an interface to old fashioned serial ports for Qt-based applications. It currently supports Mac OS X, Windows, Linux, FreeBSD. 

http://qextserialport.github.io/

## How to use (1)

* Download the source code.

* Put the source code in any directory you like. For example, 3rdparty:

```
            |-- project.pro
            |-- ....
            |-- 3rdparty\
            |     |-- qextserialport\
            |     |
```

* Add following line to your qmake project file:

```
            include(3rdparty/qextserialport/src/qextserialport.pri)
```

* Using QextSerialPort in your code. Enjoy it!

```
            #include "qextserialport.h"
            ....
            QextSerialPort * port = new QextSerialPort();
            ....
```
            
## How to use (2)
It's very easy to compile QextSerialPort directly into your application
(see above section), however, we would prefer to use it as a shared library.

* Download the source code, and put it in any location you like.

* Run following command to generate library. 

```
          qmake
          make (or nmake)
          sudo make install (or nmake install)
```

* Add following line to your project's file

```
          CONFIG += extserialport
```

* Using QextSerialPort in your code. Enjoy it!

```
            #include "qextserialport.h"
            ....
            QextSerialPort * port = new QextSerialPort();
            ....
```

## Build Documents
* Run
```
           qmake
           make docs
```

* Note: More information can be found in doc/readme.txt

## Build examples
* Goto examples directory, then run

```
           qmake (or qmake -r)
           make (or nmake)
```
