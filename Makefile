all:
	cd caper; make
	cd capella; make

clean:
	cd caper; make clean
	cd capella; make clean

publish:
	rm -rf /tmp/caper /tmp/caper-*
	svn export . /tmp/caper
	rm -f ~/caper-`date +%Y-%m-%d`.zip
	cd /tmp; zip -r9 /tmp/caper-`date +%Y-%m-%d`.zip caper

