//##############################################################################
//                                                                             #
// Copyright 2011 Meir yanovich,	                                           #
//	   Questions just email me to meiry242@gmail.com / meiryanovich@gmail.com  #
//                                                                             #
//##############################################################################
#include <QtWidgets>
#include "MainWindowContainer.h"
#include "UT.h"
#include "HttpClient.h"
#include <QtWebKitWidgets/QWebFrame>
#include <QtWebKit/QWebElement>
#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValueIterator>
#include "json.h"

using namespace std;

string urlencode(const string &c);
string char2hex( char dec );

void MainWindowContainer::Check_validity_of_current_token()
{
    // Read File for old access token

    // CHECK SHIT
     /*QString ApiURLToInvoke;
     QString response1;
     QMap<QString,QString> args1;
     ApiURLToInvoke = FBApi::getInstance()->GetGENERAL_API_URL()+"/"+"oauth/access_token_info?";
     args1.insert("client_id", FBApi::getInstance()->GetAPPLICATION_ID());
     // NEED TO GET A TOKEN FOR THIS PART!
     args1.insert("access_token",OLDACCESSTOKEN);
     FBApi::getInstance()->InvokeAPI(FBApi::GET,
                                     args1,
                                     response1,
                                     ApiURLToInvoke);

     std::cerr << response1.toStdString() << std::endl;*/
}

MainWindowContainer::MainWindowContainer(QWidget* parent) : 
	QMainWindow(parent)  
{
 m_DummyFile = new QFile("/tmp/DummyFile.txt");
 m_limit = 25 ;
 m_offset = 0 ;
 ui.setupUi(this);
 progress = 0;
 m_MarkedRowPrev = -1;
 //create web view signals

 // Check_validity_of_current_token
 //Check_validity_of_current_token();

 createWebViewActions();
 createMenuActions();
 m_manager = new QNetworkAccessManager(this);

 // this is the main url for the web Authentication and authorization to FB 
 QString BaseAuthorizeUrl = "https://www.facebook.com/dialog/oauth";
 QString weburl = BaseAuthorizeUrl + "?"
				   "client_id=" + FBApi::getInstance()->GetAPPLICATION_ID()
                  + "&redirect_uri=http://www.facebook.com/connect/login_success.html&"
				  + "type=user_agent&"
				  + "display=popup&"
                  + "scope=publish_actions,user_groups";

UT::getInstance()->LogToFile("Authentication:"+weburl);  
SetNetmanager(weburl);
ui.webView->load(QUrl(weburl));
ui.webView->show();

}


void MainWindowContainer::SetNetmanager(const QString& weburl)
{
 	 ui.webView->page()->setNetworkAccessManager(m_manager);
	 m_request = QNetworkRequest();
	 m_request.setUrl(QUrl(weburl));
	 connect(m_manager, SIGNAL(finished(QNetworkReply*)),this, SLOT(finishedSlot(QNetworkReply*)));
	 m_manager->get(m_request);
 }


void MainWindowContainer::createWebViewActions()
{
    connect(ui.webView, SIGNAL(loadFinished(bool)), SLOT(adjustLocation()));
    connect(ui.webView, SIGNAL(titleChanged(QString)), SLOT(adjustTitle()));
    connect(ui.webView, SIGNAL(loadProgress(int)), SLOT(setProgress(int)));
    connect(ui.webView, SIGNAL(loadFinished(bool)), SLOT(finishLoading(bool)));
	connect(ui.webView, SIGNAL(linkClicked(const QUrl&)),this, SLOT(linkClicked(const QUrl&)));
}


void MainWindowContainer::createMenuActions()
{
	 connect(ui.actionClose, SIGNAL(triggered()),this, SLOT(CloseApp()));

}

void MainWindowContainer::CloseApp()
{
	//do somthing befor closing window (i have nothing to do ...)
   ;
}

