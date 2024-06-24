﻿/*

  Copyright (c) 2022 Ethan Zonca

  This file is part of cangaroo.

  cangaroo is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  cangaroo is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with cangaroo.  If not, see <http://www.gnu.org/licenses/>.

*/


#include "SLCANDriver.h"
#include "SLCANInterface.h"
#include <core/Backend.h>
#include <driver/GenericCanSetupPage.h>

#include <errno.h>
#include <cstring>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

//
#include <QCoreApplication>
#include <QDebug>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

int nuvoton_can = 0;

SLCANDriver::SLCANDriver(Backend &backend)
  : CanDriver(backend),
    setupPage(new GenericCanSetupPage())
{
    QObject::connect(&backend, SIGNAL(onSetupDialogCreated(SetupDialog&)), setupPage, SLOT(onSetupDialogCreated(SetupDialog&)));
}

SLCANDriver::~SLCANDriver() {
}

bool SLCANDriver::update() {

    deleteAllInterfaces();
    QSerialPort serial;

    int interface_cnt = 0;

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {

     serial.setPort(info);
     if(serial.open(QIODevice::ReadWrite)) {     //Colin 添加能够打开的串口，否则被占用
        serial.close();

        fprintf(stderr, "Name : %s \r\n",  info.portName().toStdString().c_str());
        fprintf(stderr, "Description : %s \r\n", info.description().toStdString().c_str());
        fprintf(stderr, "Manufacturer: %s \r\n", info.manufacturer().toStdString().c_str());

/*
//        qDebug()<<interface_cnt;
//        qDebug()<<info.vendorIdentifier();
//        qDebug()<<info.productIdentifier();
        qDebug()<<">>============<<\n\n";
        qDebug()<<info.description();
        qDebug()<<info.manufacturer();
        qDebug()<<info.portName();
        qDebug()<<info.serialNumber();
        qDebug()<<">>============<<\n\n";
*/
        log_info(">===>Port Info<===<");
        log_info(info.description());
        log_info(info.portName());
        log_info(info.serialNumber());


        if(info.vendorIdentifier() == 0xad50 && info.productIdentifier() == 0x60C4)
        {
             log_info("++CANable 1.0 or similar ST USB CDC device detected");
            // Create new slcan interface without FD support
            //SLCANInterface *intf =
            createOrUpdateInterface(interface_cnt, info.portName(), info.description(),false);
            interface_cnt++;
        }
        else if(info.vendorIdentifier() == 0x16D0 && info.productIdentifier() == 0x117E)
        {
             log_info("++CANable 2.0 detected");

            // Create new slcan interface without FD support
            //SLCANInterface *intf =
            createOrUpdateInterface(interface_cnt, info.portName(), info.description(),true);
            interface_cnt++;
        }
        else if(info.vendorIdentifier() == 0x0416 && info.productIdentifier() == 0x50A1)
        {
            log_info("++NuCAN 1.0 detected");
            nuvoton_can = 1;
            // Create new slcan interface without FD support
            //SLCANInterface *intf =
            createOrUpdateInterface(interface_cnt, info.portName(), info.description(),true);
            interface_cnt++;
        }
        else
        {
            perror("!! This is not a CANable device!");
        }
        log_info(">>======^O^======<<\n\n");
     }
    }
    return true;
}

QString SLCANDriver::getName() {

if(nuvoton_can == 1 )
        return "Nuvoton SLCAN";
else    return "CANable SLCAN";
}



SLCANInterface *SLCANDriver::createOrUpdateInterface(int index, QString name, QString description,bool fd_support) {


    foreach (CanInterface *intf, getInterfaces()) {
        SLCANInterface *scif = dynamic_cast<SLCANInterface*>(intf);
		if (scif->getIfIndex() == index) {
			scif->setName(name);
            scif->setDescription(description);//Colin
            return scif;
		}
	}

    SLCANInterface *scif = new SLCANInterface(this, index, name, description,fd_support);
    addInterface(scif);

    return scif;
}
