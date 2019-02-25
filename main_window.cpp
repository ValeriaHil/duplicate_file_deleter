#include <utility>

#include "main_window.h"
#include "progress_dialog.h"
#include "resources/ui/ui_main_window.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QRunnable>
#include <QThreadPool>

#include "scanner.h"

namespace DFD {

MainWindow::MainWindow() : _ui(new Ui::MainWindow) {
  _ui->setupUi(this);
  _ui->treeWidget->setSelectionMode(QAbstractItemView::MultiSelection);
  _ui->actionSelectDirectory->setIcon(QIcon(":/images/open.png"));
  _ui->actionRemoveSelectedDuplicates->setIcon(QIcon(":/images/remove.png"));
  _ui->actionScanForDuplicates->setIcon(QIcon(":/images/search.png"));

  connect(_ui->actionSelectDirectory, &QAction::triggered, this, &MainWindow::Select);
  connect(_ui->actionScanForDuplicates, &QAction::triggered, this, &MainWindow::Scan);
  connect(_ui->actionRemoveSelectedDuplicates, &QAction::triggered, this, &MainWindow::Remove);

}

MainWindow::~MainWindow() {
  delete _ui;
  stop = true;
  QThreadPool::globalInstance()->waitForDone();
}

void MainWindow::Select() {
  QString selectedDirectoryFromDialog = QFileDialog::getExistingDirectory(
      this, "Select Directory for Scanning", QString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
  );
  if (selectedDirectoryFromDialog.isEmpty()) {
    return;
  }
  _ui->lineEdit->setText(selectedDirectoryFromDialog);
}

void MainWindow::Scan() {
  QString selectedDirectory = _ui->lineEdit->text();
  if (selectedDirectory.isEmpty() || !QDir(selectedDirectory).exists()) {
    QMessageBox(QMessageBox::Warning, QString("Warning"), "Selected directory doesn't exist").exec();
    return;
  }
  if (!QDir(selectedDirectory).isReadable()) {
    QMessageBox(QMessageBox::Warning, QString("Warning"), "Can't read selected directory").exec();
    return;
  }
  stop = false;
  auto progressDialog = std::make_unique<ProgressDialog>(this, stop);
  auto scanner = new Scanner(selectedDirectory, stop);
  connect(scanner, &Scanner::setMaxVal, progressDialog.get(), &ProgressDialog::onMaxValueSet);
  connect(scanner, &Scanner::setCurVal, progressDialog.get(), &ProgressDialog::onCurrentValueSet);
  connect(scanner, &Scanner::finish, progressDialog.get(), &ProgressDialog::onFinished);
  connect(scanner, &Scanner::finish, this, &MainWindow::ScanFinished, Qt::QueuedConnection);
  QThreadPool::globalInstance()->start(scanner);
  progressDialog->exec();
}

void MainWindow::Remove() {
  auto selectedItems = _ui->treeWidget->selectedItems();
  QVector<QTreeWidgetItem*> itemsToDelete;

  for (const auto& item : selectedItems) {
    if (item->childCount() == 0) {
      itemsToDelete.push_back(item);
    }
  }
  if (itemsToDelete.empty()) {
    return;
  }

  QString confirmationQuestion = "Do you want to delete selected file(s)?";
  auto answer = QMessageBox::question(this, "Delete file(s)", confirmationQuestion);
  if (answer == QMessageBox::No) {
    return;
  }

  QMap<QString, QTreeWidgetItem*> deletedFiles;
  QVector<QString> skippedFiles;
  for (const auto& item : itemsToDelete) {
    QFile file(item->text(0));
    if (file.remove()) {
      deletedFiles.insert(file.fileName(), item);
    } else {
      skippedFiles.push_back(file.fileName());
    }
  }
  for (const auto& item : deletedFiles) {
    auto* parent = item->parent();
    parent->removeChild(item);
    parent->setText(0, QString::number(parent->childCount()));
    if (parent->childCount() == 0) {
      delete parent;
    }
  }

  QString operationInfo = QString("Can't delete ").append(QString::number(skippedFiles.size())).append(" file(s):\n");
  for (const QString& fileName : skippedFiles) {
    operationInfo.append(fileName).append("\n");
  }

  _ui->treeWidget->clearSelection();
  if (!skippedFiles.empty()) {
    QMessageBox::information(this, "Can't delete file(s)", operationInfo);
  }
}

void MainWindow::ScanFinished(QHash<QByteArray, QVector<QString>> sha256_to_file_list) {
  _ui->treeWidget->clear();
  for (const auto& file_list: sha256_to_file_list) {
    if (file_list.size() == 1) {
      continue;
    }
    QList<QTreeWidgetItem*> qListTreeWidgetItem;
    for (const auto& file_path: file_list) {
      auto* qTreeWidgetItem = new QTreeWidgetItem();
      qTreeWidgetItem->setText(0, file_path);
      qListTreeWidgetItem.push_back(qTreeWidgetItem);
    }
    auto* qTreeWidgetItem = new QTreeWidgetItem();
    qTreeWidgetItem->addChildren(qListTreeWidgetItem);
    qTreeWidgetItem->setText(0, QString::number(qListTreeWidgetItem.size()));
    _ui->treeWidget->addTopLevelItem(qTreeWidgetItem);
  }
}

} // namespace DFD