void MainWindowContainer::linkClicked(const QUrl& url)
{
	SetNetmanager(url.toString());
    ui.webView->load(url);
	ui.webView->setFocus();
}

 
void MainWindowContainer::finishedSlot(QNetworkReply* reply)
{   
	
	QNetworkReply::NetworkError networkError = QNetworkReply::NoError;
	if(!HttpClient::getInstance()->finishedNetWorkSlot(reply,networkError))
	{
		 
		switch(networkError)
		{
		case(QNetworkReply::ConnectionRefusedError):
			ui.webView->setHtml("<html><body>NO NETWORK CONNECTION !! ConnectionRefusedError</body></html>");
			break;
		case(QNetworkReply::HostNotFoundError):
			//handle the html output is no internet connection is found
			ui.webView->setHtml("<html><body>NO NETWORK CONNECTION !!HostNotFoundError</body></html>");
			break;
		}
	} 
	else 
	{
		if(HttpClient::getInstance()->IsUserAutorized)
		{
			HttpClient::getInstance()->IsUserAutorized = false;

            std::cout << "access token: " << FBApi::getInstance()->GetUserAccessToken().toStdString();

            //TestAPIs();

            QApplication::quit();
        }
	}
}
 
void MainWindowContainer::setProgress(int p)
{
    progress = p;
    adjustTitle();
}

void MainWindowContainer::adjustTitle()
{
	if (progress <= 0 || progress >= 100)
	{
		QString titl =  ui.webView->title();
		setWindowTitle(titl);
	}
    else
	{
        setWindowTitle(QString("%1 (%2%)").arg(ui.webView->title()).arg(progress));
	}
}


void MainWindowContainer::adjustLocation()
{
     
   locationEdit =  ui.webView->url().toString() ;
	 // remove it in the final code
	 // this is here only for fast testing , for you not to type the user/pass all the time
   QString code_email = "document.getElementById('email').value='derp@derp.com'";
   QString code_password = "document.getElementById('pass').value='yourpassword'";
   ui.webView->page()->mainFrame()->evaluateJavaScript(code_email);
   ui.webView->page()->mainFrame()->evaluateJavaScript(code_password);
}

void MainWindowContainer::finishLoading(bool)
{	
	progress = 100;
    adjustTitle();
	
}

void MainWindowContainer::changeLocation()
{
    
	QUrl url = QUrl(locationEdit);
	SetNetmanager(url.toString());
    ui.webView->load(url);
    ui.webView->setFocus();
}
/*


This is where we construct our FB api's 
the api's are simple http/s request . that returns json format


*/
QString MainWindowContainer::GetApiRequest(bool firstTimeCall ,QString prevLink)
{
	
 	QMap<QString,QString> args1;
	QString USER_ID;
	QString ApiURLToInvoke;
	ui.webView->setHtml("<h1> Invoking API !!! </h1><br>");
	if(firstTimeCall)
	{
		
		USER_ID = FBApi::getInstance()->GetUserIdKey();
		ApiURLToInvoke = FBApi::getInstance()->GetGENERAL_API_URL()+"/"+
                                            "me/feed?";

		args1.insert("access_token",FBApi::getInstance()->GetUserAccessToken());
        //args1.insert("limit",QString::number(m_limit));
        //args1.insert("offset",QString::number(m_offset));
        args1.insert("message", "Testing Facebook API from QT/CPP");
        Json::Value fromScratch;
        fromScratch["value"] = "SELF";
        Json::FastWriter styledWriter;
        args1.insert("privacy", styledWriter.write(fromScratch).c_str() );
        std::cerr << styledWriter.write(fromScratch);


	}
	else 
	{
 		ApiURLToInvoke = prevLink;
	}

	 
	QString response1;

    FBApi::getInstance()->InvokeAPI(FBApi::POST,
	                                args1,
	                                response1,
                                    ApiURLToInvoke);

	return response1;
}

