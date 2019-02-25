#pragma once

#include <QDialog>
#include <memory>
#include <atomic>

namespace Ui {
class ProgressDialog;
}

namespace DFD {

class ProgressDialog : public QDialog {
  Q_OBJECT
 public:
  explicit ProgressDialog(QWidget* parent, std::atomic_bool& stop);
  ~ProgressDialog() override;

 public slots:
  void onMaxValueSet(int value);
  void onCurrentValueSet(int value);
  void onFinished();

 private slots:
  void onCancelButtonClicked();

 private:
  std::atomic_bool& _stop;
  Ui::ProgressDialog* _ui;
};

} // namespace DFD
