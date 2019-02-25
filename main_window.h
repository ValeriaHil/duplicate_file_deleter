#pragma once

#include <QMainWindow>
#include <QListWidgetItem>
#include <QDir>
#include <memory>
#include <atomic>

namespace Ui {
class MainWindow;
} // namespace UI

namespace DFD {

class MainWindow : public QMainWindow {
 Q_OBJECT
 public:
  MainWindow();
   ~MainWindow() override;

 private slots:

  void Select();
  void Scan();
  void ScanFinished(QHash<QByteArray, QVector<QString>>);
  void Remove();

 private:
  std::atomic_bool stop = false;
  Ui::MainWindow* _ui;
};

} // namespace DFD

