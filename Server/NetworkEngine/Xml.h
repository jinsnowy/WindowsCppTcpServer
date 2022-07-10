#pragma once

#include "tinyxml2.h"

class XmlElement
{
	friend class Xml;
private:
	string strTag;
	string strValue;
	map<string, string> attributes;
	map<string, unique_ptr<XmlElement>> children;

public:
	XmlElement() = default;

	const string& getTag() const { return strTag; }
	const string& getValue() const { return strValue; }

	const string& at(const std::string& key) { return attributes.at(key); }
	string& operator[](const std::string& key) { return attributes[key]; }

	XmlElement* findChild(const std::string& key);
	XmlElement* findNode(const std::string& key);
	vector<XmlElement*> getChildren() const;
};

class Xml
{
private:
	std::string configName;
	unique_ptr<XmlElement> mRoot;

public:
	Xml() : mRoot(nullptr) {}

	Xml(const std::string& filePath);

	bool read(const std::string& rootName, const std::string& filePath);

	void clear();

private:
	unique_ptr<XmlElement> make(tinyxml2::XMLElement* element);
};

