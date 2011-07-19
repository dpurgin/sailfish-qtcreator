/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2011 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (info@qt.nokia.com)
**
** GNU Lesser General Public License Usage
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this file.
** Please review the following information to ensure the GNU Lesser General
** Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** Other Usage
**
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
** If you have questions regarding the use of this file, please contact
** Nokia at info@qt.nokia.com.
**
**************************************************************************/

#ifndef REMOTELINUXAPPLICATIONRUNNER_H
#define REMOTELINUXAPPLICATIONRUNNER_H

#include "portlist.h"
#include "remotelinux_export.h"

#include <QtCore/QObject>
#include <QtCore/QSharedPointer>
#include <QtCore/QStringList>

namespace Utils {
class SshConnection;
class SshRemoteProcess;
}

namespace RemoteLinux {
class LinuxDeviceConfiguration;
class RemoteLinuxRunConfiguration;

namespace Internal {
class AbstractRemoteLinuxApplicationRunnerPrivate;
class MaemoUsedPortsGatherer;
}

class REMOTELINUX_EXPORT AbstractRemoteLinuxApplicationRunner : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(AbstractRemoteLinuxApplicationRunner)
public:
    AbstractRemoteLinuxApplicationRunner(QObject *parent, RemoteLinuxRunConfiguration *runConfig);
    ~AbstractRemoteLinuxApplicationRunner();

    void start();
    void stop();

    void startExecution(const QByteArray &remoteCall);

    QSharedPointer<Utils::SshConnection> connection() const;
    QSharedPointer<const LinuxDeviceConfiguration> devConfig() const;
    const Internal::MaemoUsedPortsGatherer *usedPortsGatherer() const;
    PortList *freePorts();
    QString remoteExecutable() const;
    QString arguments() const;
    QString commandPrefix() const;

    static const qint64 InvalidExitCode;

signals:
    void error(const QString &error);
    void readyForExecution();
    void remoteOutput(const QByteArray &output);
    void remoteErrorOutput(const QByteArray &output);
    void reportProgress(const QString &progressOutput);
    void remoteProcessStarted();
    void remoteProcessFinished(qint64 exitCode);

protected:
    // Override to to additional checks.
    virtual bool canRun(QString &whyNot) const;

    void handleDeviceSetupDone(bool success);
    void handleInitialCleanupDone(bool success);
    void handleInitializationsDone(bool success);
    void handlePostRunCleanupDone();

private slots:
    void handleConnected();
    void handleConnectionFailure();
    void handleCleanupFinished(int exitStatus);
    void handleRemoteProcessStarted();
    void handleRemoteProcessFinished(int exitStatus);
    void handlePortsGathererError(const QString &errorMsg);
    void handleUsedPortsAvailable();

private:

    virtual QString killApplicationCommandLine() const=0;

    // Implement to do custom setup of the device *before* connecting.
    // Call handleDeviceSetupDone() afterwards.
    virtual void doDeviceSetup()=0;

    // Implement to do additional pre-run cleanup and call handleInitialCleanupDone().
    virtual void doAdditionalInitialCleanup()=0;

    // Implement to do additional initializations right before the application is ready.
    // Call handleInitializationsDone() afterwards.
    virtual void doAdditionalInitializations()=0;

    // Implement to do cleanups after application exit and call handlePostRunCleanupDone();
    virtual void doPostRunCleanup()=0;

    virtual void doAdditionalConnectionErrorHandling()=0;

    void setInactive();
    void emitError(const QString &errorMsg, bool force = false);
    void cleanup();

    Internal::AbstractRemoteLinuxApplicationRunnerPrivate * const m_d;
};


class REMOTELINUX_EXPORT GenericRemoteLinuxApplicationRunner : public AbstractRemoteLinuxApplicationRunner
{
    Q_OBJECT
public:
    GenericRemoteLinuxApplicationRunner(QObject *parent, RemoteLinuxRunConfiguration *runConfig);
    ~GenericRemoteLinuxApplicationRunner();

protected:
    void doDeviceSetup();
    void doAdditionalInitialCleanup();
    void doAdditionalInitializations();
    void doPostRunCleanup();
    void doAdditionalConnectionErrorHandling();

private:
    QString killApplicationCommandLine() const;
};

} // namespace RemoteLinux

#endif // REMOTELINUXAPPLICATIONRUNNER_H
