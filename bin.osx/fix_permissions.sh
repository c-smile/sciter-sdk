chmod +x packfolder
for f in *.app; do
	chmod +x $f/Contents/MacOS/*
done
