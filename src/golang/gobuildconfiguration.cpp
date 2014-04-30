#include "gobuildconfiguration.h"
#include "golangconstants.h"
#include "gotoolchain.h"
#include "gokitinformation.h"
#include "goproject.h"

#include <projectexplorer/namedwidget.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/task.h>
#include <projectexplorer/target.h>
#include <projectexplorer/kitinformation.h>
#include <projectexplorer/kit.h>
#include <projectexplorer/buildinfo.h>
#include <projectexplorer/project.h>
#include <projectexplorer/buildsteplist.h>
#include <projectexplorer/ioutputparser.h>
#include <utils/qtcassert.h>
#include <utils/qtcprocess.h>
#include <coreplugin/mimedatabase.h>

namespace GoLang {

const char GO_BUILDSTEP_ISCLEAN_KEYC[] = "GoLang.GoBuildStep.IsCleanStep";

GoBuildConfiguration::GoBuildConfiguration(ProjectExplorer::Target *target) :
    ProjectExplorer::BuildConfiguration(target,Constants::GO_BUILDCONFIGURATION_ID)
{
}

GoBuildConfiguration::GoBuildConfiguration(ProjectExplorer::Target *target, GoBuildConfiguration *other)
    : ProjectExplorer::BuildConfiguration(target,other)
{

}

ProjectExplorer::NamedWidget *GoBuildConfiguration::createConfigWidget()
{
    return new ProjectExplorer::NamedWidget();
}

QList<ProjectExplorer::NamedWidget *> GoBuildConfiguration::createSubConfigWidgets()
{
    return BuildConfiguration::createSubConfigWidgets();
}

bool GoBuildConfiguration::isEnabled() const
{
    return true;
}

QString GoBuildConfiguration::disabledReason() const
{
    return QString();
}

ProjectExplorer::BuildConfiguration::BuildType GoBuildConfiguration::buildType() const
{
    return ProjectExplorer::BuildConfiguration::Debug;
}

void GoBuildConfiguration::setBuildDirectory(const Utils::FileName &dir)
{
    ProjectExplorer::BuildConfiguration::setBuildDirectory(dir);
}

bool GoBuildConfiguration::fromMap(const QVariantMap &map)
{
    if(!ProjectExplorer::BuildConfiguration::fromMap(map))
        return false;

    return true;
}

QVariantMap GoBuildConfiguration::toMap() const
{
    QVariantMap map = ProjectExplorer::BuildConfiguration::toMap();
    if(map.isEmpty())
        return map;

    return map;
}

/*!
 * \class GoBuildConfigurationFactory
 * Factory class to create GoBuildConfiguration
 * instances.
 */
GoBuildConfigurationFactory::GoBuildConfigurationFactory(QObject *parent)
    : ProjectExplorer::IBuildConfigurationFactory(parent)
{
}

GoBuildConfigurationFactory::~GoBuildConfigurationFactory()
{
}

int GoBuildConfigurationFactory::priority(const ProjectExplorer::Target *parent) const
{
    if(canHandle(parent))
        return 1;
    return -1;
}

QList<ProjectExplorer::BuildInfo *> GoBuildConfigurationFactory::availableBuilds(const ProjectExplorer::Kit *kit, const QString &projectFilePath) const
{
    QList<ProjectExplorer::BuildInfo *> result;

    QFileInfo projFileInfo(projectFilePath);
    if(!projFileInfo.exists())
        return result;

    ProjectExplorer::BuildInfo *release = new ProjectExplorer::BuildInfo(this);
    release->displayName = tr("Release1");
    release->supportsShadowBuild = false;
    release->buildDirectory = Utils::FileName::fromString(projFileInfo.absolutePath());
    release->kitId = kit->id();
    release->typeName = QStringLiteral("Release");

    ProjectExplorer::BuildInfo *debug = new ProjectExplorer::BuildInfo(this);
    debug->displayName = tr("Debug1");
    debug->supportsShadowBuild = false;
    debug->buildDirectory = Utils::FileName::fromString(projFileInfo.absolutePath());
    debug->kitId = kit->id();
    debug->typeName = QStringLiteral("Debug");

    result << release << debug;

    return result;
}

QList<ProjectExplorer::BuildInfo *> GoBuildConfigurationFactory::availableBuilds(const ProjectExplorer::Target *parent) const
{
    QList<ProjectExplorer::BuildInfo *> result;
    if(!canHandle(parent))
        result;
    return availableBuilds(parent->kit(),parent->project()->projectFilePath());
}

int GoBuildConfigurationFactory::priority(const ProjectExplorer::Kit *k, const QString &projectPath) const
{
    if(!k)
        return -1;

    GoLang::ToolChain* tc = GoToolChainKitInformation::toolChain(k);
    if(!tc)
        return -1;

    return (Core::MimeDatabase::findByFile(QFileInfo(projectPath)).matchesType(QLatin1String(Constants::GO_PROJECT_MIMETYPE))) ? 10 : -1;
}

QList<ProjectExplorer::BuildInfo *> GoBuildConfigurationFactory::availableSetups(const ProjectExplorer::Kit *k, const QString &projectPath) const
{
    if(!GoToolChainKitInformation::toolChain(k))
        return QList<ProjectExplorer::BuildInfo *>();

    if(!Core::MimeDatabase::findByFile(QFileInfo(projectPath)).matchesType(QLatin1String(Constants::GO_PROJECT_MIMETYPE)))
        return QList<ProjectExplorer::BuildInfo *>();

    return availableBuilds(k,projectPath);
}

GoBuildConfiguration *GoBuildConfigurationFactory::create(ProjectExplorer::Target *parent, const ProjectExplorer::BuildInfo *info) const
{
    QTC_ASSERT(info->factory() == this, return 0);
    QTC_ASSERT(info->kitId == parent->kit()->id(), return 0);
    QTC_ASSERT(!info->displayName.isEmpty(), return 0);
    QTC_ASSERT(canHandle(parent),return 0);


    ProjectExplorer::BuildInfo copy(*info);
    GoProject *project = static_cast<GoProject *>(parent->project());

    if (copy.buildDirectory.isEmpty())
        copy.buildDirectory = Utils::FileName::fromString(project->projectDirectory());

    GoBuildConfiguration *bc = new GoBuildConfiguration(parent);
    bc->setDisplayName(copy.displayName);
    bc->setDefaultDisplayName(copy.displayName);

    ProjectExplorer::BuildStepList *buildSteps = bc->stepList(ProjectExplorer::Constants::BUILDSTEPS_BUILD);
    ProjectExplorer::BuildStepList *cleanSteps = bc->stepList(ProjectExplorer::Constants::BUILDSTEPS_CLEAN);

    GoBuildStep *build = new GoBuildStep(buildSteps);
    buildSteps->insertStep(0, build);

    GoBuildStep *cleanStep = new GoBuildStep(cleanSteps);
    cleanSteps->insertStep(0, cleanStep);
    cleanStep->setIsCleanStep(true);

    bc->setBuildDirectory(Utils::FileName::fromString(copy.buildDirectory.toString()));
    return bc;
}

bool GoBuildConfigurationFactory::canRestore(const ProjectExplorer::Target *parent, const QVariantMap &map) const
{
    if (!canHandle(parent))
        return false;
    return ProjectExplorer::idFromMap(map) == Constants::GO_BUILDCONFIGURATION_ID;
}

GoBuildConfiguration *GoBuildConfigurationFactory::restore(ProjectExplorer::Target *parent, const QVariantMap &map)
{
    if (!canRestore(parent, map))
        return 0;
    GoBuildConfiguration *bc = new GoBuildConfiguration(parent);
    if (bc->fromMap(map))
        return bc;

    delete bc;
    return 0;
}

bool GoBuildConfigurationFactory::canClone(const ProjectExplorer::Target *parent, ProjectExplorer::BuildConfiguration *product) const
{
    if (!canHandle(parent))
        return false;

    return product->id() == Constants::GO_BUILDCONFIGURATION_ID;
}

GoBuildConfiguration *GoBuildConfigurationFactory::clone(ProjectExplorer::Target *parent, ProjectExplorer::BuildConfiguration *product)
{
    if (!canClone(parent, product))
        return 0;
    GoBuildConfiguration *old = static_cast<GoBuildConfiguration *>(product);
    return new GoBuildConfiguration(parent, old);
}

bool GoBuildConfigurationFactory::canHandle(const ProjectExplorer::Target *t) const
{
    QTC_ASSERT(t, return false);
    if (!t->project()->supportsKit(t->kit()))
        return false;
    return qobject_cast<GoLang::GoProject *>(t->project());
}

GoBuildStep::GoBuildStep(ProjectExplorer::BuildStepList *bsl)
    : AbstractProcessStep(bsl,Constants::GO_GOSTEP_ID)
{
    setIsCleanStep(false);
}

GoBuildStep::GoBuildStep(ProjectExplorer::BuildStepList *bsl, GoBuildStep *bs)
    : AbstractProcessStep(bsl,bs)
{
    setIsCleanStep(bs->m_clean);
}

void GoBuildStep::setIsCleanStep(const bool set)
{
    if(m_clean == set)
        return;

    m_clean = set;
    if(m_clean)
        setDisplayName(tr("Run Go clean"));
    else
        setDisplayName(tr("Run Go install"));
}

bool GoBuildStep::isCleanStep() const
{
    return m_clean;
}

bool GoBuildStep::init()
{
    m_tasks.clear();

    ProjectExplorer::BuildConfiguration *bc = buildConfiguration();
    if (!bc){
        ProjectExplorer::Task t(ProjectExplorer::Task::Error
                                ,tr("No valid BuildConfiguration set for step: %1").arg(displayName())
                                ,Utils::FileName(),-1
                                ,ProjectExplorer::Constants::TASK_CATEGORY_BUILDSYSTEM);
        m_tasks.append(t);

        //GoBuildStep::run will stop if tasks exist
        return true;

    }

    ProjectExplorer::Kit* kit = bc->target()->kit();
    GoLang::ToolChain* tc = GoLang::GoToolChainKitInformation::toolChain(kit);
    if(!tc){
        ProjectExplorer::Task t(ProjectExplorer::Task::Error
                                ,tr("No valid Go Toolchain set for kit: %1").arg(kit->displayName())
                                ,Utils::FileName(),-1
                                ,ProjectExplorer::Constants::TASK_CATEGORY_BUILDSYSTEM);
        m_tasks.append(t);

        //GoBuildStep::run will stop if tasks exist
        return true;

    }

    //builds the process arguments
    QStringList arguments;
    arguments << QStringLiteral("-installsuffix")
              << bc->target()->kit()->displayName();

    if(!m_clean) {
        arguments << QStringLiteral("install");
    } else {
        arguments << QStringLiteral("-i")
                  << QStringLiteral("-r");
    }
    arguments << QStringLiteral("src/main.go");

    ProjectExplorer::ProcessParameters* params = processParameters();
    params->setMacroExpander(bc->macroExpander());

    //setup process parameters
    params->setWorkingDirectory(bc->buildDirectory().toString());
    params->setCommand(tc->compilerCommand().toString());
    params->setArguments(Utils::QtcProcess::joinArgs(arguments));

    setIgnoreReturnValue(false);

    Utils::Environment env = bc->environment();
    // Force output to english for the parsers. Do this here and not in the toolchain's
    // addToEnvironment() to not screw up the users run environment.
    env.set(QStringLiteral("LC_ALL"), QStringLiteral("C"));
    env.set(QStringLiteral("GOPATH"),bc->target()->project()->projectDirectory());
    env.set(QStringLiteral("GOBIN"),QStringLiteral("bin_")+bc->target()->kit()->displayName());
    params->setEnvironment(env);

    params->resolveAll();

    ProjectExplorer::IOutputParser *parser = target()->kit()->createOutputParser();
    if (parser) {
        setOutputParser(parser);
        outputParser()->setWorkingDirectory(params->effectiveWorkingDirectory());
    }
    return AbstractProcessStep::init();
}

void GoBuildStep::run(QFutureInterface<bool> &fi)
{
    if (m_tasks.size()) {
       foreach (const ProjectExplorer::Task& task, m_tasks) {
           addTask(task);
       }
       emit addOutput(tr("Configuration is invalid. Aborting build")
                      ,ProjectExplorer::BuildStep::MessageOutput);
       fi.reportResult(false);
       emit finished();
       return;
    }

    AbstractProcessStep::run(fi);
}

ProjectExplorer::BuildStepConfigWidget *GoBuildStep::createConfigWidget()
{
    return new ProjectExplorer::SimpleBuildStepConfigWidget(this);
}

bool GoBuildStep::fromMap(const QVariantMap &map)
{
    if(!AbstractProcessStep::fromMap(map))
        return false;

    setIsCleanStep(map.value(QLatin1String(GO_BUILDSTEP_ISCLEAN_KEYC),false).toBool());
    return true;
}

QVariantMap GoBuildStep::toMap() const
{
    QVariantMap map = AbstractProcessStep::toMap();
    map.insert(QLatin1String(GO_BUILDSTEP_ISCLEAN_KEYC),m_clean);
    return map;
}

/*!
 * \class GoBuildStepFactory
 * Factory class to create Go buildsteps
 * build steps
 */
QList<Core::Id> GoBuildStepFactory::availableCreationIds(ProjectExplorer::BuildStepList *parent) const
{
    if(!canHandle(parent->target()))
        return QList<Core::Id>();

    return QList<Core::Id>() << Core::Id(Constants::GO_GOSTEP_ID);
}

/*!
 * \brief GoBuildConfigurationFactory::canHandle
 * checks if we can create buildconfigurations for the given target
 */
bool GoBuildStepFactory::canHandle(const ProjectExplorer::Target *t) const
{
    QTC_ASSERT(t, return false);
    if (!t->project()->supportsKit(t->kit()))
        return false;

    return t->project()->id() == Core::Id(GoLang::Constants::GO_PROJECT_ID);
}

QString GoBuildStepFactory::displayNameForId(const Core::Id id) const
{
    if (id == Constants::GO_GOSTEP_ID)
        return tr("Run Go compiler", "Display name for GoStep id.");
    return QString();
}

bool GoBuildStepFactory::canCreate(ProjectExplorer::BuildStepList *parent, const Core::Id id) const
{
    if (canHandle(parent->target()))
        return availableCreationIds(parent).contains(id);
    return false;
}

ProjectExplorer::BuildStep *GoBuildStepFactory::create(ProjectExplorer::BuildStepList *parent, const Core::Id id)
{
    if (!canCreate(parent, id))
        return 0;

   if ( id == Core::Id(Constants::GO_GOSTEP_ID) ) {
        GoBuildStep *step = new GoBuildStep(parent);
        return step;
    }
    return 0;
}

bool GoBuildStepFactory::canRestore(ProjectExplorer::BuildStepList *parent, const QVariantMap &map) const
{
    return canCreate(parent, ProjectExplorer::idFromMap(map));
}

ProjectExplorer::BuildStep *GoBuildStepFactory::restore(ProjectExplorer::BuildStepList *parent, const QVariantMap &map)
{
    if (!canRestore(parent, map))
        return 0;

    ProjectExplorer::BuildStep* step = create(parent,ProjectExplorer::idFromMap(map));
    if(step->fromMap(map))
        return step;

    delete step;
    return 0;
}

bool GoBuildStepFactory::canClone(ProjectExplorer::BuildStepList *parent, ProjectExplorer::BuildStep *product) const
{
    return canCreate(parent, product->id());
}

ProjectExplorer::BuildStep *GoBuildStepFactory::clone(ProjectExplorer::BuildStepList *parent, ProjectExplorer::BuildStep *product)
{
    if (!canClone(parent, product))
        return 0;

    if(product->id() == Core::Id(Constants::GO_GOSTEP_ID))
        return new GoBuildStep(parent,static_cast<GoBuildStep *>(product));

    QTC_ASSERT(false,return 0);
}

} // namespace GoLang
