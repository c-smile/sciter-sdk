echo "packaging!"
cp -r ../../bin.osx/SciterNotes.app ~/Desktop/notes-dist/SciterNotes.app
cp ../../bin.osx/sciter-osx-64.dylib ~/Desktop/notes-dist/SciterNotes.app/Contents/MacOS/sciter-osx-64.dylib
#hdiutil create ../distributions/sciter-notes-setup.dmg -volname "SciterNotes" -srcfolder ~/Desktop/notes-dist
test -f ../distributions/sciter-notes-setup.dmg && rm ../distributions/sciter-notes-setup.dmg
~/Desktop/create-dmg/create-dmg \
--volname "Sciter Notes Installer" \
--volicon "./AppIcon.icns" \
--background "./installer-back.png" \
--window-pos 200 120 \
--window-size 600 300 \
--icon-size 100 \
--icon SciterNotes.app 100 100 \
--hide-extension SciterNotes.app \
--app-drop-link 460 100 \
../distributions/sciter-notes-setup.dmg \
~/Desktop/notes-dist/
echo "packaging!"
codesign --force --sign "Mac Developer: andrew@terrainformatica.com (46U3Y5E9ZC)" --deep --verbose --verify --timestamp=none ~/Desktop/sciter/sdk/notes/distributions/sciter-notes-setup.dmg

spctl -a -t install --verbose --verbose --context context:primary-signature -v ~/Desktop/sciter/sdk/notes/distributions/sciter-notes-setup.dmg