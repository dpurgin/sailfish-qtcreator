/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************/

#include "androidpotentialkit.h"
#include "androidconstants.h"
#include "androidconfigurations.h"

#include <utils/detailswidget.h>
#include <coreplugin/icore.h>
#include <projectexplorer/kitmanager.h>
#include <projectexplorer/kit.h>
#include <projectexplorer/kitinformation.h>
#include <qtsupport/qtversionmanager.h>
#include <qtsupport/baseqtversion.h>

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

using namespace Android;
using namespace Android::Internal;

QWidget *AndroidPotentialKit::createWidget(QWidget *parent) const
{
    QList<ProjectExplorer::Kit *> kits = ProjectExplorer::KitManager::kits();
    foreach (ProjectExplorer::Kit *kit, kits) {
        Core::Id deviceId = ProjectExplorer::DeviceKitInformation::deviceId(kit);
        if (kit->isAutoDetected()
                && deviceId == Core::Id(Constants::ANDROID_DEVICE_ID)
                && !kit->isSdkProvided()) {
            return 0;
        }
    }

    bool found = false;
    foreach (QtSupport::BaseQtVersion *version, QtSupport::QtVersionManager::validVersions()) {
        if (version->type() == QLatin1String(Constants::ANDROIDQT)) {
            found = true;
            break;
        }
    }

    if (!found) // no android qt
        return 0;

    return new AndroidPotentialKitWidget(parent);
}

AndroidPotentialKitWidget::AndroidPotentialKitWidget(QWidget *parent)
    : Utils::DetailsWidget(parent)
{
    setSummaryText(QLatin1String("<b>Create Android Kits</b>"));
    setIcon(QIcon(QLatin1String(Constants::ANDROID_SETTINGS_CATEGORY_ICON)));
    //detailsWidget->setState(Utils::DetailsWidget::NoSummary);
    QWidget *mainWidget = new QWidget(this);
    setWidget(mainWidget);

    QGridLayout *layout = new QGridLayout(mainWidget);
    layout->setMargin(0);
    QLabel *label = new QLabel;
    label->setText(tr("Qt Creator needs additional settings to enable Android support."
                      " You can configure those settings in the Options dialog."));
    label->setWordWrap(true);
    layout->addWidget(label, 0, 0, 1, 2);

    QPushButton *openOptions = new QPushButton;
    openOptions->setText(tr("Open Settings"));
    openOptions->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    layout->addWidget(openOptions, 1, 1);

    connect(openOptions, SIGNAL(clicked()),
            this, SLOT(openOptions()));

    connect(AndroidConfigurations::instance(), SIGNAL(updated()),
            this, SLOT(recheck()));
}

void AndroidPotentialKitWidget::openOptions()
{
    Core::ICore::showOptionsDialog(Constants::ANDROID_SETTINGS_CATEGORY,
                                   Constants::ANDROID_SETTINGS_ID);
}

void AndroidPotentialKitWidget::recheck()
{
    QList<ProjectExplorer::Kit *> kits = ProjectExplorer::KitManager::kits();
    foreach (ProjectExplorer::Kit *kit, kits) {
        Core::Id deviceId = ProjectExplorer::DeviceKitInformation::deviceId(kit);
        if (kit->isAutoDetected()
                && deviceId == Core::Id(Constants::ANDROID_DEVICE_ID)
                && !kit->isSdkProvided()) {
            setVisible(false);
            return;
        }
    }
}