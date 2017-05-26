.PHONY: all clean scp*

all:
	make -C SB_APIs $@
	make -C cgi $@
	make -C app $@

clean:
	make -C SB_APIs $@
	make -C cgi $@
	make -C app $@

#-scp -r htdocs/[!.]* root@192.168.10.19:/var/www/html/
scp19:
	-rsync -rv --exclude='.*' htdocs/ root@192.168.10.19:/var/www/html/ 
	make -C cgi $@
	make -C app $@
	
