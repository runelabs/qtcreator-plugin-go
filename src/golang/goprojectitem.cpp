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

#include "goprojectitem.h"
#include "filefilteritems.h"

#include <QDebug>
#include <QDir>

namespace GoLang {

class GoCommandItemPrivate : public QObject {
    Q_OBJECT

public:
    QString sourceDirectory;
    QStringList importPaths;
    QStringList absoluteImportPaths;
    QString mainFile;

    QList<QmlFileFilterItem*> qmlFileFilters() const;

    // content property
    QList<GoProjectContentItem*> content;
};

QList<QmlFileFilterItem*> GoCommandItemPrivate::qmlFileFilters() const
{
    QList<QmlFileFilterItem*> qmlFilters;
    for (int i = 0; i < content.size(); ++i) {
        GoProjectContentItem *contentElement = content.at(i);
        QmlFileFilterItem *qmlFileFilter = qobject_cast<QmlFileFilterItem*>(contentElement);
        if (qmlFileFilter)
            qmlFilters << qmlFileFilter;
    }
    return qmlFilters;
}

GoCommandItem::GoCommandItem(QObject *parent) :
        QObject(parent),
        d_ptr(new GoCommandItemPrivate)
{
//    Q_D(QmlProjectItem);
//
//    QmlFileFilter *defaultQmlFilter = new QmlFileFilter(this);
//    d->content.append(defaultQmlFilter);
}

GoCommandItem::~GoCommandItem()
{
    delete d_ptr;
}

QString GoCommandItem::sourceDirectory() const
{
    Q_D(const GoCommandItem);
    return d->sourceDirectory;
}

// kind of initialization
void GoCommandItem::setSourceDirectory(const QString &directoryPath)
{
    Q_D(GoCommandItem);

    if (d->sourceDirectory == directoryPath)
        return;

    d->sourceDirectory = directoryPath;

    for (int i = 0; i < d->content.size(); ++i) {
        GoProjectContentItem *contentElement = d->content.at(i);
        FileFilterBaseItem *fileFilter = qobject_cast<FileFilterBaseItem*>(contentElement);
        if (fileFilter) {
            fileFilter->setDefaultDirectory(directoryPath);
            connect(fileFilter, SIGNAL(filesChanged(QSet<QString>,QSet<QString>)),
                    this, SIGNAL(qmlFilesChanged(QSet<QString>,QSet<QString>)));
        }
    }

    setImportPaths(d->importPaths);

    emit sourceDirectoryChanged();
}

QStringList GoCommandItem::importPaths() const
{
    Q_D(const GoCommandItem);
    return d->absoluteImportPaths;
}

void GoCommandItem::setImportPaths(const QStringList &importPaths)
{
    Q_D(GoCommandItem);

    if (d->importPaths != importPaths)
        d->importPaths = importPaths;

    // convert to absolute paths
    QStringList absoluteImportPaths;
    const QDir sourceDir(sourceDirectory());
    foreach (const QString &importPath, importPaths)
        absoluteImportPaths += QDir::cleanPath(sourceDir.absoluteFilePath(importPath));

    if (d->absoluteImportPaths == absoluteImportPaths)
        return;

    d->absoluteImportPaths = absoluteImportPaths;
    emit importPathsChanged();
}

/* Returns list of absolute paths */
QStringList GoCommandItem::files() const
{
    Q_D(const GoCommandItem);
    QStringList files;

    if(!mainFile().isEmpty())
        files.append(mainFile());

    for (int i = 0; i < d->content.size(); ++i) {
        GoProjectContentItem *contentElement = d->content.at(i);
        FileFilterBaseItem *fileFilter = qobject_cast<FileFilterBaseItem*>(contentElement);
        if (fileFilter) {
            foreach (const QString &file, fileFilter->files()) {
                if (!files.contains(file))
                    files << file;
            }
        }
    }
    return files;
}

/**
  Check whether the project would include a file path
  - regardless whether the file already exists or not.

  @param filePath: absolute file path to check
  */
bool GoCommandItem::matchesFile(const QString &filePath) const
{
    Q_D(const GoCommandItem);
    for (int i = 0; i < d->content.size(); ++i) {
        GoProjectContentItem *contentElement = d->content.at(i);
        FileFilterBaseItem *fileFilter = qobject_cast<FileFilterBaseItem*>(contentElement);
        if (fileFilter) {
            if (fileFilter->matchesFile(filePath))
                return true;
        }
    }
    return false;
}

QString GoCommandItem::mainFile() const
{
    Q_D(const GoCommandItem);
    return d->mainFile;
}

void GoCommandItem::setMainFile(const QString &mainFilePath)
{
    Q_D(GoCommandItem);
    if (mainFilePath == d->mainFile)
        return;
    d->mainFile = mainFilePath;
    emit mainFileChanged();
}

void GoCommandItem::appendContent(GoProjectContentItem *contentItem)
{
    Q_D(GoCommandItem);
    d->content.append(contentItem);
}


GoProjectItem::GoProjectItem(QObject *parent) : QObject(parent)
{

}

void GoProjectItem::appendCommand(GoCommandItem *contentItem)
{
    m_content.append(contentItem);
    connect(contentItem,SIGNAL(qmlFilesChanged(QSet<QString>,QSet<QString>)),this,SIGNAL(filesChanged(QSet<QString>,QSet<QString>)));
}

QList<GoCommandItem *> GoProjectItem::commands() const
{
    return m_content;
}

QStringList GoProjectItem::files() const
{
    QStringList files;
    foreach(GoCommandItem* it, m_content)
        files += it->files();

    return files;
}

bool GoProjectItem::matchesFile(const QString &filePath) const
{
    foreach(GoCommandItem* it, m_content) {
        if(it->matchesFile(filePath))
            return true;
    }
    return false;
}

QString GoProjectItem::sourceDirectory() const
{
    return m_sourceDir;
}

void GoProjectItem::setSourceDirectory(const QString &directoryPath)
{
    foreach(GoCommandItem* it, m_content)
        it->setSourceDirectory(directoryPath);

    m_sourceDir = directoryPath;
}

} // namespace QmlProjectManager

#include "goprojectitem.moc"