void MainWindowContainer::WriteToDummyFile(QString printData)
{
    /*QString printData;
	printData.append("FBApi " + GetCurrentDateTime()+"  "+data);*/
	
	if ( m_DummyFile->open( IO_WriteOnly | IO_Append  ) )
	{
		QTextStream stream(m_DummyFile );
		//stream.setAutoDetectUnicode(true);
		stream.setCodec("UTF-8");
		stream << printData << endl;
		m_DummyFile->close();
	} 
	else 
	{
		QTextStream stream(m_DummyFile );
		//stream.setAutoDetectUnicode(true);
		stream.setCodec("UTF-8");
		stream << printData << endl;
		m_DummyFile->close();
	}
}

 
 
inline void StrReplace(std::string& str, const std::string& old, const std::string& newstr)
{
  size_t pos = 0;
  while((pos = str.find(old, pos)) != std::string::npos)
  {
     str.replace(pos, old.length(), newstr);
     pos += newstr.length();
  }
}
// this (long) function will exstract group wall info , with the help of jsoncpp lib
QString MainWindowContainer::GetGroupWallFeeds(QString strWallFeeds)
{
	QString QpreviousLink = "";
	UT::getInstance()->LogToFile("Rerutend from API:"+strWallFeeds);
	std::string jsonResponse(strWallFeeds.toLatin1());
	Json::Value root;   // will contains the root value after parsing.
	Json::Reader reader;
	bool parsingSuccessful = reader.parse( jsonResponse, root );
	if ( !parsingSuccessful )
	{
		// report to the user the failure and their locations in the document.
		std::string ss;
		ss.append("Failed to parse configuration ");
		ss.append(reader.getFormatedErrorMessages());
		QString s0 = QString::fromStdString(ss);
		 UT::getInstance()->LogToFile(s0); 
		return QpreviousLink;
	}
	
	const Json::Value datas = root["data"];
	QString Qrec ="";
	 
	for ( unsigned int index = 0; index < datas.size(); ++index )  // Iterates over the sequence elements.
	{
		 Json::Value text = datas[index];
		
		 std::string txtlink = text["link"].asString();
		 StrReplace(txtlink,","," ");
		 StrReplace(txtlink,"\""," ");
		 StrReplace(txtlink,"'"," ");
		 QString Qtxtlink = QString::fromStdString(txtlink);
		 if(!Qtxtlink.contains("http://www.youtube.com/watch", Qt::CaseInsensitive))
		 {
			 continue;
		 }
		  
 		 //Json::Value::Members  mn = text.getMemberNames();
		 Json::Value txtFrom = text["from"];
		 std::string txtFromName = txtFrom["name"].asString();
		 StrReplace(txtFromName,","," ");
		 StrReplace(txtFromName,"\""," ");
		 StrReplace(txtFromName,"'"," ");
		 QString QtxtFromName = QString::fromStdString(txtFromName);
		 //UT::getInstance()->LogToFile(QtxtFromName); 
		 //WriteToDummyFile(QtxtFromName);
 
		 std::string txtSongName = text["name"].asString();
		 StrReplace(txtSongName,","," ");
		 StrReplace(txtSongName,"\""," ");
		 StrReplace(txtSongName,"'"," ");
		 QString QtxtSongName = QString::fromStdString(txtSongName);
		 
		 //UT::getInstance()->LogToFile(" "+QtxtSongName); 
		 //WriteToDummyFile(" "+QtxtSongName);
		 std::string txtCreatedTime = text["created_time"].asString();
		 StrReplace(txtCreatedTime,","," ");
		 StrReplace(txtCreatedTime,"\""," ");
		 StrReplace(txtCreatedTime,"'"," ");
		 StrReplace(txtCreatedTime,"T"," ");
		 StrReplace(txtCreatedTime,"+0000"," ");
		 QString QtxtCreatedTime = QString::fromStdString(txtCreatedTime);
		 
		 
		 Json::Value likesValue = text["likes"];
		 int itxtlikes = text["likes"].asInt();
		 QString Qitxtlikes = QString::number(itxtlikes);
		 //UT::getInstance()->LogToFile("     "+Qitxtlikes);
		 //WriteToDummyFile("     "+Qitxtlikes);  
		 Qrec = QtxtFromName+","+QtxtSongName+","+Qitxtlikes+","+Qtxtlink+","+QtxtCreatedTime;
		 WriteToDummyFile(Qrec);
		 Qrec.clear();


		 
	}
	std::string previousLink ="";
    const Json::Value paging = root["paging"];
	//std::string direction = "previous";
	std::string direction = "next";
    if(!paging[direction].isNull())
    {
        previousLink = paging[direction].asString();
    }
    
	 
	 
	 QpreviousLink = QString::fromUtf8(previousLink.c_str());
	 // convert to unixtime
	 QByteArray urlByteArr;
	 urlByteArr.append(QpreviousLink);
	 QByteArray text = QByteArray::fromPercentEncoding(urlByteArr);
	 QString tmpUrlDecod(text.data());            // returns "Qt is great!"
     UT::getInstance()->LogToFile("fromPercentEncoding:"+tmpUrlDecod);
	 QpreviousLink = tmpUrlDecod;
	 
      
 return QpreviousLink;
}
 
