//##############################################################################
//                                                                             #
// Copyright 2015 Michael Bazzinotti | mbazzinotti@gmail.com                   #
// Copyright 2011 Meir yanovich,	                                           #
//	   Questions just email me to meiry242@gmail.com / meiryanovich@gmail.com  #
//                                                                             #
//##############################################################################

Notes from Michael: 
(Prequel) - The original was designed for QT 4, this was modified for QT5..
This demo was further modified, and will post to your facebook wall with the SELF privacy, only you can see it.
If you want to test public posting, make sure also that your app is enabled in 
developers.facebook.com. The funny thing is that the setting to do so also says "allow all people to use this app"
or something like that, but you must ablige to gain public visibility (so they say). 

I honestly did not plan on publishing this demo but I feel I should at least make an effort, get it preserved on
github before I really start modifying the heck out of it. I did this because the original was very out-dated (2011?) and
QT4 only. I'm not sure that there are better solutions for Desktop, outside of maybe a Java solution. 

I've been wanting to create a desktop app that uses the Facebook API without need
for a local web server or a need to access a web server.. For the case of an app that only
serves 1 person or for people who don't mind embedding the app secret into everyone's desktop,
which I do not recommend, this is a solution demo program that you can build from. 

In my case, I will be using this as a Desktop app for many users, so I will still require a web server
to keep the secret key out of consumer hands.

to compile do this simple steps:
-- on visual studio express 2008 
-------------------------------------------
1. download latest Qt sdk for visual studio i used version 4.7.1 for windows xp. 
2. open cmd ,point it to this directory. 
3. type in the cmd : qmake -tp vc FB_GraphApi.pro 
    it will create the visual studio c++ project files .
4.load the created vc solution file. 
5.open the file "FBApi.cpp" in the top fill your developer id and application id you got from facebook. 
  group id if you like to use the demo example 	
5 hit f5 to compile and run the code.

-- on Qt creator 2.1 
1. download latest Qt sdk for mingw i used version 4.7.2 for windows xp. 
   download the mingw in the same page , and Qt creator 2.1 for windows
2. open the FB_GraphApi_mini.pro into the Qt Creator , it supposed to configure the project.
3. open the file "FBApi.cpp" in the top fill your developer id and application id you got from facebook. 
   group id if you like to use the demo example 
4. hit ctrl+shift+B or the hammer icon to build . 

