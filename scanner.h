#pragma once

#include <QRunnable>
#include <QDirIterator>
#include <QCryptographicHash>

namespace DFD {

class Scanner : public QObject, public QRunnable {
 Q_OBJECT

  class InterruptionException {};

 public:
  explicit Scanner(QString dir, std::atomic_bool& stop) : _dir(std::move(dir)), _stop(stop) {}

  void run() override {
    try {
      QHash<quint64, QVector<QString>> size_to_file_list;
      QDirIterator it(_dir, QDirIterator::Subdirectories);
      quint32 totalCount = 0;
      while (it.hasNext()) {
        if (_stop) {
          throw InterruptionException();
        }
        QFileInfo file(it.next());
        if (file.isFile() && file.permission(QFile::ReadUser)) {
          ++totalCount;
          size_to_file_list[file.size()].push_back(file.absoluteFilePath());
        }
      }
      emit setMaxVal(totalCount);
      quint32 processed = 0;
      QHash<QByteArray, QVector<QString>> sha256_to_file_list;
      for (const auto& file_list: size_to_file_list) {
        if (_stop) {
          throw InterruptionException();
        }
        processed += file_list.size();
        if (file_list.size() == 1) {
          continue;
        }
        for (const auto& file_path: file_list) {
          sha256_to_file_list[get_sha256(QFile(file_path))].push_back(file_path);
        }
        emit setCurVal(processed);
      }
      emit setCurVal(processed);
      emit finish(sha256_to_file_list);
    } catch (InterruptionException&) {
      emit finish({});
    }
  }

 signals:
  void setMaxVal(int);
  void setCurVal(int);
  void finish(QHash<QByteArray, QVector<QString>>);

 private:
  QByteArray get_sha256(QFile file) {
    static constexpr const quint32 BUFFER_SIZE = 512 * 1024;

    QCryptographicHash hash(QCryptographicHash::Sha256);
    file.open(QIODevice::ReadOnly);
    while (!file.atEnd()) {
      if (_stop) {
        throw InterruptionException();
      }
      hash.addData(file.read(BUFFER_SIZE));
    }
    file.close();
    return hash.result();
  }

  QString _dir;
  std::atomic_bool& _stop;
};

}
