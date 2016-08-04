#ifndef QTFTP_H
#define QTFTP_H

#include <QObject>
#include <QString>

class QTftp
{
public:
	QTftp();
	~QTftp();
	
	int init();
	int deinit();
	int get_file(QString url, QString file_src, QString file_des);
	int put_file(QString url, QString file_src, QString file_des);

private:
	void	*_d;
};

#endif // QTFTP_H
