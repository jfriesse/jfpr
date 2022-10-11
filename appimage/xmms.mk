xmms-1.2.11-x86_64.AppImage: AppRun-x86_64 appimagetool-x86_64.AppImage xmms.AppDir/usr/bin/xmms
	rm -rf xmms.AppDir/etc
	rm -rf xmms.AppDir/install
	rm -rf xmms.AppDir/usr/doc
	rm -rf xmms.AppDir/usr/include
	rm -rf xmms.AppDir/usr/lib64/*.a
	rm -rf xmms.AppDir/usr/lib64/glib
	rm -rf xmms.AppDir/usr/lib64/pkgconfig
	rm -rf xmms.AppDir/usr/share/aclocal
	rm -rf xmms.AppDir/usr/share/locale
	cp AppRun-x86_64 xmms.AppDir/AppRun
	cp xmms.AppDir/usr/share/pixmaps/xmms.png xmms.AppDir/xmms.png
	cp xmms.AppDir/usr/share/applications/xmms.desktop xmms.AppDir/xmms.desktop
	./appimagetool-x86_64.AppImage xmms.AppDir xmms-1.2.11-x86_64.AppImage

xmms.AppDir/usr/bin/xmms: glib-1.2.10-x86_64-3.txz gtk+-1.2.10-x86_64-5.txz xmms-1.2.11-x86_64-5.txz
	mkdir "xmms.AppDir"

	tar -C "xmms.AppDir" -xf glib-1.2.10-x86_64-3.txz
	ln -sf libglib-1.2.so.0.0.10 xmms.AppDir/usr/lib64/libglib-1.2.so.0
	ln -sf libgthread-1.2.so.0.0.10 xmms.AppDir/usr/lib64/libgthread-1.2.so.0
	ln -sf libgmodule-1.2.so.0.0.10 xmms.AppDir/usr/lib64/libgmodule-1.2.so.0

	tar -C "xmms.AppDir" -xf gtk+-1.2.10-x86_64-5.txz
	ln -sf libgtk-1.2.so.0.9.1 xmms.AppDir/usr/lib64/libgtk-1.2.so.0
	ln -sf libgdk-1.2.so.0.9.1 xmms.AppDir/usr/lib64/libgdk-1.2.so.0

	tar -C "xmms.AppDir" -xf xmms-1.2.11-x86_64-5.txz
	ln -sf libxmms.so.1.3.1 xmms.AppDir/usr/lib64/libxmms.so.1
	sed -i -e 's#/usr#././#g' xmms.AppDir/usr/bin/xmms
	sed -i -e 's#/usr#././#g' xmms.AppDir/usr/lib64/libxmms.so.1.3.1

AppRun-x86_64:
	wget 'https://github.com/AppImage/AppImageKit/releases/download/continuous/AppRun-x86_64'
	chmod 755 AppRun-x86_64

appimagetool-x86_64.AppImage:
	wget 'https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage'
	chmod 755 appimagetool-x86_64.AppImage

xmms-1.2.11-x86_64-5.txz:
	wget 'https://mirrors.slackware.com/slackware/slackware64-14.2/slackware64/xap/xmms-1.2.11-x86_64-5.txz'

glib-1.2.10-x86_64-3.txz:
	wget 'https://mirrors.slackware.com/slackware/slackware64-14.2/slackware64/l/glib-1.2.10-x86_64-3.txz'

gtk+-1.2.10-x86_64-5.txz:
	wget 'https://mirrors.slackware.com/slackware/slackware64-14.2/slackware64/l/gtk+-1.2.10-x86_64-5.txz'
