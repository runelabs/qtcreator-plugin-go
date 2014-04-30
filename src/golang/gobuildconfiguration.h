#ifndef GOLANG_INTERNAL_GOBUILDCONFIGURATION_H
#define GOLANG_INTERNAL_GOBUILDCONFIGURATION_H

#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/abi.h>
#include <projectexplorer/abstractprocessstep.h>
#include <projectexplorer/task.h>

namespace GoLang {

class GoBuildConfiguration : public ProjectExplorer::BuildConfiguration
{
    Q_OBJECT
public:
    GoBuildConfiguration(ProjectExplorer::Target *target);
    GoBuildConfiguration(ProjectExplorer::Target *target, GoBuildConfiguration *other);

    // BuildConfiguration interface
    virtual ProjectExplorer::NamedWidget *createConfigWidget();
    virtual QList<ProjectExplorer::NamedWidget *> createSubConfigWidgets();
    virtual bool isEnabled() const;
    virtual QString disabledReason() const;
    virtual BuildType buildType() const;

protected:
    virtual void setBuildDirectory(const Utils::FileName &dir);

    // ProjectConfiguration interface
public:
    virtual bool fromMap(const QVariantMap &map);
    virtual QVariantMap toMap() const;

    friend class GoBuildConfigurationFactory;
};

class GoBuildConfigurationFactory : public ProjectExplorer::IBuildConfigurationFactory
{
    Q_OBJECT
public:
    GoBuildConfigurationFactory(QObject *parent = 0);
    ~GoBuildConfigurationFactory();

    // IBuildConfigurationFactory interface
    virtual int priority(const ProjectExplorer::Target *parent) const;
    virtual QList<ProjectExplorer::BuildInfo *> availableBuilds(const ProjectExplorer::Target *parent) const;
    virtual int priority(const ProjectExplorer::Kit *k, const QString &projectPath) const;
    virtual QList<ProjectExplorer::BuildInfo *> availableSetups(const ProjectExplorer::Kit *k, const QString &projectPath) const;
    virtual GoBuildConfiguration *create(ProjectExplorer::Target *parent, const ProjectExplorer::BuildInfo *info) const;
    virtual bool canRestore(const ProjectExplorer::Target *parent, const QVariantMap &map) const;
    virtual GoBuildConfiguration *restore(ProjectExplorer::Target *parent, const QVariantMap &map);
    virtual bool canClone(const ProjectExplorer::Target *parent, ProjectExplorer::BuildConfiguration *product) const;
    virtual GoBuildConfiguration*clone(ProjectExplorer::Target *parent, ProjectExplorer::BuildConfiguration *product);
private:
    virtual QList<ProjectExplorer::BuildInfo *> availableBuilds(const ProjectExplorer::Kit *kit, const QString &projectFilePath) const;
    bool canHandle(const ProjectExplorer::Target *t) const;

};

class GoBuildStepFactory : public ProjectExplorer::IBuildStepFactory
{
    Q_OBJECT

public:
    // IBuildStepFactory interface
    virtual QList<Core::Id> availableCreationIds(ProjectExplorer::BuildStepList *parent) const;
    virtual QString displayNameForId(const Core::Id id) const;
    virtual bool canCreate(ProjectExplorer::BuildStepList *parent, const Core::Id id) const;
    virtual ProjectExplorer::BuildStep *create(ProjectExplorer::BuildStepList *parent, const Core::Id id);
    virtual bool canRestore(ProjectExplorer::BuildStepList *parent, const QVariantMap &map) const;
    virtual ProjectExplorer::BuildStep *restore(ProjectExplorer::BuildStepList *parent, const QVariantMap &map);
    virtual bool canClone(ProjectExplorer::BuildStepList *parent, ProjectExplorer::BuildStep *product) const;
    virtual ProjectExplorer::BuildStep *clone(ProjectExplorer::BuildStepList *parent, ProjectExplorer::BuildStep *product);

private:
    bool canHandle(const ProjectExplorer::Target *t) const;
};

class GoBuildStep : public ProjectExplorer::AbstractProcessStep
{
    Q_OBJECT
public:

    GoBuildStep(ProjectExplorer::BuildStepList *bsl);
    GoBuildStep(ProjectExplorer::BuildStepList *bsl, GoBuildStep *bs);

    void setIsCleanStep (const bool set = true);
    bool isCleanStep () const;

    // BuildStep interface
    virtual bool init();
    virtual void run(QFutureInterface<bool> &fi);
    virtual ProjectExplorer::BuildStepConfigWidget *createConfigWidget();

    // ProjectConfiguration interface
    virtual bool fromMap(const QVariantMap &map);
    virtual QVariantMap toMap() const;

private:
    QList<ProjectExplorer::Task> m_tasks;
    bool m_clean;

    friend class GoBuildStepFactory;
};

} // namespace GoLang

#endif // GOLANG_INTERNAL_GOBUILDCONFIGURATION_H
