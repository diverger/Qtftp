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
{

}

/*!
* \brief	The write callback function
*
* \param[in]	ptr		The data pointer to the buffer stored the data to write
* \param[in]	size	The
* \return
*/
size_t	write_callback(char const *data, size_t element_size, size_t element_count, void *fp)
{
	return fwrite(data, element_size, element_count, (FILE *)fp);
}

size_t read_callback(char *data, size_t element_size, size_t element_count, void *fp)
{
	return fread(data, element_size, element_count, (FILE *)fp);
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
	
	do 
	{
		fp = fopen(qPrintable(file_des), "wb");

		if (!fp)
		{
			err = -1;
			break;
		}

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

	do 
	{
		fp = fopen(qPrintable(file_src), "rb");

		if (!fp)
		{
			err = -1;
			break;
		}

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
		res = curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

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
