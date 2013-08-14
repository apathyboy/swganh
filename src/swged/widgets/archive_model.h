
#pragma once

#include <string>
#include <vector>
#include <QAbstractItemModel>

namespace swganh {

    class ArchiveModel : public QAbstractItemModel
    {
        Q_OBJECT

    public:
        ArchiveModel(std::vector<std::string> filenames, QObject* parent = nullptr);
        ~ArchiveModel();

        QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
        QModelIndex parent(const QModelIndex& index) const override;
        int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        int columnCount(const QModelIndex& parent = QModelIndex()) const override;
        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    private:
        std::vector<std::string> filenames_;
    };

}
