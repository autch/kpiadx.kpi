
.PHONY: zip

ZIPNAME=kpiadx_`date +%Y%m%d`
ARC = release/kpiadx.kpi kpiadx.txt kpiadx.ini

zip:
	zip -9j $(ZIPNAME) $(ARC)
