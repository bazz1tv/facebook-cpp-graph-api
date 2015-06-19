//##############################################################################
//                                                                             #
// Copyright 2011 Meir yanovich,	                                           #
//	   Questions just email me to meiry242@gmail.com / meiryanovich@gmail.com  #
//                                                                             #
//##############################################################################

#include "FBApi.h"
#include "UT.h"
#include "HttpClient.h"
#include "config.h"

FBApi* FBApi::_instance = NULL ;


FBApi::FBApi()
{
	callId = 0;
}


FBApi* FBApi::getInstance()
{
    
 	if(_instance == NULL)
    {
        _instance = new FBApi();
        
    }
    return _instance;
}

void FBApi::SetApiKeys(const QString& fbSecretKey,
					   const QString& fbSessionKey,
					   const QString& fbuIdKey,
					   const QString& fbSigKey
					   )
{
	m_fbSecretKey = fbSecretKey;
	m_fbSessionKey = fbSessionKey;
	m_fbuIdKey = fbuIdKey;
	m_fbSigKey = fbSigKey;

	 
}



bool FBApi::InvokeAPI(REQ_METHOD req,
                     QMap<QString,QString>& ApiArgsMap,
					 QString& response,
					 QString& BaseUrl)
{
	 
	if(req == FBApi::POST)
	{
		if(HttpClient::getInstance()->PostRequest(response,ApiArgsMap,BaseUrl))
    	{
			return true;
		}
	} 
	else 
	{

		if(HttpClient::getInstance()->GetRequest(response,ApiArgsMap,BaseUrl))
    	{
			return true;
		}
	}
	return false;
}


 

bool FBApi::AuthorizAndAuthenticatUser(const QString& json)
{

	QMap<QString,QString> JsonSessionMap;
	//needed to init the map for the first api call to FB 
	JsonSessionMap["access_token"] = json ;
	 
	
	 
	//Check to see if the user are the same and vaild by checking the Auto api uid value 
	// with the return parametr from the Users_getLoggedInUser api
	
	FBApi::getInstance()->SetAccessToken(json);
	return true;
	//return FBApi::getInstance()->Users_getLoggedInUser();

}

void FBApi::ParseJsonFromHTTP(const QString& http_response,QMap<QString,QString>& JsonSessionMap)
{	 
	QRegExp rx;
    rx.setMinimal(true);
	rx.setPattern("\"session_key\":\"(.*)\",");
	if(rx.indexIn(http_response)>0)
	{
		JsonSessionMap["session_key"] = rx.cap(1);
	}
	rx.setPattern("\"uid\":(.*),");
	if(rx.indexIn(http_response)>0)
	{
		JsonSessionMap["uid"] = rx.cap(1);
	}	
	rx.setPattern("\"expires\":(.*)\",");
	if(rx.indexIn(http_response)>0)
	{
		JsonSessionMap["expires"] = rx.cap(1);
	}
	rx.setPattern("\"secret\":\"(.*)\",");
	if(rx.indexIn(http_response)>0)
	{
		JsonSessionMap["secret"] = rx.cap(1);
	}	
	rx.setPattern("\"sig\":\"(.*)\"");
	if(rx.indexIn(http_response)>0)
	{
		JsonSessionMap["sig"] = rx.cap(1);
	}

}
