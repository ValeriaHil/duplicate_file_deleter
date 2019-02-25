#include "progress_dialog.h"
#include "resources/ui/ui_progress_dialog.h"

namespace DFD {

ProgressDialog::ProgressDialog(QWidget* parent, std::atomic_bool& stop) :
  QDialog(parent), _stop(stop), _ui(new Ui::ProgressDialog)
{
  _ui->setupUi(this);
  connect(_ui->cancelButton, &QPushButton::clicked, this, &ProgressDialog::onCancelButtonClicked);
}

void ProgressDialog::onCancelButtonClicked() {
  _stop = true;
}

ProgressDialog::~ProgressDialog() {
  delete _ui;
}

void ProgressDialog::onMaxValueSet(int value) {
  _ui->progressBar->setMaximum(value);
}

void ProgressDialog::onCurrentValueSet(int value) {
  int percents = value * 100 / _ui->progressBar->maximum();
  _ui->progressBar->setValue(value);
}

void ProgressDialog::onFinished() {
  QDialog::accept();
}

}
