"C:\Program Files (x86)\Inno Setup 5\compil32.exe" /cc "inno-setup.iss"
"C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\bin\signtool.exe" sign /a /n "Terra Informatica Software, Inc." /t http://timestamp.verisign.com/scripts/timstamp.dll /du "https://sciter.com" ../distributions/sciter-notes-setup.exe
"C:\Program Files\WinRAR\WinRAR" a -r -m5 ..\distributions\sciter-notes.zip ..\..\bin\32\notes.exe
"C:\Program Files\WinRAR\WinRAR" a -r -m5 ..\distributions\sciter-notes.zip ..\..\bin\32\sciter.dll
"C:\Program Files\WinRAR\WinRAR" a -r -m5 ..\distributions\sciter-notes.zip ..\..\bin\64\notes.exe
"C:\Program Files\WinRAR\WinRAR" a -r -m5 ..\distributions\sciter-notes.zip ..\..\bin\64\sciter.dll
