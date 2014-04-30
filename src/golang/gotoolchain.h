#ifndef GOLANG_INTERNAL_GOTOOLCHAIN_H
#define GOLANG_INTERNAL_GOTOOLCHAIN_H

#include "toolchain.h"

namespace GoLang {
namespace Internal {

class GoToolChain : public ToolChain
{
public:
    GoToolChain(ToolChain::Detection detect);

    void setCompilerCommand(const Utils::FileName &path, const Utils::FileName &goRoot);
    QString version() const;
    void setTargetAbi(const ProjectExplorer::Abi &abi);
    QList<ProjectExplorer::Abi> supportedAbis () const;
    virtual QString defaultDisplayName() const;

    virtual Utils::FileName goRoot() const;

    // ToolChain interface
    virtual QString type() const;
    virtual QString typeDisplayName() const;
    virtual ProjectExplorer::Abi targetAbi() const;
    virtual bool isValid() const;
    virtual QList<ProjectExplorer::HeaderPath> systemGoPaths() const;
    virtual void addToEnvironment(Utils::Environment &env) const;
    virtual Utils::FileName compilerCommand() const;
    virtual ProjectExplorer::IOutputParser *outputParser() const;
    virtual ToolChainConfigWidget *configurationWidget();
    virtual bool canClone() const;
    virtual ToolChain *clone() const;
    virtual QVariantMap toMap() const;
    virtual QList<ProjectExplorer::Task> validateKit(const ProjectExplorer::Kit *k) const;

    static void addCommandPathToEnvironment(const Utils::FileName &command, Utils::Environment &env);
    static QString toString(ProjectExplorer::Abi::Architecture arch, int width);
protected:
    GoToolChain(const GoToolChain &other);
    virtual bool fromMap(const QVariantMap &data);
    virtual QList<ProjectExplorer::Abi> detectSupportedAbis() const;

    void updateSupportedAbis() const;
private:
    Utils::FileName m_compilerCommand;
    Utils::FileName m_goRoot;
    ProjectExplorer::Abi m_targetAbi;
    mutable QList<ProjectExplorer::HeaderPath> m_headerPaths;
    mutable QList<ProjectExplorer::Abi> m_supportedAbis;

    friend class GoToolChainFactory;
};


class GoToolChainFactory : public ToolChainFactory
{
    Q_OBJECT
public:
    GoToolChainFactory ();

    virtual QList<ToolChain *> autoDetect();
    virtual bool canCreate();
    virtual GoLang::ToolChain *create();
    virtual bool canRestore(const QVariantMap &data);
    virtual GoLang::ToolChain *restore(const QVariantMap &data);
protected:
    virtual GoToolChain *createToolChain(bool autoDetect);
};

} // namespace Internal
} // namespace GoLang

#endif // GOLANG_INTERNAL_GOTOOLCHAIN_H
