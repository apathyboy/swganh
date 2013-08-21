
#pragma once

#include <memory>

#include <QImage>
#include <QString>
#include <QWidget>

#include "swganh/byte_buffer.h"

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
		void loadDDSFromBuffer(swganh::ByteBuffer buffer);
		QImage getDDSPreview() const;

		QPaintEngine* paintEngine() const { return 0; }

	protected:
		void paintEvent(QPaintEvent *event);

	private:
		struct DDSPreviewImpl;
		std::unique_ptr<DDSPreviewImpl> impl_;
	};

}

