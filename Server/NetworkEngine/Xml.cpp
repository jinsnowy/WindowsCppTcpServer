#include "pch.h"
#include "Xml.h"
#include "Logger.h"

XmlElement* XmlElement::findChild(const std::string& key)
{
	auto iter = children.find(key); 
	if (iter != children.end()) 
		return iter->second.get();

	return nullptr;
}

XmlElement* XmlElement::findNode(const std::string& key)
{
	std::queue<XmlElement*> elements;
	std::for_each(children.begin(), children.end(),
		[&elements](const auto& pair) { elements.push(pair.second.get()); });

	while (!elements.empty())
	{
		auto element = elements.front();
		elements.pop();

		if (element->strTag == key)
			return element;

		std::for_each(element->children.begin(), element->children.end(),
			[&elements](const auto& pair) { elements.push(pair.second.get()); });
	}

	return nullptr;
}

vector<XmlElement*> XmlElement::getChildren() const
{
	std::vector<XmlElement*> elements;
	std::for_each(children.begin(), children.end(),
		[&elements](const auto& pair) { elements.push_back(pair.second.get()); });

	return elements;
}

Xml::Xml(const std::string& filePath)
{
	read("", filePath);
}

bool Xml::read(const std::string& rootName, const std::string& filePath)
{
	clear();

	tinyxml2::XMLError error;
	configName = rootName;
	Logger* logger = Logger::getInstance();

	const auto check_error = [logger](tinyxml2::XMLError err)
	{
		if (err != tinyxml2::XMLError::XML_SUCCESS)
		{
			LOG_ERROR(logger, "XML parsing error %s", tinyxml2::XMLDocument::ErrorIDToName(err));
			return true;
		}

		return false;
	};

	try
	{
		tinyxml2::XMLDocument doc;
		error = doc.LoadFile(filePath.c_str());
		if (check_error(error))
			return false;

		auto root = doc.RootElement();
		if (!root)
			return false;

		if (rootName.empty())
		{
			mRoot = make(root);
			return true;
		}

		if (root->Name() == rootName)
		{
			mRoot = make(root);
			return true;
		}

		tinyxml2::XMLElement* start = nullptr;
		std::queue<tinyxml2::XMLElement*> elements;
		for (auto child = root->FirstChildElement(); child; child = child->NextSiblingElement())
		{
			elements.push(child);
		}

		while (!elements.empty())
		{
			auto element = elements.front();
			elements.pop();

			if (element->Name() == rootName)
			{
				mRoot = make(root);
				return true;
			}

			for (auto child = element->FirstChildElement(); child; child = child->NextSiblingElement())
			{
				elements.push(child);
			}
		}

		return false;
	}
	catch (std::exception e)
	{
		LOG_ERROR(logger, "XML parsing exception %s", e.what());
	}

	return false;
}

void Xml::clear()
{
	mRoot = nullptr;
}

unique_ptr<XmlElement> Xml::make(tinyxml2::XMLElement* element)
{
	if (element == nullptr)
		return nullptr;

	auto pNode = make_unique<XmlElement>();;
	pNode->strTag = element->Name();
	auto text = element->GetText();
	if (text)
	{
		pNode->strValue = text;
	}

	for (auto attr = element->FirstAttribute(); attr; attr = attr->Next())
	{
		(*pNode)[attr->Name()] = attr->Value();
	}

	for (auto child = element->FirstChildElement(); child; child = child->NextSiblingElement())
	{
		auto pChild = make(child);
		if (pChild)
		{
			pNode->children[pChild->strTag] = std::move(pChild);
		}
	}

	return pNode;
}
