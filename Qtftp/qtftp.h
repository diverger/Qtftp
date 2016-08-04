#ifndef QTFTP_H
#define QTFTP_H

#include <QObject>
#include <QString>

// Ref: https://curl.haxx.se/libcurl/c/CURLOPT_PROGRESSFUNCTION.html

/*!
 * \brief	The callback used by curl internal
 *
 * \param	clientp		clientp is the pointer set with CURLOPT_PROGRESSDATA, it is not used by libcurl but is only passed along from the application to the callback.
 * \param	dltotal		The total number of bytes libcurl expects to download in this transfer
 * \param	dlnow		The number of bytes downloaded so far
 * \param	ultotal		The total number of bytes libcurl expects to upload in this transfer
 * \param	ulnow		The number of bytes uploaded so far
 * \return	Returning a non-zero value from this callback will cause libcurl to abort the transfer and return CURLE_ABORTED_BY_CALLBACK.
 */
typedef int(*progress_callback_fptr)(void *clientp, qint64 dltotal, qint64 dlnow, qint64 ultotal, qint64 ulnow);

class QTftp
{
public:
	QTftp();
	~QTftp();
	
	int init();
	int deinit();

	/*!
	 * \brief	
	 *
	 * \param	
	 * \return	
	 */
	void set_progress_data(void *data = NULL);
	void set_progress_callback(progress_callback_fptr callback = NULL);
	int get_file(QString url, QString file_src, QString file_des);
	int put_file(QString url, QString file_src, QString file_des);

private:
	void					*_d;
	void					*_progress_data;
	progress_callback_fptr	_progress_callback;
};

#endif // QTFTP_H
