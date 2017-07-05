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
	chmod -R 644 htdocs/*
	chmod 755 htdocs/images htdocs/cgi-bin

#-scp -r htdocs/[!.]* root@192.168.10.19:/var/www/html/
scp19: htdocs
	-rsync -rv --exclude='.*' htdocs/ root@192.168.10.19:/var/www/html/ 
	make -C cgi $@
	make -C app $@
	
#-scp -r htdocs/[!.]* root@192.168.10.3:/var/www/html/
scp3: htdocs
	-rsync -rv --exclude='.*' htdocs/ root@192.168.10.3:/var/www/html/ 
	make -C cgi $@
	make -C app $@
