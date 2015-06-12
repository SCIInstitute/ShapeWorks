/*
 * Shapeworks license
 */

/**
 * @file MeshWorker.h
 * @brief Worker class for parallel mesh reconstruction
 *
 * The MeshWorker implements each threads mesh construction management
 */

#ifndef SHAPEWORKS_WORKER_H
#define SHAPEWORKS_WORKER_H

#include <QObject>

class ShapeworksWorker : public QObject
{
  Q_OBJECT

public:
	ShapeworksWorker(QString exe, QStringList args);
	~ShapeworksWorker();

public Q_SLOTS:
  void process();

Q_SIGNALS:
  void result_ready();
  void run_error();
  void step_made(int val);

private:
	QString exe_;
	QStringList args_;

};

#endif // ifndef SHAPEWORKS_WORKER_H