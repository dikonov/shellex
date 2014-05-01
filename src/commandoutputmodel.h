#ifndef COMMANDOUTPUTMODEL_H
#define COMMANDOUTPUTMODEL_H

#include <QAbstractListModel>
#include <QStringList>

class CommandOutputModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int linesMax READ linesMax WRITE setLinesMax NOTIFY linesMaxChanged)
    Q_PROPERTY(QString outputString READ outputString)

public:
    CommandOutputModel(QObject* parent = 0);

    void append(QString outputString);
    void removeFromFront();

    QString outputString();

    int rowCount(const QModelIndex & parent = QModelIndex()) const;

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    int linesMax();
    void setLinesMax(int lines);

signals:

    void linesMaxChanged();

public slots:


    void clear();

private:

    QStringList m_data;
    int m_lines_max;

};

#endif // COMMANDOUTPUTMODEL_H
