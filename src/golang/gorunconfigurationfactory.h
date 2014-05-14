#ifndef GOLANG_INTERNAL_GORUNCONFIGURATIONFACTORY_H
#define GOLANG_INTERNAL_GORUNCONFIGURATIONFACTORY_H

#include <projectexplorer/runconfiguration.h>

namespace GoLang {
namespace Internal {

class GoRunConfigurationFactory : public ProjectExplorer::IRunConfigurationFactory
{
public:
    GoRunConfigurationFactory(QObject *parent = 0);
    QList<Core::Id> availableCreationIds(ProjectExplorer::Target *parent) const;
    QString displayNameForId(const Core::Id id) const;
    bool canHandle(ProjectExplorer::Target *parent) const;
    bool canCreate(ProjectExplorer::Target *parent, const Core::Id id) const;
    ProjectExplorer::RunConfiguration *doRestore(ProjectExplorer::Target *parent, const QVariantMap &map);
    bool canClone(ProjectExplorer::Target *parent, ProjectExplorer::RunConfiguration *product) const;
    ProjectExplorer::RunConfiguration *clone(ProjectExplorer::Target *parent, ProjectExplorer::RunConfiguration *source);
    bool canRestore(ProjectExplorer::Target *parent, const QVariantMap &map) const;
    ProjectExplorer::RunConfiguration *doCreate(ProjectExplorer::Target *parent, const Core::Id id);
};

} // namespace Internal
} // namespace GoLang

#endif // GOLANG_INTERNAL_GORUNCONFIGURATIONFACTORY_H
