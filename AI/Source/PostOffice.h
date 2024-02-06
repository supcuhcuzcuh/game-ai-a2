#ifndef POST_OFFICE_H
#define POST_OFFICE_H

#include "SingletonTemplate.h"

#include <string>
#include <map>
#include "ObjectBase.h"
#include "Message.h"

class PostOffice : public Singleton<PostOffice>
{
	friend Singleton<PostOffice>;

public:
	void Register(const std::string &address, ObjectBase *object);
	bool Send(const std::string &address, Message *message);
	
	template<typename T, typename... ARGS>
	T* QuickSend(const std::string& address, ARGS... args)
	{
		T* message = new T(args...);
		std::map<std::string, ObjectBase*>::iterator it = m_addressBook.find(address);
		if (m_addressBook.find(address) == m_addressBook.end())
		{
			delete message;
			return nullptr;
		}
		ObjectBase *object = (ObjectBase*)it->second;

		// If the Message can be handled, return the message, otherwise return nullptr
		if (object->Handle(message))
			return message;
	
		return nullptr;
	}
	//void BroadCast(Message *message);

private:
	PostOffice();
	~PostOffice();
	std::map<std::string, ObjectBase*> m_addressBook;
};

#endif
