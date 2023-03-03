/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *									   *
 *   Peak/RMS added (c)19/06/2007 by Jason Lucas			   *
 ***************************************************************************/

#include "ecvoltagesignal.h"
#include "ecnode.h"
#include "libraryitem.h"
#include "pin.h"
#include "simulator.h"
#include "voltagesignal.h"

#include <KLocalizedString>
#include <QPainter>
#include <QString>
#include <cmath>

Item *ECVoltageSignal::construct(ItemDocument *itemDocument, bool newItem, const char *id)
{
    return new ECVoltageSignal((ICNDocument *)itemDocument, newItem, id);
}

LibraryItem *ECVoltageSignal::libraryItem()
{
    return new LibraryItem(QStringList(QString("ec/voltage_signal")), i18n("Voltage Signal"), i18n("Sources"), "voltagesignal.png", LibraryItem::lit_component, ECVoltageSignal::construct);
}

ECVoltageSignal::ECVoltageSignal(ICNDocument *icnDocument, bool newItem, const char *id)
    : Component(icnDocument, newItem, id ? id : "voltage_signal")
{
    m_name = i18n("Voltage Signal");
    setSize(-8, -8, 16, 16);

    init1PinLeft();
    init1PinRight();

    m_pNNode[0]->pin()->setGroundType(Pin::gt_medium);
    m_voltageSignal = createVoltageSignal(m_pNNode[0], m_pPNode[0], 0.);
    m_voltageSignal->setStep(ElementSignal::st_sinusoidal, 50., 0);

    createProperty("frequency", Variant::Type::Double);
    property("frequency")->setCaption(i18n("Frequency"));
    property("frequency")->setUnit("Hz");
    property("frequency")->setMinValue(1e-9);
    property("frequency")->setMaxValue(1e3);
    property("frequency")->setValue(50.0);

    createProperty("voltage", Variant::Type::Double);
    property("voltage")->setCaption(i18n("Voltage Range"));
    property("voltage")->setUnit("V");
    property("voltage")->setMinValue(-1e12);
    property("voltage")->setMaxValue(1e12);
    property("voltage")->setValue(5.0);

    addDisplayText("~", QRect(-8, -8, 16, 16), "~");
    addDisplayText("voltage", QRect(-16, -24, 32, 16), "");

    createProperty("peak-rms", Variant::Type::Select);
    property("peak-rms")->setCaption(i18n("Output"));
    QStringMap allowed;
    allowed["Peak"] = i18n("Peak");
    allowed["RMS"] = i18n("RMS");
    property("peak-rms")->setAllowed(allowed);
    property("peak-rms")->setValue("Peak");

    createProperty("phase-angle", Variant::Type::Double);
    property("phase-angle")->setCaption(i18n("Phase Angle"));
    property("phase-angle")->setUnit("rad");
    property("phase-angle")->setMinValue(0);
    property("phase-angle")->setMaxValue(2 * M_PI);
    property("phase-angle")->setValue(0);
}

ECVoltageSignal::~ECVoltageSignal()
{
}

void ECVoltageSignal::dataChanged()
{
    const double voltage = dataDouble("voltage");
    const double frequency = dataDouble("frequency");
    bool rms = dataString("peak-rms") == "RMS";
    const double phase = dataDouble("phase-angle");

    m_voltageSignal->setStep(ElementSignal::st_sinusoidal, frequency, phase);
    if (rms) {
        QString display = QString::number(voltage / getMultiplier(voltage), 'g', 3) + getNumberMag(voltage) + "V RMS";
        setDisplayText("voltage", display);
        m_voltageSignal->setVoltage(voltage * M_SQRT2);
    } else {
        QString display = QString::number(voltage / getMultiplier(voltage), 'g', 3) + getNumberMag(voltage) + "V Peak";
        setDisplayText("voltage", display);
        m_voltageSignal->setVoltage(voltage);
    }
}

void ECVoltageSignal::drawShape(QPainter &p)
{
    initPainter(p);
    p.drawEllipse((int)x() - 8, (int)y() - 8, width(), height());
    deinitPainter(p);
}
