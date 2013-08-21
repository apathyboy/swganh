
#pragma once

#include <memory>

#include <QString>
#include <QWidget>

class QPlainTextEdit;

namespace swged {

	class DDSPreview : public QWidget
	{
		Q_OBJECT

	public:
		explicit DDSPreview(QWidget* parent = nullptr);
		~DDSPreview();

		void setConsole(QPlainTextEdit* console);
		void loadDDSFromFile(QString filename);

	private:
		struct DDSPreviewImpl;
		std::unique_ptr<DDSPreviewImpl> impl_;
	};

}

