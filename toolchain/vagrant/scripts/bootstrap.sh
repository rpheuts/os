if [[ ! -f ~/.bootstrapped ]]
then
	# Update Aptitude
	sudo apt-get update

	# Install build environment for building ruby native dependencies
	sudo apt-get install build-essential ruby1.9.1-dev --no-upgrade -y

	# Install required Chef version
	sudo gem install chef --version 11.4.2 --no-rdoc --no-ri --conservative

	# Ensuring bootstrap is idempotent
	touch ~/.bootstrapped
else
	echo Already bootstrapped, skipping bootstrap...
fi
