Vagrant.configure("2") do |config|
	config.vm.box = "hashicorp/precise32"
    config.vm.hostname = "contiki"

    config.vm.synced_folder "./", "/var/sensors", create: true, group: "vagrant", owner: "vagrant"

    config.vm.provider "virtualbox" do |v|
    	v.name = "Contiki VM"
    	v.customize ["modifyvm", :id, "--memory", "1024"]
    end

    config.vm.provision "shell" do |s|
    	s.path = "provision/bootstrap.sh"
    end

    config.ssh.forward_x11 = true
end
