
#pragma once

#include <memory>

#include <QImage>
#include <QString>
#include <QWidget>

class QPlainTextEdit;
class QPaintEngine;

namespace swged {

	class DDSPreview : public QWidget
	{
		Q_OBJECT

	public:
		explicit DDSPreview(QWidget* parent = nullptr);
		~DDSPreview();

		void setConsole(QPlainTextEdit* console);
		void loadDDSFromFile(QString filename);
		QImage getDDSPreview() const;

		QPaintEngine* paintEngine() const { return 0; }

	protected:
		void paintEvent(QPaintEvent *event);

	private:
		struct DDSPreviewImpl;
		std::unique_ptr<DDSPreviewImpl> impl_;
	};

}

