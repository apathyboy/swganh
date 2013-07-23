
#pragma once

#include <memory>
#include <vector>

#include <QTreeWidget>

namespace swganh {

	namespace tre {
		class ContainerLayer;
		class Layer;
	}
	
	class ConstructionLayerTree : public QTreeWidget
	{
		Q_OBJECT

	public:
		ConstructionLayerTree(QWidget* parent = nullptr);

		void initialize(std::vector<std::shared_ptr<tre::ContainerLayer>>* layers);

	private:
		void addLayer(std::shared_ptr<tre::Layer> layer, QTreeWidgetItem* parent = nullptr);

		std::vector<std::shared_ptr<tre::ContainerLayer>>* layers_;
	};
}

Q_DECLARE_METATYPE(std::shared_ptr<::swganh::tre::Layer>);
