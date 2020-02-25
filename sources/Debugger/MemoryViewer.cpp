//
// Created by anonymus-raccoon on 2/17/20.
//

#include <iostream>
#include <cmath>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QSpinBox>
#include "MemoryViewer.hpp"
#include "../SNES.hpp"
#include "../Utility/Utility.hpp"

MemoryViewerModel::MemoryViewerModel(std::shared_ptr<Ram> memory, QObject *parent) :
	QAbstractTableModel(parent),
	_memory(std::move(memory))
{ }

int MemoryViewerModel::rowCount(const QModelIndex &) const
{
	return this->_memory->getSize() / 16u;
}

int MemoryViewerModel::columnCount(const QModelIndex &parent) const
{
	if (parent.row() == this->rowCount(parent) - 1)
		return this->_memory->getSize() - (parent.row() << 8u);
	return 16;
}

QVariant MemoryViewerModel::data(const QModelIndex &index, int role) const
{
	if (role == Qt::TextAlignmentRole)
		return Qt::AlignCenter;
	if (role != Qt::DisplayRole)
		return QVariant();
	char buf[3];
	snprintf(buf, 3, "%02X", this->_memory->read_internal((index.row() << 4u) + index.column()));
	return QString(buf);
}

QVariant MemoryViewerModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();
	if (orientation == Qt::Horizontal) {
		char buf[2];
		snprintf(buf, 2, "%1X", section);
		return QString(buf);
	} else {
		char buf[6];
		snprintf(buf, 6, "%0*Xx", this->_headerIndentSize, section);
		return QString(buf);
	}
}

void MemoryViewerModel::setMemory(std::shared_ptr<Ram> memory)
{
	this->_memory = std::move(memory);
	this->_headerIndentSize = this->_memory->getSize() >= 0x10000 ? 4 : 3;
	emit this->layoutChanged();
}

namespace ComSquare::Debugger
{
	MemoryViewer::MemoryViewer(ComSquare::SNES &snes) :
		QMainWindow(),
		_snes(snes),
		_ui(),
		_model(snes.wram)
	{
		this->setContextMenuPolicy(Qt::NoContextMenu);
		this->setAttribute(Qt::WA_QuitOnClose, false);

		this->_ui.setupUi(this);
		this->_ui.tableView->setModel(&this->_model);
		this->_ui.tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
		this->_ui.tabs->addTab("&WRam");
		this->_ui.tabs->addTab("&SRam");
		this->_ui.tabs->addTab("&Rom");
//		this->_ui.tabs->addTab("&VRam");
		QMainWindow::connect(this->_ui.actionGoto, &QAction::triggered, this, &MemoryViewer::gotoAddr);
		QObject::connect(this->_ui.tabs, &QTabBar::currentChanged, this, &MemoryViewer::changeRam);
		this->show();
	}

	void MemoryViewer::changeRam(int id)
	{
		switch (id) {
		default:
		case 0:
			this->_model.setMemory(this->_snes.wram);
			break;
		case 1:
			this->_model.setMemory(this->_snes.sram);
			break;
		case 2:
			this->_model.setMemory(this->_snes.cartridge);
			break;
//		case 3:
//			this->_model.setMemory(this->_snes.vram);
//			break;
		}
	}


	void MemoryViewer::gotoAddr()
	{
		QDialog dialog(this);
		Ui::GotoDialog dialogUI;
		dialogUI.setupUi(&dialog);
		QFont font = dialogUI.spinBox->font();
		font.setCapitalization(QFont::AllUppercase);
		dialogUI.spinBox->setFont(font);
		dialogUI.spinBox->selectAll();

		if (dialog.exec() != QDialog::Accepted)
			return;
		long value = std::strtol(dialogUI.spinBox->text().toStdString().c_str() + 1, nullptr, 16);
		QModelIndex index = this->_ui.tableView->model()->index(value >> 4, value & 0x0000000F);
		this->_ui.tableView->scrollTo(index);
		this->_ui.tableView->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect);
	}
}