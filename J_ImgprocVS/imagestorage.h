#pragma once

#ifndef _IN_VS_
#define _IN_VS_ 1
#endif

#if _IN_VS_

#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <vector>
#include <tuple>
#include <map>
#include <stdlib.h>
//#include <qstring.h>
#include "debug.h"
#include "qfiledialog.h"
#endif

namespace Ui {
class ImageStorage;
typedef cv::Mat Image;
typedef std::vector<Image> Images;
typedef std::tuple<int,Image , Images> SrcAndRes;
typedef std::map<std::string, SrcAndRes> Storage;
}

class ImageStorage
{
private:
	static ImageStorage *_singleton;
	Ui::Storage *_storage;

	
	ImageStorage()
	{
		_storage = new Ui::Storage();
		_current = nullptr;
	}

	ImageStorage(ImageStorage&);
	ImageStorage & operator = (ImageStorage&);
	~ImageStorage()
	{
		delete _storage;
		delete _current;
	}

	std::pair<const std::string, Ui::SrcAndRes> *_current;
	//Ui::Image _src;
public:

	// Singleton static member
	static ImageStorage *getInstance()
	{
		if (_singleton == nullptr)
		{
			_singleton = new ImageStorage();
		}
		return _singleton;
	}

	bool setInputImage(QString filename)
	{
		//
		std::string stdstr = filename.toStdString();

		Ui::Image src = cv::imread(stdstr);

		if (!src.data)
			return false;
		
		QStringList l1 = filename.split("/");
		QStringList l2 = l1.last().split(".");
		std::string key = l2.first().toStdString();
		addSrc(key, src);

		return true;
	}

	//add source image to storage
	void addSrc(std::string name, Ui::Image src)
	{
		Ui::Storage::iterator iter;
		std::string _name = name;
		int counter = 0;

		for(iter = _storage->begin(); iter != _storage->end(); ++iter)
		{
			if (iter->first == name)
			{
				std::get<0>(iter->second)++;
				counter = std::get<0>(iter->second);
				
				char buffer[20];
				_itoa(counter, buffer, 10);
				_name.append(buffer);
				break;
			}
		}

		
		/* add src to histories too */
		//Ui::SrcAndRes snr(0, src, Ui::Images());
		/*  */
		cv::cvtColor(src, src, CV_BGR2RGB);
		Ui::Images histories;
		histories.push_back(src);
		Ui::SrcAndRes snr(0, src, histories);
		_storage->insert(Ui::Storage::value_type(_name, snr));
		modifyCurrent(_name);
	}

	bool addResult(std::string name, Ui::Images results)
	{
		Ui::Storage::iterator pos = _storage->find(name);
		if (pos == _storage->end())
		{
			ASSERT(0);
			return false;
		}
		else
		{
			std::get<2>(pos->second) = results;
			return true;
		}
	}

	//
	std::pair<const std::string, Ui::SrcAndRes>* find(std::string name)
	{
		Ui::Storage::iterator pos = _storage->find(name);
		if (pos == _storage->end())
		{
			return nullptr;
		}
		else
		{
			return &(*pos);
		}
	}


	void remove(std::string name)
	{
		_storage->erase(name);
		_current = nullptr;
	}

	std::pair<const std::string, Ui::SrcAndRes>* getCurrent()
	{
		return _current;
	}

	bool modifyCurrent(std::string objname)
	{
		Ui::Storage::iterator pos = _storage->find(objname);
		if (pos == _storage->end())
		{
			//ASSERT(0);
			_current = nullptr;
			return false;
		}
		else
		{
			_current = &(*pos);
		}
		return true;
	}

	bool isEmpty()
	{
		if (_storage == nullptr)
		{
			return true;
		}
		return _storage->empty();
	}
};