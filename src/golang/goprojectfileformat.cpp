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

#include "goprojectfileformat.h"
#include "goprojectitem.h"
#include "filefilteritems.h"
#include <qmljs/qmljssimplereader.h>

#include <QVariant>
#include <QDebug>

enum {
    debug = false
};

namespace   {

void setupFileFilterItem(GoLang::FileFilterBaseItem *fileFilterItem, const QmlJS::SimpleReaderNode::Ptr &node)
{
    const QVariant directoryProperty = node->property(QLatin1String("directory"));
    if (directoryProperty.isValid())
        fileFilterItem->setDirectory(directoryProperty.toString());

    const QVariant recursiveProperty = node->property(QLatin1String("recursive"));
    if (recursiveProperty.isValid())
        fileFilterItem->setRecursive(recursiveProperty.toBool());

    const QVariant pathsProperty = node->property(QLatin1String("paths"));
    if (pathsProperty.isValid())
        fileFilterItem->setPathsProperty(pathsProperty.toStringList());

    const QVariant filterProperty = node->property(QLatin1String("filter"));
    if (filterProperty.isValid())
        fileFilterItem->setFilter(filterProperty.toString());

    if (debug)
        qDebug() << "directory:" << directoryProperty << "recursive" << recursiveProperty << "paths" << pathsProperty;
}

} //namespace

namespace GoLang {

GoProjectItem *GoProjectFileFormat::parseProjectFile(const QString &fileName, QString *errorMessage)
{
    QmlJS::SimpleReader simpleQmlJSReader;

    const QmlJS::SimpleReaderNode::Ptr rootNode = simpleQmlJSReader.readFile(fileName);

    if (!simpleQmlJSReader.errors().isEmpty() || !rootNode->isValid()) {
        qWarning() << "unable to parse:" << fileName;
        qWarning() << simpleQmlJSReader.errors();
        if (errorMessage)
            *errorMessage = simpleQmlJSReader.errors().join(QLatin1String(", "));
        return 0;
    }

    if (rootNode->name() == QLatin1String("GoProject")) {
        GoProjectItem *projectItem = new GoProjectItem();

        foreach (const QmlJS::SimpleReaderNode::Ptr &prochildNode, rootNode->children()) {
            if (prochildNode->name() == QLatin1String("Command")) {
                GoCommandItem* commandItem = new GoCommandItem(projectItem);

                const QVariant mainFileProperty = prochildNode->property(QLatin1String("mainFile"));
                if (mainFileProperty.isValid())
                    commandItem->setMainFile(mainFileProperty.toString());

                const QVariant importPathsProperty = prochildNode->property(QLatin1String("qmlImportPaths"));
                if (importPathsProperty.isValid())
                    commandItem->setImportPaths(importPathsProperty.toStringList());

                if (debug)
                    qDebug() << "importPath:" << importPathsProperty << "mainFile:" << mainFileProperty;

                projectItem->appendCommand(commandItem);

                foreach (const QmlJS::SimpleReaderNode::Ptr &childNode, prochildNode->children()) {
                    if (childNode->name() == QLatin1String("GoFiles")) {
                        if (debug)
                            qDebug() << "GoFiles";
                        GoFileFilterItem *qmlFileFilterItem = new GoFileFilterItem(commandItem);
                        setupFileFilterItem(qmlFileFilterItem, childNode);
                        commandItem->appendContent(qmlFileFilterItem);
                    } else if (childNode->name() == QLatin1String("QmlFiles")) {
                        if (debug)
                            qDebug() << "QmlFiles";
                        QmlFileFilterItem *qmlFileFilterItem = new QmlFileFilterItem(commandItem);
                        setupFileFilterItem(qmlFileFilterItem, childNode);
                        commandItem->appendContent(qmlFileFilterItem);
                    } else if (childNode->name() == QLatin1String("JavaScriptFiles")) {
                        if (debug)
                            qDebug() << "JavaScriptFiles";
                        JsFileFilterItem *jsFileFilterItem = new JsFileFilterItem(commandItem);
                        setupFileFilterItem(jsFileFilterItem, childNode);
                        commandItem->appendContent(jsFileFilterItem);
                    } else if (childNode->name() == QLatin1String("ImageFiles")) {
                        if (debug)
                            qDebug() << "ImageFiles";
                        ImageFileFilterItem *imageFileFilterItem = new ImageFileFilterItem(commandItem);
                        setupFileFilterItem(imageFileFilterItem, childNode);
                        commandItem->appendContent(imageFileFilterItem);

                    } else if (childNode->name() == QLatin1String("CssFiles")) {
                        if (debug)
                            qDebug() << "CssFiles";
                        CssFileFilterItem *cssFileFilterItem = new CssFileFilterItem(commandItem);
                        setupFileFilterItem(cssFileFilterItem, childNode);
                        commandItem->appendContent(cssFileFilterItem);
                    } else if (childNode->name() == QLatin1String("Files")) {
                        if (debug)
                            qDebug() << "Files";
                        OtherFileFilterItem *otherFileFilterItem = new OtherFileFilterItem(commandItem);
                        setupFileFilterItem(otherFileFilterItem, childNode);
                        commandItem->appendContent(otherFileFilterItem);
                    } else {
                        qWarning() << "Unknown type:" << childNode->name();
                    }
                }
            }
        }
        return projectItem;
    }

    if (errorMessage)
        *errorMessage = tr("Invalid root element: %1").arg(rootNode->name());

    return 0;
}

} // namespace QmlProjectManager