/*
 this is Container function to test api's 
 the api here will use the FB graph api to extract group wall data 
 from current date until the beginning of the group.
 the result will be dumped to DummyFile.txt. 
 Note : this is can take very long time , depending on group size 

*/
void MainWindowContainer::TestAPIs()
{
    
    QString response1;
	response1 = GetApiRequest(true,"");
    //QString QpreviousLink = GetGroupWallFeeds(response1);
    /*
		This is of course nice old trick to mimic recursive calls 
		without using recursive call's using stack

	*/
    /*QStack<QString > prevLinkStack;
	if(QpreviousLink.length() > 2)
	{
		prevLinkStack.push(QpreviousLink);
		while(prevLinkStack.count() > 0)
		{
			QString QprevLink = prevLinkStack.pop();
			
			response1 = GetApiRequest(false,QprevLink);
			QpreviousLink = GetGroupWallFeeds(response1);
			if(QpreviousLink.length() > 2)
			{
				prevLinkStack.push(QpreviousLink);
			}
		}
    }*/
	ui.webView->setHtml("<h1> Done Invoking API !!! </h1>");	
}

//based on javascript encodeURIComponent()
string urlencode(const string &c)
{

    string escaped="";
    int max = c.length();
    for(int i=0; i<max; i++)
    {
        if (c[i] == '\n')
            continue;
        else if ( (48 <= c[i] && c[i] <= 57) ||//0-9
             (65 <= c[i] && c[i] <= 90) ||//abc...xyz
             (97 <= c[i] && c[i] <= 122) || //ABC...XYZ
             (c[i]=='~' || c[i]=='!' || c[i]=='*' || c[i]=='(' || c[i]==')' || c[i]=='\'')
        )
        {
            escaped.append( &c[i], 1);
        }
        else
        {
            escaped.append("%");
            escaped.append( char2hex(c[i]) );//converts char 255 to string "ff"
        }
    }
    return escaped;
}

string char2hex( char dec )
{
    char dig1 = (dec&0xF0)>>4;
    char dig2 = (dec&0x0F);
    if ( 0<= dig1 && dig1<= 9) dig1+=48;    //0,48inascii
    if (10<= dig1 && dig1<=15) dig1+=97-10; //a,97inascii
    if ( 0<= dig2 && dig2<= 9) dig2+=48;
    if (10<= dig2 && dig2<=15) dig2+=97-10;

    string r;
    r.append( &dig1, 1);
    r.append( &dig2, 1);
    return r;
}



