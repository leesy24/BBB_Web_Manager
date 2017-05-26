.PHONY: all clean htdocs scp*

all:
	make -C SB_APIs $@
	make -C cgi $@
	make -C app $@

clean:
	make -C SB_APIs $@
	make -C cgi $@
	make -C app $@

htdocs:
	chmod 755 htdocs/images htdocs/cgi-bin
	chmod -R 644 htdocs/*

#-scp -r htdocs/[!.]* root@192.168.10.19:/var/www/html/
scp19: htdocs
	-rsync -rv --exclude='.*' htdocs/ root@192.168.10.19:/var/www/html/ 
	make -C cgi $@
	make -C app $@
	
