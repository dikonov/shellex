#include <QProcess>
#include <QStandardPaths>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDebug>
#include <QVariantList>
#include <QJsonArray>
#include <QJsonObject>

#include "shellexecutor.h"
#include "shellcommand.h"
#include "commandsmodel.h"

ShellExecutor::ShellExecutor(QObject *parent) :
    QObject(parent), m_commands(new CommandsModel(this)), m_fingerterm_installed(false)
{
    connect(m_commands, &CommandsModel::commandsModelChanged, this, &ShellExecutor::refreshCommandsModel);

    QProcess findFingerterm;

    findFingerterm.setProcessChannelMode(QProcess::MergedChannels);
    findFingerterm.start("which", QStringList() << "fingerterm");

    findFingerterm.waitForStarted();

    QString output;
    while(findFingerterm.waitForReadyRead())
    {
        output.append(QString(findFingerterm.readAll()));
    }

    qDebug() << output;
    m_fingerterm_installed = !(output.contains("which: no"));
    emit fingertermInstalledChanged();
}


void ShellExecutor::quickExecute(QJsonObject commandObject, Executor runner)
{

    ShellCommand* comm = m_commands->findCommandByName(commandObject["name"].toString());

    if( comm == NULL)
    {
        comm = new ShellCommand(this, commandObject["name"].toString(),
                (ShellCommand::CommandType)(int)commandObject["type"].toDouble(),
                commandObject["content"].toString());
        m_commands->insert(comm);
    }
    comm->startProcess(runner);

}

void ShellExecutor::executeByIndex(int index, ShellExecutor::Executor runner)
{
    if(index >= 0 && index < m_commands->rowCount())
    {
        m_commands->at(index)->startProcess(runner);
    }
}

void ShellExecutor::executeDetached(QString commands, ShellExecutor::Executor runner)
{

}

void ShellExecutor::executeDetachedByIndex(int index, ShellExecutor::Executor runner)
{

}

void ShellExecutor::stopAllCommands()
{
    for(int i = 0; i < m_commands->rowCount(); i++)
    {
        ShellCommand* temp = m_commands->at(i);
        if(temp == NULL)
        {
            qDebug() << "Error: Null pointer in the model at " << i;
        }
        else
        {
            if(temp->isRunning())
            {
                temp->stopProcess();
            }
        }
    }

}

CommandsModel* ShellExecutor::commandsModel()
{
    return m_commands;
}

bool ShellExecutor::fingertermInstalled()
{
    return m_fingerterm_installed;
}


//legacy method
void ShellExecutor::initFromArray(QVariantList array)
{
    for(int i = 0; i < array.length(); i++)
    {
        QString name = array.at(i).toString();
        m_commands->insert(

                        new ShellCommand(
                            this, name, ShellCommand::SingleLiner, name
                            )
                    );
    }
}

void ShellExecutor::initFromJSON(QString jsonString)
{

}

void ShellExecutor::initFromJSONArray(QJsonArray jsonArray)
{
    for(int i=0; i< jsonArray.size(); i++)
    {
        m_commands->insert(ShellCommand::fromJSONObject(jsonArray.at(i).toObject()));
    }
}


QVariantList ShellExecutor::getCommandNames()
{
    QList<QVariant> list;
    for(int i=0; i < m_commands->rowCount(); i++)
    {
        list.push_back(m_commands->at(i)->name());
    }

    return list;
}

QJsonArray ShellExecutor::getCommandsAsJSON()
{
    QJsonArray tempArray;
    for(int i=0; i < m_commands->rowCount(); i++)
    {
        tempArray.push_back(QJsonValue(m_commands->at(i)->getAsJSONObject()));
    }

    return tempArray;
}

QObject *ShellExecutor::getCommandNamed(QString name)
{
    return dynamic_cast<QObject*>(m_commands->findCommandByName(name));
}

void ShellExecutor::removeCommandByIndex(int i)
{

    //error checks done in the method
    m_commands->removeAt(i);

}

void ShellExecutor::removeCommandById(unsigned int id)
{
    ShellCommand* temp = m_commands->findCommandById(id);
    if(temp != NULL)
    {
        m_commands->removeAt(m_commands->indexOf(temp));
        temp->deleteLater();
    }
}

void ShellExecutor::updateCommandById(unsigned int id)
{
    ShellCommand* temp = m_commands->findCommandById(id);

    if(temp != NULL)
    {
       m_commands->reInsertCommand(temp);
    }
}

QObject* ShellExecutor::addCommandFromJSON(QJsonObject object)
{
    ShellCommand* temp = ShellCommand::fromJSONObject(object);
    m_commands->insert(temp);

    return dynamic_cast<QObject*>(temp);
}

void ShellExecutor::reloadCommandsModel(QString searchString)
{
    m_commands->reloadCommandsModel(searchString);
}

void ShellExecutor::sortCommands(int type, bool isResort)
{
    m_commands->sortCommands((CommandsModel::SortType)type, isResort);
}

void ShellExecutor::reSortCommands()
{
    m_commands->reSortCommands();
}

void ShellExecutor::refreshCommandsModel()
{
    emit commandsModelChanged();
}