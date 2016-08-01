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
	int get_file(QString file_src, QString file_des, QString url);
	int put_file(QString file_src, QString file_des, QString url);

private:
	void		*_d;
};

#endif // QTFTP_H
