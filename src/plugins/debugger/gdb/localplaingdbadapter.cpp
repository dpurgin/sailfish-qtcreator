/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** Commercial Usage
**
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
**
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://qt.nokia.com/contact.
**
**************************************************************************/

#include "localplaingdbadapter.h"

#include "gdbengine.h"
#include "procinterrupt.h"
#include "debuggerstringutils.h"

#include <utils/qtcassert.h>

#include <QtCore/QFileInfo>
#include <QtCore/QProcess>
#include <QtGui/QMessageBox>

namespace Debugger {
namespace Internal {

///////////////////////////////////////////////////////////////////////
//
// PlainGdbAdapter
//
///////////////////////////////////////////////////////////////////////

LocalPlainGdbAdapter::LocalPlainGdbAdapter(GdbEngine *engine, QObject *parent)
    : AbstractPlainGdbAdapter(engine, parent)
{
    // Output
    connect(&m_outputCollector, SIGNAL(byteDelivery(QByteArray)),
        engine, SLOT(readDebugeeOutput(QByteArray)));
}

AbstractGdbAdapter::DumperHandling LocalPlainGdbAdapter::dumperHandling() const
{
    // LD_PRELOAD fails for System-Qt on Mac.
#if defined(Q_OS_WIN) || defined(Q_OS_MAC)
    return DumperLoadedByGdb;
#else
    return DumperLoadedByGdbPreload;
#endif
}

void LocalPlainGdbAdapter::startAdapter()
{
    QTC_ASSERT(state() == EngineStarting, qDebug() << state());
    setState(AdapterStarting);
    showMessage(_("TRYING TO START ADAPTER"));

    QStringList gdbArgs;

    if (!m_outputCollector.listen()) {
        emit adapterStartFailed(tr("Cannot set up communication with child process: %1")
                .arg(m_outputCollector.errorString()), QString());
        return;
    }
    gdbArgs.append(_("--tty=") + m_outputCollector.serverName());

    if (!startParameters().workingDirectory.isEmpty())
        m_gdbProc.setWorkingDirectory(startParameters().workingDirectory);
    if (!startParameters().environment.isEmpty())
        m_gdbProc.setEnvironment(startParameters().environment);

    if (!m_engine->startGdb(gdbArgs)) {
        m_outputCollector.shutdown();
        return;
    }

    emit adapterStarted();
    checkForReleaseBuild();
}

void LocalPlainGdbAdapter::checkForReleaseBuild()
{
    // Quick check for a "release" build
    QProcess proc;
    QStringList args;
    args.append(_("-h"));
    args.append(_("-j"));
    args.append(_(".debug_info"));
    args.append(startParameters().executable);
    proc.start(_("objdump"), args);
    proc.closeWriteChannel();
    QTC_ASSERT(proc.waitForStarted(), qDebug() << "UNABLE TO RUN OBJDUMP");
    proc.waitForFinished();
    QByteArray ba = proc.readAllStandardOutput();
    // This should yield something like
    // "debuggertest:     file format elf32-i386\n\n"
    // "Sections:\nIdx Name          Size      VMA       LMA       File off  Algn\n"
    // "30 .debug_info   00087d36  00000000  00000000  0006bbd5  2**0\n"
    // " CONTENTS, READONLY, DEBUGGING"
    if (ba.contains("Sections:") && !ba.contains(".debug_info")) {
        m_engine->showMessageBox(QMessageBox::Information, "Warning",
           tr("This does not seem to be a \"Debug\" build.\n"
              "Setting breakpoints by file name and line number may fail."));
    }
}

void LocalPlainGdbAdapter::interruptInferior()
{
    const qint64 attachedPID = runControl()->inferiorPid();
    if (attachedPID <= 0) {
        showMessage(_("TRYING TO INTERRUPT INFERIOR BEFORE PID WAS OBTAINED"));
        return;
    }

    if (!interruptProcess(attachedPID))
        showMessage(_("CANNOT INTERRUPT %1").arg(attachedPID));
}

void LocalPlainGdbAdapter::shutdown()
{
    showMessage(_("PLAIN ADAPTER SHUTDOWN %1").arg(state()));
    m_outputCollector.shutdown();
}

QByteArray LocalPlainGdbAdapter::execFilePath() const
{
    return QFileInfo(startParameters().executable)
            .absoluteFilePath().toLocal8Bit();
}

bool LocalPlainGdbAdapter::infoTargetNecessary() const
{
#ifdef Q_OS_LINUX
    return true;
#else
    return false;
#endif
}

QByteArray LocalPlainGdbAdapter::toLocalEncoding(const QString &s) const
{
    return s.toLocal8Bit();
}

QString LocalPlainGdbAdapter::fromLocalEncoding(const QByteArray &b) const
{
    return QString::fromLocal8Bit(b);
}

} // namespace Internal
} // namespace Debugger
