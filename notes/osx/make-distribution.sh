echo "packaging!"
cp -r ../../bin.osx/SciterNotes.app ~/Desktop/notes-dist/SciterNotes.app
cp ../../bin.osx/sciter-osx-64.dylib ~/Desktop/notes-dist/SciterNotes.app/Contents/MacOS/sciter-osx-64.dylib
hdiutil create ../distributions/sciter-notes-setup.dmg -volname "SciterNotes" -srcfolder ~/Desktop/notes-dist