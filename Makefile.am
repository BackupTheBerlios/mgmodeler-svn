AUTOMAKE_OPTIONS=dist-bzip2
ACLOCAL=@ACLOCAL@ -I m4
SUBDIRS=src icons plugins wrl
#plugins help icons

mostlyclean-local:
	@-rm -f *~
maintainer-clean-local:
	@-rm -f bootstrap.log configure Makefile.in missing config.guess
	@-rm -f mkinstalldirs Doxyfile config.log INSTALL aclocal.m4
	@-rm -f config.status install-sh Makefile config.sub ltmain.sh
	@-rm -fr autom4te.cache
	@-rm -f COPYING depcomp
	@-rm -fr admin/*
	@-rm -rf libltdl
