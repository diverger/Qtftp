/*!
 * \file qtftp.cpp
 * \date 2016/08/04 11:00
 *
 * \author diverger
 *
 * \brief 
 *
 * TODO: Reference: https://curl.haxx.se/libcurl/c/ftpupload.html
 *
 * \note
 */
#include <curl/curl.h>
#include <curl/easy.h>
#include "qtftp.h"


QTftp::QTftp()
	: _d(0)
	, _progress_data(0)
	, _progress_callback(0)
{
}

int QTftp::init()
{
	CURLcode code = CURLE_OK;

	/* In windows, this will init the winsock stuff */
	curl_global_init(CURL_GLOBAL_ALL);

	_d = curl_easy_init();

	if (!_d)
	{
		return -1;
	}

	do
	{
		code = curl_easy_setopt((CURL *)_d, CURLOPT_TFTP_BLKSIZE, 512);

		if (code != CURLE_OK)
		{
			break;
		}

		code = curl_easy_setopt((CURL *)_d, CURLOPT_TFTP_NO_OPTIONS, 1);

		if (code != CURLE_OK)
		{
			break;
		}
				
	} while (0);

	return 0;
}

int QTftp::deinit()
{
	if (_d)
	{
		curl_easy_cleanup((CURL *)_d);
	}

	return 0;
}

QTftp::~QTftp()
{

}

/*!
 * \brief	Set the progress data
 *
 * \param	data	The pointer to the progress data
 * \return	none
 */
void QTftp::set_progress_data(void *data /* = NULL */)
{
	_progress_data = data;
}

/*!
 * \brief	Set the progress callback
 *
 * \param	callback	The callback function pointer, should be C language calling convention 
 * \return	none
 */
void QTftp::set_progress_callback(progress_callback_fptr callback /* = NULL */)
{
	_progress_callback = callback;
}

/*!
 * \brief	Get a file from the given URL
 *
 * \param[in]	file_src	The source file at URL
 * \param[in]	file_des	The destination file at local
 * \param[in]	url			The remote server name or IP address 
 * \return					Error code
 */
int QTftp::get_file(QString url, QString file_src, QString file_des)
{
	Q_ASSERT(!file_src.isEmpty());
	Q_ASSERT(!file_des.isEmpty());
	Q_ASSERT(!url.isEmpty());

	int err = 0;

	if (file_src.isEmpty() || file_des.isEmpty())
	{
		return -1;
	}

	if (url.isEmpty())
	{
		return -1;
	}

	CURL *curl = (CURL *)_d;
	CURLcode res = CURLE_OK;

	// open the file for write
	FILE *fp = 0;
	size_t fsize = 0;
	
	do 
	{
		fp = fopen(qPrintable(file_des), "wb");

		if (!fp)
		{
			err = -1;
			break;
		}

		// Note: This may not safe, refer to:
		// http://www.cplusplus.com/reference/cstdio/fseek/
		// http://stackoverflow.com/questions/238603/how-can-i-get-a-files-size-in-c
		fseek(fp, 0, SEEK_END);
		fsize = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)fp);

		if (res != CURLE_OK)
		{
			err = res;
			break;
		}

		res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);

		if (res != CURLE_OK)
		{
			err = res;
			break;
		}

		res = curl_easy_setopt(curl, CURLOPT_UPLOAD, FALSE);

		if (res != CURLE_OK)
		{
			err = res;
			break;
		}

		res = curl_easy_setopt(curl, CURLOPT_NOPROGRESS, FALSE);

		if (res != CURLE_OK)
		{
			err = res;
			break;
		}

		res = curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, _progress_callback);

		if (res != CURLE_OK)
		{
			err = res;
			break;
		}

		res = curl_easy_setopt(curl, CURLOPT_XFERINFODATA, _progress_data);

		if (res != CURLE_OK)
		{
			err = res;
			break;
		}

		res = curl_easy_setopt(curl, CURLOPT_INFILESIZE, fsize);

		if (res != CURLE_OK)
		{
			err = res;
			break;
		}

		res = curl_easy_setopt(curl, CURLOPT_URL, qPrintable(QString("tftp:/") + url + "/" + file_src));

		if (res != CURLE_OK)
		{
			err = res;
			break;
		}
		
		res = curl_easy_perform(curl);

		if (res != CURLE_OK)
		{
			err = res;
		}

	} while (0);

	if (fp)
	{
		fclose(fp);
	}

	return err;
}


/*!
 * \brief	Put a file to the given URL
 *
 * \param[in]	file_src	The source file at local
 * \param[in]	file_des	The destination file at URL
 * \param[in]	url			The remote serve name or IP address
 * \return	
 */
int QTftp::put_file(QString url, QString file_src, QString file_des)
{
	Q_ASSERT(!file_src.isEmpty());
	Q_ASSERT(!file_des.isEmpty());
	Q_ASSERT(!url.isEmpty());

	int err = 0;

	if (file_src.isEmpty() || file_des.isEmpty())
	{
		return -1;
	}

	if (url.isEmpty())
	{
		return -1;
	}

	CURL *curl = (CURL *)_d;
	CURLcode res = CURLE_OK;

	// open the file for read
	FILE *fp = 0;
	size_t fsize = 0;

	do 
	{
		fp = fopen(qPrintable(file_src), "rb");

		if (!fp)
		{
			err = -1;
			break;
		}

		// Note: This may not safe, refer to:
		// http://www.cplusplus.com/reference/cstdio/fseek/
		// http://stackoverflow.com/questions/238603/how-can-i-get-a-files-size-in-c
		fseek(fp, 0, SEEK_END);
		fsize = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		res = curl_easy_setopt(curl, CURLOPT_READDATA, (void *)fp);

		if (res != CURLE_OK)
		{
			err = res;
			break;
		}

		res = curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);

		if (res != CURLE_OK)
		{
			err = res;
			break;
		}
	
		/* enable uploading */
		res = curl_easy_setopt(curl, CURLOPT_UPLOAD, TRUE);

		if (res != CURLE_OK)
		{
			err = res;
			break;
		}

		res = curl_easy_setopt(curl, CURLOPT_NOPROGRESS, FALSE);

		if (res != CURLE_OK)
		{
			err = res;
			break;
		}

		res = curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, _progress_callback);

		if (res != CURLE_OK)
		{
			err = res;
			break;
		}

		res = curl_easy_setopt(curl, CURLOPT_XFERINFODATA, _progress_data);

		if (res != CURLE_OK)
		{
			err = res;
			break;
		}

		res = curl_easy_setopt(curl, CURLOPT_INFILESIZE, fsize);

		if (res != CURLE_OK)
		{
			err = res;
			break;
		}

		res = curl_easy_setopt(curl, CURLOPT_URL, qPrintable(QString("tftp:/") + url + "/" + file_des));

		if (res != CURLE_OK)
		{
			err = res;
			break;
		}
		
		res = curl_easy_perform(curl);

		if (res != CURLE_OK)
		{
			err = res;
		}

	} while (0);

	if (fp)
	{
		fclose(fp);
	}

	return err;
}
