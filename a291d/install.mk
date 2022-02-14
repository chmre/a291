#set(THIS a291d)

debug-install:
	$(NORMAL_INSTALL) # Normal commands follow
	# Install the config file
	mkdir /etc/a291d
	cp resource/config/a291d.conf /etc/a291d/a291d.conf
	
	# Install the systemd file
	cp resource/systemd/a291d.service /etc/systemd/system/a291d.service

	# Install the binary program
	mkdir /usr/bin/a291d
	cp Debug/a291d /usr/bin/a291d/a291d

release-install:
	$(NORMAL_INSTALL) # Normal commands follow
	# Install the config file
	mkdir /etc/a291d
	cp resource/config/a291d.conf /etc/a291d/a291d.conf
	
	# Install the systemd file
	cp resource/systemd/a291d.service /etc/systemd/system/a291d.service

	# Install the binary program
	mkdir /usr/bin/a291d
	cp Debug/a291d /usr/bin/a291d/a291d

remove:
	rm /etc/a291d/a291d.conf
	rmdir /etc/a291d
	rm /etc/systemd/system/a291d.service
	rm /usr/bin/a291d/a291d
	rmdir /usr/bin/a291d