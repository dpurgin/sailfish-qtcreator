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

#include "indenter.h"
#include "tabsettings.h"

#include <QTextDocument>
#include <QTextCursor>

using namespace TextEditor;

Indenter::Indenter()
{}

Indenter::~Indenter()
{}

bool Indenter::isElectricCharacter(const QChar &) const
{
    return false;
}

void Indenter::indentBlock(QTextDocument *doc,
                             const QTextBlock &block,
                             const QChar &typedChar,
                             const TextEditor::TabSettings &tabSettings)
{
    Q_UNUSED(doc);
    Q_UNUSED(block);
    Q_UNUSED(typedChar);
    Q_UNUSED(tabSettings);
}

void Indenter::indent(QTextDocument *doc,
                      const QTextCursor &cursor,
                      const QChar &typedChar,
                      const TextEditor::TabSettings &tabSettings)
{
    if (cursor.hasSelection()) {
        QTextBlock block = doc->findBlock(cursor.selectionStart());
        const QTextBlock end = doc->findBlock(cursor.selectionEnd()).next();
        do {
            indentBlock(doc, block, typedChar, tabSettings);
            block = block.next();
        } while (block.isValid() && block != end);
    } else {
        indentBlock(doc, cursor.block(), typedChar, tabSettings);
    }
}

void Indenter::reindent(QTextDocument *doc, const QTextCursor &cursor, const TextEditor::TabSettings &tabSettings)
{
    if (cursor.hasSelection()) {
        QTextBlock block = doc->findBlock(cursor.selectionStart());
        const QTextBlock end = doc->findBlock(cursor.selectionEnd()).next();

        // skip empty blocks
        while (block.isValid() && block != end) {
            QString bt = block.text();
            if (tabSettings.firstNonSpace(bt) < bt.size())
                break;
            indentBlock(doc, block, QChar::Null, tabSettings);
            block = block.next();
        }

        int previousIndentation = tabSettings.indentationColumn(block.text());
        indentBlock(doc, block, QChar::Null, tabSettings);
        int currentIndentation = tabSettings.indentationColumn(block.text());
        int delta = currentIndentation - previousIndentation;

        block = block.next();
        while (block.isValid() && block != end) {
            tabSettings.reindentLine(block, delta);
            block = block.next();
        }
    } else {
        indentBlock(doc, cursor.block(), QChar::Null, tabSettings);
    }
}

void Indenter::setCodeStylePreferences(ICodeStylePreferences *)
{

}

void Indenter::invalidateCache(QTextDocument *)
{
}
